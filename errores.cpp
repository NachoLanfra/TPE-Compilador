#include "errores.h"
#include <iostream>

std::vector<std::string> errores;

void error(const std::string &etapa, int linea, const std::string &msg) {
    std::string m = msg;
    if (m.rfind("Error: ", 0) == 0) m = m.substr(7);
    errores.push_back("Linea " + std::to_string(linea) + ": Error " + etapa + ": " + m);
}

void registrar_error(const std::string &etapa, int linea, const std::string &msg) {
    error(etapa, linea, msg);
}

void advertencia(const std::string &etapa, int linea, const std::string &msg) {
    errores.push_back("Linea " + std::to_string(linea) + ": Warning " + etapa + ": " + msg);
}
