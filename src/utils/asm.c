#include "../includes/asm.h"

void generar_asm(FILE *out_s, Instrucciones *instrucciones)
{

    Tipo_Operador op = instrucciones->expr->op;
    Simbolo *arg1 = instrucciones->expr->arg1;
    Simbolo *arg2 = instrucciones->expr->arg2;
    Simbolo *res = instrucciones->expr->resultado;

    calcular_offset(arg1, arg2, res);

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
            fprintf(out_s, "\tcmpl\t$0, -%d(%%rbp)\n", arg1->info->id.offset);
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
            fprintf(out_s, "\tcmpl\t$0, -%d(%%rbp)\n", arg1->info->id.offset);
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
        OFFSET = arg1->info->funcion_decl.cantVariables * OFFSET_INC;
        fprintf(out_s, "\tenter\t$%d, $0\n", arg1->info->funcion_decl.cantVariables * 4);
        break;

    case RET:

        if (arg1 && arg1->flag == LITERAL)
            fprintf(out_s, "\tmovl\t$%d\n, %%eax\n", *(int *)arg1->info->literal.valor);
        else if (arg1 && arg1->flag == ID)
            fprintf(out_s, "\tmovl\t-%d(%%rbp), %%eax\n", arg1->info->id.offset);

        fputs("\tret\n", out_s);

        break;

    case END_FUN:
        fputs("\tleave\n", out_s);
        break;

    case MOV:
        if (arg1->flag == LITERAL)
            fprintf(out_s, "\tmovl\t$%d, -%d(%%rbp)\n", *(int *)arg1->info->literal.valor, res->info->id.offset);
        else if (arg1->flag == ID)
        {
            fprintf(out_s, "\tmovl\t-%d(%%rbp), %%eax\n", arg1->info->id.offset);
            fprintf(out_s, "\tmovl\t%%eax, -%d(%%rbp)\n", res->info->id.offset);
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
                fprintf(out_s, "\tmovl\t-%d(%%rbp), %%eax\n", arg1->info->id.offset);

            fprintf(out_s, "\tnegl\t%%eax\n");
            fprintf(out_s, "\tmovl\t%%eax, -%d(%%rbp)\n", res->info->id.offset);
            break;
        }
    case MULT:
    case OR:
    case AND:
    case ADD:
        if (arg1->flag == LITERAL)
            fprintf(out_s, "\tmovl\t$%d, %%eax\n", *(int *)arg1->info->literal.valor);
        else if (arg1->flag == ID)
            fprintf(out_s, "\tmovl\t-%d(%%rbp), %%eax\n", arg1->info->id.offset);

        if (arg2->flag == LITERAL)
            fprintf(out_s, "\t%s\t$%d, %%eax\n", tipo_op_asm[op], *(int *)arg2->info->literal.valor);
        else if (arg2->flag == ID)
            fprintf(out_s, "\t%s\t-%d(%%rbp), %%eax\n", tipo_op_asm[op], arg2->info->id.offset);

        fprintf(out_s, "\tmovl\t%%eax, -%d(%%rbp)\n", res->info->id.offset);
        break;

    case MOD:
    case DIV:
        if (arg1->flag == LITERAL)
            fprintf(out_s, "\tmovl\t$%d, %%eax\n", *(int *)arg1->info->literal.valor);
        else if (arg1->flag == ID)
            fprintf(out_s, "\tmovl\t-%d(%%rbp), %%eax\n", arg1->info->id.offset);
        fputs("\tcltd\n", out_s);

        if (arg2->flag == LITERAL)
            fprintf(out_s, "\tidivl\t$%d, %%eax\n", *(int *)arg2->info->literal.valor);
        else if (arg2->flag == ID)
            fprintf(out_s, "\tidivl\t-%d(%%rbp), %%eax\n", arg1->info->id.offset);

        fprintf(out_s, "\tmovl\t%s, -%d(%%rbp)\n", tipo_op_asm[op], res->info->id.offset);
        break;

    case COMP:
    case LT:
    case GT:
        if (arg1->flag == LITERAL)
            fprintf(out_s, "\tmovl\t$%d, %%eax\n", *(int *)arg1->info->literal.valor);
        else if (arg1->flag == ID)
            fprintf(out_s, "\tmovl\t-%d(%%rbp), %%eax\n", arg1->info->id.offset);

        if (arg2->flag == LITERAL)
            fprintf(out_s, "\tcmpl\t$%d, %%eax\n", *(int *)arg2->info->literal.valor);
        else if (arg2->flag == ID)
            fprintf(out_s, "\tcmpl\t-%d(%%rbp), %%eax\n", arg1->info->id.offset);

        fprintf(out_s, "\t%s\t%%al\n", tipo_op_asm[op]);
        fputs("\tmovzbl\t%al, %eax\n", out_s);
        fprintf(out_s, "\tmovl\t%%eax, -%d(%%rbp)\n", res->info->id.offset);

        break;
    default:
        break;
    }
}

void calcular_offset(Simbolo *arg1, Simbolo *arg2, Simbolo *res)
{
    if (arg1 && arg1->flag == ID && arg1->info->id.offset == -1)
    {
        arg1->info->id.offset = OFFSET;
        OFFSET -= OFFSET_INC;
    }

    if (arg2 && arg2->flag == ID && arg2->info->id.offset == -1)
    {
        arg2->info->id.offset = OFFSET;
        OFFSET -= OFFSET_INC;
    }

    if (res && res->flag == ID && res->info->id.offset == -1)
    {
        res->info->id.offset = OFFSET;
        OFFSET -= OFFSET_INC;
    }
}
