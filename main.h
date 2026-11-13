#ifndef MAIN_H
#define MAIN_H

#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>

enum class Tipo { NINGUNO, USHORTINT, DOUBLEF };


// Rol semántico de un identificador dentro de la Tabla de Símbolos
enum class Uso {
    NINGUNO,      // ID visto por el léxico pero todavía no declarado
    VARIABLE,
    PARAMETRO, 
    FUNCION,
    CLASE, 
    ATRIBUTO, 
    METODO,
    OBJETO, 
    TYPEDEF, 
    PROGRAMA,
    CONSTANTE, 
    CADENA
};

struct EntradaTS;

// Tipo de un valor: primitivo en t, o clase/enum en clase (nullptr si es primitivo)
struct InfoTipo {
    Tipo t;
    EntradaTS *clase;
};

// Entrada de la tabla de símbolos: cada símbolo usa solo los campos que le corresponden
struct EntradaTS {
    std::string lexema;               // clave en la tabla: "x" en la cruda, "x:main:f" ya declarada
    Uso  uso  = Uso::NINGUNO;         // qué es el símbolo: variable, función, clase, constante...
    Tipo tipo = Tipo::NINGUNO;        // tipo primitivo; en función/método es el tipo de retorno
    std::string tipo_nombre;          // objeto o enum: nombre mangleado de la clase/typedef
    std::string ambito;               // dónde se declaró ("main", "main:f", ...)
    std::vector<EntradaTS*> params_formales; // función/método: sus parámetros en orden, para chequear las llamadas
    std::vector<EntradaTS*> atributos;       // clase: atributos propios en orden
    std::vector<EntradaTS*> metodos;         // clase: métodos propios
    std::vector<std::string> padres;  // clase: claves de los extends en orden, define dónde se busca primero
    std::vector<std::string> valores; // typedef: valores del enum 
    bool tiene_ret = false;           // función/método: si queda en false se agrega un ret por defecto
};


// Tabla global, la clave es "nombre:ambito"; los punteros a sus entradas no cambian al insertar
extern std::unordered_map<std::string, EntradaTS> tabla_simbolos;
extern int LINEA_ACTUAL;
extern std::ifstream archivo;

extern std::vector<std::string> errores;
void error(const std::string &etapa, int linea, const std::string &msg);
void registrar_error(const std::string &etapa, int linea, const std::string &msg);
void advertencia(const std::string &etapa, int linea, const std::string &msg);

#endif
