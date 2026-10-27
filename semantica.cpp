#include "semantica.h"
#include <iostream>

const InfoTipo SIN_TIPO = { Tipo::NINGUNO, nullptr };

// Pilas de ámbitos y estado
static std::vector<std::string> pila_ambitos = { "main" };
static std::vector<EntradaTS*> funciones_abiertas;
static std::vector<EntradaTS*> clases_abiertas;
static std::vector<EntradaTS*> ids_pendientes;
static std::vector<std::string> valores_pendientes;

// Tema 20: Pilas para llamadas y orden de evaluación (maneja anidamiento f(g(x)))
static std::vector<std::vector<NodoArbol*>> pila_params_reales;
static std::vector<std::vector<int>> pila_orden_eval;

std::string ambito_actual() {
    std::string r;
    for (size_t i = 0; i < pila_ambitos.size(); i++) {
        if (i) r += ":";
        r += pila_ambitos[i];
    }
    return r;
}

std::string nombre_base(const EntradaTS *e) {
    if (!e) return "";
    return e->lexema.substr(0, e->lexema.find(':'));
}

std::string valor_por_defecto(Tipo tipo) {
    switch (tipo) {
        case Tipo::USHORTINT: return "0";
        case Tipo::DOUBLEF:   return "0.0";
        default:              return "0";
    }
}

const char* nombre_uso(Uso u) {
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

EntradaTS* declarar(EntradaTS *raw, Uso uso, const InfoTipo &t) {
    if (!raw) return nullptr;
    std::string clave = raw->lexema + ":" + ambito_actual();
    auto it = tabla_simbolos.find(clave);
    if (it != tabla_simbolos.end()) {
        error("Semantico", LINEA_ACTUAL, "'" + raw->lexema + "' ya fue declarado en el ambito " + ambito_actual());
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

void sem_declarar_variables(const InfoTipo &tipo) {
    for (EntradaTS *id : ids_pendientes) declarar(id, Uso::VARIABLE, tipo);
    ids_pendientes.clear();
    #ifdef DEBUG
    std::cout << "Declaracion de variables (linea " << LINEA_ACTUAL << ")" << std::endl;
    #endif
}

void sem_declarar_programa(EntradaTS *id) {
    declarar(id, Uso::PROGRAMA, SIN_TIPO);
}

void sem_abrir_funcion(EntradaTS *id, const InfoTipo &tipo) {
    EntradaTS *f = declarar(id, Uso::FUNCION, tipo);
    f->tiene_ret = false;
    f->params_formales.clear();
    funciones_abiertas.push_back(f);
    pila_ambitos.push_back(id->lexema);
}

void sem_cerrar_funcion(NodoArbol *cuerpo) {
    EntradaTS *func = funciones_abiertas.back();
    registrar_arbol_funcion(func->lexema, cuerpo);
    funciones_abiertas.pop_back();
    pila_ambitos.pop_back();

    #ifdef DEBUG
    if (!func->tiene_ret) {
        std::cout << "Linea " << LINEA_ACTUAL
                  << ": Aviso: la funcion '" << nombre_base(func)
                  << "' no tiene sentencia 'ret'. Se asume valor de retorno por defecto ("
                  << valor_por_defecto(func->tipo) << ")." << std::endl;
    }
    std::cout << "Declaracion de funcion (linea " << LINEA_ACTUAL << ")" << std::endl;
    #endif
}

void sem_destruir_cabecera_funcion() {
    funciones_abiertas.pop_back();
    pila_ambitos.pop_back();
}

void sem_abrir_clase(EntradaTS *id) {
    EntradaTS *c = declarar(id, Uso::CLASE, SIN_TIPO);
    c->padres.clear();
    clases_abiertas.push_back(c);
    pila_ambitos.push_back(id->lexema);
}

void sem_cerrar_clase() {
    clases_abiertas.pop_back();
    pila_ambitos.pop_back();
    #ifdef DEBUG
    std::cout << "Declaracion de clase (linea " << LINEA_ACTUAL << ")" << std::endl;
    #endif
}

void sem_destruir_cabecera_clase() {
    clases_abiertas.pop_back();
    pila_ambitos.pop_back();
}

void sem_declarar_atributo(EntradaTS *id, const InfoTipo &tipo) {
    EntradaTS *atr = declarar(id, Uso::ATRIBUTO, tipo);
    if (atr && !clases_abiertas.empty()) {
        clases_abiertas.back()->atributos.push_back(atr);
    }
}

void sem_abrir_metodo(EntradaTS *id, const InfoTipo &tipo) {
    EntradaTS *m = declarar(id, Uso::METODO, tipo);
    m->tiene_ret = false;
    m->params_formales.clear();
    funciones_abiertas.push_back(m);
    pila_ambitos.push_back(id->lexema);
    if (!clases_abiertas.empty()) {
        clases_abiertas.back()->metodos.push_back(m);
    }
}

void sem_cerrar_metodo(NodoArbol *cuerpo) {
    EntradaTS *metodo = funciones_abiertas.back();
    registrar_arbol_funcion(metodo->lexema, cuerpo);
    funciones_abiertas.pop_back();
    pila_ambitos.pop_back();

    #ifdef DEBUG
    if (!metodo->tiene_ret) {
        std::cout << "Linea " << LINEA_ACTUAL
                  << ": Aviso: el metodo '" << nombre_base(metodo)
                  << "' no tiene sentencia 'ret'. Se asume valor de retorno por defecto ("
                  << valor_por_defecto(metodo->tipo) << ")." << std::endl;
    }
    #endif
}

void sem_extends() {
    for (EntradaTS *id : ids_pendientes) {
        EntradaTS *padre = resolver(id);
        if (!padre || clases_abiertas.empty()) continue;
        if (padre->uso != Uso::CLASE)
            error("Semantico", LINEA_ACTUAL, "'" + id->lexema + "' es " + nombre_uso(padre->uso) + " y se usa como clase en 'extends'");
        else if (padre == clases_abiertas.back())
            error("Semantico", LINEA_ACTUAL, "la clase '" + id->lexema + "' no puede heredar de si misma");
        else
            clases_abiertas.back()->padres.push_back(padre->lexema);
    }
    ids_pendientes.clear();
    #ifdef DEBUG
    std::cout << "Clausula EXTENDS (linea " << LINEA_ACTUAL << ")" << std::endl;
    #endif
}

void sem_typedef(EntradaTS *id) {
    EntradaTS *t = declarar(id, Uso::TYPEDEF, SIN_TIPO);
    t->valores = valores_pendientes;
    valores_pendientes.clear();
    #ifdef DEBUG
    std::cout << "Declaracion de typedef (linea " << LINEA_ACTUAL << ")" << std::endl;
    #endif
}

void sem_agregar_param_formal(EntradaTS *id, const InfoTipo &tipo) {
    EntradaTS *p = declarar(id, Uso::PARAMETRO, tipo);
    if (!funciones_abiertas.empty()) {
        funciones_abiertas.back()->params_formales.push_back(p);
    }
}

void sem_agregar_id_pendiente(EntradaTS *id) {
    ids_pendientes.push_back(id);
}

void sem_limpiar_ids_pendientes() {
    ids_pendientes.clear();
}

void sem_agregar_valor_pendiente(const std::string &val) {
    valores_pendientes.push_back(val);
}

void sem_limpiar_valores_pendientes() {
    valores_pendientes.clear();
}

// Búsqueda léxica hacia afuera
EntradaTS* buscar(const std::string &nombre) {
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

EntradaTS* resolver(EntradaTS *raw) {
    if (!raw) return nullptr;
    EntradaTS *e = buscar(raw->lexema);
    if (!e)
        error("Semantico", LINEA_ACTUAL, "'" + raw->lexema + "' no esta declarado o no es visible en el ambito " + ambito_actual());
    return e;
}

EntradaTS* resolver_valor(EntradaTS *raw) {
    EntradaTS *e = resolver(raw);
    if (e && e->uso != Uso::VARIABLE && e->uso != Uso::PARAMETRO && e->uso != Uso::ATRIBUTO) {
        error("Semantico", LINEA_ACTUAL, "'" + raw->lexema + "' es " + nombre_uso(e->uso) + " y se usa como variable");
        return nullptr;
    }
    return e;
}

EntradaTS* resolver_funcion(EntradaTS *raw) {
    EntradaTS *e = resolver(raw);
    if (e && e->uso != Uso::FUNCION && e->uso != Uso::METODO) {
        error("Semantico", LINEA_ACTUAL, "'" + raw->lexema + "' es " + nombre_uso(e->uso) + " y se usa como funcion");
        return nullptr;
    }
    return e;
}

EntradaTS* resolver_tipo(EntradaTS *raw) {
    EntradaTS *e = resolver(raw);
    if (e && e->uso != Uso::CLASE && e->uso != Uso::TYPEDEF) {
        error("Semantico", LINEA_ACTUAL, "'" + raw->lexema + "' es " + nombre_uso(e->uso) + " y se usa como tipo");
        return nullptr;
    }
    return e;
}

EntradaTS* resolver_objeto(EntradaTS *raw) {
    EntradaTS *e = resolver_valor(raw);
    if (!e) return nullptr;
    bool es_objeto = false;
    if (!e->tipo_nombre.empty()) {
        auto it = tabla_simbolos.find(e->tipo_nombre);
        es_objeto = (it != tabla_simbolos.end() && it->second.uso == Uso::CLASE);
    }
    bool tipo_roto = (e->tipo == Tipo::NINGUNO && e->tipo_nombre.empty());
    if (!es_objeto && !tipo_roto) {
        error("Semantico", LINEA_ACTUAL, "'" + raw->lexema + "' no es un objeto de una clase y no admite acceso a miembros");
        return nullptr;
    }
    return e;
}

// Asignaciones
NodoArbol* sem_asignacion(EntradaTS *id, NodoArbol *exp) {
    EntradaTS *res = resolver_valor(id);
    NodoArbol *hoja_id = crear_hoja(res);
    // Tema 35: Asignación entero a flotante se convierte el lado derecho
    if (hoja_id && exp && hoja_id->tipo.t == Tipo::DOUBLEF && exp->tipo.t == Tipo::USHORTINT) {
        exp = crear_nodo_conversion(exp, Tipo::DOUBLEF);
    }
    #ifdef DEBUG
    std::cout << "Asignacion (linea " << LINEA_ACTUAL << ")" << std::endl;
    #endif
    return crear_nodo_asignacion(":=", hoja_id, exp);
}

// Tema 17: Asignación dentro de expresión
NodoArbol* sem_asignacion_expr(EntradaTS *id, NodoArbol *exp) {
    EntradaTS *res_id = resolver_valor(id);
    NodoArbol *hoja_id = crear_hoja(res_id);
    // Tema 35: Si el destino es doublef y la expresión es ushortint, convertir implícitamente
    if (hoja_id && exp && hoja_id->tipo.t == Tipo::DOUBLEF && exp->tipo.t == Tipo::USHORTINT) {
        exp = crear_nodo_conversion(exp, Tipo::DOUBLEF);
    }
    return crear_nodo_asignacion("=", hoja_id, exp);
}

NodoArbol* sem_asignacion_atributo(EntradaTS *id_obj, EntradaTS *id_atr, NodoArbol *exp) {
    EntradaTS *res_obj = resolver_objeto(id_obj);
    NodoArbol *obj = crear_hoja(res_obj);
    NodoArbol *atr = crear_hoja(id_atr);
    NodoArbol *acceso = crear_nodo_acceso(obj, atr);
    #ifdef DEBUG
    std::cout << "Asignacion de atributo (linea " << LINEA_ACTUAL << ")" << std::endl;
    #endif
    return crear_nodo_asignacion(":=", acceso, exp);
}

// Tema 29: Acceso posicional en asignación
NodoArbol* sem_asignacion_posicional(EntradaTS *id_obj, NodoArbol *indice, NodoArbol *exp) {
    EntradaTS *res_obj = resolver_objeto(id_obj);
    NodoArbol *obj = crear_hoja(res_obj);
    NodoArbol *acceso_pos = crear_nodo_acceso_posicional(obj, indice);
    #ifdef DEBUG
    std::cout << "Asignacion posicional de atributo (linea " << LINEA_ACTUAL << ")" << std::endl;
    #endif
    return crear_nodo_asignacion(":=", acceso_pos, exp);
}

NodoArbol* sem_retorno(NodoArbol *exp) {
    if (!funciones_abiertas.empty()) {
        funciones_abiertas.back()->tiene_ret = true;
    }
    #ifdef DEBUG
    std::cout << "Sentencia RET (linea " << LINEA_ACTUAL << ")" << std::endl;
    #endif
    return crear_nodo("ret", exp, nullptr, TipoNodo::RETORNO);
}

NodoArbol* sem_pout_str(EntradaTS *cte_str) {
    #ifdef DEBUG
    std::cout << "Sentencia POUT (linea " << LINEA_ACTUAL << ")" << std::endl;
    #endif
    return crear_nodo("pout", crear_hoja(cte_str), nullptr, TipoNodo::SALIDA);
}

NodoArbol* sem_pout_exp(NodoArbol *exp) {
    #ifdef DEBUG
    std::cout << "Sentencia POUT (linea " << LINEA_ACTUAL << ")" << std::endl;
    #endif
    return crear_nodo("pout", exp, nullptr, TipoNodo::SALIDA);
}

// Operaciones binarias con conversiones implícitas (Tema 35)
NodoArbol* sem_op_binaria(const std::string &op, NodoArbol *izq, NodoArbol *der) {
    Tipo res = aplicar_conversion_implicita(izq, der);
    NodoArbol *nodo = crear_nodo(op, izq, der, TipoNodo::OPERADOR);
    nodo->tipo = { res, nullptr };
    return nodo;
}

NodoArbol* sem_condicion(const char *op, NodoArbol *izq, NodoArbol *der) {
    aplicar_conversion_implicita(izq, der);
    return crear_nodo(op, izq, der, TipoNodo::OPERADOR);
}

NodoArbol* sem_factor_id(EntradaTS *id) {
    return crear_hoja(resolver_valor(id));
}

NodoArbol* sem_factor_cte(EntradaTS *cte) {
    return crear_hoja(cte);
}

NodoArbol* sem_factor_cte_neg_int(EntradaTS *cte) {
    error("Semantico", LINEA_ACTUAL, "la constante '" + cte->lexema + "' es de tipo USHORTINT (sin signo), no admite el signo '-'");
    return crear_hoja(cte);
}

NodoArbol* sem_factor_cte_neg_float(EntradaTS *cte) {
    std::string lexema_neg = "-" + cte->lexema;
    auto it = tabla_simbolos.find(lexema_neg);
    if (it == tabla_simbolos.end()) {
        EntradaTS nueva_entrada;
        nueva_entrada.lexema = lexema_neg;
        nueva_entrada.tipo = Tipo::DOUBLEF;
        nueva_entrada.uso  = Uso::CONSTANTE;
        it = tabla_simbolos.insert({lexema_neg, nueva_entrada}).first;
    }
    return crear_hoja(&(it->second));
}

// Invocaciones a funciones y orden de evaluación (Tema 20)
void sem_invocacion_nuevo_param(NodoArbol *param) {
    pila_params_reales.back().push_back(param);
}

void sem_invocacion_iniciar_params(NodoArbol *param) {
    pila_params_reales.push_back({ param });
}

void sem_invocacion_agregar_orden(EntradaTS *cte) {
    try {
        std::string s = cte->lexema;
        size_t p = s.find('$');
        if (p != std::string::npos) s = s.substr(0, p);
        pila_orden_eval.back().push_back(std::stoi(s));
    } catch (...) {}
}

void sem_invocacion_iniciar_orden(EntradaTS *cte) {
    std::vector<int> nuevo_orden;
    try {
        std::string s = cte->lexema;
        size_t p = s.find('$');
        if (p != std::string::npos) s = s.substr(0, p);
        nuevo_orden.push_back(std::stoi(s));
    } catch (...) {}
    pila_orden_eval.push_back(nuevo_orden);
}

void sem_invocacion_orden_vacio() {
    pila_orden_eval.push_back({});
}

NodoArbol* sem_invocacion(EntradaTS *id) {
    std::vector<NodoArbol*> args = pila_params_reales.back();
    pila_params_reales.pop_back();

    std::vector<int> ord = pila_orden_eval.back();
    pila_orden_eval.pop_back();

    EntradaTS *f = resolver_funcion(id);
    return crear_nodo_llamada(f, args, ord);
}

void sem_invocacion_error() {
    if (!pila_params_reales.empty()) pila_params_reales.pop_back();
    if (!pila_orden_eval.empty()) pila_orden_eval.pop_back();
}

NodoArbol* sem_acceso_atributo(EntradaTS *id_obj, EntradaTS *id_atr) {
    EntradaTS *obj_ts = resolver_objeto(id_obj);
    NodoArbol *obj = crear_hoja(obj_ts);
    NodoArbol *atr = crear_hoja(id_atr);
    return crear_nodo_acceso(obj, atr);
}

NodoArbol* sem_acceso_posicional(EntradaTS *id_obj, NodoArbol *indice) {
    EntradaTS *obj_ts = resolver_objeto(id_obj);
    NodoArbol *obj = crear_hoja(obj_ts);
    return crear_nodo_acceso_posicional(obj, indice);
}

void sem_llamada_opcional_pop() {
    if (!pila_params_reales.empty()) pila_params_reales.pop_back();
}

