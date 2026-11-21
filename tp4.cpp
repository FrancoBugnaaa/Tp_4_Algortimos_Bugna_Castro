#include <fstream>
#include <iostream>
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

// ================================================================
//                  FUNCIONES AUXILIARES
// ================================================================

void dfs(const GrafoCiudades& g, string ciudad, vector<bool>& visitado, vector<string>& componente) {

    int i = indiceDe(g, ciudad);
    visitado[i] = true;

    componente.push_back(ciudad);

    vector<string> vecinos = g.get_neighbors(ciudad);

    for (int j = 0; j < (int)vecinos.size(); j++) {

        int indiceVecino = indiceDe(g, vecinos[j]);

        if (!visitado[indiceVecino]) {
            dfs(g, vecinos[j], visitado, componente);
        }
    }
}

// ================================================================
//                  EJERCICIO 1: COMPONENTES CONEXAS
// ================================================================

vector<vector<string>> componentesConexas(const GrafoCiudades& g) {
    
    vector<string>ciudades = g.get_vertices();
    vector<vector<string>> componentes;

    size_t n = ciudades.size();
    vector<bool> visitado(n, false);
    int cantidad_componentes = 0;
    vector<string> Componente_Mayor;

    for(int i = 0; i < n;i++){
        if(!visitado[i]){
            vector<string> componente;
            dfs(g,ciudades[i],visitado,componente);
            componentes.push_back(componente);
            cantidad_componentes++;
            if(componente.size()> Componente_Mayor.size()){
                Componente_Mayor = componente;
            }
        }
    }
    cout << "Cantidad de componentes: "
         << cantidad_componentes << endl;

    cout << "Tamaño de la componente mayor: "
         << Componente_Mayor.size() << endl;

    cout << "Ciudades fuera de la componente mayor:" << endl;

    for (int i = 0; i < (int)ciudades.size(); i++) {

        bool pertenece = false;

        for (int j = 0; j < (int)Componente_Mayor.size(); j++) {
            if (ciudades[i] == Componente_Mayor[j]) {
                pertenece = true;
                break;
            }
        }

        if (!pertenece) {
            cout << ciudades[i] << endl;
        }
    }
    return componentes;
}

// ================================================================
//        EJERCICIO 2: CAMINO MÍNIMO CON PESOS DE TODOS CON TODOS
// ================================================================

const int INFINITO = 1000000000;

// dist[i][j]: distancia mínima en km entre la ciudad i y la ciudad j, con
// las ciudades numeradas en el orden de g.get_vertices() (INFINITO si no hay
// camino).

vector<vector<int>> distanciasTodosConTodos(const GrafoCiudades& g) {
    // TODO: implementar

    vector<string> vertices = g.get_vertices();
    int tamano_vertices = static_cast<int>(vertices.size());

    unordered_map<string, int> vertice_indice;
    vertice_indice.reserve(tamano_vertices);

    for (int i = 0, i < tamano_vertices, i++) {
        vertice_indice[vertices[i]] = i;
    }

    vector<vector<int>> dist(tamano_vertices, vector<int>(tamano_vertices, INFINITO)); // Inicializamos una matriz (n (Filas) x m (Columnas)) 
                                                                                       // con todas las casillas con valor INFINITO.

    for (int i = 0, i < tamano_vertices; i++) {
        dist[i] = Dijkstra(g, i, vertices, vertice_indice);
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
    // TODO: implementar
    return {0, "", ""};
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
    // TODO: tamaño de la más grande y ciudades que quedan fuera de ella

    cout << "\n=== Ejercicio 2 ===\n";
    vector<vector<int>> dist = distanciasTodosConTodos(g);
    // TODO: distancias Buenos Aires - La Quiaca y Buenos Aires - Ushuaia

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
