#ifndef SEMANTICA_H
#define SEMANTICA_H

#include <string>
#include <vector>
#include "main.h"
#include "arbol.h"

// =======================================================================
// Gestión de Ámbitos y Declaraciones
// =======================================================================
extern const InfoTipo SIN_TIPO;

std::string ambito_actual();
std::string nombre_base(const EntradaTS *e);
std::string valor_por_defecto(Tipo tipo);
const char* nombre_uso(Uso u);

EntradaTS* declarar(EntradaTS *raw, Uso uso, const InfoTipo &t);
void sem_declarar_variables(const InfoTipo &tipo);
void sem_declarar_programa(EntradaTS *id);
void sem_abrir_funcion(EntradaTS *id, const InfoTipo &tipo);
void sem_cerrar_funcion(NodoArbol *cuerpo);
void sem_destruir_cabecera_funcion();
void sem_abrir_clase(EntradaTS *id);
void sem_cerrar_clase();
void sem_destruir_cabecera_clase();
void sem_abrir_metodo(EntradaTS *id, const InfoTipo &tipo);
void sem_cerrar_metodo(NodoArbol *cuerpo);
void sem_extends();
void sem_typedef(EntradaTS *id);
void sem_agregar_param_formal(EntradaTS *id, const InfoTipo &tipo);

// Manejo de listas temporales en declarativas
void sem_agregar_id_pendiente(EntradaTS *id);
void sem_limpiar_ids_pendientes();
void sem_agregar_valor_pendiente(const std::string &val);
void sem_limpiar_valores_pendientes();

// =======================================================================
// Resolución Semántica de Identificadores
// =======================================================================
EntradaTS* buscar(const std::string &nombre);
EntradaTS* resolver(EntradaTS *raw);
EntradaTS* resolver_valor(EntradaTS *raw);
EntradaTS* resolver_funcion(EntradaTS *raw);
EntradaTS* resolver_tipo(EntradaTS *raw);
EntradaTS* resolver_objeto(EntradaTS *raw);

// =======================================================================
// Acciones Semánticas para Sentencias y Expresiones
// =======================================================================
NodoArbol* sem_asignacion(EntradaTS *id, NodoArbol *exp);
NodoArbol* sem_asignacion_expr(EntradaTS *id, NodoArbol *exp);
NodoArbol* sem_asignacion_atributo(EntradaTS *id_obj, EntradaTS *id_atr, NodoArbol *exp);
NodoArbol* sem_asignacion_posicional(EntradaTS *id_obj, NodoArbol *indice, NodoArbol *exp);

NodoArbol* sem_retorno(NodoArbol *exp);
NodoArbol* sem_pout_str(EntradaTS *cte_str);
NodoArbol* sem_pout_exp(NodoArbol *exp);

NodoArbol* sem_op_binaria(const std::string &op, NodoArbol *izq, NodoArbol *der);
NodoArbol* sem_condicion(const char *op, NodoArbol *izq, NodoArbol *der);

NodoArbol* sem_factor_id(EntradaTS *id);
NodoArbol* sem_factor_cte(EntradaTS *cte);
NodoArbol* sem_factor_cte_neg_int(EntradaTS *cte);
NodoArbol* sem_factor_cte_neg_float(EntradaTS *cte);

// Tema 20: Invocaciones y paso de parámetros
void sem_invocacion_nuevo_param(NodoArbol *param);
void sem_invocacion_iniciar_params(NodoArbol *param);
void sem_invocacion_agregar_orden(EntradaTS *cte);
void sem_invocacion_iniciar_orden(EntradaTS *cte);
void sem_invocacion_orden_vacio();
NodoArbol* sem_invocacion(EntradaTS *id);
void sem_invocacion_error();

// Accesos
NodoArbol* sem_acceso_atributo(EntradaTS *id_obj, EntradaTS *id_atr);
NodoArbol* sem_acceso_posicional(EntradaTS *id_obj, NodoArbol *indice);
void sem_llamada_opcional_pop();

#endif // SEMANTICA_H

