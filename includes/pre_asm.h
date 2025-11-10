#include "info.h"
#include "tsim.h"
#include "enums.h"
#include <stdio.h>
#include <stdlib.h>
/**
 * Estructura de una cuádrupla (instrucción de código intermedio).
 * - op: El operador de la instrucción
 * - arg1: Puntero al símbolo del primer argumento
 * - arg2: Puntero al símbolo del segundo argumento
 * - resultado: Puntero al símbolo donde se almacena el resultado
 */
typedef struct Cuadruplo
{
    Tipo_Operador op;
    Simbolo *arg1;
    Simbolo *arg2;
    Simbolo *resultado;
} Cuadruplo;

/**
 * Estructura para un nodo de la lista enlazada de instrucciones.
 * - expr: Puntero a la cuádrupla de esta instrucción
 * - next: Puntero a la siguiente instrucción en la lista
 */
typedef struct Instrucciones
{
    Cuadruplo *expr;
    struct Instrucciones *next;
} Instrucciones;

/**
 * Estructura contenedora para la lista de instrucciones
 * - head: Puntero al primer nodo de la lista
 * - tail: Puntero al último nodo de la lista
 */
typedef struct Lista_Inst
{
    Instrucciones *head;
    Instrucciones *tail;
} Lista_Inst;

/**
 * Estructura para administrar una lista de símbolos temporales disponibles.
 * - head: Puntero al primer símbolo temporal libre
 */
typedef struct Temporales
{
    Simbolo *head;
} Temporales;

extern int CANT_LINEAS;
extern int CANT_TEMP;
extern int CANT_JUMP;
extern int CANT_TAG;
extern int OFFSET;
extern int OFFSET_INC;
extern int OFFSET_PARAMS_REG;
extern int OFFSET_PARAMS;
extern int OFFSET_GAP;
extern int CANT_VAR;

extern char **codigo;
extern Lista_Inst *lista_inst;
extern Temporales *temporales_libres;

/**
 * Función raíz que inicia la generación de código recorriendo el AST.
 * Procesa declaraciones de variables (hijo izq) y declaraciones de métodos (hijo der).
 *
 * @param nodo          Nodo raíz del AST
 * @param instrucciones Puntero a la lista de instrucciones
 */
void generar_codigo(Arbol *nodo, Lista_Inst *instrucciones);

/**
 * Recorre el subárbol de declaraciones de variables.
 * Genera código de asignación (para inicializaciones) y cuenta las variables locales (CANT_VAR).
 *
 * @param nodo          Nodo del AST
 * @param instrucciones Puntero a la lista de instrucciones
 */
void construir_declaracion_variables(Arbol *nodo, Lista_Inst *instrucciones);

/**
 * Recorre el subárbol de declaraciones de métodos.
 * Llama a 'construir_funcion_decl' para cada método (nodo DECL_FUNCION) que encuentra.
 *
 * @param nodo          Nodo del AST
 * @param instrucciones Puntero a la lista de instrucciones
 */
void construir_declaracion_metodos(Arbol *nodo, Lista_Inst *instrucciones);

/**
 * Recorre una lista enlazada de sentencias (hijo izq, hijo der)
 * y llama a 'construir_sentencia' para cada una.
 *
 * @param nodo          Nodo del AST
 * @param instrucciones Puntero a la lista de instrucciones
 */
void construir_sentencias(Arbol *nodo, Lista_Inst *instrucciones);

/**
 * Procesa un único nodo de sentencia.
 * Llama a la función constructora específica según el tipo de sentencia.
 *
 * @param nodo          Nodo del AST
 * @param instrucciones Puntero a la lista de instrucciones
 */
void construir_sentencia(Arbol *nodo, Lista_Inst *instrucciones);

/**
 * Genera una cuádrupla 'RET'. Si hay valor de retorno (nodo->izq),
 * genera el código para esa expresión y lo asigna a 'arg1'.
 *
 * @param nodo          Nodo del AST (tipo RETURN)
 * @param instrucciones Puntero a la lista de instrucciones
 */
void construir_return(Arbol *nodo, Lista_Inst *instrucciones);

/**
 * Obtiene el Símbolo para un argumento de una expresión.
 * Si es complejo (operación, call), genera el CI y devuelve el temporal con el resultado.
 * Si es simple (ID, LITERAL), crea un símbolo.
 *
 * @param nodo          Nodo del AST
 * @param instrucciones Puntero a la lista de instrucciones
 * @return              Puntero al Símbolo (temporal o variable) con el resultado
 */
Simbolo *obtener_arg(Arbol *nodo, Lista_Inst *instrucciones);

/**
 * Genera código para una operación (binaria o unaria).
 *
 * @param nodo          Nodo del AST
 * @param instrucciones Puntero a la lista de instrucciones
 * @return              Puntero al Símbolo (temporal) que almacena el resultado
 */
Simbolo *construir_op(Arbol *nodo, Lista_Inst *instrucciones);

/**
 * Convierte un operador en formato string (del AST, ej. "+")
 * a su 'Tipo_Operador' (enum, ej. ADD) para la cuádrupla.
 *
 * @param op String del operador
 * @return   El valor Tipo_Operador correspondiente
 */
Tipo_Operador traducir_op(char *op);

/**
 * Genera código para una sentencia condicional.
 *
 * @param nodo          Nodo del AST (tipo IF)
 * @param instrucciones Puntero a la lista de instrucciones
 */
void construir_condicional(Arbol *nodo, Lista_Inst *instrucciones);

/**
 * Genera código para una sentencia de asignación.
 * Evalúa la expresión (nodo->der) y crea una cuádrupla 'MOV' hacia el símbolo (nodo->izq).
 *
 * @param nodo          Nodo del AST (tipo ASIGNACION)
 * @param instrucciones Puntero a la lista de instrucciones
 */
void construir_asignacion(Arbol *nodo, Lista_Inst *instrucciones);

/**
 * Genera código para una iteración.
 * Crea etiquetas 'TAG' de inicio/fin y saltos 'JMPC', 'JMP'.
 *
 * @param nodo          Nodo del AST (tipo WHILE)
 * @param instrucciones Puntero a la lista de instrucciones
 */
void construir_iteracion(Arbol *nodo, Lista_Inst *instrucciones);

/**
 * Genera código para la declaración de una función.
 * Inserta 'EXTERN' (si es externa) o 'TAG' (etiqueta de función), 'START_FUN',
 * procesa el cuerpo ('construir_bloque'), y 'END_FUN'.
 * Calcula y almacena el offset total de variables locales.
 *
 * @param nodo          Nodo del AST (tipo DECL_FUNCION)
 * @param instrucciones Puntero a la lista de instrucciones
 */
void construir_funcion_decl(Arbol *nodo, Lista_Inst *instrucciones);

/**
 * Procesa un bloque de código.
 * Genera código para declaraciones locales (nodo->izq) y luego sentencias (nodo->der).
 *
 * @param nodo          Nodo del AST (tipo BLOQUE)
 * @param instrucciones Puntero a la lista de instrucciones
 */
void construir_bloque(Arbol *nodo, Lista_Inst *instrucciones);

/**
 * Genera cuádruplos 'PARAM' para una llamada a función.
 * Recorre recursivamente la lista de parámetros para generarlos en orden inverso de evaluación.
 *
 * @param params_call   Lista de parámetros de la llamada (del AST)
 * @param instrucciones Lista de instrucciones de la función invocadora (para evaluar expresiones)
 * @param parametros    Lista de instrucciones (de salida) donde se generan los 'PARAM'
 */
void construir_params(Parametro_Call *params_call, Lista_Inst *instrucciones, Lista_Inst *parametros);

/**
 * Genera código para una llamada a función.
 * Genera cuádruplos 'PARAM' en una lista temporal.
 * Añade los cuádruplos 'PARAM' a la lista principal.
 * Inserta el cuádruplo 'CALL' y crea un temporal para el resultado.
 *
 * @param nodo          Nodo del AST (tipo CALL_FUNCION)
 * @param instrucciones Puntero a la lista de instrucciones
 * @return              Puntero al Símbolo (temporal) que almacena el valor de retorno
 */
Simbolo *construir_funcion_call(Arbol *nodo, Lista_Inst *instrucciones);

/**
 * Función para procesar cualquier nodo de expresión.
 * Llama a 'construir_op', 'crear_simbolo' o 'construir_funcion_call'.
 *
 * @param nodo          Nodo del AST
 * @param instrucciones Puntero a la lista de instrucciones
 * @return              Puntero al Símbolo (temporal o variable) con el resultado
 */
Simbolo *construir_expresion(Arbol *nodo, Lista_Inst *instrucciones);

/**
 * Inserta una cuádruplo al final de la lista global de instrucciones.
 * Utiliza el puntero 'lista_inst->tail' para enlazar el nuevo nodo.
 *
 * @param c    Cuádruplo a insertar.
 * @param inst Puntero a la lista de instrucciones.
 */
void insertar_lista(Cuadruplo *c, Lista_Inst *lista_inst);

/**
 * Inserta el inicio de la lista 'p' como el siguiente del último de la lista 'q'
 * y asigna como nueva cola de la lista 'q' al último elemento de la lista 'p'
 *
 * @param p Lista origen (a ser agregada)
 * @param q Lista destino
 */
void insertar_listas(Lista_Inst *p, Lista_Inst *lista_inst);

/**
 * Libera un símbolo temporal (si tiene el flag 'temp' activado)
 * añadiéndolo al frente de la lista 'temporales_libres' para su reutilización.
 *
 * @param temp Símbolo temporal que ya no se necesita
 */
void actualizar_temp(Simbolo *temp);

/**
 * Recorre una lista de instrucciones (de parámetros) y libera
 * el temporal asociado a 'arg1' de cada instrucción (que suele ser 'PARAM').
 *
 * @param params Lista de instrucciones (PARAM)
 */
void actualizar_temp_params(Instrucciones *params);

/**
 * Crea un nuevo símbolo de tipo 'ETIQUETA'.
 * Si 'nombre' es NULL, genera un nombre único usando 'CANT_TAG'.
 * Si se provee un nombre, lo utiliza.
 *
 * @param nombre Nombre deseado para la etiqueta, o NULL para uno automático
 * @return       Puntero al nuevo Símbolo de etiqueta creado
 */
Simbolo *crear_etiqueta(char *nombre);

/**
 * Crea un nuevo símbolo.
 * Si 'flag' es 'ID' e 'info' es NULL, crea un nuevo temporal.
 * Intenta reutilizar uno de 'temporales_libres' (lo saca del stack LIFO).
 * Si no hay libres, crea uno nuevo (ej. t0, t1) usando 'CANT_TEMP'.
 *
 * @param info Información asociada al símbolo (o NULL para crear un temporal)
 * @param flag Tipo de información (ID, LITERAL, ETIQUETA, etc.)
 * @return     Puntero al nuevo Símbolo creado o reutilizado.
 */
Simbolo *crear_simbolo(Info_Union *info, Tipo_Info flag);

/**
 * Reserva memoria para la estructura 'Lista_Inst' para la lista de instrucciones.
 *
 * @return Puntero a la nueva lista
 */
Lista_Inst *crear_lista_instrucciones();

/**
 * Reserva memoria para el nodo raíz de una lista de instrucciones.
 *
 * @return Puntero al nuevo nodo 'Instrucciones' inicializado
 */
Instrucciones *crear_instrucciones();

/**
 * Convierte un Símbolo a su representación en string y lo escribe en el 'buffer' de salida. Usado para imprimir el CI.
 *
 * @param s      Símbolo a convertir
 * @param buffer Buffer de destino donde se escribirá el string
 */
void simbolo_a_str(Simbolo *s, char buffer[64]);

/**
 * Itera sobre la lista de cuádruplos, convierte cada uno a texto usando 'simbolo_a_str',
 * y almacena todas las líneas en el array global 'codigo'.
 *
 * @param instrucciones Puntero a la lista de instrucciones a procesar
 */
void instrucciones_to_str(Instrucciones *instrucciones);

/**
 * Imprime el contenido del array global 'codigo' en el archivo de salida especificado.
 *
 * @param out_ci Puntero al archivo de salida (FILE*) donde se escribirá el CI
 */
void imprimir_codigo_ci(FILE *out_ci);