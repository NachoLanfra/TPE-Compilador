#ifndef ARBOL_H
#define ARBOL_H

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include "main.h"

enum class TipoNodo {
    HOJA,           // Identificador, constante, cadena (apunta a la TS)
    OPERADOR,       // +, -, *, /, ==, !=, <=, >=, <, >
    ASIGNACION,     // := (sentencia) o = (asignación en expresión)
    CONTROL,        // IF, THEN, ELSE, CUERPO, WHILE, REPEAT_WHILE, COND
    SENTENCIAS,     // Secuencia / bloque de sentencias (hijo izquierdo: sentencia, derecho: resto)
    CONVERSION,     // Conversión implícita de entero a doublef (Tema 35)
    LLAMADA,        // Invocación a función / método (Tema 20)
    ACCESO,         // Acceso tradicional obj.atr (Tema 28)
    ACCESO_POS,     // Acceso posicional a atributo obj[pos] (Tema 29)
    SALIDA,         // Sentencia pout
    RETORNO,        // Sentencia ret
    OTRO
};

struct NodoArbol {
    std::string valor;              // Etiqueta del nodo: "+", ":=", "=", "IF", "REPEAT_WHILE", etc.
    TipoNodo tipo_nodo;             // Categoría del nodo
    EntradaTS *simbolo;             // Puntero a la entrada en la TS (si aplica)
    InfoTipo tipo;                  // Información de tipo (ushortint, doublef, clase, typedef)
    
    NodoArbol *izq;                 // Hijo izquierdo (operando 1, condición, objeto, etc.)
    NodoArbol *der;                 // Hijo derecho (operando 2, cuerpo, atributo/índice, etc.)
    
    // Tema 20: Parámetros reales en llamadas a funciones
    std::vector<NodoArbol*> params; // Parámetros reales en su orden sintáctico
    std::vector<int> orden_eval;    // Orden de evaluación opcional [1, 3, 2, ...] (1-based como el PDF)
};

extern NodoArbol *arbol_programa_principal;
extern std::unordered_map<std::string, NodoArbol*> arboles_funciones;

// Constructores de nodos generales
NodoArbol* crear_hoja(EntradaTS *simbolo);
NodoArbol* crear_hoja_valor(const std::string &valor, InfoTipo tipo = { Tipo::NINGUNO, nullptr });
NodoArbol* crear_nodo(const std::string &valor, NodoArbol *izq, NodoArbol *der = nullptr, TipoNodo tipo_nodo = TipoNodo::OPERADOR);
NodoArbol* crear_nodo_control(const std::string &valor, NodoArbol *izq, NodoArbol *der = nullptr);

// Conector de bloques / lista de sentencias
NodoArbol* crear_nodo_sentencias(NodoArbol *sentencia_actual, NodoArbol *siguientes = nullptr);

// Constructores específicos para los temas asignados
// Tema 17: Asignaciones de expresiones en expresiones (operador '=')
// Genera nodo idéntico a las asignaciones comunes, pero tipado como expresión.
NodoArbol* crear_nodo_asignacion(const std::string &op, NodoArbol *izq, NodoArbol *der);

// Tema 13: repeat <bloque> while (<condicion>);
NodoArbol* crear_nodo_repeat_while(NodoArbol *cuerpo, NodoArbol *condicion);

// Sentencias de selección (IF - THEN - ELSE)
NodoArbol* crear_nodo_if(NodoArbol *condicion, NodoArbol *bloque_then, NodoArbol *bloque_else = nullptr);

// Tema 20: Invocaciones a funciones con orden opcional de evaluación de parámetros
// params: lista de expresiones reales
// orden_eval: índices opcionales (vacío si no se especificó corchetes)
NodoArbol* crear_nodo_llamada(EntradaTS *funcion, const std::vector<NodoArbol*> &params, const std::vector<int> &orden_eval = {});

// Tema 29: Acceso posicional a atributos: obj[pos]
NodoArbol* crear_nodo_acceso_posicional(NodoArbol *objeto, NodoArbol *indice_pos);

// Acceso tradicional a atributos / métodos: obj.miembro (para Tema 28 / 30)
NodoArbol* crear_nodo_acceso(NodoArbol *objeto, NodoArbol *miembro);

// Tema 35: Conversiones Implícitas (Entero -> Flotante)
NodoArbol* crear_nodo_conversion(NodoArbol *nodo_origen, Tipo tipo_destino = Tipo::DOUBLEF);

// Gestión de árboles por función y programa principal
void registrar_arbol_funcion(const std::string &nombre_funcion, NodoArbol *raiz);
void registrar_arbol_programa(NodoArbol *raiz);

// Visualización 
void imprimir_arbol_sintactico(NodoArbol *raiz, const std::string &etiqueta = "Raiz", int nivel = 0, std::ostream &out = std::cout);
void imprimir_todos_los_arboles(std::ostream &out = std::cout);

// Exportación gráfica opcional (Graphviz DOT)
void graficar_arbol(NodoArbol *raiz, const std::string &nombre_archivo = "arbol.dot");

// Limpieza de memoria
void liberar_arbol(NodoArbol *raiz);
void liberar_todos_los_arboles();

#endif // ARBOL_H
