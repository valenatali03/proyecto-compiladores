#include "../includes/asm.h"

void cargar_a_registro(FILE *out_s, Simbolo *sym, const char *reg)
{
    if (!sym) return;
    
    if (sym->flag == LITERAL)
        fprintf(out_s, "\tmovl\t$%d, %s\n", *(int *)sym->info->literal.valor, reg);
    else if (sym->flag == ID && sym->info->id.global)
        fprintf(out_s, "\tmovl\t%s(%%rip), %s\n", sym->info->id.nombre, reg);
    else if (sym->flag == ID)
        fprintf(out_s, "\tmovl\t%d(%%rbp), %s\n", sym->info->id.offset, reg);
    else if (sym->flag == ETIQUETA)
        fprintf(out_s, "\tmovl\t%s, %s\n", sym->info->etiqueta.nombre, reg);
}

void guardar_desde_registro(FILE *out_s, const char *reg, Simbolo *dest)
{
    if (dest->info->id.global)
        fprintf(out_s, "\tmovl\t%s, %s(%%rip)\n", reg, dest->info->id.nombre);
    else
        fprintf(out_s, "\tmovl\t%s, %d(%%rbp)\n", reg, dest->info->id.offset);
}

void aplicar_operacion_binaria(FILE *out_s, Simbolo *arg2, const char *instruccion)
{
    if (arg2->flag == LITERAL)
        fprintf(out_s, "\t%s\t$%d, %%eax\n", instruccion, *(int *)arg2->info->literal.valor);
    else if (arg2->flag == ID && arg2->info->id.global)
        fprintf(out_s, "\t%s\t%s(%%rip), %%eax\n", instruccion, arg2->info->id.nombre);
    else if (arg2->flag == ID)
        fprintf(out_s, "\t%s\t%d(%%rbp), %%eax\n", instruccion, arg2->info->id.offset);
    else if (arg2->flag == ETIQUETA)
        fprintf(out_s, "\t%s\t%s, %%eax\n", instruccion, arg2->info->etiqueta.nombre);
}

void generar_asm(FILE *out_s, Instrucciones *instrucciones)
{
    Instrucciones *aux = instrucciones;
    Simbolo *params = NULL;
    Simbolo *ptr_params = params;
    int cant_params;
    int cant_params_sin_reg = 0;
    bool inicio_fun = false;

    while (aux)
    {
        Tipo_Operador op = aux->expr->op;
        Simbolo *arg1 = aux->expr->arg1;
        Simbolo *arg2 = aux->expr->arg2;
        Simbolo *res = aux->expr->resultado;

        calcular_offset_var(arg1);
        calcular_offset_var(arg2);
        calcular_offset_var(res);

        switch (op)
        {
        case NOT:
        case MINUS:
        case MULT:
        case OR:
        case AND:
        case ADD:
        case MOD:
        case DIV:
        case COMP:
        case LT:
        case GT:
            operadores(out_s, aux);

            break;

        case IF_FALSE:
        case JMPC:
            if (arg1->flag == LITERAL)
            {
                cargar_a_registro(out_s, arg1, "%eax");
                fprintf(out_s, "\tcmpl\t$0, %%eax\n");
            }
            else if (arg1->flag == ID && arg1->info->id.global)
            {
                fprintf(out_s, "\tcmpl\t$0, %s(%%rip)\n", arg1->info->id.nombre);
            }
            else if (arg1->flag == ID)
            {
                fprintf(out_s, "\tcmpl\t$0, %d(%%rbp)\n", arg1->info->id.offset);
            }
            fprintf(out_s, "\tje\t%s\n", res->info->etiqueta.nombre);

            break;

        case JMP:
            fprintf(out_s, "\tjmp\t%s\n", res->info->etiqueta.nombre);
            break;

        case TAG:
            if (aux->next->expr->op == START_FUN)
            {
                fprintf(out_s, "\t.globl\t%s\n", res->info->etiqueta.nombre);
            }

            fprintf(out_s, "%s:\n", res->info->etiqueta.nombre);
            break;

        case PARAM:
            if (!params)
            {
                params = malloc(sizeof(Simbolo));
                params->info = arg1->info;
                params->flag = arg1->flag;
                params->next = NULL;
                ptr_params = params;
            }
            else
            {
                ptr_params->next = arg1;
                ptr_params = ptr_params->next;
            }

            break;

        case CALL:
            ptr_params = params;
            cant_params = *(int *)arg1->info->literal.valor;
            Simbolo *params_sin_reg = NULL;
            Simbolo *ptr_params_sin_reg = params_sin_reg;
            cant_params_sin_reg = 0;
            for (int i = cant_params - 1; i >= 0; i--)
            {
                if (i > 5)
                {
                    if (!ptr_params_sin_reg)
                    {
                        params_sin_reg = malloc(sizeof(Simbolo));
                        params_sin_reg->info = ptr_params->info;
                        params_sin_reg->flag = ptr_params->flag;
                        params_sin_reg->next = NULL;
                        ptr_params_sin_reg = params_sin_reg;
                    }
                    else
                    {
                        Simbolo *aux = malloc(sizeof(Simbolo));
                        aux->info = ptr_params->info;
                        aux->flag = ptr_params->flag;
                        aux->next = NULL;
                        ptr_params_sin_reg->next = aux;
                        ptr_params_sin_reg = ptr_params_sin_reg->next;
                    }
                    cant_params_sin_reg++;
                }
                else
                {
                    char *reg = reg_str[i];

                    if (i == 0)
                    {
                        reg = "%eax";
                    }

                    cargar_a_registro(out_s, ptr_params, reg);
                }

                ptr_params = ptr_params->next;
            }

            ptr_params_sin_reg = params_sin_reg;

            while (ptr_params_sin_reg)
            {
                cargar_a_registro(out_s, ptr_params_sin_reg, "%edi");
                fputs("\tpushq\t%rdi\n", out_s);
                ptr_params_sin_reg = ptr_params_sin_reg->next;
            }

            fputs("\tmovl\t%eax, %edi\n", out_s);
            fprintf(out_s, "\tcall\t%s\n", arg2->info->etiqueta.nombre);
            guardar_desde_registro(out_s, "%eax", res);
            ptr_params = NULL;
            params = NULL;
            break;

        case START_FUN:
            inicio_fun = true;
            int cant_var = arg1->info->funcion_decl.cantVariables;
            cant_params = arg1->info->funcion_decl.cant_params;
            cant_params_sin_reg = 0;
            cant_var += cant_params < 6 ? cant_params : 6;
            OFFSET = cant_var * OFFSET_INC;
            OFFSET_PARAMS = 0;
            int total_alloc = ((OFFSET + 15) & ~15);

            fputs("\tpushq\t%rbp\n", out_s);
            fputs("\tmovq\t%rsp, %rbp\n", out_s);

            if (cant_var > 0)
            {
                fprintf(out_s, "\tsubq\t$%d, %%rsp\n", total_alloc);
            }

            params_decl(arg1->info->funcion_decl.params, out_s, cant_var, cant_params);
            break;

        case RET:
            cargar_a_registro(out_s, arg1, "%eax");
            break;

        case END_FUN:
            if (cant_params_sin_reg > 0)
            {
                fprintf(out_s, "\taddq\t$%d, %%rsp\n", cant_params_sin_reg * 8);
            }

            fputs("\tleave\n", out_s);
            fputs("\tret\n", out_s);
            break;

        case MOV:
            mov(out_s, arg1, res, inicio_fun);

            break;
        default:
            break;
        }

        aux = aux->next;
    }
}

void operadores(FILE *out_s, Instrucciones *instrucciones)
{
    Tipo_Operador op = instrucciones->expr->op;
    Simbolo *arg1 = instrucciones->expr->arg1;
    Simbolo *arg2 = instrucciones->expr->arg2;
    Simbolo *res = instrucciones->expr->resultado;

    switch (op)
    {
    case NOT:
        cargar_a_registro(out_s, arg1, "%eax");
        fputs("\ttestl\t%eax, %eax\n", out_s);
        fputs("\tsete\t%al\n", out_s);
        fputs("\tmovzbl\t%al, %eax\n", out_s);

        fprintf(out_s, "\tmovl\t%%eax, %d(%%rbp)\n", res->info->id.offset);

        break;

    case MINUS:
        if (!arg2)
        {
            cargar_a_registro(out_s, arg1, "%eax");
            fprintf(out_s, "\tnegl\t%%eax\n");
            guardar_desde_registro(out_s, "%eax", res);
            break;
        }
    case MULT:
    case OR:
    case AND:
    case ADD:
        cargar_a_registro(out_s, arg1, "%eax");
        aplicar_operacion_binaria(out_s, arg2, tipo_op_asm[op]);
        guardar_desde_registro(out_s, "%eax", res);
        break;

    case MOD:
    case DIV:
        cargar_a_registro(out_s, arg1, "%eax");
        fputs("\tcltd\n", out_s);
        aplicar_operacion_binaria(out_s, arg2, "idivl");
        fprintf(out_s, "\tmovl\t%s, %d(%%rbp)\n", op == DIV ? "%eax" : "%edx", res->info->id.offset);
        break;

    case COMP:
    case LT:
    case GT:
        cargar_a_registro(out_s, arg1, "%eax");

        if (arg2->flag == LITERAL)
            fprintf(out_s, "\tcmpl\t$%d, %%eax\n", *(int *)arg2->info->literal.valor);
        else if (arg2->flag == ID && arg2->info->id.global)
            fprintf(out_s, "\tcmpl\t%s(%%rip), %%eax\n", arg2->info->id.nombre);
        else if (arg2->flag == ID)
            fprintf(out_s, "\tcmpl\t%d(%%rbp), %%eax\n", arg2->info->id.offset);
        else if (arg1->flag == ETIQUETA)
            fprintf(out_s, "\tcmpl\t%s, %%eax\n", arg2->info->etiqueta.nombre);

        fprintf(out_s, "\t%s\t%%al\n", tipo_op_asm[op]);
        fputs("\tmovzbl\t%al, %eax\n", out_s);
        guardar_desde_registro(out_s, "%eax", res);
        break;

    default:
        break;
    }
}

void mov(FILE *out_s, Simbolo *arg1, Simbolo *res, bool inicio_fun)
{
    if (!arg1 || !res)
        return;

    bool res_global = res->info->id.global;
    bool arg_global = arg1->info->id.global;

    if (res_global && !inicio_fun)
    {
        fprintf(out_s, "\t.globl \t%s\n", res->info->id.nombre);
        fprintf(out_s, "\t.data\n");
        fprintf(out_s, "%s:\n", res->info->id.nombre);
        fprintf(out_s, "\t.long \t%d\n", *(int *)arg1->info->literal.valor);
        fprintf(out_s, "\t.text\n");
        return;
    }

    switch (arg1->flag)
    {
    case LITERAL:
        if (res_global)
            fprintf(out_s, "\tmovl\t$%d, %s(%%rip)\n",
                    *(int *)arg1->info->literal.valor,
                    res->info->id.nombre);
        else
            fprintf(out_s, "\tmovl\t$%d, %d(%%rbp)\n",
                    *(int *)arg1->info->literal.valor,
                    res->info->id.offset);
        break;

    case ID:
        cargar_a_registro(out_s, arg1, "%eax");
        guardar_desde_registro(out_s, "%eax", res);
        break;

    case ETIQUETA:
        guardar_desde_registro(out_s, "%eax", res);
        break;

    default:
        fprintf(stderr, "tipo de argumento no soportado\n");
        break;
    }
}

void params_decl(Parametro_Decl *params, FILE *out_s, int cant_var, int cant_params)
{
    if (cant_params == 0)
    {
        return;
    }

    Parametro_Decl *param = params;

    for (int i = 0; i < cant_params; i++)
    {
        if (i < 6)
        {
            param->info->id.offset = -OFFSET;
            OFFSET -= OFFSET_INC;
            fprintf(out_s, "\tmovl\t%s, %d(%%rbp)\n", reg_str[i], param->info->id.offset);
        }
        else
        {
            OFFSET_PARAMS = OFFSET_GAP + (i - 6) * 8;
            param->info->id.offset = OFFSET_PARAMS;
        }

        param = param->next;
    }
}

void calcular_offset_var(Simbolo *var)
{
    if (!var)
    {
        return;
    }

    if (var->flag != ID)
    {
        return;
    }

    if (var->info->id.offset != -1)
    {
        return;
    }

    var->info->id.offset = -OFFSET;
    OFFSET -= OFFSET_INC;
}