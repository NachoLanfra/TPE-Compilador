#include "semantica.h"
#include "errores.h"
#include <algorithm>
#include <memory>

const InfoTipo SIN_TIPO = { Tipo::NINGUNO, nullptr };

 // Estado Global del Análisis Semántico
static std::vector<std::string> pila_ambitos = { "main" };
static std::vector<EntradaTS*> funciones_abiertas;
static std::vector<EntradaTS*> clases_abiertas;
static std::vector<EntradaTS*> ids_pendientes;
static std::vector<std::string> valores_pendientes;

// Manejo de llamadas anidadas f(g(x)) y su orden explícito de evaluación (Tema 20)
static std::vector<std::vector<NodoArbol*>> pila_params_reales;
static std::vector<std::vector<int>> pila_orden_eval;

static void error_sem(const std::string &msg) {
    registrar_error("Semantico", LINEA_ACTUAL, msg);
}

// Helpers de Ámbitos y Tipado

// Reconstruye el nombre completo del ámbito jerárquico actual (ej. "main:f")
std::string ambito_actual() {
    std::string r;
    for (size_t i = 0; i < pila_ambitos.size(); i++) {
        if (i) r += ":";
        r += pila_ambitos[i];
    }
    return r;
}

// Retorna únicamente el identificador base omitiendo el sufijo de ámbito
std::string nombre_base(const EntradaTS *e) {
    if (!e) return "";
    return e->lexema.substr(0, e->lexema.find(':'));
}

// Obtiene la estructura InfoTipo vinculada (primitivo, clase o typedef)
InfoTipo info_de(const EntradaTS *e) {
    if (!e) return SIN_TIPO;
    EntradaTS *c = nullptr;
    if (!e->tipo_nombre.empty()) {
        auto it = tabla_simbolos.find(e->tipo_nombre);
        if (it != tabla_simbolos.end()) c = &it->second;
    }
    return { e->tipo, c };
}

std::string nombre_tipo(const InfoTipo &t) {
    if (t.clase) return nombre_base(t.clase);
    switch (t.t) {
        case Tipo::USHORTINT: return "ushortint";
        case Tipo::DOUBLEF:   return "doublef";
        default:              return "desconocido";
    }
}

// Predicados de clasificación de tipos
static bool es_roto(const InfoTipo &t)     { return t.t == Tipo::NINGUNO && t.clase == nullptr; }
static bool es_numerico(const InfoTipo &t) { return t.clase == nullptr && (t.t == Tipo::USHORTINT || t.t == Tipo::DOUBLEF); }
static bool es_clase(const InfoTipo &t)    { return t.clase && t.clase->uso == Uso::CLASE; }
static bool es_enum(const InfoTipo &t)     { return t.clase && t.clase->uso == Uso::TYPEDEF; }

// Verificaciones Complejas de Tipos (Temas 23 y 25)

// Tema 25: Equivale si coinciden en cantidad, tipo y orden de atributos propios
static bool misma_estructura(const EntradaTS *c1, const EntradaTS *c2) {
    if (c1 == c2) return true;
    if (c1->atributos.size() != c2->atributos.size()) return false;
    for (size_t i = 0; i < c1->atributos.size(); i++) {
        const EntradaTS *a = c1->atributos[i], *b = c2->atributos[i];
        if (a->tipo != b->tipo || a->tipo_nombre != b->tipo_nombre) return false;
    }
    return true;
}

// Extrae el valor numérico de un lexema binario/flotante para validar pertenencia
static bool lexema_entero(const std::string &lx) { return lx.find('$') != std::string::npos; }

static double valor_de(const std::string &lx) {
    if (lexema_entero(lx)) return std::stod(lx.substr(0, lx.find('$')));
    std::string s = lx;
    size_t p = s.find('d');
    if (p != std::string::npos) s[p] = 'e';
    p = s.find('D');
    if (p != std::string::npos) s[p] = 'E';
    return std::stod(s);
}

// Tema 23: Verifica si una constante pertenece a la lista de valores del typedef
static bool valor_en_enum(const EntradaTS *constante, const EntradaTS *typedef_) {
    bool es_int = lexema_entero(constante->lexema);
    double v = valor_de(constante->lexema);
    for (const std::string &lx : typedef_->valores)
        if (lexema_entero(lx) == es_int && valor_de(lx) == v) return true;
    return false;
}

// Coerción, Promoción y Adaptación de Tipos (Tema 35)

// Valida y adapta la expresión origen según el tipo esperado. Genera nodos de conversión ushortint -> doublef
bool adaptar_a_tipo(const InfoTipo &dest, NodoArbol *&orig, const std::string &ctx) {
    if (!orig || es_roto(dest) || es_roto(orig->tipo)) return false;
    const InfoTipo o = orig->tipo;
    const std::string incompat = "Incompatibilidad de tipos en " + ctx + ": no se puede usar un valor de tipo '" + nombre_tipo(o) + "' donde se espera '" + nombre_tipo(dest) + "'";

    if (es_enum(dest)) {
        if (es_clase(o)) { error_sem(incompat); return false; }
        if (es_enum(o)) {
            if (o.clase != dest.clase) { error_sem(incompat); return false; }
            return true;
        }
        if (orig->tipo_nodo == TipoNodo::HOJA && orig->simbolo && orig->simbolo->uso == Uso::CONSTANTE
            && !valor_en_enum(orig->simbolo, dest.clase)) {
            error_sem("el valor '" + orig->simbolo->lexema + "' no pertenece al enumerado '" + nombre_base(dest.clase) + "' (" + ctx + ")");
            return false;
        }
        return true;
    }

    if (es_clase(dest)) {
        if (!es_clase(o) || !misma_estructura(dest.clase, o.clase)) {
            error_sem(incompat + (es_clase(o) ? " (distinta estructura)" : ""));
            return false;
        }
        return true;
    }

    if (!es_numerico(dest) || !es_numerico(o)) { error_sem(incompat); return false; }
    if (dest.t == o.t) return true;
    if (dest.t == Tipo::DOUBLEF && o.t == Tipo::USHORTINT) {
        orig = crear_nodo_conversion(orig, Tipo::DOUBLEF);
        return true;
    }
    error_sem(incompat);
    return false;
}

bool adaptar_asignacion(const NodoArbol *dest, NodoArbol *&orig) {
    if (!dest) return false;
    return adaptar_a_tipo(dest->tipo, orig, "la asignacion");
}

bool verificar_param(const EntradaTS *formal, NodoArbol *&real) {
    return adaptar_a_tipo(info_de(formal), real, "el parametro '" + nombre_base(formal) + "'");
}

// Operaciones aritméticas y relacionales con conversión implícita del operando de menor rango
static bool es_comparador(const std::string &op) {
    return op == "==" || op == "!=" || op == "<=" || op == ">=" || op == "<" || op == ">";
}

NodoArbol* operar(const std::string &op, NodoArbol *izq, NodoArbol *der) {
    if (!izq || !der) return nullptr;
    const InfoTipo ti = izq->tipo, td = der->tipo;
    if (es_roto(ti) || es_roto(td)) return nullptr;

    if (!es_numerico(ti) || !es_numerico(td)) {
        const InfoTipo &malo = es_numerico(ti) ? td : ti;
        error_sem("un operando de tipo '" + nombre_tipo(malo) + "' no se puede usar con el operador '" + op + "'");
        return nullptr;
    }

    Tipo res = ti.t;
    if (ti.t != td.t) {
        res = Tipo::DOUBLEF;
        if (ti.t == Tipo::USHORTINT) izq = crear_nodo_conversion(izq, Tipo::DOUBLEF);
        else                         der = crear_nodo_conversion(der, Tipo::DOUBLEF);
    }

    NodoArbol *n = crear_nodo(op, izq, der, TipoNodo::OPERADOR);
    n->tipo = es_comparador(op) ? InfoTipo{ Tipo::NINGUNO, nullptr } : InfoTipo{ res, nullptr };
    return n;
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

// Gestión de Símbolos y Declaraciones

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

static void agregar_unico(std::vector<EntradaTS*> &v, EntradaTS *e) {
    if (e && std::find(v.begin(), v.end(), e) == v.end()) v.push_back(e);
}

static bool ya_declarado(EntradaTS *raw) {
    return tabla_simbolos.count(raw->lexema + ":" + ambito_actual()) > 0;
}

static bool es_descartable(const EntradaTS *e) {
    return e->lexema.find("#dup") != std::string::npos;
}

// Si ocurre una redeclaración, crea un objeto ficticio (#dup) para aislar errores y no pisar el símbolo original
static EntradaTS* declarar_abriendo(EntradaTS *id, Uso uso, const InfoTipo &t, std::string &nombre_ambito) {
    nombre_ambito = id->lexema;
    if (!ya_declarado(id)) return declarar(id, uso, t);
    declarar(id, uso, t);
    static std::vector<std::unique_ptr<EntradaTS>> descartadas;
    descartadas.emplace_back(new EntradaTS());
    EntradaTS *e = descartadas.back().get();
    e->lexema = id->lexema + "#dup";
    e->uso = uso;
    e->tipo = t.t;
    e->ambito = ambito_actual();
    if (t.clase) e->tipo_nombre = t.clase->lexema;
    nombre_ambito += "#dup";
    return e;
}

void sem_declarar_variables(const InfoTipo &tipo) {
    for (EntradaTS *id : ids_pendientes) declarar(id, Uso::VARIABLE, tipo);
    ids_pendientes.clear();
}

void sem_declarar_programa(EntradaTS *id) {
    declarar(id, Uso::PROGRAMA, SIN_TIPO);
}

void sem_abrir_funcion(EntradaTS *id, const InfoTipo &tipo) {
    std::string amb;
    EntradaTS *f = declarar_abriendo(id, Uso::FUNCION, tipo, amb);
    f->tiene_ret = false;
    f->params_formales.clear();
    funciones_abiertas.push_back(f);
    pila_ambitos.push_back(amb);
}

void sem_cerrar_funcion(NodoArbol *cuerpo) {
    EntradaTS *func = funciones_abiertas.back();
    if (!func->tiene_ret) {
        std::string def_val = valor_por_defecto(func->tipo);
        InfoTipo it = { func->tipo, nullptr };
        NodoArbol *hoja_def = crear_hoja_valor(def_val, it);
        NodoArbol *nodo_ret_def = crear_nodo("ret", hoja_def, nullptr, TipoNodo::RETORNO);
        cuerpo = cuerpo ? crear_nodo_sentencias(cuerpo, nodo_ret_def) : nodo_ret_def;
    }
    if (es_descartable(func)) liberar_arbol(cuerpo);
    else registrar_arbol_funcion(func->lexema, cuerpo);
    funciones_abiertas.pop_back();
    pila_ambitos.pop_back();
}

void sem_destruir_cabecera_funcion() {
    funciones_abiertas.pop_back();
    pila_ambitos.pop_back();
}

void sem_abrir_clase(EntradaTS *id) {
    std::string amb;
    EntradaTS *c = declarar_abriendo(id, Uso::CLASE, SIN_TIPO, amb);
    c->padres.clear();
    clases_abiertas.push_back(c);
    pila_ambitos.push_back(amb);
}

void sem_cerrar_clase() {
    clases_abiertas.pop_back();
    pila_ambitos.pop_back();
}

void sem_destruir_cabecera_clase() {
    clases_abiertas.pop_back();
    pila_ambitos.pop_back();
}

void sem_declarar_atributo(EntradaTS *id, const InfoTipo &tipo) {
    EntradaTS *atr = declarar(id, Uso::ATRIBUTO, tipo);
    if (!clases_abiertas.empty()) agregar_unico(clases_abiertas.back()->atributos, atr);
}

void sem_abrir_metodo(EntradaTS *id, const InfoTipo &tipo) {
    std::string amb;
    EntradaTS *m = declarar_abriendo(id, Uso::METODO, tipo, amb);
    m->tiene_ret = false;
    m->params_formales.clear();
    funciones_abiertas.push_back(m);
    pila_ambitos.push_back(amb);
    if (!clases_abiertas.empty() && !es_descartable(m)) agregar_unico(clases_abiertas.back()->metodos, m);
}

void sem_cerrar_metodo(NodoArbol *cuerpo) {
    EntradaTS *func = funciones_abiertas.back();
    if (!func->tiene_ret) {
        std::string def_val = valor_por_defecto(func->tipo);
        InfoTipo it = { func->tipo, nullptr };
        NodoArbol *hoja_def = crear_hoja_valor(def_val, it);
        NodoArbol *nodo_ret_def = crear_nodo("ret", hoja_def, nullptr, TipoNodo::RETORNO);
        cuerpo = cuerpo ? crear_nodo_sentencias(cuerpo, nodo_ret_def) : nodo_ret_def;
    }
    if (es_descartable(func)) liberar_arbol(cuerpo);
    else registrar_arbol_funcion(func->lexema, cuerpo);
    funciones_abiertas.pop_back();
    pila_ambitos.pop_back();
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
}

void sem_typedef(EntradaTS *id) {
    EntradaTS *t = declarar(id, Uso::TYPEDEF, SIN_TIPO);
    t->valores = valores_pendientes;
    valores_pendientes.clear();
}

void sem_agregar_param_formal(EntradaTS *id, const InfoTipo &tipo) {
    EntradaTS *p = declarar(id, Uso::PARAMETRO, tipo);
    if (!funciones_abiertas.empty()) {
        funciones_abiertas.back()->params_formales.push_back(p);
    }
}

void sem_agregar_id_pendiente(EntradaTS *id)           { ids_pendientes.push_back(id); }
void sem_limpiar_ids_pendientes()                      { ids_pendientes.clear(); }
void sem_agregar_valor_pendiente(const std::string &v) { valores_pendientes.push_back(v); }
void sem_limpiar_valores_pendientes()                  { valores_pendientes.clear(); }

// Resolución y Búsqueda Léxica

// Busca un lexema recorriendo la pila de ámbitos desde el interno hacia el global (main)
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

// Herencia y Miembros de Clases (Temas 29 y 30)

static EntradaTS* clase_de(const EntradaTS *e) {
    if (!e || e->tipo_nombre.empty()) return nullptr;
    auto it = tabla_simbolos.find(e->tipo_nombre);
    return (it != tabla_simbolos.end() && it->second.uso == Uso::CLASE) ? &it->second : nullptr;
}

// Busca un miembro en la clase activa y luego en la jerarquía 'extends' (recursivo)
static EntradaTS* buscar_miembro(const EntradaTS *clase, const std::string &nombre) {
    for (EntradaTS *a : clase->atributos) if (nombre_base(a) == nombre) return a;
    for (EntradaTS *m : clase->metodos)   if (nombre_base(m) == nombre) return m;
    for (const std::string &p : clase->padres) {
        auto it = tabla_simbolos.find(p);
        if (it == tabla_simbolos.end()) continue;
        if (EntradaTS *r = buscar_miembro(&it->second, nombre)) return r;
    }
    return nullptr;
}

static EntradaTS* miembro_de_objeto(EntradaTS *raw_obj, EntradaTS *raw_miembro, EntradaTS *&obj) {
    obj = resolver_objeto(raw_obj);
    EntradaTS *clase = clase_de(obj);
    if (!clase) return nullptr;
    EntradaTS *m = buscar_miembro(clase, raw_miembro->lexema);
    if (!m)
        error_sem("la clase '" + nombre_base(clase) + "' (ni las que hereda) no tiene el miembro '" + raw_miembro->lexema + "'");
    return m;
}

// Generación del AST con Verificación Semántica

NodoArbol* sem_asignacion(EntradaTS *id, NodoArbol *exp) {
    EntradaTS *res = resolver_valor(id);
    NodoArbol *hoja_id = crear_hoja(res);
    adaptar_asignacion(hoja_id, exp);
    return crear_nodo_asignacion(":=", hoja_id, exp);
}

NodoArbol* sem_asignacion_expr(EntradaTS *id, NodoArbol *exp) {
    EntradaTS *res_id = resolver_valor(id);
    NodoArbol *hoja_id = crear_hoja(res_id);
    adaptar_asignacion(hoja_id, exp);
    return crear_nodo_asignacion("=", hoja_id, exp);
}

NodoArbol* sem_asignacion_atributo(EntradaTS *id_obj, EntradaTS *id_atr, NodoArbol *exp) {
    NodoArbol *acceso = sem_acceso_atributo(id_obj, id_atr);
    if (!acceso || !exp) return nullptr;
    if (!adaptar_asignacion(acceso, exp)) return nullptr;
    return crear_nodo_asignacion(":=", acceso, exp);
}

NodoArbol* sem_asignacion_posicional(EntradaTS *id_obj, NodoArbol *indice, NodoArbol *exp) {
    NodoArbol *acceso = sem_acceso_posicional(id_obj, indice);
    if (!acceso || !exp) return nullptr;
    if (!adaptar_asignacion(acceso, exp)) return nullptr;
    return crear_nodo_asignacion(":=", acceso, exp);
}

NodoArbol* sem_retorno(NodoArbol *exp) {
    if (funciones_abiertas.empty()) {
        error("Semantico", LINEA_ACTUAL, "la sentencia 'ret' solo puede utilizarse dentro de una funcion o metodo");
        return nullptr;
    }
    funciones_abiertas.back()->tiene_ret = true;
    adaptar_a_tipo(info_de(funciones_abiertas.back()), exp, "la sentencia 'ret'");
    return crear_nodo("ret", exp, nullptr, TipoNodo::RETORNO);
}

NodoArbol* sem_pout_str(EntradaTS *cte_str) {
    return crear_nodo("pout", crear_hoja(cte_str), nullptr, TipoNodo::SALIDA);
}

NodoArbol* sem_pout_exp(NodoArbol *exp) {
    return crear_nodo("pout", exp, nullptr, TipoNodo::SALIDA);
}

NodoArbol* sem_op_binaria(const std::string &op, NodoArbol *izq, NodoArbol *der) {
    return operar(op, izq, der);
}

NodoArbol* sem_condicion(const char *op, NodoArbol *izq, NodoArbol *der) {
    return operar(op, izq, der);
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

// Invocaciones a Funciones y Orden Explícito (Tema 20)

void sem_invocacion_nuevo_param(NodoArbol *param) {
    if (pila_params_reales.empty()) pila_params_reales.push_back({});
    pila_params_reales.back().push_back(param);
}

void sem_invocacion_iniciar_params(NodoArbol *param) {
    pila_params_reales.push_back({ param });
}

void sem_invocacion_agregar_orden(EntradaTS *cte) {
    if (pila_orden_eval.empty()) pila_orden_eval.push_back({});
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

static bool verificar_args(EntradaTS *f, std::vector<NodoArbol*> &args) {
    if (!f) return false;
    if (args.size() != f->params_formales.size()) {
        error_sem("cantidad incorrecta de parametros en la invocacion a '" + nombre_base(f) + "': se esperaban " + std::to_string(f->params_formales.size()) + ", se recibieron " + std::to_string(args.size()));
        return false;
    }
    bool ok = true;
    for (size_t i = 0; i < args.size(); i++)
        if (!verificar_param(f->params_formales[i], args[i])) ok = false;
    return ok;
}

// Tema 20: La lista de orden de evaluación debe ser una permutación válida de 1..n
static bool orden_valido(EntradaTS *f, const std::vector<int> &ord) {
    size_t n = f->params_formales.size();
    if (ord.size() != n) {
        error_sem("la lista de orden de evaluacion de '" + nombre_base(f) + "' tiene " + std::to_string(ord.size()) + " valor(es) y la funcion tiene " + std::to_string(n) + " parametro(s)");
        return false;
    }
    std::vector<bool> visto(n + 1, false);
    for (int k : ord) {
        if (k < 1 || (size_t)k > n || visto[k]) {
            error_sem("la lista de orden de evaluacion de '" + nombre_base(f) + "' debe ser una permutacion de 1 a " + std::to_string(n));
            return false;
        }
        visto[k] = true;
    }
    return true;
}

NodoArbol* sem_invocacion(EntradaTS *id) {
    std::vector<NodoArbol*> args;
    if (!pila_params_reales.empty()) { args = pila_params_reales.back(); pila_params_reales.pop_back(); }
    std::vector<int> ord;
    if (!pila_orden_eval.empty()) { ord = pila_orden_eval.back(); pila_orden_eval.pop_back(); }

    EntradaTS *f = resolver_funcion(id);
    if (!f) return crear_nodo_llamada(nullptr, args, ord);

    bool ok = verificar_args(f, args);
    if (!ord.empty() && !orden_valido(f, ord)) ok = false;
    if (!ok) return crear_nodo_llamada(f, args, ord);

    // Mapea y encola la copia de argumentos reales a formales según el orden indicado
    NodoArbol *copias = nullptr;
    for (size_t k = 0; k < args.size(); k++) {
        size_t i = ord.empty() ? k : (size_t)ord[k] - 1;
        NodoArbol *copia = crear_nodo_asignacion(":=", crear_hoja(f->params_formales[i]), args[i]);
        copias = crear_nodo_sentencias(copias, copia);
    }
    NodoArbol *llamada = crear_nodo_llamada(f, {}, ord);
    llamada->izq = copias;
    return llamada;
}

void sem_invocacion_error() {
    if (!pila_params_reales.empty()) pila_params_reales.pop_back();
    if (!pila_orden_eval.empty()) pila_orden_eval.pop_back();
}

// Accesos a Objetos y Miembros

NodoArbol* sem_acceso_atributo(EntradaTS *id_obj, EntradaTS *id_atr) {
    EntradaTS *obj = nullptr;
    EntradaTS *m = miembro_de_objeto(id_obj, id_atr, obj);
    if (!m) return nullptr;
    if (m->uso != Uso::ATRIBUTO) {
        error_sem("'" + id_atr->lexema + "' es un metodo y se usa como atributo");
        return nullptr;
    }
    return crear_nodo_acceso(crear_hoja(obj), crear_hoja(m));
}

NodoArbol* sem_acceso_metodo(EntradaTS *id_obj, EntradaTS *id_met) {
    std::vector<NodoArbol*> args;
    if (!pila_params_reales.empty()) { args = pila_params_reales.back(); pila_params_reales.pop_back(); }
    EntradaTS *obj = nullptr;
    EntradaTS *m = miembro_de_objeto(id_obj, id_met, obj);
    if (!m) return nullptr;
    if (m->uso != Uso::METODO) {
        error_sem("'" + id_met->lexema + "' es un atributo y se usa como metodo");
        return nullptr;
    }
    verificar_args(m, args);
    return crear_nodo_acceso(crear_hoja(obj), crear_nodo_llamada(m, args));
}

// Tema 29: Acceso posicional obj[k]
NodoArbol* sem_acceso_posicional(EntradaTS *id_obj, NodoArbol *indice) {
    EntradaTS *obj = resolver_objeto(id_obj);
    EntradaTS *clase = clase_de(obj);
    if (!clase || !indice) return nullptr;

    const std::vector<EntradaTS*> &attrs = clase->atributos;
    if (attrs.empty()) {
        error_sem("la clase '" + nombre_base(clase) + "' no tiene atributos");
        return nullptr;
    }

    InfoTipo res = info_de(attrs[0]);
    if (indice->simbolo && indice->simbolo->uso == Uso::CONSTANTE) {
        const std::string &lx = indice->simbolo->lexema;
        size_t k = std::stoul(lx.substr(0, lx.find('$')));
        if (k >= attrs.size()) {
            error_sem("indice " + std::to_string(k) + " fuera de rango: la clase '" + nombre_base(clase) + "' tiene " + std::to_string(attrs.size()) + " atributo(s)");
            return nullptr;
        }
        res = info_de(attrs[k]);
    } else {
        if (es_roto(indice->tipo)) return nullptr;
        if (!es_numerico(indice->tipo) || indice->tipo.t != Tipo::USHORTINT) {
            error_sem("el indice de '" + nombre_base(obj) + "[...]' debe ser entero");
            return nullptr;
        }
        for (const EntradaTS *a : attrs)
            if (a->tipo != attrs[0]->tipo || a->tipo_nombre != attrs[0]->tipo_nombre) {
                error_sem("no se puede determinar el tipo de '" + nombre_base(obj) + "[...]': los atributos de '" + nombre_base(clase) + "' tienen tipos distintos; use un indice constante");
                return nullptr;
            }
    }
    return crear_nodo_acceso_posicional(crear_hoja(obj), indice, res);
}