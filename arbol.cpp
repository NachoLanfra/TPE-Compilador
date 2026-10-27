#include "arbol.h"
#include <iomanip>

// Estructuras globales
NodoArbol *arbol_programa_principal = nullptr;
std::unordered_map<std::string, NodoArbol*> arboles_funciones;

NodoArbol* crear_hoja(EntradaTS *simbolo) {
    if (!simbolo) return nullptr;
    NodoArbol *nodo = new NodoArbol();
    nodo->valor = simbolo->lexema;
    nodo->tipo_nodo = TipoNodo::HOJA;
    nodo->simbolo = simbolo;
    nodo->tipo = { simbolo->tipo, nullptr };
    if (!simbolo->tipo_nombre.empty()) {
        auto it = tabla_simbolos.find(simbolo->tipo_nombre);
        if (it != tabla_simbolos.end()) {
            nodo->tipo.clase = &(it->second);
        }
    }
    nodo->izq = nullptr;
    nodo->der = nullptr;
    return nodo;
}

NodoArbol* crear_hoja_valor(const std::string &valor, InfoTipo tipo) {
    NodoArbol *nodo = new NodoArbol();
    nodo->valor = valor;
    nodo->tipo_nodo = TipoNodo::HOJA;
    nodo->simbolo = nullptr;
    nodo->tipo = tipo;
    nodo->izq = nullptr;
    nodo->der = nullptr;
    return nodo;
}

NodoArbol* crear_nodo(const std::string &valor, NodoArbol *izq, NodoArbol *der, TipoNodo tipo_nodo) {
    NodoArbol *nodo = new NodoArbol();
    nodo->valor = valor;
    nodo->tipo_nodo = tipo_nodo;
    nodo->simbolo = nullptr;
    nodo->tipo = { Tipo::NINGUNO, nullptr };
    nodo->izq = izq;
    nodo->der = der;
    return nodo;
}

NodoArbol* crear_nodo_control(const std::string &valor, NodoArbol *izq, NodoArbol *der) {
    return crear_nodo(valor, izq, der, TipoNodo::CONTROL);
}

NodoArbol* crear_nodo_sentencias(NodoArbol *sentencia_actual, NodoArbol *siguientes) {
    if (!sentencia_actual) return siguientes;
    if (!siguientes) return sentencia_actual;
    return crear_nodo("sentencias", sentencia_actual, siguientes, TipoNodo::SENTENCIAS);
}

// Tema 17: Asignación normal ':=' o dentro de expresión '='
NodoArbol* crear_nodo_asignacion(const std::string &op, NodoArbol *izq, NodoArbol *der) {
    NodoArbol *nodo = crear_nodo(op, izq, der, TipoNodo::ASIGNACION);
    if (izq) {
        nodo->tipo = izq->tipo; // El tipo de la expresión de asignación es el tipo del destino
    }
    return nodo;
}

// Tema 13: repeat <bloque> while (<condicion>);
NodoArbol* crear_nodo_repeat_while(NodoArbol *cuerpo, NodoArbol *condicion) {
    NodoArbol *nodo_cuerpo = crear_nodo_control("cuerpo", cuerpo, nullptr);
    NodoArbol *nodo_cond = crear_nodo_control("cond", condicion, nullptr);
    return crear_nodo_control("REPEAT_WHILE", nodo_cuerpo, nodo_cond);
}

// Selección: IF-THEN-ELSE (siguiendo el esquema de árbol sintáctico de las filminas pág. 30/31)
NodoArbol* crear_nodo_if(NodoArbol *condicion, NodoArbol *bloque_then, NodoArbol *bloque_else) {
    NodoArbol *nodo_cond = crear_nodo_control("cond", condicion, nullptr);
    NodoArbol *nodo_then = crear_nodo_control("then", bloque_then, nullptr);
    NodoArbol *nodo_else = bloque_else ? crear_nodo_control("else", bloque_else, nullptr) : nullptr;
    
    NodoArbol *cuerpo = crear_nodo_control("cuerpo", nodo_then, nodo_else);
    return crear_nodo_control("IF", nodo_cond, cuerpo);
}

// Tema 20: Invocaciones a funciones con orden opcional de evaluación de parámetros
NodoArbol* crear_nodo_llamada(EntradaTS *funcion, const std::vector<NodoArbol*> &params, const std::vector<int> &orden_eval) {
    NodoArbol *nodo = new NodoArbol();
    nodo->valor = funcion ? funcion->lexema : "CALL";
    nodo->tipo_nodo = TipoNodo::LLAMADA;
    nodo->simbolo = funcion;
    nodo->tipo = { funcion ? funcion->tipo : Tipo::NINGUNO, nullptr };
    nodo->izq = nullptr;
    nodo->der = nullptr;
    nodo->params = params;
    nodo->orden_eval = orden_eval;
    return nodo;
}

// Tema 29: Acceso posicional a atributo obj[pos]
NodoArbol* crear_nodo_acceso_posicional(NodoArbol *objeto, NodoArbol *indice_pos) {
    NodoArbol *nodo = crear_nodo("[]", objeto, indice_pos, TipoNodo::ACCESO_POS);
    if (indice_pos && indice_pos->simbolo) {
        nodo->tipo = indice_pos->tipo;
    }
    return nodo;
}

// Acceso tradicional obj.atr / obj.metodo
NodoArbol* crear_nodo_acceso(NodoArbol *objeto, NodoArbol *miembro) {
    NodoArbol *nodo = crear_nodo(".", objeto, miembro, TipoNodo::ACCESO);
    if (miembro) {
        nodo->tipo = miembro->tipo;
    }
    return nodo;
}

// Tema 35: Conversiones Implícitas (Entero -> Flotante)
NodoArbol* crear_nodo_conversion(NodoArbol *nodo_origen, Tipo tipo_destino) {
    std::string etiqueta = (tipo_destino == Tipo::DOUBLEF) ? "CONV_DOUBLEF" : "CONV_USHORTINT";
    NodoArbol *nodo = crear_nodo(etiqueta, nodo_origen, nullptr, TipoNodo::CONVERSION);
    nodo->tipo = { tipo_destino, nullptr };
    return nodo;
}

// Helper para insertar la conversión implícita binaria automáticamente
Tipo aplicar_conversion_implicita(NodoArbol *&izq, NodoArbol *&der) {
    if (!izq || !der) return Tipo::NINGUNO;

    Tipo t_izq = izq->tipo.t;
    Tipo t_der = der->tipo.t;

    if (t_izq == t_der) {
        return t_izq;
    }

    // Entero (USHORTINT) + Flotante (DOUBLEF) -> Se convierte entero a flotante
    if (t_izq == Tipo::USHORTINT && t_der == Tipo::DOUBLEF) {
        izq = crear_nodo_conversion(izq, Tipo::DOUBLEF);
        return Tipo::DOUBLEF;
    }

    if (t_izq == Tipo::DOUBLEF && t_der == Tipo::USHORTINT) {
        der = crear_nodo_conversion(der, Tipo::DOUBLEF);
        return Tipo::DOUBLEF;
    }

    return Tipo::NINGUNO;
}

void registrar_arbol_funcion(const std::string &nombre_funcion, NodoArbol *raiz) {
    arboles_funciones[nombre_funcion] = raiz;
}

void registrar_arbol_programa(NodoArbol *raiz) {
    arbol_programa_principal = raiz;
}

// Visualización
void imprimir_arbol_sintactico(NodoArbol *raiz, const std::string &etiqueta, int nivel, std::ostream &out) {
    if (!raiz) return;

    for (int i = 0; i < nivel; ++i) {
        out << "  ";
    }

    out << etiqueta << ": " << raiz->valor;

    if (raiz->simbolo) {
        out << " [TS: " << raiz->simbolo->lexema << "]";
    }
    if (raiz->tipo.clase) {
        out << " (tipo: " << raiz->tipo.clase->lexema << ")";
    } else if (raiz->tipo.t == Tipo::DOUBLEF) {
        out << " (tipo: doublef)";
    } else if (raiz->tipo.t == Tipo::USHORTINT) {
        out << " (tipo: ushortint)";
    }

    // Para llamadas con orden de evaluación (Tema 20)
    if (!raiz->orden_eval.empty()) {
        out << " [orden eval: ";
        for (size_t i = 0; i < raiz->orden_eval.size(); ++i) {
            if (i > 0) out << ", ";
            out << raiz->orden_eval[i];
        }
        out << "]";
    }

    out << "\n";

    if (raiz->izq) {
        imprimir_arbol_sintactico(raiz->izq, "Hijo izquierdo", nivel + 1, out);
    }
    if (raiz->der) {
        imprimir_arbol_sintactico(raiz->der, "Hijo derecho", nivel + 1, out);
    }

    // Parámetros de llamadas o listas de expresiones
    for (size_t i = 0; i < raiz->params.size(); ++i) {
        std::string etiqueta_param = "Parametro " + std::to_string(i + 1);
        imprimir_arbol_sintactico(raiz->params[i], etiqueta_param, nivel + 1, out);
    }
}

void imprimir_todos_los_arboles(std::ostream &out) {
    out << "            REPRESENTACION INTERMEDIA: AST            \n";

    for (const auto &par : arboles_funciones) {
        out << "\n--- Arbol Funcion / Metodo: " << par.first << " ---\n";
        imprimir_arbol_sintactico(par.second, "Raiz", 0, out);
    }

    out << "\n--- Arbol Programa Principal (main) ---\n";
    imprimir_arbol_sintactico(arbol_programa_principal, "Raiz", 0, out);
    out << "\n======================================================\n";
}

static void exportar_dot_recursivo(NodoArbol *nodo, std::ostream &out, int &contador) {
    if (!nodo) return;
    int id_actual = ++contador;

    std::string label = nodo->valor;
    if (nodo->simbolo) {
        label += "\\n(TS: " + nodo->simbolo->lexema + ")";
    }
    if (nodo->tipo.clase) {
        label += "\\n[" + nodo->tipo.clase->lexema + "]";
    } else if (nodo->tipo.t == Tipo::DOUBLEF) {
        label += "\\n[doublef]";
    } else if (nodo->tipo.t == Tipo::USHORTINT) {
        label += "\\n[ushortint]";
    }

    out << "  node" << id_actual << " [label=\"" << label << "\"];\n";

    if (nodo->izq) {
        int id_hijo = contador + 1;
        exportar_dot_recursivo(nodo->izq, out, contador);
        out << "  node" << id_actual << " -> node" << id_hijo << " [label=\"izq\"];\n";
    }
    if (nodo->der) {
        int id_hijo = contador + 1;
        exportar_dot_recursivo(nodo->der, out, contador);
        out << "  node" << id_actual << " -> node" << id_hijo << " [label=\"der\"];\n";
    }
    for (size_t i = 0; i < nodo->params.size(); ++i) {
        int id_hijo = contador + 1;
        exportar_dot_recursivo(nodo->params[i], out, contador);
        out << "  node" << id_actual << " -> node" << id_hijo << " [label=\"p" << (i + 1) << "\"];\n";
    }
}

void graficar_arbol(NodoArbol *raiz, const std::string &nombre_archivo) {
    std::ofstream out(nombre_archivo);
    if (!out.is_open()) return;
    out << "digraph AST {\n";
    out << "  node [shape=box, fontname=\"Arial\"];\n";
    int contador = 0;
    exportar_dot_recursivo(raiz, out, contador);
    out << "}\n";
    out.close();
}

void liberar_arbol(NodoArbol *raiz) {
    if (!raiz) return;
    liberar_arbol(raiz->izq);
    liberar_arbol(raiz->der);
    for (auto *p : raiz->params) {
        liberar_arbol(p);
    }
    delete raiz;
}

void liberar_todos_los_arboles() {
    liberar_arbol(arbol_programa_principal);
    arbol_programa_principal = nullptr;
    for (auto &par : arboles_funciones) {
        liberar_arbol(par.second);
    }
    arboles_funciones.clear();
}
