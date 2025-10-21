#include "../includes/asm.h"

void generar_asm(FILE *out_s, Instrucciones *instrucciones)
{
    Instrucciones *aux = instrucciones;
    Simbolo *params = NULL;
    Simbolo *ptr_params = params;
    int cant_params;
    int cant_params_sin_reg = 0;

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
            if (arg1->flag == LITERAL)
            {
                fprintf(out_s, "\tmovl\t$%d, %%eax\n", *(int *)arg1->info->literal.valor);
                fprintf(out_s, "\tcmpl\t$0, %%eax\n");
            }
            else if (arg1->flag == ID)
            {
                fprintf(out_s, "\tcmpl\t$0, %d(%%rbp)\n", arg1->info->id.offset);
            }
            fprintf(out_s, "\tje\t%s\n", res->info->etiqueta.nombre);

            break;

        case JMPC:
            if (arg1->flag == LITERAL)
            {
                fprintf(out_s, "\tmovl\t$%d, %%eax\n", *(int *)arg1->info->literal.valor);
                fprintf(out_s, "\tcmpl\t$0, %%eax\n");
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

                    if (ptr_params->flag == LITERAL)
                    {
                        fprintf(out_s, "\tmovl\t$%d, %s\n", *(int *)ptr_params->info->literal.valor, reg);
                    }
                    else if (ptr_params->flag == ID)
                    {
                        fprintf(out_s, "\tmovl\t%d(%%rbp), %s\n", ptr_params->info->id.offset, reg);
                    }
                }

                ptr_params = ptr_params->next;
            }

            ptr_params_sin_reg = params_sin_reg;

            while (ptr_params_sin_reg)
            {
                if (ptr_params_sin_reg->flag == LITERAL)
                    fprintf(out_s, "\tmovl\t$%d, %%edi\n", *(int *)ptr_params_sin_reg->info->literal.valor);
                else if (ptr_params_sin_reg->flag == ID)
                    fprintf(out_s, "\tmovl\t%d(%%rbp), %%edi\n", ptr_params_sin_reg->info->id.offset);

                fputs("\tpushq\t%rdi\n", out_s);
                ptr_params_sin_reg = ptr_params_sin_reg->next;
            }

            fputs("\tmovl\t%eax, %edi\n", out_s);
            fprintf(out_s, "\tcall\t%s\n", res->info->etiqueta.nombre);
            ptr_params = NULL;
            params = NULL;
            break;

        case START_FUN:
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
            if (cant_params_sin_reg > 0)
            {
                fprintf(out_s, "\taddq\t$%d, %%rsp\n", cant_params_sin_reg * 8);
            }

            if (arg1 && arg1->flag == LITERAL)
                fprintf(out_s, "\tmovl\t$%d, %%eax\n", *(int *)arg1->info->literal.valor);
            else if (arg1 && arg1->flag == ID)
                fprintf(out_s, "\tmovl\t%d(%%rbp), %%eax\n", arg1->info->id.offset);

            fputs("\tleave\n", out_s);
            fputs("\tret\n", out_s);

            break;

        case MOV:
            if (arg1->flag == LITERAL)
            {
                fprintf(out_s, "\tmovl\t$%d, %d(%%rbp)\n", *(int *)arg1->info->literal.valor, res->info->id.offset);
            }
            else if (arg1->flag == ID)
            {
                fprintf(out_s, "\tmovl\t%d(%%rbp), %%eax\n", arg1->info->id.offset);
                fprintf(out_s, "\tmovl\t%%eax, %d(%%rbp)\n", res->info->id.offset);
            }
            else if (arg1->flag == ETIQUETA)
            {
                fprintf(out_s, "\tmovl\t%%eax, %d(%%rbp)\n", res->info->id.offset);
            }

            break;
        default:
            break;
        }

        aux = aux->next;
    }
}

void directivas(FILE *out_s)
{
    fputs("\t.globl main\n", out_s);
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
    case MINUS:
        if (!arg2)
        {
            if (arg1->flag == LITERAL)
                fprintf(out_s, "\tmovl\t$%d, %%eax\n", *(int *)arg1->info->literal.valor);
            else if (arg1->flag == ID)
                fprintf(out_s, "\tmovl\t%d(%%rbp), %%eax\n", arg1->info->id.offset);
            else if (arg1->flag == ETIQUETA)
                fprintf(out_s, "\tmovl\t%s, %%eax\n", arg1->info->etiqueta.nombre);

            fprintf(out_s, "\tnegl\t%%eax\n");
            fprintf(out_s, "\tmovl\t%%eax, %d(%%rbp)\n", res->info->id.offset);
            break;
        }
    case MULT:
    case OR:
    case AND:
    case ADD:
        if (arg1->flag == LITERAL)
            fprintf(out_s, "\tmovl\t$%d, %%eax\n", *(int *)arg1->info->literal.valor);
        else if (arg1->flag == ID)
            fprintf(out_s, "\tmovl\t%d(%%rbp), %%eax\n", arg1->info->id.offset);
        else if (arg1->flag == ETIQUETA)
            fprintf(out_s, "\tmovl\t%s, %%eax\n", arg1->info->etiqueta.nombre);

        if (arg2->flag == LITERAL)
            fprintf(out_s, "\t%s\t$%d, %%eax\n", tipo_op_asm[op], *(int *)arg2->info->literal.valor);
        else if (arg2->flag == ID)
            fprintf(out_s, "\t%s\t%d(%%rbp), %%eax\n", tipo_op_asm[op], arg2->info->id.offset);
        else if (arg2->flag == ETIQUETA)
            fprintf(out_s, "\t%s\t%s, %%eax\n", tipo_op_asm[op], arg2->info->etiqueta.nombre);

        fprintf(out_s, "\tmovl\t%%eax, %d(%%rbp)\n", res->info->id.offset);
        break;

    case MOD:
    case DIV:
        if (arg1->flag == LITERAL)
            fprintf(out_s, "\tmovl\t$%d, %%eax\n", *(int *)arg1->info->literal.valor);
        else if (arg1->flag == ID)
            fprintf(out_s, "\tmovl\t%d(%%rbp), %%eax\n", arg1->info->id.offset);
        else if (arg1->flag == ETIQUETA)
            fprintf(out_s, "\tmovl\t%s, %%eax\n", arg1->info->etiqueta.nombre);
        fputs("\tcltd\n", out_s);

        if (arg2->flag == LITERAL)
            fprintf(out_s, "\tidivl\t$%d, %%eax\n", *(int *)arg2->info->literal.valor);
        else if (arg2->flag == ID)
            fprintf(out_s, "\tidivl\t%d(%%rbp), %%eax\n", arg2->info->id.offset);
        else if (arg2->flag == ETIQUETA)
            fprintf(out_s, "\tidivl\t%s, %%eax\n", arg2->info->etiqueta.nombre);

        fprintf(out_s, "\tmovl\t%s, %d(%%rbp)\n", tipo_op_asm[op], res->info->id.offset);
        break;

    case COMP:
    case LT:
    case GT:
        if (arg1->flag == LITERAL)
            fprintf(out_s, "\tmovl\t$%d, %%eax\n", *(int *)arg1->info->literal.valor);
        else if (arg1->flag == ID)
            fprintf(out_s, "\tmovl\t%d(%%rbp), %%eax\n", arg1->info->id.offset);
        else if (arg1->flag == ETIQUETA)
            fprintf(out_s, "\tmovl\t%s, %%eax\n", arg1->info->etiqueta.nombre);

        if (arg2->flag == LITERAL)
            fprintf(out_s, "\tcmpl\t$%d, %%eax\n", *(int *)arg2->info->literal.valor);
        else if (arg2->flag == ID)
            fprintf(out_s, "\tcmpl\t%d(%%rbp), %%eax\n", arg2->info->id.offset);
        else if (arg1->flag == ETIQUETA)
            fprintf(out_s, "\tcmpl\t%s, %%eax\n", arg2->info->etiqueta.nombre);

        fprintf(out_s, "\t%s\t%%al\n", tipo_op_asm[op]);
        fputs("\tmovzbl\t%al, %eax\n", out_s);
        fprintf(out_s, "\tmovl\t%%eax, %d(%%rbp)\n", res->info->id.offset);

        break;
    default:
        break;
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