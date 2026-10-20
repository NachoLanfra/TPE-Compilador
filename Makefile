all: y.tab.c
	g++ y.tab.c main.cpp arbol.cpp -o compilador

y.tab.c: gramatica.y
	bison -dy -Wno-yacc gramatica.y

test: test-lexico test-sintaxis test-eof

test-lexico: all
	./compilador test/test_lexico_completo.txt
	./compilador test/test_lexico_eof_comentario.txt
	./compilador test/test_lexico_eofcadena.txt

test-sintaxis: all
	./compilador test/test_gramatica_validos.txt
	#./compilador test/test_gramatica_errores.txt
	#./compilador test/test_negativos.txt

test-eof: all
	./compilador test/test_prog_sin_end.txt
	./compilador test/test_prog_sin_puntoycoma.txt
	./compilador test/test_prog_sin_end_con_puntoycoma.txt


clean:
	rm -f compilador y.tab.c y.tab.h
