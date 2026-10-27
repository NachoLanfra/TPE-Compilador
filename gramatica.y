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

%union {
    EntradaTS *ts_ref;
    InfoTipo info_tipo;
    int marca_f;   // marcador de función/método abierto
    int marca_c;   // marcador de clase abierta
    NodoArbol *nodo;
    const char *str_val;
}

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

/* Si un error sintáctico hace que el parser descarte una cabecera ya reducida, se deshace lo que apiló */
%destructor { sem_destruir_cabecera_funcion(); } <marca_f>
%destructor { sem_destruir_cabecera_clase();    } <marca_c>

%left '+' '-'
%left '*' '/'

%%

prog
    : nombre_prog sent_decl_lista PR_BEGIN sent_ejec_lista PR_END ';'
      {
          registrar_arbol_programa($4);
          #ifdef DEBUG
          std::cout << "Se reconocio un programa" << std::endl;
          #endif
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

/* ---------- sentencias declarativas ---------- */

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

extends_clause
    : PR_EXTENDS lista_ids ';'
      {
          sem_extends();
      }
    | PR_EXTENDS error ';'
      {
          sem_limpiar_ids_pendientes();
          error("Sintactico", LINEA_ACTUAL, "clausula 'extends' mal formada");
          yyerrok;
      }
    ;

decl_typedef
    : PR_TYPEDEF ID '=' '[' lista_valores ']' ';'
      {
          sem_typedef($2);
      }
    | PR_TYPEDEF error ';'
      {
          sem_limpiar_valores_pendientes();
          error("Sintactico", LINEA_ACTUAL, "declaracion de typedef mal formada");
          yyerrok;
      }
    ;

lista_valores
    : lista_valores ',' cte { sem_agregar_valor_pendiente($3->lexema); }
    | cte                   { sem_agregar_valor_pendiente($1->lexema); }
    ;

/* ---------- sentencias ejecutables ---------- */

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

asignacion_atributo
    : ID '.' ID '=' expresion ';'
      {
          $$ = sem_asignacion_atributo($1, $3, $5);
      }
    | ID '[' indice ']' '=' expresion ';'
      {
          $$ = sem_asignacion_posicional($1, $3, $6);
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
    : CTE_INT { $$ = crear_hoja($1); }
    | ID      { $$ = sem_factor_id($1); }
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
          #ifdef DEBUG
          std::cout << "Sentencia IF (linea " << LINEA_ACTUAL << ")" << std::endl;
          #endif
      }
    | PR_IF '(' condicion ')' bloque_sent_ejec PR_ELSE bloque_sent_ejec PR_END_IF ';'
      {
          $$ = crear_nodo_if($3, $5, $7);
          #ifdef DEBUG
          std::cout << "Sentencia IF-ELSE (linea " << LINEA_ACTUAL << ")" << std::endl;
          #endif
      }
    | PR_IF error ';'
      {
          $$ = nullptr;
          error("Sintactico", LINEA_ACTUAL, "sentencia 'if' mal formada, incompleta, o le falta 'end_if'");
          yyerrok;
      }
    ;

repeat_while
    : PR_REPEAT bloque_sent_ejec PR_WHILE '(' condicion ')' ';'
      {
          $$ = crear_nodo_repeat_while($2, $5);
          #ifdef DEBUG
          std::cout << "Sentencia REPEAT-WHILE (linea " << LINEA_ACTUAL << ")" << std::endl;
          #endif
      }
    | PR_REPEAT error ';'
      {
          $$ = nullptr;
          error("Sintactico", LINEA_ACTUAL, "sentencia 'repeat-while' mal formada");
          yyerrok;
      }
    ;

/* ---------- expresiones ---------- */

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
    | ID '=' '(' expresion_restr ')'  { $$ = sem_asignacion_expr($1, $4); }
    ;

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

invocacion
    : ID '(' lista_params_reales ')' lista_ctes_opcional
      {
          $$ = sem_invocacion($1);
      }
    | ID '(' error ')' lista_ctes_opcional
      {
          sem_invocacion_error();
          $$ = nullptr;
          error("Sintactico", LINEA_ACTUAL, "invocacion a funcion mal formada");
          yyerrok;
      }
    ;

lista_params_reales
    : lista_params_reales ',' expresion { sem_invocacion_nuevo_param($3); }
    | expresion                         { sem_invocacion_iniciar_params($1); }
    ;

lista_ctes_opcional
    : '[' lista_constantes ']'
    | '[' error ']'
      {
          sem_invocacion_orden_vacio();
          error("Sintactico", LINEA_ACTUAL, "lista de orden de evaluacion mal formada");
          yyerrok;
      }
    |
      {
          sem_invocacion_orden_vacio();
      }
    ;

lista_constantes
    : lista_constantes ',' CTE_INT { sem_invocacion_agregar_orden($3); }
    | CTE_INT                      { sem_invocacion_iniciar_orden($1); }
    ;

acceso_objeto
    : ID '.' ID llamada_opcional   { $$ = sem_acceso_atributo($1, $3); }
    | ID '[' indice ']'            { $$ = sem_acceso_posicional($1, $3); }
    ;

llamada_opcional
    : '(' lista_params_reales ')'  { sem_llamada_opcional_pop(); }
    |
    ;

%%

void yyerror(const char *s) {
	if (std::string(s).find("unexpected end of file") != std::string::npos)
		error("Sintactico", LINEA_ACTUAL, "fin de archivo inesperado. Verificar que el programa termine con 'end;' y que no haya sentencias o bloques sin cerrar");
	else
    	error("Sintactico", LINEA_ACTUAL, s);
}
