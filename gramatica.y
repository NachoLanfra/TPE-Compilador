%define parse.error verbose

%code requires {
#include "main.h"
#include "arbol.h"
}

%{
#include <iostream>
#include "main.h"
#include "arbol.h"
#include "semantica.h"

int yylex();
void yyerror(const char *s);
%}

/* Mapeo de Tipos para la Pila Semántica del Parser (%union)*/
%union {
    EntradaTS *ts_ref;      /* Puntero a entrada en la Tabla de Símbolos */
    InfoTipo info_tipo;     /* Tipo primitivo, clase o typedef */
    int marca_f;            /* Marcador de apertura de ámbito de función */
    int marca_c;            /* Marcador de apertura de ámbito de clase */
    NodoArbol *nodo;        /* Puntero a nodo del Árbol Sintáctico (AST) */
    const char *str_val;    /* Operador o comparador en texto plano */
}

/* Tokens de la gramática */
%token <ts_ref> ID CTE_INT CTE_FLOAT CTE_STR
%type <info_tipo> tipo
%type <marca_f> cabecera_funcion cabecera_metodo
%type <marca_c> cabecera_clase
%type <ts_ref> cte
%type <str_val> comparador
%type <nodo> factor factor_restr invocacion acceso_objeto
%type <nodo> expresion termino expresion_restr termino_restr
%type <nodo> sent_ejec sent_ejec_lista bloque_sent_ejec
%type <nodo> asignacion asignacion_atributo retorno pout seleccion repeat_while
%type <nodo> condicion indice

%token PR_IF PR_ELSE PR_END_IF PR_BEGIN PR_END
%token PR_POUT PR_RET PR_CLASS PR_FUNCTION PR_USHORTINT PR_DOUBLEF
%token PR_EXTENDS PR_TYPEDEF PR_REPEAT PR_WHILE
%token OP_ASIGNACION OP_IGUAL_IGUAL OP_MENOR_IGUAL OP_MAYOR_IGUAL OP_DISTINTO

/* Si falla el parseo dentro de una función o clase, deshace la apertura de ámbito que hizo la cabecera */
%destructor { sem_destruir_cabecera_funcion(); } <marca_f>
%destructor { sem_destruir_cabecera_clase();    } <marca_c>

/* Precedencia y asociatividad de operadores matemáticos */
%left '+' '-'
%left '*' '/'

%%

/* Axioma Principal de la Gramática*/

prog
    : nombre_prog sent_decl_lista PR_BEGIN sent_ejec_lista PR_END ';'
      {
          registrar_arbol_programa($4);
      }
    | nombre_prog sent_decl_lista error sent_ejec_lista PR_END ';'
      {
          registrar_arbol_programa($4);
          error("Sintactico", LINEA_ACTUAL, "falta 'begin' o esta mal formada la seccion declarativa del programa");
          yyerrok;
      }
    | nombre_prog sent_decl_lista PR_BEGIN sent_ejec_lista PR_END error
      {
          registrar_arbol_programa($4);
          error("Sintactico", LINEA_ACTUAL, "falta ';' despues del 'end' del programa");
          yyerrok;
      }
    ;

nombre_prog
    : ID { sem_declarar_programa($1); }
    ;

/* Sentencias Declarativas (Variables, Funciones, Clases, Typedefs)*/

sent_decl_lista
    : sent_decl_lista sent_decl
    | sent_decl_lista error ';'
      {
          sem_limpiar_ids_pendientes();
          sem_limpiar_valores_pendientes();
          error("Sintactico", LINEA_ACTUAL, "sentencia declarativa mal formada. Se descartan tokens hasta ';'");
          yyerrok;
      }
    |
    ;

sent_decl
    : decl_variables
    | decl_funcion
    | decl_clase
    | decl_typedef
    ;

tipo
    : PR_USHORTINT { $$.t = Tipo::USHORTINT; $$.clase = nullptr; }
    | PR_DOUBLEF   { $$.t = Tipo::DOUBLEF;   $$.clase = nullptr; }
    | ID           { $$.t = Tipo::NINGUNO;   $$.clase = resolver_tipo($1); }
    ;

lista_ids
    : lista_ids ',' ID { sem_agregar_id_pendiente($3); }
    | ID               { sem_agregar_id_pendiente($1); }
    ;

decl_variables
    : tipo lista_ids ';'
      {
          sem_declarar_variables($1);
      }
    | tipo error ';'
      {
          sem_limpiar_ids_pendientes();
          error("Sintactico", LINEA_ACTUAL, "declaracion de variables mal formada");
          yyerrok;
      }
    ;

lista_params_formales
    : lista_params_formales ',' tipo ID { sem_agregar_param_formal($4, $3); }
    | tipo ID                           { sem_agregar_param_formal($2, $1); }
    ;

/* La cabecera es regla aparte para que el %destructor cierre el ámbito si falla el resto de la función */
cabecera_funcion
    : tipo PR_FUNCTION ID
      {
          sem_abrir_funcion($3, $1);
          $$ = 1;
      }
    ;

decl_funcion
    : cabecera_funcion '(' lista_params_formales ')' sent_decl_lista PR_BEGIN sent_ejec_lista PR_END ';'
      {
          sem_cerrar_funcion($7);
      }
    | cabecera_funcion error PR_END ';'
      {
          sem_destruir_cabecera_funcion();
          error("Sintactico", LINEA_ACTUAL, "declaracion de funcion mal formada");
          yyerrok;
      }
    ;

/* Igual que cabecera_funcion: abre el ámbito en una regla propia por el %destructor */
cabecera_clase
    : PR_CLASS ID
      {
          sem_abrir_clase($2);
          $$ = 1;
      }
    ;

decl_clase
    : cabecera_clase PR_BEGIN clase_item_lista PR_END ';'
      {
          sem_cerrar_clase();
      }
    | cabecera_clase error PR_END ';'
      {
          sem_destruir_cabecera_clase();
          error("Sintactico", LINEA_ACTUAL, "declaracion de clase mal formada");
          yyerrok;
      }
    ;

clase_item_lista
    : clase_item_lista clase_item
    | clase_item
    ;

/* Igual que cabecera_funcion */
cabecera_metodo
    : tipo ID
      {
          sem_abrir_metodo($2, $1);
          $$ = 1;
      }
    ;

clase_item
    : tipo ID ';'
      {
          sem_declarar_atributo($2, $1);
      }
    | cabecera_metodo '(' lista_params_formales ')' sent_decl_lista PR_BEGIN sent_ejec_lista PR_END ';'
      {
          sem_cerrar_metodo($7);
      }
    | extends_clause
    | tipo error ';'
      {
          error("Sintactico", LINEA_ACTUAL, "declaracion de atributo o metodo mal formada");
          yyerrok;
      }
    ;

/* Herencia múltiple (Tema 30) */
extends_clause
    : PR_EXTENDS lista_ids ';'
      {
          sem_extends(); // los ids de lista_ids son los padres
      }
    | PR_EXTENDS error ';'
      {
          sem_limpiar_ids_pendientes(); // descarta los padres a medio juntar
          error("Sintactico", LINEA_ACTUAL, "clausula 'extends' mal formada");
          yyerrok;
      }
    ;

/* Enumerados / Tipos definidos (Tema 23) */
decl_typedef
    : PR_TYPEDEF ID '=' '[' lista_valores ']' ';'
      {
          sem_typedef($2); // los valores ya los juntó lista_valores
      }
    | PR_TYPEDEF error ';'
      {
          sem_limpiar_valores_pendientes(); // descarta los valores a medio juntar
          error("Sintactico", LINEA_ACTUAL, "declaracion de typedef mal formada");
          yyerrok;
      }
    ;

lista_valores
    : lista_valores ',' cte { sem_agregar_valor_pendiente($3->lexema); } /* se guarda el lexema, ej. 2$us */
    | cte                   { sem_agregar_valor_pendiente($1->lexema); }
    ;

/*Sentencias Ejecutables*/

sent_ejec_lista
    : sent_ejec_lista sent_ejec { $$ = crear_nodo_sentencias($1, $2); }
    | sent_ejec_lista error ';'
      {
          $$ = $1;
          error("Sintactico", LINEA_ACTUAL, "sentencia ejecutable mal formada. Se descartan tokens hasta ';'");
          yyerrok;
      }
    | { $$ = nullptr; }
    ;

sent_ejec
    : asignacion            { $$ = $1; }
    | asignacion_atributo   { $$ = $1; }
    | invocacion ';'        { $$ = $1; }
    | seleccion             { $$ = $1; }
    | pout                  { $$ = $1; }
    | retorno               { $$ = $1; }
    | repeat_while          { $$ = $1; }
    ;

asignacion
    : ID OP_ASIGNACION expresion ';'
      {
          $$ = sem_asignacion($1, $3);
      }
    | ID error ';'
      {
          $$ = nullptr;
          error("Sintactico", LINEA_ACTUAL, "asignacion mal formada");
          yyerrok;
      }
    ;

/* Asignaciones sobre miembros de objeto (tradicionales o posicionales - Tema 29) */
asignacion_atributo
    : ID '.' ID '=' expresion ';'
      {
          $$ = sem_asignacion_atributo($1, $3, $5); // obj.atr = exp
      }
    | ID '[' indice ']' '=' expresion ';'
      {
          $$ = sem_asignacion_posicional($1, $3, $6); // Tema 29: obj[k] = exp
      }
    | ID '.' ID '(' lista_params_reales ')' ';'
      {
          $$ = sem_acceso_metodo($1, $3); // obj.metodo(...) como sentencia
      }
    | ID '.' ID error ';'
      {
          $$ = nullptr;
          error("Sintactico", LINEA_ACTUAL, "asignacion de atributo mal formada");
          yyerrok;
      }
    | ID '[' error ';'
      {
          $$ = nullptr;
          error("Sintactico", LINEA_ACTUAL, "asignacion posicional mal formada");
          yyerrok;
      }
    ;

indice
    : CTE_INT { $$ = crear_hoja($1); } /* índice constante, se chequea el rango */
    | ID      { $$ = sem_factor_id($1); } /* índice variable, tiene que ser entero */
    ;

retorno 
    : PR_RET '(' expresion ')' ';'
      {
          $$ = sem_retorno($3);
      }
    | PR_RET error ';'
      {
          $$ = nullptr;
          error("Sintactico", LINEA_ACTUAL, "sentencia 'ret' mal formada");
          yyerrok;
      }
    ;

pout
    : PR_POUT '(' CTE_STR ')' ';'
      {
          $$ = sem_pout_str($3);
      }
    | PR_POUT '(' expresion ')' ';'
      {
          $$ = sem_pout_exp($3);
      }
    | PR_POUT error ';'
      {
          $$ = nullptr;
          error("Sintactico", LINEA_ACTUAL, "sentencia 'pout' mal formada");
          yyerrok;
      }
    ;

bloque_sent_ejec
    : sent_ejec                           { $$ = $1; }
    | PR_BEGIN sent_ejec_lista PR_END ';' { $$ = $2; }
    ;

condicion
    : expresion comparador expresion
      {
          $$ = sem_condicion($2, $1, $3);
      }
    ;

comparador
    : OP_IGUAL_IGUAL { $$ = "=="; }
    | OP_MENOR_IGUAL { $$ = "<="; }
    | OP_MAYOR_IGUAL { $$ = ">="; }
    | OP_DISTINTO    { $$ = "!="; }
    | '<'            { $$ = "<"; }
    | '>'            { $$ = ">"; }
    ;

seleccion
    : PR_IF '(' condicion ')' bloque_sent_ejec PR_END_IF ';'
      {
          $$ = crear_nodo_if($3, $5, nullptr);
      }
    | PR_IF '(' condicion ')' bloque_sent_ejec PR_ELSE bloque_sent_ejec PR_END_IF ';'
      {
          $$ = crear_nodo_if($3, $5, $7);
      }
    | PR_IF error ';'
      {
          $$ = nullptr;
          error("Sintactico", LINEA_ACTUAL, "sentencia 'if' mal formada, incompleta, o le falta 'end_if'");
          yyerrok;
      }
    ;

/* Bucle REPEAT-WHILE (Tema 13) */
repeat_while
    : PR_REPEAT bloque_sent_ejec PR_WHILE '(' condicion ')' ';'
      {
          $$ = crear_nodo_repeat_while($2, $5); // $2 es el cuerpo y $5 la condición
      }
    | PR_REPEAT error ';'
      {
          $$ = nullptr;
          error("Sintactico", LINEA_ACTUAL, "sentencia 'repeat-while' mal formada");
          yyerrok;
      }
    ;

/* Expresiones Aritméticas, Términos y Factores*/

expresion
    : expresion '+' termino { $$ = sem_op_binaria("+", $1, $3); }
    | expresion '-' termino { $$ = sem_op_binaria("-", $1, $3); }
    | termino               { $$ = $1; }
    ;

termino
    : termino '*' factor    { $$ = sem_op_binaria("*", $1, $3); }
    | termino '/' factor    { $$ = sem_op_binaria("/", $1, $3); }
    | factor                { $$ = $1; }
    ;

factor
    : ID                              { $$ = sem_factor_id($1); }
    | cte                             { $$ = sem_factor_cte($1); }
    | '-' CTE_INT                     { $$ = sem_factor_cte_neg_int($2); }
    | '-' CTE_FLOAT                   { $$ = sem_factor_cte_neg_float($2); }
    | invocacion                      { $$ = $1; }
    | acceso_objeto                   { $$ = $1; }
    | ID '=' '(' expresion_restr ')'  { $$ = sem_asignacion_expr($1, $4); } /* Asignación en expresión (Tema 17) */
    ;

/* Copia de expresion sin la alternativa ID = ( ... ), para que la asignación en expresión (Tema 17) no se anide */
expresion_restr
    : expresion_restr '+' termino_restr { $$ = sem_op_binaria("+", $1, $3); }
    | expresion_restr '-' termino_restr { $$ = sem_op_binaria("-", $1, $3); }
    | termino_restr                     { $$ = $1; }
    ;

termino_restr
    : termino_restr '*' factor_restr    { $$ = sem_op_binaria("*", $1, $3); }
    | termino_restr '/' factor_restr    { $$ = sem_op_binaria("/", $1, $3); }
    | factor_restr                      { $$ = $1; }
    ;

factor_restr
    : ID            { $$ = sem_factor_id($1); }
    | cte           { $$ = sem_factor_cte($1); }
    | '-' CTE_INT   { $$ = sem_factor_cte_neg_int($2); }
    | '-' CTE_FLOAT { $$ = sem_factor_cte_neg_float($2); }
    | invocacion    { $$ = $1; }
    | acceso_objeto { $$ = $1; }
    ;

cte
    : CTE_INT   { $$ = $1; }
    | CTE_FLOAT { $$ = $1; }
    ;

/* Invocaciones a funciones y paso de orden de evaluación explícito (Tema 20) */
invocacion
    : ID '(' lista_params_reales ')' lista_ctes_opcional
      {
          $$ = sem_invocacion($1); // saca de las pilas lo de esta llamada
      }
    | ID '(' error ')' lista_ctes_opcional
      {
          sem_invocacion_error(); // descarta lo que se apiló para esta llamada
          $$ = nullptr;
          error("Sintactico", LINEA_ACTUAL, "invocacion a funcion mal formada");
          yyerrok;
      }
    ;

lista_params_reales
    : lista_params_reales ',' expresion { sem_invocacion_nuevo_param($3); } /* los demás van a la lista del tope */
    | expresion                         { sem_invocacion_iniciar_params($1); } /* el primero abre una lista nueva */
    ;

/* Si la lista de orden falta o está rota se pushea una vacía, así sem_invocacion siempre tiene algo que sacar */
lista_ctes_opcional
    : '[' lista_constantes ']'
    | '[' error ']'
      {
          sem_invocacion_orden_vacio(); // lista rota: cuenta como sin orden
          error("Sintactico", LINEA_ACTUAL, "lista de orden de evaluacion mal formada");
          yyerrok;
      }
    |
      {
          sem_invocacion_orden_vacio(); // sin lista: orden natural
      }
    ;

lista_constantes
    : lista_constantes ',' CTE_INT { sem_invocacion_agregar_orden($3); } /* los demás van a la lista del tope */
    | CTE_INT                      { sem_invocacion_iniciar_orden($1); } /* el primero abre una lista nueva */
    ;

/* Accesos a miembros de clases (atributos, métodos, accesos posicionales) */
acceso_objeto
    : ID '.' ID                              { $$ = sem_acceso_atributo($1, $3); }
    | ID '.' ID '(' lista_params_reales ')'  { $$ = sem_acceso_metodo($1, $3); } /* Tema 30: busca también en los padres */
    | ID '[' indice ']'                      { $$ = sem_acceso_posicional($1, $3); } /* Tema 29 */
    ;

%%

/* Routine de reporte de errores sintácticos invocado por Bison */
void yyerror(const char *s) {
	if (std::string(s).find("unexpected end of file") != std::string::npos)
		error("Sintactico", LINEA_ACTUAL, "fin de archivo inesperado. Verificar que el programa termine con 'end;' y que no haya sentencias o bloques sin cerrar");
	else
    	error("Sintactico", LINEA_ACTUAL, s);
}