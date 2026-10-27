#ifndef ERRORES_H
#define ERRORES_H

#include <string>
#include <vector>

extern std::vector<std::string> errores;
void error(const std::string &etapa, int linea, const std::string &msg);
void registrar_error(const std::string &etapa, int linea, const std::string &msg);

#endif

