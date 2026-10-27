#ifndef MAIN_H
#define MAIN_H

#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>

enum class Tipo { NINGUNO, USHORTINT, DOUBLEF };

enum class Uso {
    NINGUNO,      // ID visto por el léxico pero todavía no declarado
    VARIABLE, PARAMETRO, FUNCION,
    CLASE, ATRIBUTO, METODO,
    OBJETO, TYPEDEF, PROGRAMA,
    CONSTANTE, CADENA
};

struct EntradaTS;

struct InfoTipo {
    Tipo t;
    EntradaTS *clase;
};

struct EntradaTS {
    std::string lexema;
    Uso  uso  = Uso::NINGUNO;
    Tipo tipo = Tipo::NINGUNO;        // variable/atributo/param: su tipo; función/método: tipo de retorno
    std::string tipo_nombre;          // objeto: nombre de su clase
    std::string ambito;               // dónde se declaró ("main", "main:f", "main:c", ...)
    std::vector<EntradaTS*> params_formales; // función/método: parámetros formales
    std::vector<EntradaTS*> atributos;       // clase: atributos en orden de declaración
    std::vector<EntradaTS*> metodos;         // clase: métodos en orden de declaración
    std::vector<std::string> padres;  // clase: lista de extends
    std::vector<std::string> valores; // typedef: elementos de [ ... ]
    bool tiene_ret = false;           // función/método: tiene al menos un ret (si no, retorna valor por defecto de su tipo)
};


extern std::unordered_map<std::string, EntradaTS> tabla_simbolos;
extern int LINEA_ACTUAL;
extern std::ifstream archivo;

extern std::vector<std::string> errores;
void error(const std::string &etapa, int linea, const std::string &msg);
void registrar_error(const std::string &etapa, int linea, const std::string &msg);

#endif
