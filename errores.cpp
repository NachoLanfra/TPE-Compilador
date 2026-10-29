#include "errores.h"
#include <iostream>

// Contenedor global centralizado para almacenar todos los mensajes de error y advertencias
std::vector<std::string> errores;

// Registra un error de compilación. Normaliza el texto quitando prefijos "Error: " duplicados
void error(const std::string &etapa, int linea, const std::string &msg) {
    std::string m = msg;
    if (m.rfind("Error: ", 0) == 0) m = m.substr(7);
    errores.push_back("Linea " + std::to_string(linea) + ": Error " + etapa + ": " + m);
}

// Alias de 'error' para mantener una interfaz uniforme en el análisis semántico
void registrar_error(const std::string &etapa, int linea, const std::string &msg) {
    error(etapa, linea, msg);
}

// Registra una advertencia de compilación (warnings que no interrumpen el análisis)
void advertencia(const std::string &etapa, int linea, const std::string &msg) {
    errores.push_back("Linea " + std::to_string(linea) + ": Warning " + etapa + ": " + msg);
}