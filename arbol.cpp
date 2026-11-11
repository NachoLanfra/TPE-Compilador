#include "arbol.h"
#include "semantica.h"
#include <iomanip>

// Estructuras globales
NodoArbol *arbol_programa_principal = nullptr;
std::vector<std::pair<std::string, NodoArbol*>> arboles_funciones;

// Hoja que apunta a una entrada de la tabla (id, constante o cadena) y toma el tipo de ahí
NodoArbol* crear_hoja(EntradaTS *simbolo) {
    if (!simbolo) return nullptr;
    NodoArbol *nodo = new NodoArbol();
    nodo->valor = simbolo->lexema;
    nodo->tipo_nodo = TipoNodo::HOJA;
    nodo->simbolo = simbolo;
    nodo->tipo = info_de(simbolo);
    nodo->izq = nullptr;
    nodo->der = nullptr;
    return nodo;
}

// Hoja con un valor suelto, sin entrada en la tabla (ej. el 0 del ret implícito)
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

// Nodo interno genérico, nace sin tipo; los constructores de abajo lo completan
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

// Nodo de estructura de control (IF, cuerpo, cond, REPEAT_WHILE)
NodoArbol* crear_nodo_control(const std::string &valor, NodoArbol *izq, NodoArbol *der) {
    return crear_nodo(valor, izq, der, TipoNodo::CONTROL);
}

// Encadena sentencias; si un lado es nulo devuelve el otro para no crear nodos de más
NodoArbol* crear_nodo_sentencias(NodoArbol *sentencia_actual, NodoArbol *siguientes) {
    if (!sentencia_actual) return siguientes;
    if (!siguientes) return sentencia_actual;
    return crear_nodo("sentencias", sentencia_actual, siguientes, TipoNodo::SENTENCIAS);
}

// Asignación ':=' o '=' (Tema 17); el nodo toma el tipo del destino
NodoArbol* crear_nodo_asignacion(const std::string &op, NodoArbol *izq, NodoArbol *der) {
    NodoArbol *nodo = crear_nodo(op, izq, der, TipoNodo::ASIGNACION);
    if (izq) {
        nodo->tipo = izq->tipo; // El tipo de la expresión de asignación es el tipo del destino
    }
    return nodo;
}

// Tema 13: REPEAT_WHILE con el cuerpo a la izquierda y la condición a la derecha
NodoArbol* crear_nodo_repeat_while(NodoArbol *cuerpo, NodoArbol *condicion) {
    NodoArbol *nodo_cuerpo = crear_nodo_control("cuerpo", cuerpo, nullptr); // el cuerpo se ejecuta primero
    NodoArbol *nodo_cond = crear_nodo_control("cond", condicion, nullptr); // la condición se evalúa después de cada vuelta
    return crear_nodo_control("REPEAT_WHILE", nodo_cuerpo, nodo_cond);
}

// IF con la condición a la izquierda y a la derecha un nodo cuerpo que agrupa then y else
NodoArbol* crear_nodo_if(NodoArbol *condicion, NodoArbol *bloque_then, NodoArbol *bloque_else) {
    NodoArbol *nodo_cond = crear_nodo_control("cond", condicion, nullptr);
    NodoArbol *nodo_then = crear_nodo_control("then", bloque_then, nullptr);
    NodoArbol *nodo_else = bloque_else ? crear_nodo_control("else", bloque_else, nullptr) : nullptr;
    
    NodoArbol *cuerpo = crear_nodo_control("cuerpo", nodo_then, nodo_else);
    return crear_nodo_control("IF", nodo_cond, cuerpo);
}

// Tema 20: nodo de llamada; los reales van en params y el orden explícito en orden_eval
NodoArbol* crear_nodo_llamada(EntradaTS *funcion, const std::vector<NodoArbol*> &params, const std::vector<int> &orden_eval) {
    NodoArbol *nodo = new NodoArbol();
    nodo->valor = funcion ? funcion->lexema : "CALL"; // CALL si la función no se resolvió
    nodo->tipo_nodo = TipoNodo::LLAMADA;
    nodo->simbolo = funcion;
    nodo->tipo = funcion ? info_de(funcion) : InfoTipo{Tipo::NINGUNO, nullptr};
    nodo->izq = nullptr;
    nodo->der = nullptr;
    nodo->params = params; // vacío cuando semantica armó las copias
    nodo->orden_eval = orden_eval; // se muestra en la etiqueta del nodo
    return nodo;
}

// Tema 29: obj[pos]; el tipo lo decide semantica según el índice
NodoArbol* crear_nodo_acceso_posicional(NodoArbol *objeto, NodoArbol *indice_pos, InfoTipo tipo_resultado) {
    NodoArbol *nodo = crear_nodo("[]", objeto, indice_pos, TipoNodo::ACCESO_POS); // objeto a la izquierda, índice a la derecha
    nodo->tipo = tipo_resultado;
    return nodo;
}

// obj.atr u obj.metodo(); toma el tipo del miembro
NodoArbol* crear_nodo_acceso(NodoArbol *objeto, NodoArbol *miembro) {
    NodoArbol *nodo = crear_nodo(".", objeto, miembro, TipoNodo::ACCESO);
    if (miembro) {
        nodo->tipo = miembro->tipo;
    }
    return nodo;
}

// Tema 35: nodo CONV_DOUBLEF que envuelve al origen para la promoción implícita
NodoArbol* crear_nodo_conversion(NodoArbol *nodo_origen, Tipo tipo_destino) {
    std::string etiqueta = (tipo_destino == Tipo::DOUBLEF) ? "CONV_DOUBLEF" : "CONV_USHORTINT"; // CONV_USHORTINT no se usa, esa conversión es error
    NodoArbol *nodo = crear_nodo(etiqueta, nodo_origen, nullptr, TipoNodo::CONVERSION);
    nodo->tipo = { tipo_destino, nullptr }; // el hijo conserva su tipo original
    return nodo;
}

// Cada función o método tiene su propio árbol; el del programa principal va aparte
void registrar_arbol_funcion(const std::string &nombre_funcion, NodoArbol *raiz) {
    arboles_funciones.push_back({ nombre_funcion, raiz });
}

void registrar_arbol_programa(NodoArbol *raiz) {
    arbol_programa_principal = raiz;
}

// Visualización

// Etiqueta de un nodo: su valor, el tipo entre corchetes y el orden de evaluación si tiene
static std::string obtener_etiqueta_nodo(const NodoArbol *nodo) {
    std::string etiqueta = nodo->valor;

    if (nodo->tipo.clase) {
        etiqueta += " [" + nodo->tipo.clase->lexema + "]";
    } else if (nodo->tipo.t == Tipo::DOUBLEF) {
        etiqueta += " [doublef]";
    } else if (nodo->tipo.t == Tipo::USHORTINT) {
        etiqueta += " [ushortint]";
    }

    if (!nodo->orden_eval.empty()) {
        etiqueta += " {orden eval: ";
        for (size_t i = 0; i < nodo->orden_eval.size(); ++i) {
            if (i > 0) etiqueta += ", ";
            etiqueta += std::to_string(nodo->orden_eval[i]);
        }
        etiqueta += "}";
    }

    return etiqueta;
}

// Junta en un vector las sentencias de una cadena de nodos SENTENCIAS para imprimirlas al mismo nivel
static void recopilar_sentencias(
    NodoArbol *nodo,
    std::vector<NodoArbol *> &sentencias
) {
    if (!nodo) return;

    if (nodo->tipo_nodo == TipoNodo::SENTENCIAS) {
        recopilar_sentencias(nodo->izq, sentencias);
        recopilar_sentencias(nodo->der, sentencias);
    } else {
        sentencias.push_back(nodo);
    }
}

// Imprime un nodo y sus hijos con el dibujo de árbol; prefijo trae las barras de los niveles de arriba
static void imprimir_nodo_recursivo(
    NodoArbol *nodo,
    const std::string &prefijo,
    bool ultimo,
    const std::string &etiqueta,
    std::ostream &out,
    bool raiz = false
) {
    if (!nodo) return;

    if (raiz) {
        out << etiqueta << ": ";
    } else {
        out << prefijo << (ultimo ? "└── " : "├── ");
        if (!etiqueta.empty()) {
            out << etiqueta << ": ";
        }
    }

    out << obtener_etiqueta_nodo(nodo) << '\n';

    std::vector<std::pair<std::string, NodoArbol *>> hijos;

    // Las sentencias se muestran todas al mismo nivel en vez de anidar un nodo "sentencias" por instrucción
    if (nodo->tipo_nodo == TipoNodo::SENTENCIAS) {
        std::vector<NodoArbol *> sentencias;
        recopilar_sentencias(nodo, sentencias);

        for (NodoArbol *sentencia : sentencias) {
            hijos.push_back({"", sentencia});
        }
    } else {
        // Un hijo que es lista de sentencias también se aplana
        if (nodo->izq) {
            if (nodo->izq->tipo_nodo == TipoNodo::SENTENCIAS) {
                std::vector<NodoArbol *> sentencias;
                recopilar_sentencias(nodo->izq, sentencias);

                for (NodoArbol *sentencia : sentencias) {
                    hijos.push_back({"", sentencia});
                }
            } else {
                hijos.push_back({"", nodo->izq});
            }
        }

        if (nodo->der) {
            if (nodo->der->tipo_nodo == TipoNodo::SENTENCIAS) {
                std::vector<NodoArbol *> sentencias;
                recopilar_sentencias(nodo->der, sentencias);

                for (NodoArbol *sentencia : sentencias) {
                    hijos.push_back({"", sentencia});
                }
            } else {
                hijos.push_back({"", nodo->der});
            }
        }

        // Los parámetros de una llamada van como hijos numerados
        for (size_t i = 0; i < nodo->params.size(); ++i) {
            hijos.push_back({
                "Parametro " + std::to_string(i + 1),
                nodo->params[i]
            });
        }
    }

    // Los hijos heredan el prefijo más una barra, o espacios si este nodo era el último hermano
    std::string siguiente_prefijo = prefijo;
    if (!raiz) {
        siguiente_prefijo += ultimo ? "    " : "│   ";
    }

    for (size_t i = 0; i < hijos.size(); ++i) {
        imprimir_nodo_recursivo(
            hijos[i].second,
            siguiente_prefijo,
            i == hijos.size() - 1,
            hijos[i].first,
            out
        );
    }
}

// Punto de entrada para imprimir un árbol; nivel ya no se usa
void imprimir_arbol_sintactico(
    NodoArbol *raiz,
    const std::string &etiqueta,
    int nivel,
    std::ostream &out
) {
    (void)nivel;

    if (!raiz) {
        out << etiqueta << ": (vacio)\n";
        return;
    }

    imprimir_nodo_recursivo(
        raiz,
        "",
        true,
        etiqueta,
        out,
        true
    );
}

// Imprime los árboles de todas las funciones y después el del programa principal
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


// Libera el nodo y todo lo que cuelga de él, params incluidos
void liberar_arbol(NodoArbol *raiz) {
    if (!raiz) return;
    liberar_arbol(raiz->izq);
    liberar_arbol(raiz->der);
    for (auto *p : raiz->params) {
        liberar_arbol(p);
    }
    delete raiz;
}

// Libera todos los árboles al terminar
void liberar_todos_los_arboles() {
    liberar_arbol(arbol_programa_principal);
    arbol_programa_principal = nullptr;
    for (auto &par : arboles_funciones) {
        liberar_arbol(par.second);
    }
    arboles_funciones.clear();
}
