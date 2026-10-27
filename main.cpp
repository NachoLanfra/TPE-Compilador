#include <iostream> // Comenatarios
#include <array> // Arreglos
#include <cctype> // Manejo de caracteres
#include <cmath> // Abs Float
#include <fstream> // Archivos
#include <unordered_map> // Hashmap
#include <string> // Manejo de strings

#include "main.h"    // EntradaTS, tabla_simbolos, LINEA_ACTUAL, archivo (declaraciones compartidas)
#include "arbol.h"   // AST (Árbol Sintáctico)
#include "y.tab.h"   // ID, CTE_INT, PR_IF, etc. 

// Constantes para las matrices del autómata
const int CANT_ESTADOS = 21;  
const int CANT_COLUMNAS = 22;  

// Constantes para los estados que no son un número normal
const int ESTADO_ERROR = -1;
const int ESTADO_FINAL = 99; 

int LINEA_ACTUAL = 1;
std::ifstream archivo;
std::string lexema_actual = "";

// La tabla de referencias es de solo lectura y se carga al compilar
const std::unordered_map<std::string, int> tabla_referencias = {
    {"if",       PR_IF},
    {"else",     PR_ELSE},
    {"end_if",   PR_END_IF},
    {"begin",    PR_BEGIN},
    {"end",      PR_END},
    {"pout",     PR_POUT},
    {"ret",      PR_RET},
    {"class",    PR_CLASS},
    {"function", PR_FUNCTION},
    {"ushortint", PR_USHORTINT},  
    {"doublef",   PR_DOUBLEF},
    {"extends",   PR_EXTENDS},   
    {"typedef",   PR_TYPEDEF},   
    {"repeat",    PR_REPEAT},   
    {"while",     PR_WHILE},    
};

// Tabla de Símbolos
// (EntradaTS ahora está definido en main.h, porque el %union del parser también lo necesita
std::unordered_map<std::string, EntradaTS> tabla_simbolos;

// Tabla de operadores
const std::unordered_map<std::string, int> tabla_operadores = {
    {":=", OP_ASIGNACION},
    {"==", OP_IGUAL_IGUAL},
    {"<=", OP_MENOR_IGUAL},
    {">=", OP_MAYOR_IGUAL},
    {"!=", OP_DISTINTO}
};


// Acciones semánticas
enum AccionSemantica {
    A_NONE = 0,
    A_ADD,
    A_NL,
    A_ERROR,
    A_ID,
    A_PR,
    A_USH,
    A_FLT,
    A_STR,
    A_ATK,
    A_RTK,
    A_COM
};

// Columnas de la matriz de transiciones
enum ColumnaMatriz {
    COL_L_MAYUS         = 0,  // 'L' (Mayúsculas)
    COL_L_MINUS_SIN_USD = 1,  // 'l' - ('u', 's', 'd')
    COL_U               = 17,  // 'u'
    COL_S               = 18,  // 's'
    COL_D               = 19,  // 'd'
    
    COL_DIGITO          = 2,  // 'd' (0-9)
    COL_BLANCO_TAB      = 3,  // 'b', 't' (espacio, \t)
    COL_NUEVA_LINEA     = 13,  // 'nl' (\n)
    
    COL_GUION_BAJO      = 4,  // '_'
    COL_PUNTO           = 5,  // '.'
    COL_EXCLAMACION     = 6, // '!'
    COL_DOS_PUNTOS      = 7, // ':'
    COL_COMILLA_SIMPLE  = 20, // '''
    COL_PESOS           = 16, // '$'
    
    COL_OP_SUMA_RESTA   = 8, // '+', '-'
    COL_OP_MULT_DIV_ETC = 9, // '/', '*', ',', ')', '(', ';'
    COL_IGUAL           = 10, // '='
    COL_MENOR           = 11, // '<'
    COL_MAYOR           = 12, // '>'
    
    COL_LLAVE_ABRE      = 14, // '{'
    COL_LLAVE_CIERRA    = 15, // '}'
    
    COL_DESCONOCIDO     = 21  // Cualquier otro caracter que no pertenezca al lenguaje
};


// Matriz de Transiciones de Estado
std::array<std::array<int, CANT_COLUMNAS>, CANT_ESTADOS> matriz_estados = {{
    {            9,            1,            2,            0, ESTADO_ERROR,           20,           18,           10, ESTADO_FINAL, ESTADO_FINAL,           11,           12,           13,            0,           14, ESTADO_ERROR, ESTADO_ERROR,            1,            1,            1,           16, ESTADO_ERROR },  // Estado 0
    {            9,            1,            1, ESTADO_FINAL,            1, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL,            1,            1,            1, ESTADO_FINAL, ESTADO_ERROR },  // Estado 1
    { ESTADO_ERROR, ESTADO_ERROR,            2, ESTADO_ERROR, ESTADO_ERROR,            5, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR,            3, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR },  // Estado 2
    { ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR,            4, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR },  // Estado 3
    { ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_FINAL, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR },  // Estado 4
    { ESTADO_ERROR, ESTADO_ERROR,           19, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR },  // Estado 5
    { ESTADO_ERROR, ESTADO_ERROR,            8, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR,            7, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR },  // Estado 6
    { ESTADO_ERROR, ESTADO_ERROR,            8, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR },  // Estado 7
    { ESTADO_FINAL, ESTADO_FINAL,            8, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_ERROR },  // Estado 8
    {            9,            9, ESTADO_FINAL, ESTADO_FINAL,            9, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL,            9,            9,            9, ESTADO_FINAL, ESTADO_ERROR },  // Estado 9
    { ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_FINAL, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR },  // Estado 10
    { ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_ERROR },  // Estado 11
    { ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_ERROR },  // Estado 12
    { ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_ERROR },  // Estado 13
    { ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR,           15, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR },  // Estado 14
    {           15,           15,           15,           15,           15,           15,           15,           15,           15,           15,           15,           15,           15,           15,           15,           17,           15,           15,           15,           15,           15, 15 },  // Estado 15
    {           16,           16,           16,           16,           16,           16,           16,           16,           16,           16,           16,           16,           16, ESTADO_ERROR,           16,           16,           16,           16,           16,           16, ESTADO_FINAL, 16 },  // Estado 16
    {           15,           15,           15,           15,           15,           15,           15,           15,           15,           15,           15,           15,           15,           15,           15,            0,           15,           15,           15,           15,           15, 15 },  // Estado 17
    { ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_FINAL, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR, ESTADO_ERROR },  // Estado 18
    { ESTADO_FINAL, ESTADO_FINAL,           19, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL,            6, ESTADO_FINAL, ESTADO_ERROR },  // Estado 19
    { ESTADO_FINAL, ESTADO_FINAL,           19, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_FINAL, ESTADO_ERROR },  // Estado 20 ('.' tentativo: digito -> entra a 19 (parte decimal); cualquier otra cosa -> retract y devuelve '.' solo
}};


// Matriz de Acciones Semanticas
std::array<std::array<AccionSemantica, CANT_COLUMNAS>, CANT_ESTADOS> matriz_acciones = {{
    {     A_ADD,     A_ADD,     A_ADD,    A_NONE,   A_ERROR,     A_ADD,     A_ADD,     A_ADD,     A_ATK,     A_ATK,     A_ADD,     A_ADD,     A_ADD,      A_NL,    A_NONE,   A_ERROR,   A_ERROR,     A_ADD,     A_ADD,     A_ADD,    A_NONE,   A_ERROR },  // Estado 0
    {     A_ADD,     A_ADD,     A_ADD,      A_ID,     A_ADD,      A_ID,      A_ID,      A_ID,      A_ID,      A_ID,      A_ID,      A_ID,      A_ID,      A_ID,      A_ID,      A_ID,      A_ID,      A_ADD,      A_ADD,      A_ADD,      A_ID,   A_ERROR },  // Estado 1
    {   A_ERROR,   A_ERROR,     A_ADD,   A_ERROR,   A_ERROR,     A_ADD,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,     A_ADD,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR },  // Estado 2
    {   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,     A_ADD,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR },  // Estado 3
    {   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,     A_USH,   A_ERROR,   A_ERROR,   A_ERROR },  // Estado 4
    {   A_ERROR,   A_ERROR,     A_ADD,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR },  // Estado 5
    {   A_ERROR,   A_ERROR,     A_ADD,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,     A_ADD,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR },  // Estado 6
    {   A_ERROR,   A_ERROR,     A_ADD,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR },  // Estado 7
    {     A_FLT,     A_FLT,     A_ADD,     A_FLT,     A_FLT,     A_FLT,     A_FLT,     A_FLT,     A_FLT,     A_FLT,     A_FLT,     A_FLT,     A_FLT,     A_FLT,     A_FLT,     A_FLT,     A_FLT,     A_FLT,     A_FLT,     A_FLT,     A_FLT,   A_ERROR },  // Estado 8
    {     A_ADD,     A_ADD,      A_PR,      A_PR,     A_ADD,      A_PR,      A_PR,      A_PR,      A_PR,      A_PR,      A_PR,      A_PR,      A_PR,      A_PR,      A_PR,      A_PR,      A_PR,     A_ADD,     A_ADD,     A_ADD,      A_PR,   A_ERROR },  // Estado 9
    {   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,     A_ATK,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR },  // Estado 10
    {     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_ATK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,   A_ERROR },  // Estado 11
    {     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_ATK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,   A_ERROR },  // Estado 12
    {     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_ATK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,   A_ERROR },  // Estado 13
    {   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,    A_NONE,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR },  // Estado 14
    {    A_NONE,    A_NONE,    A_NONE,    A_NONE,    A_NONE,    A_NONE,    A_NONE,    A_NONE,    A_NONE,    A_NONE,    A_NONE,    A_NONE,    A_NONE,      A_NL,    A_NONE,    A_NONE,    A_NONE,    A_NONE,    A_NONE,    A_NONE,    A_NONE,   A_NONE },  // Estado 15
    {     A_ADD,     A_ADD,     A_ADD,     A_ADD,     A_ADD,     A_ADD,     A_ADD,     A_ADD,     A_ADD,     A_ADD,     A_ADD,     A_ADD,     A_ADD,   A_ERROR,     A_ADD,     A_ADD,     A_ADD,     A_ADD,     A_ADD,     A_ADD,     A_STR,   A_ADD },  // Estado 16
    {    A_NONE,    A_NONE,    A_NONE,    A_NONE,    A_NONE,    A_NONE,    A_NONE,    A_NONE,    A_NONE,    A_NONE,    A_NONE,    A_NONE,    A_NONE,      A_NL,    A_NONE,    A_COM,    A_NONE,    A_NONE,    A_NONE,    A_NONE,    A_NONE,   A_NONE },  // Estado 17
    {   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,     A_ATK,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR,   A_ERROR },  // Estado 18
    {     A_FLT,     A_FLT,     A_ADD,     A_FLT,     A_FLT,     A_FLT,     A_FLT,     A_FLT,     A_FLT,     A_FLT,     A_FLT,     A_FLT,     A_FLT,     A_FLT,     A_FLT,     A_FLT,     A_FLT,     A_FLT,     A_FLT,     A_ADD,     A_FLT,   A_ERROR },  // Estado 19
    {     A_RTK,     A_RTK,     A_ADD,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,     A_RTK,   A_ERROR },  // Estado 20
}};


int obtener_columna(char c) {
    //Funcion que dice en que columna debo mirar AS y Estado sig
    unsigned char uc = static_cast<unsigned char>(c); // Convertir a unsigned char para evitar problemas con ñ, acentos y otros caracteres especiales

    if (std::isupper(uc)) return COL_L_MAYUS;
    
    if (std::islower(uc)) {
        if (c == 'u') return COL_U;
        if (c == 's') return COL_S;
        if (c == 'd') return COL_D;
        return COL_L_MINUS_SIN_USD;
    }
    
    if (std::isdigit(uc)) return COL_DIGITO;
    
    if (c == ' ' || c == '\t') return COL_BLANCO_TAB;
    if (c == '\n') return COL_NUEVA_LINEA;
    
    switch (c) {
        case '_': return COL_GUION_BAJO;
        case '.': return COL_PUNTO;
        case '!': return COL_EXCLAMACION;
        case ':': return COL_DOS_PUNTOS;
        case '\'': return COL_COMILLA_SIMPLE;
        case '$': return COL_PESOS;
        case '+': return COL_OP_SUMA_RESTA;
        case '-': return COL_OP_SUMA_RESTA;
        case '/': return COL_OP_MULT_DIV_ETC;
        case '*': return COL_OP_MULT_DIV_ETC;
        case ',': return COL_OP_MULT_DIV_ETC;
        case ')': return COL_OP_MULT_DIV_ETC;
        case '(': return COL_OP_MULT_DIV_ETC;
        case ';': return COL_OP_MULT_DIV_ETC;
        case '[': return COL_OP_MULT_DIV_ETC;
        case ']': return COL_OP_MULT_DIV_ETC;
        case '=': return COL_IGUAL;
        case '<': return COL_MENOR;
        case '>': return COL_MAYOR;
        case '{': return COL_LLAVE_ABRE;
        case '}': return COL_LLAVE_CIERRA;
    }
    
    return COL_DESCONOCIDO;
}

void a_add(char c, std::string& cadena) {
    cadena += c; // Añade el caracter a la cadena actual
}

void a_retract(std::ifstream& archivo) {
    archivo.unget(); // Devuelve el último carácter leído al flujo
}

void a_nl() {
    LINEA_ACTUAL++; // Suma 1 a la variable global de línea actual
}

void a_trunc(std::string& cadena) {
    // Trunca la cadena si supera los 22 caracteres y emite un warning
    if (cadena.length() > 22) {
        std::cerr << "Warning Léxico: El identificador '" << cadena << "' supera los 22 caracteres. Fue truncado." << std::endl;
        cadena = cadena.substr(0, 22);
    }
}

void a_lower(std::string& cadena) {
    // Convierte la cadena a minúsculas
    for (char& c : cadena) {
        c = std::tolower(c);
    }
}

bool a_val_int(const std::string& cadena) {
    // Valida si el entero está dentro del rango de 0 a 255
    try {
        size_t pos = cadena.find('$');
        std::string num_str = cadena.substr(0, pos); // Extrae la parte antes del '$'
        int valor = std::stoi(num_str);
        if (valor < 0 || valor > 255) { // Verifica si está fuera del rango
            std::cerr << "Error Léxico en línea " << LINEA_ACTUAL
            << ": Entero fuera de rango (0-255): "
            << valor << std::endl;
            return false; 
        }
        return true; 
    } catch (const std::exception& e) {
        std::cerr << "Error Léxico en línea " << LINEA_ACTUAL
        << ": Error convirtiendo a entero: " << cadena << std::endl;
        return false;
    }
}

bool a_val_float(std::string& cadena) {
    // Valida si el flotante está dentro del rango de 64 bits y corrige la notación científica si es necesario
    size_t pos_e = cadena.find('d'); 
    if (pos_e != std::string::npos && pos_e + 1 < cadena.length()) { // Si hay un 'd' y no es el último carácter
        if (cadena[pos_e + 1] != '+' && cadena[pos_e + 1] != '-') { // Si el siguiente carácter no es '+' ni '-', inserta un '+' después del 'd'
            cadena.insert(pos_e + 1, "+");
        }
    }

    try {
        // Convierte la cadena a double y verifica si está dentro del rango de 64 bits
        std::string temp = cadena;
        size_t p = temp.find('d');
        if (p != std::string::npos) temp[p] = 'e'; // Cambia 'd' por 'e' para que std::stod pueda interpretarlo correctamente
        double valor = std::stod(temp); 

        const double MIN_NORMAL = 2.2250738585072014e-308;
        const double MAX_NORMAL = 1.7976931348623157e+308;
        double abs_valor = std::fabs(valor);

        if (abs_valor != 0.0 && (abs_valor < MIN_NORMAL || abs_valor > MAX_NORMAL)) { // Verifica si está fuera del rango de 64 bits
            std::cerr << "Error Léxico en línea " << LINEA_ACTUAL
        << ": Flotante fuera de rango de 64 bits: "
        << cadena << std::endl;
            return false;
        }
        return true; 
    
    } catch (const std::out_of_range& e) { 
        std::cerr << "Error Léxico en línea " << LINEA_ACTUAL
        << ": Flotante fuera de rango de 64 bits: " << cadena << std::endl;
        return false;
    } catch (...) {
        std::cerr << "Error Léxico en línea " << LINEA_ACTUAL
        << ": Error desconocido al convertir flotante: " << cadena << std::endl;
        return false;
    }
}

EntradaTS* a_ts(const std::string& lexema) {
    // Funcion que busca lexema en la Tabla de Simbolos y lo da de alta si no existe
    auto it = tabla_simbolos.find(lexema); // Busca el lexema
    if (it == tabla_simbolos.end()) { // Si no existe en la tabla, lo da de alta
        EntradaTS nueva_entrada;
        nueva_entrada.lexema = lexema;
        it = tabla_simbolos.insert({lexema, nueva_entrada}).first;
    }
    
    return &(it->second); // Devuelve la dirección de memoria (&) del struct asociado a esa clave
}

int a_tpr(const std::string& lexema) {
    // Funcion que busca lexema en la Tabla de Palabras Reservadas
    auto it = tabla_referencias.find(lexema);
    if (it != tabla_referencias.end()) {
        return it->second; // Retorna el Token asociado a la palabra reservada
    }

    return ID;     // Si no es palabra reservada, lo tratamos como un ID normal
}

void a_error(char c) {
    // Funcion que imprime el error lexico y la linea en la que se encuentra
    std::cerr << "Error Léxico en línea " << LINEA_ACTUAL << " con: " << c << std::endl;
}

int yylex() { 
    int estado_actual = 0;
    lexema_actual = ""; 
    char c;

    while (archivo.get(c)) {
        // Obtengo la columna de la matriz de transiciones de estado según el caracter leído
        int columna = obtener_columna(c);
        AccionSemantica accion = matriz_acciones[estado_actual][columna];
        
        // Ejecución de acciones simples, se hacen aca porque no tienen return, no son finales.
        if (accion == A_ADD) {
            a_add(c, lexema_actual); // Añade el caracter a la cadena actual
        } else if (accion == A_NL) {
            a_nl(); // Suma 1 a la variable global
        }else if (accion == A_COM) { // Se ejecuta cuando se cierra un comentario multilinea
            std::cout << "-> Info: Comentario multilineas cerrado e ignorado exitosamente." << std::endl;
        }
        
        estado_actual = matriz_estados[estado_actual][columna]; 

        if (estado_actual == ESTADO_FINAL) {
            
            switch (accion) { // Switch para las acciones semánticas finales, que retornan un token
                case A_ID: {
                    // Si es un ID, lo agregamos a la tabla de simbolos y retornamos el token ID
                    a_retract(archivo); // Devuelve el último carácter leído al flujo
                    a_trunc(lexema_actual); // Trunca el lexema si es necesario(22 bits)
                    int token = a_tpr(lexema_actual);
                    if (token == ID) {
                        yylval.ts_ref = a_ts(lexema_actual);
                    }
                    //std::cout << "Token detectado en línea " << LINEA_ACTUAL << ": " << lexema_actual << std::endl;
                    return token;
                }
                case A_PR: {
                    // Si es una palabra reservada, la buscamos en la tabla de referencias y retornamos el token correspondiente
                    a_retract(archivo);
                    std::string lexema_original = lexema_actual; // Conserva las mayúsculas para el mensaje de error
                    a_lower(lexema_actual);
                    int token = a_tpr(lexema_actual);
                    if (token == ID) { 
                        std::cerr << "Error Léxico en línea " << LINEA_ACTUAL << ": identificador con mayúscula(s) no permitido: '" << lexema_original << "'" << std::endl;
                        lexema_actual = ""; // error, no encuentra la palabra reservada y como tenia mayusculas no es un ID valido, entonces descarta el lexema
                        estado_actual = 0;
                        continue; 
                    }
                    //std::cout << "Token detectado en línea " << LINEA_ACTUAL << ": " << lexema_actual << std::endl;
                    return token;
                }
                    
                case A_USH:
                    // Si es un entero, lo agregamos a la tabla de simbolos y retornamos el token CTE_INT
                    a_add(c, lexema_actual); 
                    if (!a_val_int(lexema_actual)) { // Si la función devuelve false por pasarse del límite
                        estado_actual = 0;
                        lexema_actual = "";
                        continue; // Lo descarta y sigue leyendo
                    }
                    yylval.ts_ref = a_ts(lexema_actual);
                    //std::cout << "Token detectado en línea " << LINEA_ACTUAL << ": " << lexema_actual << std::endl;
                    yylval.ts_ref->tipo = Tipo::USHORTINT; yylval.ts_ref->uso = Uso::CONSTANTE;
                    return CTE_INT;
                    
                case A_FLT:
                    // Si es un flotante, lo agregamos a la tabla de simbolos y retornamos el token CTE_FLOAT
                    a_retract(archivo); 
                    if (!a_val_float(lexema_actual)) { // Si la función devuelve false por pasarse del límite
                        estado_actual = 0;
                        lexema_actual = "";
                        continue; // Lo descarta y sigue leyendo
                    }
                    yylval.ts_ref = a_ts(lexema_actual);
                    //std::cout << "Token detectado en línea " << LINEA_ACTUAL << ": " << lexema_actual << std::endl;
                    yylval.ts_ref->tipo = Tipo::DOUBLEF; yylval.ts_ref->uso = Uso::CONSTANTE;
                    return CTE_FLOAT;
                    
                case A_STR:
                    // Si es una cadena, lo agregamos a la tabla de simbolos y retornamos el token CTE_STR
                    yylval.ts_ref = a_ts(lexema_actual); 
                    //std::cout << "Token detectado en línea " << LINEA_ACTUAL << ": " << lexema_actual << std::endl;
                    yylval.ts_ref->uso = Uso::CADENA;
                    return CTE_STR;
                    
                case A_ATK: {
                    // Consume el caracter para formar un token compuesto (ej: '<=') sin hacer retract.
                    a_add(c, lexema_actual);
                    auto it = tabla_operadores.find(lexema_actual);
                    if (it != tabla_operadores.end()) {
                        return it->second;
                    }
                    //std::cout << "Token detectado en línea " << LINEA_ACTUAL << ": " << lexema_actual[0] << std::endl;
                    return lexema_actual[0];
                }
                case A_RTK:
                    // Devuelve el caracter extra leído al archivo y cierra el token simple (ej: '<').
                    a_retract(archivo); 
                    //std::cout << "Token detectado en línea " << LINEA_ACTUAL << ": " << lexema_actual[0] << std::endl;
                    return lexema_actual[0]; 
                    
                case A_NONE:
                case A_ADD:
                case A_NL:
                case A_ERROR:
                case A_COM:
                    break;
            }
        }
        else if (estado_actual == ESTADO_ERROR) {
            // Si el estado es de error, imprimimos el error, reiniciamos el estado y el lexema
            a_error(c);
            if (c == '\n') {
                a_nl(); 
            }
            estado_actual = 0;
            lexema_actual = "";
        }
    }

    if (estado_actual == 14 || estado_actual == 15 || estado_actual == 17) {
        std::cerr << "Error Léxico en línea " << LINEA_ACTUAL
        << ": comentario sin cerrar al final del archivo."
        << std::endl;
        return 0;
    }

    if (!lexema_actual.empty()) { // Si al llegar al fin de archivo, el lexema no está vacío, significa que hay un lexema incompleto que debe ser procesado.
        AccionSemantica accion_eof = matriz_acciones[estado_actual][COL_BLANCO_TAB]; // Usamos la columna de espacio/tab para determinar la acción semántica al final del archivo
        switch (accion_eof) {
            case A_ID: { // 
                // Si es un ID, lo agregamos a la tabla de simbolos y retornamos el token ID
                a_trunc(lexema_actual);
                int token = a_tpr(lexema_actual);
                if (token == ID) {
                    yylval.ts_ref = a_ts(lexema_actual);
                }
                //std::cout << "Token detectado en línea " << LINEA_ACTUAL << ": " << lexema_actual << std::endl;
                return token;
            }
            case A_PR: {
                // Si es una palabra reservada, la buscamos en la tabla de referencias y retornamos el token correspondiente
                std::string lexema_original = lexema_actual;
                a_lower(lexema_actual);
                int token = a_tpr(lexema_actual);
                if (token == ID) {
                    //std::cerr << "Error Léxico en línea " << LINEA_ACTUAL << ": identificador con mayúscula(s) no permitido: '" << lexema_original << "'" << std::endl;
                    lexema_actual = "";
                    return 0;
                }
                std::cout << "Token detectado en línea " << LINEA_ACTUAL << ": " << lexema_actual << std::endl;
                return token;
            }
            case A_RTK:
                // Si es un caracter especial, lo devolvemos directamente.
                // std::cout << "Token detectado en línea " << LINEA_ACTUAL << ": " << lexema_actual[0] << std::endl;
                return lexema_actual[0];
            case A_FLT:
                // Si es un flotante, lo agregamos a la tabla de simbolos y retornamos el token CTE_FLOAT
                if (!a_val_float(lexema_actual)) {
                    lexema_actual = "";
                    return 0; 
                }                
                yylval.ts_ref = a_ts(lexema_actual);
                // std::cout << "Token detectado en línea " << LINEA_ACTUAL << ": " << lexema_actual << std::endl;
                yylval.ts_ref->tipo = Tipo::DOUBLEF; yylval.ts_ref->uso = Uso::CONSTANTE;
                return CTE_FLOAT;

            case A_USH:
                // Si es un entero, lo agregamos a la tabla de simbolos y retornamos el token CTE_INT
                if (!a_val_int(lexema_actual)) {
                    lexema_actual = "";
                    return 0; 
                }
                yylval.ts_ref = a_ts(lexema_actual);
                // std::cout << "Token detectado en línea " << LINEA_ACTUAL << ": " << lexema_actual << std::endl;
                yylval.ts_ref->tipo = Tipo::USHORTINT; yylval.ts_ref->uso = Uso::CONSTANTE;
                return CTE_INT;    
                    
            default: // Si es un lexema incompleto, imprimimos un mensaje de error y descartamos el lexema, ejemplo: cadena sin cerrar, etc.
                if (estado_actual == 16) {
                    std::cerr << "Error Léxico en línea " << LINEA_ACTUAL
                    << ": cadena sin cerrar al final del archivo: '"
                    << lexema_actual << "'" << std::endl;
                } else {
                    std::cerr << "Error Léxico en línea " << LINEA_ACTUAL
                    << ": fin de archivo inesperado, lexema incompleto descartado: '"
                    << lexema_actual << "'" << std::endl;
                }
                break;
        }
    }

    return 0;
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Error: Faltan parámetros." << std::endl;
        std::cerr << "Uso: " << argv[0] << " <ruta_del_archivo.txt>" << std::endl;
        return 1;
    }

    archivo.open(argv[1]);   // "archivo" ahora es global (declarada en main.h)
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo '" << argv[1] << "'" << std::endl;
        return 1;
    }

    std::cout << "--- INICIANDO ANÁLISIS SINTÁCTICO ---" << std::endl;

    int resultado = yyparse();

    if (resultado == 0) {
        std::cout << ">>> El programa termino." << std::endl;
        imprimir_todos_los_arboles();
        graficar_arbol(arbol_programa_principal, "arbol.dot");
    } else {
        std::cout << ">>> El programa no se pudo recuperar del error." << std::endl;
    }

    // --- ESTADÍSTICAS Y TABLA DE SÍMBOLOS ---
    std::cout << "\n======================================" << std::endl;
    std::cout << "Líneas totales analizadas: " << LINEA_ACTUAL << std::endl;
    std::cout << "======================================" << std::endl;
    
    std::cout << "\n--- CONTENIDO DE LA TABLA DE SÍMBOLOS ---" << std::endl;
    if (tabla_simbolos.empty()) {
        std::cout << "La tabla está vacía." << std::endl;
    } else {
        for (const auto& par : tabla_simbolos) {
            std::cout << " -> " << par.first << std::endl;
        }
    }
    std::cout << "-----------------------------------------" << std::endl;

    liberar_todos_los_arboles();

    archivo.close();
    return 0;
}
