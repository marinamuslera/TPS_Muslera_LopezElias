#include "funcion_hash.h"

size_t funcion_hash(const std::string& clave) {
    size_t hash = 0;
    for (char c : clave) {
        hash = hash * 31 + (unsigned char)c;
    }
    return hash;
}

size_t funcion_hash(int clave) {
    return (size_t)clave;
}
