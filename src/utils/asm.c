#include "../includes/asm.h"

void generar_asm(FILE *out_s, Instrucciones *instrucciones)
{

    Tipo_Operador op = instrucciones->expr->op;
    Simbolo *arg1 = instrucciones->expr->arg1;
    Simbolo *arg2 = instrucciones->expr->arg2;
    Simbolo *res = instrucciones->expr->resultado;

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
        operadores(out_s, instrucciones);

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

    case START_FUN:
        int cant_var = arg1->info->funcion_decl.cantVariables;
        int cant_params = arg1->info->funcion_decl.cant_params;
        OFFSET = cant_var * OFFSET_INC;
        OFFSET_PARAMS_REG = OFFSET_INC;
        OFFSET_PARAMS = 0;
        if (cant_var == 1)
            OFFSET_PARAMS_REG = OFFSET + OFFSET_GAP;
        fputs("\tpushq\t%rbp\n", out_s);
        fputs("\tmovq\t%rsp, %rbp\n", out_s);
        if (cant_var > 0)
        {
            fprintf(out_s, "\tsubq\t$%d, %%rbp\n", arg1->info->funcion_decl.cantVariables * 4);
        }

        params_decl(arg1->info->funcion_decl.params, out_s, cant_var, cant_params);
        break;

    case RET:
        fputs("\tleave\n", out_s);
        if (arg1 && arg1->flag == LITERAL)
            fprintf(out_s, "\tmovl\t$%d\n, %%eax\n", *(int *)arg1->info->literal.valor);
        else if (arg1 && arg1->flag == ID)
            fprintf(out_s, "\tmovl\t%d(%%rbp), %%eax\n", arg1->info->id.offset);

        fputs("\tret\n", out_s);

        break;

    case MOV:
        if (arg1->flag == LITERAL)
            fprintf(out_s, "\tmovl\t$%d, %d(%%rbp)\n", *(int *)arg1->info->literal.valor, res->info->id.offset);
        else if (arg1->flag == ID)
        {
            fprintf(out_s, "\tmovl\t%d(%%rbp), %%eax\n", arg1->info->id.offset);
            fprintf(out_s, "\tmovl\t%%eax, %d(%%rbp)\n", res->info->id.offset);
        }

        break;
    default:
        break;
    }

    if (instrucciones->next)
    {
        generar_asm(out_s, instrucciones->next);
    }
}

void directivas(FILE *out_s)
{
    fputs(".globl main\n", out_s);
}

void operadores(FILE *out_s, Instrucciones *instrucciones)
{
    Tipo_Operador op = instrucciones->expr->op;
    Simbolo *arg1 = instrucciones->expr->arg1;
    Simbolo *arg2 = instrucciones->expr->arg2;
    Simbolo *res = instrucciones->expr->resultado;

    switch (op)
    {
    case MINUS:
        if (!arg2)
        {
            if (arg1->flag == LITERAL)
                fprintf(out_s, "\tmovl\t$%d, %%eax\n", *(int *)arg1->info->literal.valor);
            else if (arg1->flag == ID)
                fprintf(out_s, "\tmovl\t%d(%%rbp), %%eax\n", arg1->info->id.offset);

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

        if (arg2->flag == LITERAL)
            fprintf(out_s, "\t%s\t$%d, %%eax\n", tipo_op_asm[op], *(int *)arg2->info->literal.valor);
        else if (arg2->flag == ID)
            fprintf(out_s, "\t%s\t%d(%%rbp), %%eax\n", tipo_op_asm[op], arg2->info->id.offset);

        fprintf(out_s, "\tmovl\t%%eax, %d(%%rbp)\n", res->info->id.offset);
        break;

    case MOD:
    case DIV:
        if (arg1->flag == LITERAL)
            fprintf(out_s, "\tmovl\t$%d, %%eax\n", *(int *)arg1->info->literal.valor);
        else if (arg1->flag == ID)
            fprintf(out_s, "\tmovl\t%d(%%rbp), %%eax\n", arg1->info->id.offset);
        fputs("\tcltd\n", out_s);

        if (arg2->flag == LITERAL)
            fprintf(out_s, "\tidivl\t$%d, %%eax\n", *(int *)arg2->info->literal.valor);
        else if (arg2->flag == ID)
            fprintf(out_s, "\tidivl\t%d(%%rbp), %%eax\n", arg1->info->id.offset);

        fprintf(out_s, "\tmovl\t%s, %d(%%rbp)\n", tipo_op_asm[op], res->info->id.offset);
        break;

    case COMP:
    case LT:
    case GT:
        if (arg1->flag == LITERAL)
            fprintf(out_s, "\tmovl\t$%d, %%eax\n", *(int *)arg1->info->literal.valor);
        else if (arg1->flag == ID)
            fprintf(out_s, "\tmovl\t%d(%%rbp), %%eax\n", arg1->info->id.offset);

        if (arg2->flag == LITERAL)
            fprintf(out_s, "\tcmpl\t$%d, %%eax\n", *(int *)arg2->info->literal.valor);
        else if (arg2->flag == ID)
            fprintf(out_s, "\tcmpl\t%d(%%rbp), %%eax\n", arg1->info->id.offset);

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

void calcular_offset_param_reg(Parametro_Decl *param, int cant_var)
{
    if (!param)
    {
        return;
    }

    if (OFFSET >= OFFSET_PARAMS_REG)
    {
        OFFSET_PARAMS_REG = OFFSET + OFFSET_GAP;
        param->info->id.offset = OFFSET_PARAMS_REG;
        return;
    }

    param->info->id.offset = -OFFSET_PARAMS_REG;
    OFFSET_PARAMS_REG += OFFSET_INC;
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
            calcular_offset_param_reg(param, cant_var);
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
