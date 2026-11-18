#include "funcion_hash.h"

size_t funcion_hash(const std::string& clave) {
    size_t hash = 0;
    for (char c : clave) {
        hash = hash * 31 + (unsigned char)c; // los números impares y primos convienen para evitar más colisiones. 
    }
    return hash;
}

size_t funcion_hash(int clave) {
    return (size_t)clave;
}
