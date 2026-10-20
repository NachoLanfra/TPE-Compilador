%define parse.error verbose
%{
#include <iostream>
#include "main.h"
int yylex();
void yyerror(const char *s);

static const InfoTipo SIN_TIPO = { Tipo::NINGUNO, nullptr };

// Pila de ámbitos: main, main:f, main:f:g ... (base del name mangling)
static std::vector<std::string> pila_ambitos = { "main" };
static std::vector<EntradaTS*> funciones_abiertas;  // funciones/métodos en curso (hay anidamiento)
static std::vector<EntradaTS*> clases_abiertas;     // clases en curso
static std::vector<EntradaTS*> ids_pendientes;      // lista_ids en construcción
static std::vector<std::string> valores_pendientes; // lista_valores (typedef) en construcción

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

static const char* nombre_uso(Uso u) {
    switch (u) {
        case Uso::VARIABLE:  return "variable";
        case Uso::PARAMETRO: return "parametro";
        case Uso::FUNCION:   return "funcion";
        case Uso::CLASE:     return "clase";
        case Uso::ATRIBUTO:  return "atributo";
        case Uso::METODO:    return "metodo";
        case Uso::OBJETO:    return "objeto";
        case Uso::TYPEDEF:   return "tipo definido (typedef)";
        case Uso::PROGRAMA:  return "nombre de programa";
        case Uso::CONSTANTE: return "constante";
        case Uso::CADENA:    return "cadena";
        default:             return "simbolo";
    }
}

// Busca 'nombre' desde el ámbito actual hacia afuera: nombre:main:f:g, nombre:main:f, nombre:main
static EntradaTS* buscar(const std::string &nombre) {
    for (size_t n = pila_ambitos.size(); n > 0; n--) {
        std::string amb;
        for (size_t i = 0; i < n; i++) {
            if (i) amb += ":";
            amb += pila_ambitos[i];
        }
        auto it = tabla_simbolos.find(nombre + ":" + amb);
        if (it != tabla_simbolos.end()) return &(it->second);
    }
    return nullptr;
}

// Todo uso de un identificador pasa por acá. Devuelve nullptr (y informa) si no es visible.
static EntradaTS* resolver(EntradaTS *raw) {
    EntradaTS *e = buscar(raw->lexema);
    if (!e)
        std::cerr << "Linea " << LINEA_ACTUAL << ": Error: '" << raw->lexema
                  << "' no esta declarado o no es visible en el ambito " << ambito_actual() << std::endl;
    return e;
}

// Identificador usado como variable (lado izquierdo de una asignacion, operando, indice)
static EntradaTS* resolver_valor(EntradaTS *raw) {
    EntradaTS *e = resolver(raw);
    if (e && e->uso != Uso::VARIABLE && e->uso != Uso::PARAMETRO && e->uso != Uso::ATRIBUTO) {
        std::cerr << "Linea " << LINEA_ACTUAL << ": Error: '" << raw->lexema << "' es "
                  << nombre_uso(e->uso) << " y se usa como variable" << std::endl;
        return nullptr;
    }
    return e;
}

// Identificador usado como funcion o metodo (invocacion)
static EntradaTS* resolver_funcion(EntradaTS *raw) {
    EntradaTS *e = resolver(raw);
    if (e && e->uso != Uso::FUNCION && e->uso != Uso::METODO) {
        std::cerr << "Linea " << LINEA_ACTUAL << ": Error: '" << raw->lexema << "' es "
                  << nombre_uso(e->uso) << " y se usa como funcion" << std::endl;
        return nullptr;
    }
    return e;
}

// Identificador usado como tipo (declaracion de variables, parametros, retornos)
static EntradaTS* resolver_tipo(EntradaTS *raw) {
    EntradaTS *e = resolver(raw);
    if (e && e->uso != Uso::CLASE && e->uso != Uso::TYPEDEF) {
        std::cerr << "Linea " << LINEA_ACTUAL << ": Error: '" << raw->lexema << "' es "
                  << nombre_uso(e->uso) << " y se usa como tipo" << std::endl;
        return nullptr;
    }
    return e;
}

// Identificador a la izquierda de '.' o '[' : tiene que ser una variable cuyo tipo sea una clase
static EntradaTS* resolver_objeto(EntradaTS *raw) {
    EntradaTS *e = resolver_valor(raw);
    if (!e) return nullptr;
    bool es_objeto = false;
    if (!e->tipo_nombre.empty()) {
        auto it = tabla_simbolos.find(e->tipo_nombre);
        es_objeto = (it != tabla_simbolos.end() && it->second.uso == Uso::CLASE);
    }
    bool tipo_roto = (e->tipo == Tipo::NINGUNO && e->tipo_nombre.empty());  // ya se informo el error en su declaracion
    if (!es_objeto && !tipo_roto) {
        std::cerr << "Linea " << LINEA_ACTUAL << ": Error: '" << raw->lexema
                  << "' no es un objeto de una clase" << std::endl;
        return nullptr;
    }
    return e;
}

%}

%union {
    EntradaTS *ts_ref;
    InfoTipo info_tipo;
    int marca_f;   // marcador de función/método abierto
    int marca_c;   // marcador de clase abierta
}

%token <ts_ref> ID CTE_INT CTE_FLOAT CTE_STR
%type <info_tipo> tipo
%type <marca_f> cabecera_funcion cabecera_metodo
%type <marca_c> cabecera_clase
%type <ts_ref> cte factor factor_restr invocacion acceso_objeto

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
    : nombre_prog sent_decl_lista PR_BEGIN sent_ejec_lista PR_END ';'
      {
          std::cout << "Se reconocio un programa" << std::endl;
      }
    | nombre_prog sent_decl_lista error sent_ejec_lista PR_END ';'
      {
          std::cerr << "Linea " << LINEA_ACTUAL
                    << ": Error: falta 'begin' o esta mal formada la seccion declarativa del programa."
                    << std::endl;
          yyerrok;
      }
    | nombre_prog sent_decl_lista PR_BEGIN sent_ejec_lista PR_END error
      {
      	  std::cerr << "Linea " << LINEA_ACTUAL
      	            << ": Error: falta ';' despues del 'end' del programa."
      	            << std::endl;
      	  yyerrok;
      }
    ;

nombre_prog
    : ID { declarar($1, Uso::PROGRAMA, SIN_TIPO); }
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
    | ID           { $$.t = Tipo::NINGUNO;   $$.clase = resolver_tipo($1); }   /* clase o typedef: debe estar declarado */
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
          for (EntradaTS *id : ids_pendientes) {
              EntradaTS *padre = resolver(id);
              if (!padre || clases_abiertas.empty()) continue;
              if (padre->uso != Uso::CLASE)
                  std::cerr << "Linea " << LINEA_ACTUAL << ": Error: '" << id->lexema << "' es "
                            << nombre_uso(padre->uso) << " y se usa como clase en 'extends'" << std::endl;
              else if (padre == clases_abiertas.back())
                  std::cerr << "Linea " << LINEA_ACTUAL << ": Error: la clase '" << id->lexema
                            << "' no puede heredar de si misma" << std::endl;
              else
                  clases_abiertas.back()->padres.push_back(padre->lexema);
          }
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
      	  resolver_valor($1);
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
      	  resolver_objeto($1);
          std::cout << "Asignacion de atributo (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
    | ID '[' indice ']' '=' expresion ';'
      {
      	  resolver_objeto($1);
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
    | ID { resolver_valor($1); }
    ;

retorno 
    : PR_RET '(' expresion ')' ';'
      {
          if (!funciones_abiertas.empty()) {
              funciones_abiertas.back()->tiene_ret = true;
          }
          std::cout << "Sentencia RET (linea "
                    << LINEA_ACTUAL << ")" << std::endl;
      }
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
    : ID { $$ = resolver_valor($1); }
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

          $$ = &(it->second);
      }
    | invocacion
    | acceso_objeto
    | ID '=' '(' expresion_restr ')' { $$ = resolver_valor($1); }
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
    : ID { $$ = resolver_valor($1); }
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
              nueva_entrada.tipo = Tipo::DOUBLEF;
              nueva_entrada.uso  = Uso::CONSTANTE;

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
    : ID '(' lista_params_reales ')' lista_ctes_opcional { $$ = resolver_funcion($1); }
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
    : ID '.' ID llamada_opcional { $$ = resolver_funcion($1); }
    | ID '[' indice ']' { $$ = resolver_funcion($1); }
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
