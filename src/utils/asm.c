#include "../includes/asm.h"

void generar_asm(FILE *out_s, Instrucciones *instrucciones)
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

        fprintf(out_s, "\t%s\n", tipo_op_asm[op]);
        fputs("\tmovzbl\t%%al, %%eax\n", out_s);
        fprintf(out_s, "\tmovl\t%%eax, -%d(%%rbp)\n", res->info->id.offset);

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
