%define parse.error verbose
%{
#include <iostream>
#include "main.h"
int yylex();
void yyerror(const char *s);

static Tipo tipo_actual   = Tipo::NINGUNO;
static Uso  uso_actual    = Uso::NINGUNO;
static std::string ambito_actual = "global";
static std::string clase_actual;     // para objetos
static std::vector<bool> retornos;   // una bandera por función abierta (hay funciones anidadas)

static void declarar(EntradaTS *e, Uso uso, Tipo tipo) {
    if (e->uso != Uso::NINGUNO)
        std::cerr << "Linea " << LINEA_ACTUAL << ": '" << e->lexema
                  << "' ya declarado" << std::endl;
    e->uso = uso;
    e->tipo = tipo;
    e->ambito = ambito_actual;
}
%}

%union {
    EntradaTS *ts_ref;
}

%token <ts_ref> ID CTE_INT CTE_FLOAT CTE_STR
%type <ts_ref> cte factor factor_restr invocacion acceso_objeto

%token PR_IF PR_ELSE PR_END_IF PR_BEGIN PR_END
%token PR_POUT PR_RET PR_CLASS PR_FUNCTION PR_USHORTINT PR_DOUBLEF
%token PR_EXTENDS PR_TYPEDEF PR_REPEAT PR_WHILE
%token OP_ASIGNACION OP_IGUAL_IGUAL OP_MENOR_IGUAL OP_MAYOR_IGUAL OP_DISTINTO

%left '+' '-'
%left '*' '/'

%%

prog
    : ID sent_decl_lista PR_BEGIN sent_ejec_lista PR_END ';'
      {
          std::cout << "Se reconocio un programa" << std::endl;
      }
    | ID sent_decl_lista error sent_ejec_lista PR_END ';'
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: falta 'begin' o esta mal formada la seccion declarativa del programa."
                    << std::endl;
          yyerrok;
      }
    | ID sent_decl_lista PR_BEGIN sent_ejec_lista PR_END error
      {
      	  std::cerr << "Linea " << LINEA_ACTUAL
      	            << ": Error: falta ';' despues del 'end' del programa."
      	            << std::endl;
      	  yyerrok;
      }
    ;

/* ---------- sentencias declarativas ---------- */

sent_decl_lista
    : sent_decl_lista sent_decl
    | sent_decl_lista error ';'
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: sentencia declarativa mal formada. "
                    << "Se descartan tokens hasta ';'."
                    << std::endl;
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
    : PR_USHORTINT
    | PR_DOUBLEF
    | ID
    ;

lista_ids
    : lista_ids ',' ID
    | ID
    ;

decl_variables
    : tipo lista_ids ';'
      {
          std::cout << "Declaracion de variables (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
    | tipo error ';'
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: declaracion de variables mal formada."
                    << std::endl;
          yyerrok;
      }
    ;

lista_params_formales
    : lista_params_formales ',' tipo ID
    | tipo ID
    ;

decl_funcion : tipo PR_FUNCTION ID { retornos.push_back(false); }
       '(' lista_params_formales ')' sent_decl_lista
       PR_BEGIN sent_ejec_lista PR_END ';'
       {
           if (!retornos.back()) {
               yyerror("la funcion no tiene sentencia return");
           }
           retornos.pop_back();
           std::cout << "Declaracion de funcion (linea "
                     << LINEA_ACTUAL << ")" << std::endl;
       }
             
    | tipo PR_FUNCTION ID error PR_END ';'
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: declaracion de funcion mal formada."
                    << std::endl;
          yyerrok;
      }
    ;

decl_clase
    : PR_CLASS ID PR_BEGIN clase_item_lista PR_END ';'
      {
          std::cout << "Declaracion de clase (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
    | PR_CLASS ID error PR_END ';'
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: declaracion de clase mal formada."
                    << std::endl;
          yyerrok;
      }
    ;

clase_item_lista
    : clase_item_lista clase_item
    | clase_item
    ;

clase_item
    : tipo ID ';'
    | tipo ID '(' lista_params_formales ')' sent_decl_lista
      PR_BEGIN sent_ejec_lista PR_END ';'
    | extends_clause
    | tipo error ';'
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: declaracion de atributo o metodo mal formada."
                    << std::endl;
          yyerrok;
      }
    ;

extends_clause
    : PR_EXTENDS lista_ids ';'
      {
          std::cout << "Clausula EXTENDS (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
    | PR_EXTENDS error ';'
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: clausula 'extends' mal formada."
                    << std::endl;
          yyerrok;
      }
    ;

decl_typedef
    : PR_TYPEDEF ID '=' '[' lista_valores ']' ';'
      {
          std::cout << "Declaracion de typedef (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
    | PR_TYPEDEF error ';'
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: declaracion de typedef mal formada."
                    << std::endl;
          yyerrok;
      }
    ;

lista_valores
    : lista_valores ',' cte
    | cte
    ;

/* ---------- sentencias ejecutables ---------- */

sent_ejec_lista
    : sent_ejec_lista sent_ejec
    | sent_ejec_lista error ';'
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: sentencia ejecutable mal formada. "
                    << "Se descartan tokens hasta ';'."
                    << std::endl;
          yyerrok;
      }
    |
    ;

sent_ejec
    : asignacion
    | asignacion_atributo
    | invocacion ';'
    | seleccion
    | pout
    | retorno
    | repeat_while
    ;

asignacion
    : ID OP_ASIGNACION expresion ';'
      {
          std::cout << "Asignacion (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
    | ID error ';'
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: asignacion mal formada."
                    << std::endl;
          yyerrok;
      }
    ;

asignacion_atributo
    : ID '.' ID '=' expresion ';'
      {
          std::cout << "Asignacion de atributo (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
    | ID '[' indice ']' '=' expresion ';'
      {
          std::cout << "Asignacion posicional de atributo (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
    | ID '.' ID error ';'
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: asignacion de atributo mal formada."
                    << std::endl;
          yyerrok;
      }
    | ID '[' error ';'
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: asignacion posicional mal formada."
                    << std::endl;
          yyerrok;
      }
    ;

indice
    : CTE_INT
    | ID
    ;

retorno 
    :PR_RET '(' expresion ')' ';'
      {
          if (!retornos.empty()) {
              retornos.back() = true;
          }
          std::cout << "Sentencia RET (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }

      /*              if (!retornos.empty()) {
                  retornos.back() = true;
              } else {
                  yyerror("return fuera de una funcion");
              } */

    // lo que esta comentado es por si no se admite un return fuera de una funcion

    | PR_RET error ';'
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: sentencia 'ret' mal formada."
                    << std::endl;
          yyerrok;
      }
    ;

pout
    : PR_POUT '(' CTE_STR ')' ';'
      {
          std::cout << "Sentencia POUT (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
    | PR_POUT '(' expresion ')' ';'
      {
          std::cout << "Sentencia POUT (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
    | PR_POUT error ';'
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: sentencia 'pout' mal formada."
                    << std::endl;
          yyerrok;
      }
    ;

bloque_sent_ejec
    : sent_ejec
    | PR_BEGIN sent_ejec_lista PR_END ';'
    ;

condicion
    : expresion comparador expresion
    ;

comparador
    : OP_IGUAL_IGUAL
    | OP_MENOR_IGUAL
    | OP_MAYOR_IGUAL
    | OP_DISTINTO
    | '<'
    | '>'
    ;

seleccion
    : PR_IF '(' condicion ')' bloque_sent_ejec PR_END_IF ';'
      {
          std::cout << "Sentencia IF (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
    | PR_IF '(' condicion ')' bloque_sent_ejec
      PR_ELSE bloque_sent_ejec PR_END_IF ';'
      {
          std::cout << "Sentencia IF-ELSE (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
    | PR_IF error ';'
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: sentencia 'if' mal formada, incompleta, "
                    << "o le falta 'end_if'."
                    << std::endl;
          yyerrok;
      }
    ;

repeat_while
    : PR_REPEAT bloque_sent_ejec PR_WHILE '(' condicion ')' ';'
      {
          std::cout << "Sentencia REPEAT-WHILE (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
    | PR_REPEAT error ';'
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: sentencia 'repeat-while' mal formada."
                    << std::endl;
          yyerrok;
      }
    ;

/* ---------- expresiones ---------- */

expresion
    : expresion '+' termino
    | expresion '-' termino
    | termino
    ;

termino
    : termino '*' factor
    | termino '/' factor
    | factor
    ;

factor
    : ID
    | cte
    | '-' CTE_INT
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error semantico: la constante '"
                    << $2->lexema
                    << "' es de tipo USHORTINT (sin signo), "
                    << "no admite el signo '-'."
                    << std::endl;

          $$ = $2;
      }
	| '-' CTE_FLOAT
        {
            std::string lexema_neg = "-" + $2->lexema;
      
            auto it = tabla_simbolos.find(lexema_neg);
      
            if (it == tabla_simbolos.end()) {
                EntradaTS nueva_entrada;
                nueva_entrada.lexema = lexema_neg;
      
                it = tabla_simbolos.insert(
                    {lexema_neg, nueva_entrada}
                ).first;
            }
      
            $$ = &(it->second);
        }
    | invocacion
    | acceso_objeto
    | ID '=' '(' expresion_restr ')'
    ;

expresion_restr
    : expresion_restr '+' termino_restr
    | expresion_restr '-' termino_restr
    | termino_restr
    ;

termino_restr
    : termino_restr '*' factor_restr
    | termino_restr '/' factor_restr
    | factor_restr
    ;

factor_restr
    : ID
    | cte
    | '-' CTE_INT
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error semantico: la constante '"
                    << $2->lexema
                    << "' es de tipo USHORTINT (sin signo), "
                    << "no admite el signo '-'."
                    << std::endl;

          $$ = $2;
      }
    | '-' CTE_FLOAT
      {
          std::string lexema_neg = "-" + $2->lexema;

          auto it = tabla_simbolos.find(lexema_neg);

          if (it == tabla_simbolos.end()) {
              EntradaTS nueva_entrada;
              nueva_entrada.lexema = lexema_neg;

              it = tabla_simbolos.insert(
                  {lexema_neg, nueva_entrada}
              ).first;
          }

          $$ = &(it->second);
      }
    | invocacion
    | acceso_objeto
    ;

cte
    : CTE_INT
    | CTE_FLOAT
    ;

invocacion
    : ID '(' lista_params_reales ')' lista_ctes_opcional
    | ID '(' error ')' lista_ctes_opcional
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: invocacion a funcion mal formada."
                    << std::endl;
          yyerrok;
      }
    ;

lista_params_reales
    : lista_params_reales ',' expresion
    | expresion
    ;

lista_ctes_opcional
    : '[' lista_constantes ']'
    | '[' error ']'
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: lista de orden de evaluacion mal formada."
                    << std::endl;
          yyerrok;
      }
    |
    ;

lista_constantes
    : lista_constantes ',' CTE_INT
    | CTE_INT
    ;

acceso_objeto
    : ID '.' ID llamada_opcional
    | ID '[' indice ']'
    ;

llamada_opcional
    : '(' lista_params_reales ')'
    |
    ;

%%

void yyerror(const char *s) {
	if (std::string(s).find("unexpected end of file") != std::string::npos)
		std::cerr << "Linea " << LINEA_ACTUAL
		          << ": Error: fin de archivo inesperado. Verificar que el programa termine con "
		          << "'end;' y que no haya sentencias o bloques sin cerrar"
		          << std::endl;
	else
    	std::cerr << "Linea " << LINEA_ACTUAL << ": Error: " << s << std::endl;
}
