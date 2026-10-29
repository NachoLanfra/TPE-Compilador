all: y.tab.c
	g++ y.tab.c main.cpp arbol.cpp semantica.cpp errores.cpp -o compilador

y.tab.c: gramatica.y
	bison -dy -Wno-yacc -Wno-other gramatica.y

test: test-semantica test-lexico test-sintaxis test-eof test-tp3

test-semantica:
	g++ -I. test/test_semantica.cpp arbol.cpp semantica.cpp errores.cpp -o /tmp/test_sem && /tmp/test_sem

test-lexico: all
	./compilador test/tp1/test_lexico_completo.txt
	./compilador test/tp1/test_lexico_eof_comentario.txt
	./compilador test/tp1/test_lexico_eofcadena.txt

test-sintaxis: all
	./compilador test/tp2/test_gramatica_validos.txt
	./compilador test/tp2/test_gramatica_errores.txt
	./compilador test/tp2/test_negativos.txt

test-eof: all
	./compilador test/tp2/test_prog_sin_end.txt
	./compilador test/tp2/test_prog_sin_puntoycoma.txt
	./compilador test/tp2/test_prog_sin_end_con_puntoycoma.txt

test-tp3: all
	./compilador test/tp3/tema02_arbol_sintactico.txt
	./compilador test/tp3/tema13_repeat_while.txt
	./compilador test/tp3/tema17_asignacion_en_expresiones.txt
	./compilador test/tp3/tema20_orden_evaluacion_parametros.txt
	./compilador test/tp3/tema23_enumeraciones.txt
	./compilador test/tp3/tema25_asignacion_estructura_objetos.txt
	./compilador test/tp3/tema29_acceso_posicional_atributos.txt
	./compilador test/tp3/tema30_herencia_multiple.txt
	./compilador test/tp3/tema35_conversiones_implicitas.txt
	./compilador test/tp3/test_funcion_sin_return.txt
	./compilador test/tp3/test_paso5_asignaciones.txt
	./compilador test/tp3/test_retornos.txt
	./compilador test/tp3/test_tres_errores.txt

clean:
	rm -f compilador y.tab.c y.tab.h