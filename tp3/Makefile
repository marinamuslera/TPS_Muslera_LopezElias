CXX = g++
CXXFLAGS = -std=c++17 -Wall -g

HEADERS = funcion_hash.h tabla_hash.h diccionario.h goleadores.h
FUENTES = funcion_hash.cpp goleadores.cpp
PROGRAMAS = goleadores explorar_hash tests

# Archivo de goles que usa make explorar. Se cambia con
# make explorar DATOS=datos/mundial_2022.txt
DATOS = datos/ejemplo.txt

.PHONY: all test ejemplo ejemplo-optativo explorar valgrind clean docker clean_docker

all: $(PROGRAMAS)

goleadores: main.cpp $(FUENTES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ main.cpp $(FUENTES)

%: %.cpp $(FUENTES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ $< $(FUENTES)

# Corre las pruebas.
test: tests
	./tests

# Corre el programa con el ejemplo y compara el resumen con el esperado.
ejemplo: goleadores
	./goleadores < datos/ejemplo.txt | head -n 2 | diff - datos/ejemplo_esperado.txt && echo "Salida correcta."

# Idem, incluyendo los dos listados (necesita CON_LISTADOS = true en main.cpp).
ejemplo-optativo: goleadores
	./goleadores < datos/ejemplo.txt | diff - datos/ejemplo_esperado_optativo.txt && echo "Salida correcta."

# Corre la exploracion de la funcion de hash sobre el archivo DATOS.
explorar: explorar_hash
	./explorar_hash < $(DATOS)

# Corre las pruebas y el ejemplo revisando el uso de memoria.
valgrind: tests goleadores
	valgrind --leak-check=full --error-exitcode=1 ./tests
	valgrind --leak-check=full --error-exitcode=1 ./goleadores < datos/ejemplo.txt

# Corre make valgrind dentro de un contenedor de Linux (para quien no tenga
# valgrind, por ejemplo en macOS). Borra los binarios antes y despues, porque
# los de Linux no corren en la maquina local y viceversa.
docker:
	docker build --tag udesa_tp3_tads .
	docker run --rm -v "$(CURDIR)":/tp udesa_tp3_tads bash -c 'make clean && make valgrind; estado=$$?; make clean; exit $$estado'

clean_docker:
	docker rmi -f udesa_tp3_tads

clean:
	rm -f $(PROGRAMAS)
