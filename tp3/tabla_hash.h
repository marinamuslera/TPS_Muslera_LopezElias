#ifndef TABLA_HASH_H
#define TABLA_HASH_H

#include <cstddef>

#include "funcion_hash.h"

#include <vector> // es el tipo que elegimos para representar la tabla de hash

// TAD TablaHash<K, V>: tabla de hash con direccionamiento cerrado.
//
// Guarda pares <clave, valor> repartidos en buckets: el par de una clave esta
// en el bucket numero funcion_hash(clave) % capacidad().
//
// Requisitos sobre los tipos:
//   - K: se puede copiar, comparar con == y existe funcion_hash(K).
//   - V: se puede copiar.
//
// Invariante de representacion:
//   - capacidad() = buckets.size() y buckets.size() >= 1
//   - Para cada par (clave,valor) guardado: el par esta en buckets[funcion_hash[clave] % buckets.size].
//   - Ninguna clave aparece mas de una vez (ni en el mismo bucket, ni en buckets distintos).
//   - cantidad_actual es la suma de los tamaños de todos los buckets.
//   - cantidad_actual <= FACTOR_DE_CARGA_MAXIMO * buckets.size().

// Complejidades: n es la cantidad de claves guardadas y c la capacidad.
// "Promedio" supone que la funcion de hash reparte las claves de manera
// uniforme y que calcularla (y comparar dos claves) cuesta O(1).
template <typename K, typename V>
class TablaHash {
  private:
    std::vector<std::vector<std::pair<K, V>>> buckets; // K y V porque es un template: no se sabe qué tipos de datos se pasan.
    size_t cantidad_actual; // es size_t porque vector.size() devuelve size_t

    // función privada de redimensionar (porque elegimos representación con vector):
    void redimensionar(); 

  public:
    static const size_t FACTOR_DE_CARGA_MAXIMO = 2;

    // Pre:  capacidad_inicial >= 1.
    // Post: crea una tabla vacia, con capacidad_inicial buckets.
    // Costo: O(capacidad_inicial).
    TablaHash(size_t capacidad_inicial);

    // Post: libera toda la memoria pedida por la tabla.
    // Costo: O(n + c) en peor caso.
    ~TablaHash();

    // Copiar una tabla copiaria los punteros y no su contenido: lo prohibimos.
    TablaHash(const TablaHash&) = delete;
    TablaHash& operator=(const TablaHash&) = delete;

    // Pre:  la clave NO esta en la tabla.
    // Post: la tabla tiene ademas el par <clave, valor>, y
    //       cantidad() <= FACTOR_DE_CARGA_MAXIMO * capacidad().
    // Costo: O(1) amortizado (en peor caso O(n), cuando toca redimensionar).
    void insertar(const K& clave, const V& valor);

    // Pre:  la clave esta en la tabla.
    // Post: la tabla ya no tiene la clave (ni su valor).
    // Costo: O(1) en promedio, O(n) en peor caso.
    void eliminar(const K& clave);

    // Pre:  -
    // Post: si la clave esta, devuelve un puntero a su valor (que se puede
    //       usar para leerlo o modificarlo); si no, devuelve nullptr.
    //       El puntero deja de ser valido al insertar o eliminar.
    // Costo: O(1) en promedio, O(n) en peor caso.
    V* buscar(const K& clave);

    // Post: devuelve la cantidad de claves guardadas. Costo: O(1).
    size_t cantidad();

    // Post: devuelve la cantidad de buckets. Costo: O(1).
    size_t capacidad();

    // Post: devuelve la cantidad de claves que comparten bucket con alguna
    //       clave que llego antes, es decir:
    //       cantidad() - (cantidad de buckets que no estan vacios).
    // Costo: O(c) en peor caso.
    size_t colisiones();

    // Iterador de la tabla: recorre todos los pares <clave, valor>, cada uno
    // exactamente una vez, en algun orden (ver TAD Iterador en el enunciado).
    // Deja de ser valido si se inserta o se elimina en la tabla.
    //
    // Invariante de representacion:
    //  - tabla apunta a la Tabla de Hash que se recorre. 
    //  - 0 <= bucket_actual <= tabla->capacidad()
    //  - Si bucket_actual < tabla->capacidad(), entonces posicion_actual < tabla->buckets[bucket_actual].size(): el iterador esta parado sobre un par real (nunca sobre un bucket vacio)
    //  - Si bucket_actual == tabla->capacidad(), el iterador esta al final. 
    //  - Todos los pares de buckets anteriores a bucket_actual, y los de posiciones anteriores a posicion_actual en ese bucket, ya fueron recorridos. 
    class Iterador {
      private:
        TablaHash *tabla; // puntero a la tabla hash
        size_t bucket_actual; // ¿en qué bucket estoy?
        size_t posicion_actual; // ¿en qué posición dentro del bucket estoy?

        // Para que crear_iterador() pueda armar el iterador.
        friend class TablaHash;

      public:
        // Pre:  -
        // Post: indica si quedan pares por recorrer.
        // Costo: O(1).
        bool hay_mas() {
            return bucket_actual<tabla->capacidad(); // con -> porque tabla es un puntero, no un elemento.
        }

        // Pre:  hay_mas().
        // Post: devuelve la clave del par actual, sin modificar el iterador.
        // Costo: O(1).
        K clave() {
            return tabla->buckets[bucket_actual][posicion_actual].first;
        }

        // Pre:  hay_mas().
        // Post: devuelve el valor del par actual, sin modificar el iterador.
        // Costo: O(1).
        V valor() {
            return tabla->buckets[bucket_actual][posicion_actual].second;
        }

        // Pre:  hay_mas().
        // Post: el iterador pasa al par siguiente (o al final, si no hay mas).
        // Costo: O(c) en peor caso. Recorrer la tabla entera (crear el
        //        iterador y avanzar hasta el final) cuesta O(n + c).
        void avanzar() {
            posicion_actual++;
            if (posicion_actual >= tabla->buckets[bucket_actual].size()) { //paso al próximo bucket no vacio (por eso el while)
                bucket_actual++;
                posicion_actual=0;
                while (bucket_actual < tabla->capacidad() && tabla->buckets[bucket_actual].size()==0) {
                    bucket_actual++;
                }
            }
        }
    };

    // Pre:  -
    // Post: devuelve un iterador al primer par de la tabla (si la tabla esta
    //       vacia, el iterador ya esta al final).
    // Costo: O(c) en peor caso.
    Iterador crear_iterador();
};

// ---------------------------------------------------------------------------
// Implementacion. Al ser una clase template, va en el mismo .h.
// ---------------------------------------------------------------------------

template <typename K, typename V>
TablaHash<K, V>::TablaHash(size_t capacidad_inicial) {
    buckets = std::vector<std::vector<std::pair<K, V>>>(capacidad_inicial); //NO tengo que declarar buckets porque ya la declaré en private
    cantidad_actual=0;
}

template <typename K, typename V>
TablaHash<K, V>::~TablaHash() {
    // no pongo nada porque elegimos la representación con std::vector, y se encargan de liberar su memoria solos al llamar al destructor.
}

template <typename K, typename V>
void TablaHash<K, V>::insertar(const K& clave, const V& valor) {
    size_t indice = funcion_hash(clave)%capacidad(); // función_hash está en un include al principio del archivo; %capacidad() es para que caiga en el vector.
    (buckets[indice]).push_back({clave, valor}); 
    cantidad_actual++;
    if (cantidad_actual > FACTOR_DE_CARGA_MAXIMO * capacidad()) {
        redimensionar();
    }
} 

template <typename K, typename V>
void TablaHash<K, V>::redimensionar() {
    size_t nueva_capacidad = capacidad()*2;
    std::vector<std::vector<std::pair<K, V>>> buckets_nuevos = std::vector<std::vector<std::pair<K, V>>>(nueva_capacidad); //SI tengo que declarar buckets_nuevos porque es la primera vez que aparece
    for (std::vector<std::pair<K, V>>& bucket: buckets) { //para cada bucket
        for (std::pair<K, V>& elemento : bucket) { //para cada tupla del bucket
            size_t indice = funcion_hash(elemento.first)%nueva_capacidad; // .first es el primer elemento del par
            (buckets_nuevos[indice]).push_back(elemento);
        }
    }
    buckets= buckets_nuevos;
}

template <typename K, typename V>
void TablaHash<K, V>::eliminar(const K& clave) {
    size_t indice = funcion_hash(clave)%capacidad();
    size_t posicion_a_eliminar;
    for (size_t i=0; i<buckets[indice].size(); i++) { // recorro todos los pares dentro del bucket
        if (buckets[indice][i].first == clave) {
            posicion_a_eliminar = i;
            break;
        }
    }
    buckets[indice].erase(buckets[indice].begin()+posicion_a_eliminar); //PREGUNTAR (lo de erase); begin()+p= desde el principio, posición p; erase() necesita la posición de p, y buckets[indice][p] te da el contenido 
    cantidad_actual--;
}

template <typename K, typename V>
V* TablaHash<K, V>::buscar(const K& clave) {
    size_t indice = funcion_hash(clave)%capacidad();
    for (std::pair<K, V> &elemento : buckets[indice]) { // importante: le pongo el & a "elemento" porque hay que pasarlo por referencia para obtener su puntero.
        if (elemento.first == clave) {
            return &elemento.second; // &=dirección de memoria
        }
    }
    return nullptr;
}

template <typename K, typename V>
size_t TablaHash<K, V>::cantidad() {
    return cantidad_actual;
}

template <typename K, typename V>
size_t TablaHash<K, V>::capacidad() {
    return buckets.size();
}

template <typename K, typename V>
size_t TablaHash<K, V>::colisiones() {
    size_t buckets_no_vacios = 0; //
    for (std::vector<std::pair<K, V>>& bucket: buckets) {
        if (bucket.size() != 0) {
            buckets_no_vacios++;
        }
    }
    return cantidad_actual - buckets_no_vacios;
}

// El "typename" le avisa al compilador que TablaHash<K, V>::Iterador es un tipo.
template <typename K, typename V>
typename TablaHash<K, V>::Iterador TablaHash<K, V>::crear_iterador() {
    Iterador it;
    it.tabla = this;
    it.bucket_actual = 0;
    it.posicion_actual = 0;
    while (it.bucket_actual<capacidad() && buckets[it.bucket_actual].size()==0) {
        it.bucket_actual++;
    }
    return it;
}

#endif
