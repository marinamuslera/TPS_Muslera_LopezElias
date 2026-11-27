#include <fstream>
#include <functional>
#include <iostream>
#include <queue>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "graphClass.h"

using namespace std;

// Vértices: nombre de la ciudad, con su provincia como dato.
// Aristas: distancia aproximada en km. Cada ruta está en los dos sentidos.
using GrafoCiudades = Graph<string, int>;

// ================================================================
//                     CARGA DEL GRAFO (ya dada)
// ================================================================

static string strip(const string& s) {
    const char* ws = " \t\r\n";
    size_t b = s.find_first_not_of(ws);
    if (b == string::npos) return "";
    size_t e = s.find_last_not_of(ws);
    return s.substr(b, e - b + 1);
}

static vector<string> split(const string& l) {
    vector<string> campos;
    stringstream ss(l);
    string campo;
    while (getline(ss, campo, ',')) campos.push_back(strip(campo));
    return campos;
}

static bool cargarCiudades(GrafoCiudades& g, const string& path) {
    ifstream file(path);
    if (!file) {
        cerr << "No se pudo abrir el archivo: " << path << "\n";
        return false;
    }
    string l;
    getline(file, l);  // encabezado: ciudad,provincia,lat,lon
    while (getline(file, l)) {
        l = strip(l);
        if (l.empty()) continue;
        vector<string> c = split(l);
        g.add_vertex(c[0], c[1]);
    }
    return true;
}

static bool cargarRutas(GrafoCiudades& g, const string& path) {
    ifstream file(path);
    if (!file) {
        cerr << "No se pudo abrir el archivo: " << path << "\n";
        return false;
    }
    string l;
    getline(file, l);  // encabezado: ciudad1,ciudad2,distancia_km
    while (getline(file, l)) {
        l = strip(l);
        if (l.empty()) continue;
        vector<string> c = split(l);
        int distancia = stoi(c[2]);
        g.add_edge(c[0], c[1], distancia);
        g.add_edge(c[1], c[0], distancia);
    }
    return true;
}

// Posición de la ciudad en g.get_vertices() (-1 si no está).
int indiceDe(const GrafoCiudades& g, const string& ciudad) {
    vector<string> ciudades = g.get_vertices();
    for (int i = 0; i < (int)ciudades.size(); i++) {
        if (ciudades[i] == ciudad) return i;
    }
    return -1;
}

// Arma un mapa ciudad: posicion en g.get_vertices(), para no usar indiceDe
// adentro de los loops (indiceDe recorre todo el vector, es O(n) cada vez).
unordered_map<string, int> mapaDeIndices(const GrafoCiudades& g) {
    vector<string> ciudades = g.get_vertices();
    unordered_map<string, int> indice;
    for (int i = 0; i < (int)ciudades.size(); i++) {
        indice[ciudades[i]] = i;
    }
    return indice;
}


// ================================================================
//                  EJERCICIO 1: COMPONENTES CONEXAS
// ================================================================

vector<vector<string>> componentesConexas(const GrafoCiudades& g) {
    // BFS desde cada ciudad que todavia no fue visitada: todo lo que alcanza
    // ese BFS es una componente. Cada vertice entra una vez a la cola.
    vector<string> ciudades = g.get_vertices();
    unordered_map<string, bool> visitada;
    vector<vector<string>> componentes;
    for (const string& inicio : ciudades) {
        if (visitada[inicio]) continue;
        // nueva componente: BFS desde inicio
        vector<string> componente;
        queue<string> cola;
        cola.push(inicio);
        visitada[inicio] = true;
        while (!cola.empty()) {
            string actual = cola.front();
            cola.pop();
            componente.push_back(actual);
            for (const string& vecino : g.get_neighbors(actual)) {
                if (!visitada[vecino]) {
                    visitada[vecino] = true;  // se marca al encolar, asi no entra dos veces
                    cola.push(vecino);
                }
            }
        }
        componentes.push_back(componente);
    }
    return componentes;
}

// ================================================================
//        EJERCICIO 2: CAMINO MÍNIMO CON PESOS DE TODOS CON TODOS
// ================================================================

const int INFINITO = 1000000000;

// Dijkstra desde una ciudad: devuelve la distancia minima a todas las demas.
// BFS no sirve porque cuenta cantidad de rutas y no km: un camino con menos
// rutas puede ser mas largo en km. Dijkstra funciona porque las distancias
// nunca son negativas. Con cola de prioridad cuesta O((n + m) log n).
vector<int> dijkstra(const GrafoCiudades& g, const unordered_map<string, int>& indice,
                     const string& origen) {
    vector<string> ciudades = g.get_vertices();
    int n = (int)ciudades.size();
    vector<int> dist(n, INFINITO);
    vector<bool> cerrada(n, false);  // ya tiene su distancia definitiva
    // la cola guarda pares (distancia, indice de la ciudad); con greater<>
    // el de menor distancia queda arriba
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> cola;
    int o = indice.at(origen);
    dist[o] = 0;
    cola.push({0, o});
    while (!cola.empty()) {
        int d = cola.top().first;
        int u = cola.top().second;
        cola.pop();
        if (cerrada[u]) continue;  // entrada vieja de la cola, ya se proceso
        cerrada[u] = true;

        for (const string& vecino : g.get_neighbors(ciudades[u])) {
            int v = indice.at(vecino);
            int peso = g.get_edge_data(ciudades[u], vecino).value();
            if (d + peso < dist[v]) {  // relajacion
                dist[v] = d + peso;
                cola.push({dist[v], v});
            }
        }
    }
    return dist;
}

// dist[i][j]: distancia mínima en km entre la ciudad i y la ciudad j, con
// las ciudades numeradas en el orden de g.get_vertices() (INFINITO si no hay
// camino).
vector<vector<int>> distanciasTodosConTodos(const GrafoCiudades& g) {
    // Dijkstra desde cada una de las n ciudades: O(n (n + m) log n) en total
    vector<string> ciudades = g.get_vertices();
    unordered_map<string, int> indice = mapaDeIndices(g);
    vector<vector<int>> dist;
    for (const string& origen : ciudades) {
        dist.push_back(dijkstra(g, indice, origen));
    }
    return dist;
}

// ================================================================
//                     EJERCICIO 3: TRIÁNGULOS
// ================================================================

int contarTriangulos(const GrafoCiudades& g) {
    // TODO: implementar
    return 0;
}

// ================================================================
//                EJERCICIO 4: DIÁMETRO CON PESOS
// ================================================================

class Diametro {
public:
    int distancia;
    string ciudad1;
    string ciudad2;
};

Diametro diametro(const GrafoCiudades& g, const vector<vector<int>>& dist) {
    // Mayor distancia minima entre dos ciudades, usando la matriz del
    // ejercicio 2. Los pares sin camino (INFINITO) se saltean: si no, el
    // diametro daria infinito. O sea que es el diametro tomando solo los pares
    // conectados. Recorre la matriz una vez: O(n^2).
    vector<string> ciudades = g.get_vertices();
    Diametro mayor = {0, "", ""};
    for (int i = 0; i < (int)dist.size(); i++) {
        for (int j = i + 1; j < (int)dist.size(); j++) {  // j > i: cada par una vez
            if (dist[i][j] != INFINITO && dist[i][j] > mayor.distancia) {
                mayor = {dist[i][j], ciudades[i], ciudades[j]};
            }
        }
    }
    return mayor;
}

// ================================================================
//          EJERCICIO 5: RANDOM WALKS Y LA CIUDAD MÁS CENTRAL
// ================================================================

// visitas[c]: cantidad de veces que los random walks pasaron por c.
using Visitas = unordered_map<string, int>;

Visitas randomWalks(const GrafoCiudades& g, int cantidad, int largo) {
    // TODO: implementar
    return {};
}

Visitas randomWalksPorDistancia(const GrafoCiudades& g, int cantidad, int largo) {
    // TODO: implementar
    return {};
}

// ================================================================
//                   EJERCICIO 6: ANÁLISIS LIBRE
// ================================================================

void analisisLibre(const GrafoCiudades& g) {
    // TODO: implementar
    return;
}

// ================================================================
//                              MAIN
// ================================================================

int main(int argc, char* argv[]) {
    string pathCiudades = argc > 1 ? argv[1] : "ciudades_argentina.csv";
    string pathRutas = argc > 2 ? argv[2] : "grafo_ciudades.csv";

    GrafoCiudades g;
    if (!cargarCiudades(g, pathCiudades)) return 1;
    if (!cargarRutas(g, pathRutas)) return 1;
    cout << "Ciudades cargadas: " << g.get_vertices().size() << "\n\n";

    cout << "=== Ejercicio 1 ===\n";
    vector<vector<string>> componentes = componentesConexas(g);
    cout << "Componentes conexas: " << componentes.size() << "\n";
     int masGrande = 0;  // posicion de la componente mas grande
    for (int i = 1; i < (int)componentes.size(); i++) {
        if (componentes[i].size() > componentes[masGrande].size()) masGrande = i;
    }
    cout << "Tamaño de la más grande: " << componentes[masGrande].size() << " ciudades\n";
    cout << "Ciudades fuera de ella:\n";
    for (int i = 0; i < (int)componentes.size(); i++) {
        if (i == masGrande) continue;
        cout << "  Componente de " << componentes[i].size() << ":";
        for (const string& c : componentes[i]) {
            cout << " " << c << " (" << g.get_vertex_data(c).value_or("?") << ")";
        }
        cout << "\n";
    }

    cout << "\n=== Ejercicio 2 ===\n";
    vector<vector<int>> dist = distanciasTodosConTodos(g);
     int ba = indiceDe(g, "Buenos Aires");
    for (string destino : {"La Quiaca", "Ushuaia"}) {
        int j = indiceDe(g, destino);
        cout << "Buenos Aires - " << destino << ": ";
        if (dist[ba][j] == INFINITO) cout << "no hay camino\n";
        else cout << dist[ba][j] << " km\n";
    }

    cout << "\n=== Ejercicio 3 ===\n";
    cout << "Triángulos: " << contarTriangulos(g) << "\n";

    cout << "\n=== Ejercicio 4 ===\n";
    Diametro d = diametro(g, dist);
    cout << "Diámetro: " << d.distancia << " km, entre " << d.ciudad1 << " y "
              << d.ciudad2 << "\n";

    cout << "\n=== Ejercicio 5 ===\n";
    Visitas visitas = randomWalks(g, 10000, 100);
    Visitas visitasPorDistancia = randomWalksPorDistancia(g, 10000, 100);
    // TODO: las 10 ciudades más visitadas en cada caso, y las 10 de mayor grado

    cout << "\n=== Ejercicio 6 ===\n";
    analisisLibre(g);

    return 0;
}
