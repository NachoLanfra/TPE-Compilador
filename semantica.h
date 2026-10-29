#ifndef SEMANTICA_H
#define SEMANTICA_H

#include <string>
#include "main.h"
#include "arbol.h"

// Valor constante para representar un tipo inválido o no determinado
extern const InfoTipo SIN_TIPO;

// Retorna la cadena del ámbito activo (ej. "main", "main:mi_funcion")
std::string ambito_actual();

// Extrae el identificador original quitando el sufijo de ámbito (ej. "x:main:f" -> "x")
std::string nombre_base(const EntradaTS *e);

// Obtiene la estructura InfoTipo asociada a una entrada de la TS (tipo primitivo, clase o typedef)
InfoTipo info_de(const EntradaTS *e);

// Representación en texto del tipo para mensajes de error (ej. "ushortint", "Persona")
std::string nombre_tipo(const InfoTipo &t);

// Retorna la cadena del valor por defecto ("0" o "0.0") segun el tipo primitivo
std::string valor_por_defecto(Tipo tipo);

// Convierte el enum Uso a texto legible para diagnóstico (ej. "variable", "metodo")
const char* nombre_uso(Uso u);

// Aplica reglas de compatibilidad en operaciones binarias e inserta conversiones si aplica
NodoArbol* operar(const std::string &op, NodoArbol *izq, NodoArbol *der);

// Verifica compatibilidad destino/origen. Inserta nodo CONV_DOUBLEF si hay promoción ushortint -> doublef
bool adaptar_a_tipo(const InfoTipo &destino, NodoArbol *&origen, const std::string &contexto);

// Wrapper de adaptar_a_tipo especializado para sentencias de asignación
bool adaptar_asignacion(const NodoArbol *destino, NodoArbol *&origen);

// Valida el parámetro real contra el parámetro formal de la función
bool verificar_param(const EntradaTS *formal, NodoArbol *&real);

// Da de alta un identificador en la TS registrando su uso, tipo y ámbito actual
EntradaTS* declarar(EntradaTS *raw, Uso uso, const InfoTipo &t);

void sem_declarar_variables(const InfoTipo &tipo);
void sem_declarar_programa(EntradaTS *id);

// Manejo del ciclo de vida de funciones (apertura de ámbito, cierre y construcción del AST)
void sem_abrir_funcion(EntradaTS *id, const InfoTipo &tipo);
void sem_cerrar_funcion(NodoArbol *cuerpo);
void sem_destruir_cabecera_funcion(); // Recuperación de errores en sintáctico

// Manejo del ciclo de vida de clases y herencia
void sem_abrir_clase(EntradaTS *id);
void sem_cerrar_clase();
void sem_destruir_cabecera_clase();
void sem_declarar_atributo(EntradaTS *id, const InfoTipo &tipo);
void sem_abrir_metodo(EntradaTS *id, const InfoTipo &tipo);
void sem_cerrar_metodo(NodoArbol *cuerpo);

void sem_extends();       // Registra la herencia múltiple de clases (Tema 30)
void sem_typedef(EntradaTS *id); // Registra enumerados y sus valores válidos (Tema 23)
void sem_agregar_param_formal(EntradaTS *id, const InfoTipo &tipo);

void sem_agregar_id_pendiente(EntradaTS *id);
void sem_limpiar_ids_pendientes();
void sem_agregar_valor_pendiente(const std::string &val);
void sem_limpiar_valores_pendientes();

// Búsqueda léxica de un lexema hacia afuera desde el ámbito actual
EntradaTS* buscar(const std::string &nombre);

// Variantes de resolución con validación contextual de Uso
EntradaTS* resolver(EntradaTS *raw);
EntradaTS* resolver_valor(EntradaTS *raw);    // Variables, parámetros o atributos
EntradaTS* resolver_funcion(EntradaTS *raw);  // Funciones o métodos
EntradaTS* resolver_tipo(EntradaTS *raw);     // Clases o typedefs
EntradaTS* resolver_objeto(EntradaTS *raw);   // Instancias válidas de una clase

NodoArbol* sem_asignacion(EntradaTS *id, NodoArbol *exp);
NodoArbol* sem_asignacion_expr(EntradaTS *id, NodoArbol *exp); // Asignación dentro de expresión (Tema 17)
NodoArbol* sem_asignacion_atributo(EntradaTS *id_obj, EntradaTS *id_atr, NodoArbol *exp);
NodoArbol* sem_asignacion_posicional(EntradaTS *id_obj, NodoArbol *indice, NodoArbol *exp); // Tema 29

NodoArbol* sem_retorno(NodoArbol *exp);
NodoArbol* sem_pout_str(EntradaTS *cte_str);
NodoArbol* sem_pout_exp(NodoArbol *exp);

NodoArbol* sem_op_binaria(const std::string &op, NodoArbol *izq, NodoArbol *der);
NodoArbol* sem_condicion(const char *op, NodoArbol *izq, NodoArbol *der);

NodoArbol* sem_factor_id(EntradaTS *id);
NodoArbol* sem_factor_cte(EntradaTS *cte);
NodoArbol* sem_factor_cte_neg_int(EntradaTS *cte);   // Reporta error por ushortint con signo
NodoArbol* sem_factor_cte_neg_float(EntradaTS *cte); // Normaliza el flotante negativo en la TS

// Manejo de invocaciones y orden explícito de evaluación de parámetros (Tema 20)
void sem_invocacion_nuevo_param(NodoArbol *param);
void sem_invocacion_iniciar_params(NodoArbol *param);
void sem_invocacion_agregar_orden(EntradaTS *cte);
void sem_invocacion_iniciar_orden(EntradaTS *cte);
void sem_invocacion_orden_vacio();
NodoArbol* sem_invocacion(EntradaTS *id);
void sem_invocacion_error();

// Accesos a miembros de clases y herencia (Temas 29 y 30)
NodoArbol* sem_acceso_atributo(EntradaTS *id_obj, EntradaTS *id_atr);
NodoArbol* sem_acceso_posicional(EntradaTS *id_obj, NodoArbol *indice); // Acceso posicional obj[pos]
NodoArbol* sem_acceso_metodo(EntradaTS *id_obj, EntradaTS *id_met);

#endif // SEMANTICA_H