#include <cassert>
#include <iostream>
#if __has_include("../semantica.h")
#include "../semantica.h"
#elif __has_include("semantica.h")
#include "semantica.h"
#endif

std::unordered_map<std::string, EntradaTS> tabla_simbolos;
int LINEA_ACTUAL = 1;

static EntradaTS* sim(const std::string &lx, Uso u, Tipo t, const std::string &tn = "") {
    EntradaTS e; e.lexema = lx; e.uso = u; e.tipo = t; e.tipo_nombre = tn;
    return &tabla_simbolos.insert({lx, e}).first->second;
}

int main() {
    EntradaTS *i = sim("i:main", Uso::VARIABLE, Tipo::USHORTINT);
    EntradaTS *d = sim("d:main", Uso::VARIABLE, Tipo::DOUBLEF);

    NodoArbol *s = operar("+", crear_hoja(i), crear_hoja(d));            // mixto: convierte el entero
    assert(s && s->tipo.t == Tipo::DOUBLEF && s->izq->tipo_nodo == TipoNodo::CONVERSION);

    NodoArbol *r = crear_hoja(i);                                        // d := i  -> conversión
    assert(adaptar_asignacion(crear_hoja(d), r) && r->tipo_nodo == TipoNodo::CONVERSION);

    NodoArbol *r2 = crear_hoja(d);                                       // i := d  -> error
    assert(!adaptar_asignacion(crear_hoja(i), r2));

    // Tema 23: enumerado color = [1$us, 2$us]
    EntradaTS *color = sim("color:main", Uso::TYPEDEF, Tipo::NINGUNO);
    color->valores = { "1$us", "2$us" };
    EntradaTS *c = sim("c:main", Uso::VARIABLE, Tipo::NINGUNO, "color:main");
    NodoArbol *ok  = crear_hoja(sim("2$us", Uso::CONSTANTE, Tipo::USHORTINT));
    NodoArbol *mal = crear_hoja(sim("5$us", Uso::CONSTANTE, Tipo::USHORTINT));
    assert(adaptar_asignacion(crear_hoja(c), ok));
    assert(!adaptar_asignacion(crear_hoja(c), mal));
    NodoArbol *v = crear_hoja(i);                                        // variable: se posterga
    assert(adaptar_asignacion(crear_hoja(c), v));

    // Tema 25: ca=[ushortint], cb=[ushortint], cc=[doublef]
    EntradaTS *ca = sim("ca:main", Uso::CLASE, Tipo::NINGUNO);
    EntradaTS *cb = sim("cb:main", Uso::CLASE, Tipo::NINGUNO);
    EntradaTS *cc = sim("cc:main", Uso::CLASE, Tipo::NINGUNO);
    ca->atributos = { sim("x:main:ca", Uso::ATRIBUTO, Tipo::USHORTINT) };
    cb->atributos = { sim("y:main:cb", Uso::ATRIBUTO, Tipo::USHORTINT) };
    cc->atributos = { sim("w:main:cc", Uso::ATRIBUTO, Tipo::DOUBLEF) };
    EntradaTS *oa = sim("oa:main", Uso::VARIABLE, Tipo::NINGUNO, "ca:main");
    EntradaTS *ob = sim("ob:main", Uso::VARIABLE, Tipo::NINGUNO, "cb:main");
    EntradaTS *oc = sim("oc:main", Uso::VARIABLE, Tipo::NINGUNO, "cc:main");
    NodoArbol *p1 = crear_hoja(ob), *p2 = crear_hoja(oc);
    assert(adaptar_asignacion(crear_hoja(oa), p1));
    assert(!adaptar_asignacion(crear_hoja(oa), p2));

    std::cout << "OK" << std::endl;
}

