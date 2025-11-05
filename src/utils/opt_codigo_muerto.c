#include "../../includes/opt_codigo_muerto.h"

void opt_codigo_muerto(Arbol *arbol)
{
  arbol = opt_declaraciones(arbol);

  opt_flujo_control(arbol);

  opt_post_return(arbol);
}

Arbol *opt_declaraciones(Arbol *arbol)
{
  if (!arbol)
    return NULL;

  arbol->izq = opt_declaraciones(arbol->izq);
  arbol->der = opt_declaraciones(arbol->der);

  switch (arbol->tipo_info)
  {
  case DECLARACION_VARIABLE:
    {
      if (!arbol->izq->info->id.usos)
      {
        reportarWarning(VAR_NO_USADA, arbol->linea, arbol->colum, arbol->izq->info->id.nombre);
        liberar_nodo(arbol);
        return NULL;
      }
      break;
    }
  case DECL_FUNCION:
    {
      if (!arbol->info->funcion_decl.usos)
      {
        reportarWarning(FUN_NO_USADA, arbol->linea, arbol->colum, arbol->info->funcion_decl.nombre);
        liberar_nodo(arbol);
        return NULL;
      }
      break;
    }
  case DECLARACIONES_VARIABLES:
  case DECLARACIONES_FUNCIONES:
    {
      if (arbol->izq == NULL && arbol->der == NULL)
      {
        liberar_nodo(arbol);
        return NULL;
      }
      if (arbol->izq && !arbol->der && arbol->izq->tipo_info == arbol->tipo_info)
      {
        Arbol *tmp = arbol->izq; 
        arbol->izq = NULL;         
        liberar_nodo(arbol);     
        arbol = tmp;
      }
      break;
    }
  default:
    break;
  }

  return arbol;
}


void opt_post_return(Arbol *arbol)
{
  if (!arbol)
    return;

  opt_post_return(arbol->izq);

  opt_post_return(arbol->der);

  if (arbol->tipo_info == SENTENCIAS)
  {
    if (arbol->izq && existe_return(arbol->izq))
    {
      liberar_nodo(arbol->der);
      arbol->der = NULL;
    }
  }
}

int existe_return(Arbol *arbol)
{
  if (!arbol)
    return 0;

  if (arbol->tipo_info == RETURN)
  {
    return 1;
  }

  if (arbol->tipo_info == IF || arbol->tipo_info == WHILE)
  {
    return 0;
  }

  return existe_return(arbol->izq) || existe_return(arbol->der);
}

void opt_flujo_control(Arbol *arbol)
{
  if (!arbol)
    return;

  arbol->izq = opt_nodo_control(arbol->izq);
  arbol->der = opt_nodo_control(arbol->der);

  opt_flujo_control(arbol->izq);
  opt_flujo_control(arbol->der);
}

Arbol *opt_nodo_control(Arbol *subarbol)
{
  if (!subarbol)
    return NULL;

  switch (subarbol->tipo_info)
  {
  case IF:
    return optimizar_if(subarbol);
  case WHILE:
    return optimizar_while(subarbol);
  default:
    return subarbol;
  }
}

Arbol *optimizar_if(Arbol *arbol)
{
  if (arbol->medio->tipo_info == LITERAL)
  {
    bool valor = *(bool *)arbol->medio->info->literal.valor;

    if (valor)
    {
      liberar_nodo(arbol->der);
      arbol->der = NULL;
      Arbol *tmp = arbol->izq;
      free(arbol);
      return tmp;
    }
    else
    {
      liberar_nodo(arbol->izq);
      arbol->izq = NULL;
      Arbol *tmp = arbol->der;
      free(arbol);
      return tmp;
    }
  }
  return arbol;
}

Arbol *optimizar_while(Arbol *arbol)
{
  if (arbol->izq->tipo_info == LITERAL)
  {
    bool valor = *(bool *)arbol->izq->info->literal.valor;

    if (valor)
    {
      reportarWarning(CONDICION_TRUE, arbol->linea, arbol->colum);
      return arbol;
    }
    else
    {
      liberar_nodo(arbol->izq);
      arbol->izq = NULL;
      free(arbol);
      return NULL;
    }
  }

  return arbol;
}

void liberar_nodo(Arbol *arbol)
{
  if (!arbol)
    return;

  liberar_nodo(arbol->izq);
  liberar_nodo(arbol->medio);
  liberar_nodo(arbol->der);

  free(arbol);
}