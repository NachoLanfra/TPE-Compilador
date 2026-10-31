#ifndef ERRORES_H
#define ERRORES_H

#include <string>
#include <vector>

// Vector global que centraliza los mensajes de error y advertencias del compilador
extern std::vector<std::string> errores;

// Funciones para registrar mensajes durante las distintas etapas del análisis
void error(const std::string &etapa, int linea, const std::string &msg);
void registrar_error(const std::string &etapa, int linea, const std::string &msg);
void advertencia(const std::string &etapa, int linea, const std::string &msg);

#endif

