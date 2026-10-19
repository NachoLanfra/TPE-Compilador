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
    OBJETO, TYPEDEF,
    CONSTANTE, CADENA
};

struct EntradaTS {
    std::string lexema;
    Uso  uso  = Uso::NINGUNO;
    Tipo tipo = Tipo::NINGUNO;        // variable/atributo/param: su tipo; función/método: tipo de retorno
    std::string tipo_nombre;          // objeto: nombre de su clase
    std::string ambito;               // dónde se declaró ("global", nombre de función, de clase)
    std::vector<Tipo> params;         // función/método: tipos de los parámetros formales
    std::vector<std::string> padres;  // clase: lista de extends
    std::vector<std::string> valores; // typedef: elementos de [ ... ]
    bool tiene_ret = false;           // función/método: tiene al menos un ret (si no, retorna valor por defecto de su tipo)
};

struct InfoTipo {
    Tipo t;
    EntradaTS *clase;
};


extern std::unordered_map<std::string, EntradaTS> tabla_simbolos;
extern int LINEA_ACTUAL;
extern std::ifstream archivo;

#endif
