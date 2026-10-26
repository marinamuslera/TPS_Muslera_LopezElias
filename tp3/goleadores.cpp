#include "goleadores.h"

#include <iostream>

Goleadores::Goleadores() {
    // goles_por_jugador arranca vacio solo (constructor de Diccionario)
    // maximo arranca como string vacio: no se consulta mientras no haya. 
    //  jugadores (Pre de maximo_goleador)
}

void Goleadores::anotar_gol(const std::string& jugador) {
    int goles_nuevos = goles(jugador)+1;
    goles_por_jugador.insertar(jugador, goles_nuevos);
    //Mantener el invariante de maximo
    if (goles_por_jugador.cantidad() ==1) {
        //Hay un solo jugador con goles; es el maximo. 
        maximo = jugador;
    } else if (goles_nuevos > goles(maximo)) {
        //Con > estricto, en un empate maximo no cambia: queda el que llego
        // primero a esa cantidad.
        maximo = jugador;
    }
}

int Goleadores::goles(const std::string& jugador) {
    if (goles_por_jugador.pertenece(jugador)){
        return goles_por_jugador.obtener(jugador);
    }
    return 0;
}

size_t Goleadores::cantidad_de_jugadores() {
    return goles_por_jugador.cantidad();
}

std::string Goleadores::maximo_goleador() {
    return maximo;
}

void Goleadores::imprimir_por_nombre() {
    // OPTATIVO
}

void Goleadores::imprimir_por_goles() {
    // OPTATIVO
}
