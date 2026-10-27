%define parse.error verbose
%code requires {
#include "main.h"
#include "arbol.h"
}
%{
#include <iostream>
#include <algorithm>
#include "main.h"
#include "arbol.h"
int yylex();
void yyerror(const char *s);

static const InfoTipo SIN_TIPO = { Tipo::NINGUNO, nullptr };

// Pila de ámbitos: main, main:f, main:f:g ... (base del name mangling)
static std::vector<std::string> pila_ambitos = { "main" };
static std::vector<EntradaTS*> funciones_abiertas;  // funciones/métodos en curso (hay anidamiento)
static std::vector<EntradaTS*> clases_abiertas;     // clases en curso
static std::vector<EntradaTS*> ids_pendientes;      // lista_ids en construcción
static std::vector<std::string> valores_pendientes; // lista_valores (typedef) en construcción

// Para llamadas a funciones (Tema 20)
static std::vector<std::vector<NodoArbol*>> pila_params_reales;
static std::vector<std::vector<int>> pila_orden_eval;

static std::string ambito_actual() {
    std::string r;
    for (size_t i = 0; i < pila_ambitos.size(); i++) {
        if (i) r += ":";
        r += pila_ambitos[i];
    }
    return r;
}

// "a:main:f" -> "a"
static std::string nombre_base(const EntradaTS *e) {
    return e->lexema.substr(0, e->lexema.find(':'));
}

static std::string valor_por_defecto(Tipo tipo) {
    switch (tipo) {
        case Tipo::USHORTINT: return "0";
        case Tipo::DOUBLEF:   return "0.0";
        default:              return "0";
    }
}

// Da de alta un símbolo con su nombre "mangleado" (nombre:ambito).
// 'raw' es la entrada que creó el léxico para el lexema sin ámbito.
static EntradaTS* declarar(EntradaTS *raw, Uso uso, const InfoTipo &t) {
    std::string clave = raw->lexema + ":" + ambito_actual();
    auto it = tabla_simbolos.find(clave);
    if (it != tabla_simbolos.end()) {
        std::cerr << "Linea " << LINEA_ACTUAL << ": Error: '" << raw->lexema
                  << "' ya fue declarado en el ambito " << ambito_actual() << std::endl;
        return &(it->second);
    }
    EntradaTS e;
    e.lexema = clave;
    e.uso    = uso;
    e.tipo   = t.t;
    e.ambito = ambito_actual();
    if (t.clase) e.tipo_nombre = t.clase->lexema;
    return &(tabla_simbolos.insert({clave, e}).first->second);
}

// Resuelve un ID buscando desde el ámbito actual hacia los ámbitos padres
static EntradaTS* resolver_valor(EntradaTS *raw) {
    if (!raw) return nullptr;
    std::string base = nombre_base(raw);

    // Buscar desde el ámbito más anidado hasta el global (main)
    for (int i = (int)pila_ambitos.size(); i >= 1; --i) {
        std::string amb = "";
        for (int j = 0; j < i; ++j) {
            if (j > 0) amb += ":";
            amb += pila_ambitos[j];
        }
        std::string clave = base + ":" + amb;
        auto it = tabla_simbolos.find(clave);
        if (it != tabla_simbolos.end()) {
            return &(it->second);
        }
    }

    // Si no se encontró en la jerarquía léxica, buscar tal cual o retornar raw
    auto it = tabla_simbolos.find(base + ":main");
    if (it != tabla_simbolos.end()) return &(it->second);

    return raw;
}
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
%destructor { funciones_abiertas.pop_back(); pila_ambitos.pop_back(); } <marca_f>
%destructor { clases_abiertas.pop_back();    pila_ambitos.pop_back(); } <marca_c>

%left '+' '-'
%left '*' '/'

%%

prog
    : ID sent_decl_lista PR_BEGIN sent_ejec_lista PR_END ';'
      {
          registrar_arbol_programa($4);
          std::cout << "Se reconocio un programa" << std::endl;
      }
    | ID sent_decl_lista error sent_ejec_lista PR_END ';'
      {
          registrar_arbol_programa($4);
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: falta 'begin' o esta mal formada la seccion declarativa del programa."
                    << std::endl;
          yyerrok;
      }
    | ID sent_decl_lista PR_BEGIN sent_ejec_lista PR_END error
      {
          registrar_arbol_programa($4);
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
          ids_pendientes.clear();
          valores_pendientes.clear();
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
    : PR_USHORTINT { $$.t = Tipo::USHORTINT; $$.clase = nullptr; }
    | PR_DOUBLEF   { $$.t = Tipo::DOUBLEF;   $$.clase = nullptr; }
    | ID           { $$.t = Tipo::NINGUNO;   $$.clase = $1; }   /* clase o typedef: se resuelve mas adelante */
    ;

lista_ids
    : lista_ids ',' ID { ids_pendientes.push_back($3); }
    | ID               { ids_pendientes.push_back($1); }
    ;

decl_variables
    : tipo lista_ids ';'
      {
          for (EntradaTS *id : ids_pendientes) declarar(id, Uso::VARIABLE, $1);
          ids_pendientes.clear();
          std::cout << "Declaracion de variables (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
    | tipo error ';'
      {
          ids_pendientes.clear();
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: declaracion de variables mal formada."
                    << std::endl;
          yyerrok;
      }
    ;

lista_params_formales
    : lista_params_formales ',' tipo ID
      {
          declarar($4, Uso::PARAMETRO, $3);
          if (!funciones_abiertas.empty()) funciones_abiertas.back()->params.push_back($3.t);
      }
    | tipo ID
      {
          declarar($2, Uso::PARAMETRO, $1);
          if (!funciones_abiertas.empty()) funciones_abiertas.back()->params.push_back($1.t);
      }
    ;

cabecera_funcion
    : tipo PR_FUNCTION ID
      {
          EntradaTS *f = declarar($3, Uso::FUNCION, $1);
          f->tiene_ret = false;
          f->params.clear();
          funciones_abiertas.push_back(f);
          pila_ambitos.push_back($3->lexema);   // parametros y locales viven en el ambito de la funcion
          $$ = 1;
      }
    ;

decl_funcion
    : cabecera_funcion
      '(' lista_params_formales ')' sent_decl_lista
      PR_BEGIN sent_ejec_lista PR_END ';'
      {
          EntradaTS *func = funciones_abiertas.back();
          registrar_arbol_funcion(func->lexema, $7);
          funciones_abiertas.pop_back();
          pila_ambitos.pop_back();

          if (!func->tiene_ret) {
              std::cout << "Linea " << LINEA_ACTUAL
                        << ": Aviso: la funcion '" << nombre_base(func)
                        << "' no tiene sentencia 'ret'. Se asume valor de retorno por defecto ("
                        << valor_por_defecto(func->tipo) << ")." << std::endl;
          }
          std::cout << "Declaracion de funcion (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
    | cabecera_funcion error PR_END ';'
      {
          funciones_abiertas.pop_back();   // la cabecera se consumio en esta regla: no la destruye el %destructor
          pila_ambitos.pop_back();
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: declaracion de funcion mal formada."
                    << std::endl;
          yyerrok;
      }
    ;

cabecera_clase
    : PR_CLASS ID
      {
          EntradaTS *c = declarar($2, Uso::CLASE, SIN_TIPO);
          c->padres.clear();
          clases_abiertas.push_back(c);
          pila_ambitos.push_back($2->lexema);   // atributos y metodos viven en el ambito de la clase
          $$ = 1;
      }
    ;

decl_clase
    : cabecera_clase PR_BEGIN clase_item_lista PR_END ';'
      {
          clases_abiertas.pop_back();
          pila_ambitos.pop_back();
          std::cout << "Declaracion de clase (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
    | cabecera_clase error PR_END ';'
      {
          clases_abiertas.pop_back();
          pila_ambitos.pop_back();
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

cabecera_metodo
    : tipo ID
      {
          EntradaTS *m = declarar($2, Uso::METODO, $1);
          m->tiene_ret = false;
          m->params.clear();
          funciones_abiertas.push_back(m);
          pila_ambitos.push_back($2->lexema);
          $$ = 1;
      }
    ;

clase_item
    : tipo ID ';'
      {
          declarar($2, Uso::ATRIBUTO, $1);
      }
    | cabecera_metodo
      '(' lista_params_formales ')' sent_decl_lista
      PR_BEGIN sent_ejec_lista PR_END ';'
      {
          EntradaTS *metodo = funciones_abiertas.back();
          registrar_arbol_funcion(metodo->lexema, $7);
          funciones_abiertas.pop_back();
          pila_ambitos.pop_back();

          if (!metodo->tiene_ret) {
              std::cout << "Linea " << LINEA_ACTUAL
                        << ": Aviso: el metodo '" << nombre_base(metodo)
                        << "' no tiene sentencia 'ret'. Se asume valor de retorno por defecto ("
                        << valor_por_defecto(metodo->tipo) << ")." << std::endl;
          }
      }
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
          for (EntradaTS *id : ids_pendientes)
              if (!clases_abiertas.empty()) clases_abiertas.back()->padres.push_back(id->lexema);
          ids_pendientes.clear();
          std::cout << "Clausula EXTENDS (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
    | PR_EXTENDS error ';'
      {
          ids_pendientes.clear();
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: clausula 'extends' mal formada."
                    << std::endl;
          yyerrok;
      }
    ;

decl_typedef
    : PR_TYPEDEF ID '=' '[' lista_valores ']' ';'
      {
          EntradaTS *t = declarar($2, Uso::TYPEDEF, SIN_TIPO);
          t->valores = valores_pendientes;
          valores_pendientes.clear();
          std::cout << "Declaracion de typedef (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
    | PR_TYPEDEF error ';'
      {
          valores_pendientes.clear();
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: declaracion de typedef mal formada."
                    << std::endl;
          yyerrok;
      }
    ;

lista_valores
    : lista_valores ',' cte { valores_pendientes.push_back($3->lexema); }
    | cte                   { valores_pendientes.push_back($1->lexema); }
    ;

/* ---------- sentencias ejecutables ---------- */

sent_ejec_lista
    : sent_ejec_lista sent_ejec
      {
          $$ = crear_nodo_sentencias($1, $2);
      }
    | sent_ejec_lista error ';'
      {
          $$ = $1;
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: sentencia ejecutable mal formada. "
                    << "Se descartan tokens hasta ';'."
                    << std::endl;
          yyerrok;
      }
    |
      {
          $$ = nullptr;
      }
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
          EntradaTS *res = resolver_valor($1);
          NodoArbol *hoja_id = crear_hoja(res);
          NodoArbol *exp = $3;
          // Tema 35: Asignación entero a flotante se convierte el lado derecho
          if (hoja_id && exp && hoja_id->tipo_dato == Tipo::DOUBLEF && exp->tipo_dato == Tipo::USHORTINT) {
              exp = crear_nodo_conversion(exp, Tipo::DOUBLEF);
          }
          $$ = crear_nodo_asignacion(":=", hoja_id, exp);
          std::cout << "Asignacion (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
    | ID error ';'
      {
          $$ = nullptr;
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: asignacion mal formada."
                    << std::endl;
          yyerrok;
      }
    ;

asignacion_atributo
    : ID '.' ID '=' expresion ';'
      {
          EntradaTS *res_obj = resolver_valor($1);
          NodoArbol *obj = crear_hoja(res_obj);
          NodoArbol *atr = crear_hoja($3);
          NodoArbol *acceso = crear_nodo_acceso(obj, atr);
          $$ = crear_nodo_asignacion(":=", acceso, $5);
          std::cout << "Asignacion de atributo (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
    | ID '[' indice ']' '=' expresion ';'
      {
          EntradaTS *res_obj = resolver_valor($1);
          NodoArbol *obj = crear_hoja(res_obj);
          NodoArbol *acceso_pos = crear_nodo_acceso_posicional(obj, $3);
          $$ = crear_nodo_asignacion(":=", acceso_pos, $6);
          std::cout << "Asignacion posicional de atributo (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
    | ID '.' ID error ';'
      {
          $$ = nullptr;
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: asignacion de atributo mal formada."
                    << std::endl;
          yyerrok;
      }
    | ID '[' error ';'
      {
          $$ = nullptr;
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: asignacion posicional mal formada."
                    << std::endl;
          yyerrok;
      }
    ;

indice
    : CTE_INT { $$ = crear_hoja($1); }
    | ID      { $$ = crear_hoja(resolver_valor($1)); }
    ;

retorno 
    : PR_RET '(' expresion ')' ';'
      {
          if (!funciones_abiertas.empty()) {
              funciones_abiertas.back()->tiene_ret = true;
          }
          $$ = crear_nodo("ret", $3, nullptr, TipoNodo::RETORNO);
          std::cout << "Sentencia RET (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
    | PR_RET error ';'
      {
          $$ = nullptr;
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: sentencia 'ret' mal formada."
                    << std::endl;
          yyerrok;
      }
    ;

pout
    : PR_POUT '(' CTE_STR ')' ';'
      {
          $$ = crear_nodo("pout", crear_hoja($3), nullptr, TipoNodo::SALIDA);
          std::cout << "Sentencia POUT (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
    | PR_POUT '(' expresion ')' ';'
      {
          $$ = crear_nodo("pout", $3, nullptr, TipoNodo::SALIDA);
          std::cout << "Sentencia POUT (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
    | PR_POUT error ';'
      {
          $$ = nullptr;
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: sentencia 'pout' mal formada."
                    << std::endl;
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
          NodoArbol *izq = $1;
          NodoArbol *der = $3;
          aplicar_conversion_implicita(izq, der);
          $$ = crear_nodo($2, izq, der, TipoNodo::OPERADOR);
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
          std::cout << "Sentencia IF (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
    | PR_IF '(' condicion ')' bloque_sent_ejec
      PR_ELSE bloque_sent_ejec PR_END_IF ';'
      {
          $$ = crear_nodo_if($3, $5, $7);
          std::cout << "Sentencia IF-ELSE (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
    | PR_IF error ';'
      {
          $$ = nullptr;
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
          $$ = crear_nodo_repeat_while($2, $5);
          std::cout << "Sentencia REPEAT-WHILE (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
    | PR_REPEAT error ';'
      {
          $$ = nullptr;
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: sentencia 'repeat-while' mal formada."
                    << std::endl;
          yyerrok;
      }
    ;

/* ---------- expresiones ---------- */

expresion
    : expresion '+' termino
      {
          NodoArbol *izq = $1;
          NodoArbol *der = $3;
          Tipo res = aplicar_conversion_implicita(izq, der);
          $$ = crear_nodo("+", izq, der, TipoNodo::OPERADOR);
          $$->tipo_dato = res;
      }
    | expresion '-' termino
      {
          NodoArbol *izq = $1;
          NodoArbol *der = $3;
          Tipo res = aplicar_conversion_implicita(izq, der);
          $$ = crear_nodo("-", izq, der, TipoNodo::OPERADOR);
          $$->tipo_dato = res;
      }
    | termino
      {
          $$ = $1;
      }
    ;

termino
    : termino '*' factor
      {
          NodoArbol *izq = $1;
          NodoArbol *der = $3;
          Tipo res = aplicar_conversion_implicita(izq, der);
          $$ = crear_nodo("*", izq, der, TipoNodo::OPERADOR);
          $$->tipo_dato = res;
      }
    | termino '/' factor
      {
          NodoArbol *izq = $1;
          NodoArbol *der = $3;
          Tipo res = aplicar_conversion_implicita(izq, der);
          $$ = crear_nodo("/", izq, der, TipoNodo::OPERADOR);
          $$->tipo_dato = res;
      }
    | factor
      {
          $$ = $1;
      }
    ;

factor
    : ID
      {
          $$ = crear_hoja(resolver_valor($1));
      }
    | cte
      {
          $$ = crear_hoja($1);
      }
    | '-' CTE_INT
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error semantico: la constante '"
                    << $2->lexema
                    << "' es de tipo USHORTINT (sin signo), "
                    << "no admite el signo '-'."
                    << std::endl;

          $$ = crear_hoja($2);
      }
    | '-' CTE_FLOAT
      {
          // El rango ya lo valido el lexico (es simetrico), solo falta registrar la constante negativa
          std::string lexema_neg = "-" + $2->lexema;

          auto it = tabla_simbolos.find(lexema_neg);

          if (it == tabla_simbolos.end()) {
              EntradaTS nueva_entrada;
              nueva_entrada.lexema = lexema_neg;
              nueva_entrada.tipo = Tipo::DOUBLEF;
              nueva_entrada.uso  = Uso::CONSTANTE;

              it = tabla_simbolos.insert(
                  {lexema_neg, nueva_entrada}
              ).first;
          }

          $$ = crear_hoja(&(it->second));
      }
    | invocacion
      {
          $$ = $1;
      }
    | acceso_objeto
      {
          $$ = $1;
      }
    | ID '=' '(' expresion_restr ')'
      {
          EntradaTS *res_id = resolver_valor($1);
          NodoArbol *hoja_id = crear_hoja(res_id);
          NodoArbol *exp = $4;
          // Tema 35: Si el destino es doublef y la expresión es ushortint, convertir implícitamente
          if (hoja_id && exp && hoja_id->tipo_dato == Tipo::DOUBLEF && exp->tipo_dato == Tipo::USHORTINT) {
              exp = crear_nodo_conversion(exp, Tipo::DOUBLEF);
          }
          $$ = crear_nodo_asignacion("=", hoja_id, exp);
      }
    ;

expresion_restr
    : expresion_restr '+' termino_restr
      {
          NodoArbol *izq = $1;
          NodoArbol *der = $3;
          Tipo res = aplicar_conversion_implicita(izq, der);
          $$ = crear_nodo("+", izq, der, TipoNodo::OPERADOR);
          $$->tipo_dato = res;
      }
    | expresion_restr '-' termino_restr
      {
          NodoArbol *izq = $1;
          NodoArbol *der = $3;
          Tipo res = aplicar_conversion_implicita(izq, der);
          $$ = crear_nodo("-", izq, der, TipoNodo::OPERADOR);
          $$->tipo_dato = res;
      }
    | termino_restr
      {
          $$ = $1;
      }
    ;

termino_restr
    : termino_restr '*' factor_restr
      {
          NodoArbol *izq = $1;
          NodoArbol *der = $3;
          Tipo res = aplicar_conversion_implicita(izq, der);
          $$ = crear_nodo("*", izq, der, TipoNodo::OPERADOR);
          $$->tipo_dato = res;
      }
    | termino_restr '/' factor_restr
      {
          NodoArbol *izq = $1;
          NodoArbol *der = $3;
          Tipo res = aplicar_conversion_implicita(izq, der);
          $$ = crear_nodo("/", izq, der, TipoNodo::OPERADOR);
          $$->tipo_dato = res;
      }
    | factor_restr
      {
          $$ = $1;
      }
    ;

factor_restr
    : ID
      {
          $$ = crear_hoja(resolver_valor($1));
      }
    | cte
      {
          $$ = crear_hoja($1);
      }
    | '-' CTE_INT
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error semantico: la constante '"
                    << $2->lexema
                    << "' es de tipo USHORTINT (sin signo), "
                    << "no admite el signo '-'."
                    << std::endl;

          $$ = crear_hoja($2);
      }
    | '-' CTE_FLOAT
      {
          std::string lexema_neg = "-" + $2->lexema;

          auto it = tabla_simbolos.find(lexema_neg);

          if (it == tabla_simbolos.end()) {
              EntradaTS nueva_entrada;
              nueva_entrada.lexema = lexema_neg;
              nueva_entrada.tipo = Tipo::DOUBLEF;
              nueva_entrada.uso  = Uso::CONSTANTE;

              it = tabla_simbolos.insert(
                  {lexema_neg, nueva_entrada}
              ).first;
          }

          $$ = crear_hoja(&(it->second));
      }
    | invocacion
      {
          $$ = $1;
      }
    | acceso_objeto
      {
          $$ = $1;
      }
    ;

cte
    : CTE_INT   { $$ = $1; }
    | CTE_FLOAT { $$ = $1; }
    ;

invocacion
    : ID '(' lista_params_reales ')' lista_ctes_opcional
      {
          std::vector<NodoArbol*> args = pila_params_reales.back();
          pila_params_reales.pop_back();

          std::vector<int> ord = pila_orden_eval.back();
          pila_orden_eval.pop_back();

          EntradaTS *f = resolver_valor($1);
          $$ = crear_nodo_llamada(f, args, ord);
      }
    | ID '(' error ')' lista_ctes_opcional
      {
          if (!pila_params_reales.empty()) pila_params_reales.pop_back();
          if (!pila_orden_eval.empty()) pila_orden_eval.pop_back();
          $$ = nullptr;
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: invocacion a funcion mal formada."
                    << std::endl;
          yyerrok;
      }
    ;

lista_params_reales
    : lista_params_reales ',' expresion
      {
          pila_params_reales.back().push_back($3);
      }
    | expresion
      {
          pila_params_reales.push_back({ $1 });
      }
    ;

lista_ctes_opcional
    : '[' lista_constantes ']'
    | '[' error ']'
      {
          pila_orden_eval.push_back({});
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: lista de orden de evaluacion mal formada."
                    << std::endl;
          yyerrok;
      }
    |
      {
          pila_orden_eval.push_back({});
      }
    ;

lista_constantes
    : lista_constantes ',' CTE_INT
      {
          try {
              std::string s = $3->lexema;
              size_t p = s.find('$');
              if (p != std::string::npos) s = s.substr(0, p);
              pila_orden_eval.back().push_back(std::stoi(s));
          } catch (...) {}
      }
    | CTE_INT
      {
          std::vector<int> nuevo_orden;
          try {
              std::string s = $1->lexema;
              size_t p = s.find('$');
              if (p != std::string::npos) s = s.substr(0, p);
              nuevo_orden.push_back(std::stoi(s));
          } catch (...) {}
          pila_orden_eval.push_back(nuevo_orden);
      }
    ;

acceso_objeto
    : ID '.' ID llamada_opcional
      {
          EntradaTS *obj_ts = resolver_valor($1);
          NodoArbol *obj = crear_hoja(obj_ts);
          NodoArbol *atr = crear_hoja($3);
          $$ = crear_nodo_acceso(obj, atr);
      }
    | ID '[' indice ']'
      {
          EntradaTS *obj_ts = resolver_valor($1);
          NodoArbol *obj = crear_hoja(obj_ts);
          $$ = crear_nodo_acceso_posicional(obj, $3);
      }
    ;

llamada_opcional
    : '(' lista_params_reales ')'
      {
          if (!pila_params_reales.empty()) pila_params_reales.pop_back();
      }
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
