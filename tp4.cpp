#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <queue>  // Agregamos la librería "queue" para poder utilizar la Cola de Prioridad en la función auxiliar del algoritmo Dijkstra utilizado.
#include <algorithm> // Agregamos la librería "algorithm" para poder utilizar el método "reverse()" en la función auxiliar "caminoBFS" implmentada para el quinto análisis del ejercicio 6.

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

vector<int> Dijkstra(const GrafoCiudades& g, const vector<string>& vertices, const unordered_map<string, size_t>& indice_vertice, size_t origen) {

    // Función Auxiliar que aplica el algoritmo de Dijkstra desde un vértice origen hacia todos los demás.

    const int INFINITO = 1000000000;

    size_t n = vertices.size();
    vector<int> dist(n, INFINITO);

    priority_queue<
    pair<int, int>,
    vector<pair<int, int>>,
    greater<pair<int, int>>
    > Cola_de_Prioridad;

    dist[origen] = 0;
    Cola_de_Prioridad.push({0, origen});

    while (Cola_de_Prioridad.empty() == false) {

        int distancia_actual = Cola_de_Prioridad.top().first;
        int u = Cola_de_Prioridad.top().second;

        Cola_de_Prioridad.pop();

        if (distancia_actual > dist[u]) {
            continue;
        }

        for (const string& vecino: g.get_neighbors(vertices[u])) {

            size_t v = indice_vertice.at(vecino);
            int peso_arista = *g.get_edge_data(vertices[u], vecino);
            int nueva_distancia = distancia_actual + peso_arista;

            if (nueva_distancia < dist[v]) {
                dist[v] = nueva_distancia;
                Cola_de_Prioridad.push({nueva_distancia, v});
            }
        }
    }

    return dist;

}

vector<vector<string>> obtenerComponentesConexas(const GrafoCiudades& g) {

    // Función Auxiliar utilizada para el segundo análisis del ejercicio 6 (Similar a la función del ejercicio 1 (Contar Componentes Conexas)) pero
    // sin su análisis específico para dicho ejercicio.
    
    vector<string>ciudades = g.get_vertices();
    vector<vector<string>> componentes;

    size_t n = ciudades.size();
    vector<bool> visitado(n, false);

    for(size_t i = 0; i < n;i++){
        if(!visitado[i]){
            vector<string> componente;
            dfs(g,ciudades[i],visitado,componente);
            componentes.push_back(componente);
            }
        }

    return componentes;

}

//

vector<string> caminoBFS (const GrafoCiudades& g, const string& origen, const string& destino) {

    queue<string> pendientes_por_visitar;

    unordered_map<string, bool> visitados;
    unordered_map<string, string> padre;

    visitados[origen] = true;

    pendientes_por_visitar.push(origen);

    while (pendientes_por_visitar.empty() == false) {
        string actual = pendientes_por_visitar.front();
        pendientes_por_visitar.pop();

        if (actual == destino) {
            break;
        }

        vector<string> vecinos = g.get_neighbors(actual);

        for (const string& vecino : vecinos) {
            if (visitados[vecino] == false) {

                visitados[vecino] = true;
                padre[vecino] = actual;

                pendientes_por_visitar.push(vecino);
            }
        }
    }

    if (visitados[destino] == false) {
      return {};
    }

    vector<string> camino;
    string actual = destino;

    while (actual != origen) {
        camino.push_back(actual);
        actual = padre.at(actual);
    }

    camino.push_back(origen);

    reverse(camino.begin(), camino.end());

    return camino;
}

vector<string> caminoDijkstra(const GrafoCiudades& g, const vector<string>& vertices, const unordered_map<string, size_t>& indice_vertice, size_t origen, size_t destino) {

    // 

    const int INFINITO = 1000000000;

    size_t n = vertices.size();
    vector<int> dist(n, INFINITO);
    vector<size_t> padre(n, n);

    priority_queue<
    pair<int, int>,
    vector<pair<int, int>>,
    greater<pair<int, int>>
    > Cola_de_Prioridad;

    dist[origen] = 0;
    Cola_de_Prioridad.push({0, origen});

    while (Cola_de_Prioridad.empty() == false) {

        int distancia_actual = Cola_de_Prioridad.top().first;
        size_t u = Cola_de_Prioridad.top().second;

        Cola_de_Prioridad.pop();

        if (distancia_actual > dist[u]) {
            continue;
        }

        if (u == destino) {
            break;
        }

        for (const string& vecino: g.get_neighbors(vertices[u])) {

            size_t v = indice_vertice.at(vecino);
            int peso_arista = *g.get_edge_data(vertices[u], vecino);
            int nueva_distancia = distancia_actual + peso_arista;

            if (nueva_distancia < dist[v]) {
                dist[v] = nueva_distancia;
                padre[v] = u;
                Cola_de_Prioridad.push({nueva_distancia, v});
            }
        }
    }

    if (dist[destino] == INFINITO) {
        return {};
    }

    vector<string> camino;
    size_t actual = destino;

    while (actual != origen) {
        camino.push_back(vertices[actual]);
        actual = padre[actual];
    }

    camino.push_back(vertices[origen]);

    reverse(camino.begin(), camino.end()); // Invertimos el orden para obtener el formato: origen -> destino.

    return camino;
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

    for(size_t i = 0; i < n;i++){
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
    size_t cantidad_vertices = vertices.size();

    vector<vector<int>> dist(cantidad_vertices, vector<int>(cantidad_vertices, INFINITO));

    unordered_map<string,size_t> indice;

    for (size_t i = 0; i < cantidad_vertices; i++) {
        indice[vertices[i]] = i;
    }

    for (size_t i = 0; i < cantidad_vertices; i++) {
        dist[i] = Dijkstra(g, vertices, indice, i);
    }

    return dist;
}

// ================================================================
//                     EJERCICIO 3: TRIÁNGULOS
// ================================================================

int contarTriangulos(const GrafoCiudades& g) {
    vector<string> ciudades = g.get_vertices();
    size_t n = ciudades.size();

    int cantidad_triangulos = 0;

    for (size_t i = 0; i < n; i++){
        for(size_t j = i+1; j < n; j++){
            for(size_t k = j+1; k < n; k++){
                if(g.edge_exists(ciudades[i],ciudades[j]) && g.edge_exists(ciudades[j],ciudades[k]) && g.edge_exists(ciudades[k],ciudades[i])){
                    cantidad_triangulos++;
                }
            }
        }
    }
    return cantidad_triangulos;
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

    vector<string> vertices = g.get_vertices();
    size_t n = vertices.size();

    Diametro resultado = {0, "", ""};

    for (size_t i = 0; i < n; i++) {
        for (size_t j = i + 1; j < n; j++) {

            if (dist[i][j] != 1000000000 && dist[i][j] > resultado.distancia) {

                resultado.distancia = dist[i][j];
                resultado.ciudad1 = vertices[i];
                resultado.ciudad2 = vertices[j];
            }
        }
    }

    return resultado;
}

// ================================================================
//          EJERCICIO 5: RANDOM WALKS Y LA CIUDAD MÁS CENTRAL
// ================================================================

// visitas[c]: cantidad de veces que los random walks pasaron por c.
using Visitas = unordered_map<string, int>;

Visitas randomWalks(const GrafoCiudades& g, int cantidad, int largo) {

    // TODO: implementar

    Visitas visitas;

    vector<string> ciudades = g.get_vertices();

    if (ciudades.empty()) return visitas;

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(0, (int)ciudades.size() - 1);

    for(int i = 0; i < cantidad; i++){
        
        string ciudad = ciudades[dist(gen)];

        visitas[ciudad]++;

        for(int j = 0; j < largo; j++){
            vector <string> vecinos = g.get_neighbors(ciudad);

            if (vecinos.empty()) break;

            uniform_int_distribution<int> distVecino(0, (int)vecinos.size() - 1);

            ciudad = vecinos[distVecino(gen)];

            visitas[ciudad]++;
        }
    }

    return visitas;
}

Visitas randomWalksPorDistancia(const GrafoCiudades& g, int cantidad, int largo) {

    // TODO: implementar

    Visitas visitas;
    vector<string> ciudades = g.get_vertices();

    if (ciudades.empty()) return visitas;

    random_device rd;
    mt19937 gen(rd());

    uniform_int_distribution<int> dist(0, (int)ciudades.size() - 1);

    for (int i = 0; i < cantidad; i++) {

        string ciudad = ciudades[dist(gen)];

        visitas[ciudad]++;

        for (int j = 0; j < largo; j++) {

            vector<string> vecinos = g.get_neighbors(ciudad);

            if (vecinos.empty()) break;

            vector<double> pesos;

            for (int k = 0; k < (int)vecinos.size(); k++) {

                int distancia = g.get_edge_data(ciudad, vecinos[k]).value();

                double peso = 1.0 / distancia;

                pesos.push_back(peso);
            }

            discrete_distribution<int> distVecino(pesos.begin(), pesos.end());

            ciudad = vecinos[distVecino(gen)];

            visitas[ciudad]++;
        }
    }

    return visitas;
}

// ================================================================
//                   EJERCICIO 6: ANÁLISIS LIBRE
// ================================================================

void analisisLibre(const GrafoCiudades& g, const vector<vector<int>>& dist) {

    // TODO: implementar

    vector<string> vertices = g.get_vertices();
    unordered_map<string, size_t> indice_vertice;

    // Primer Análisis: Consultar la distancia mínima entre Buenos Aires y Comodoro Rivadavia.
    
    for (size_t i = 0; i < vertices.size(); i++) {
        indice_vertice[vertices[i]] = i;
    }

    size_t buenos_aires = indice_vertice.at("Buenos Aires");
    size_t comodoro_rivadavia = indice_vertice.at("Comodoro Rivadavia");

    if (dist[buenos_aires][comodoro_rivadavia] == 1000000000) {
        cout << "No existe un camino entre Buenos Aires y Comodoro Rivadavia." << endl;
    }

    else {

        cout << "Distancia mínima entre Buenos Aires y Comodoro Rivadavia: " << dist[buenos_aires][comodoro_rivadavia] << " km." << endl;
        cout << endl;
    }

    // Segundo Análisis: Determinar cuáles son las ciudades que pertenecen a cada componente conexa del grafo.

    vector<vector<string>> componentes = obtenerComponentesConexas(g);

        cout << "Componentes Conexas" << endl;
        cout << "===================" << endl;

        cout << endl;

        for (size_t i = 0; i < componentes.size(); i++) {
            cout << "Componente Conexa " << i + 1 << ":" << endl;

            for (const string& ciudad : componentes[i]) {
                cout << ciudad << endl;
            }

            cout << endl;
        }

    // Tercer Análisis: Calcular la cantidad mínima de aristas necesaria para que el grafo sea conexo.

    size_t cantidad_de_aristas = componentes.size() - 1;

    cout <<"Cantidad mínima de aristas para que el grafo sea conexo: " << cantidad_de_aristas << endl;

    cout << endl;

    // Cuarto Análisis: Calcular la ciudad más aislada (menor cantidad de vecinos) y la ciudad más conectada (mayor cantidad de vecinos).

    string ciudad_mas_aislada = vertices[0];
    size_t menor_grado =  g.get_neighbors(vertices[0]).size();

    for (const string& ciudad : vertices) {
        size_t grado = g.get_neighbors(ciudad).size();

        if (grado < menor_grado) {
            menor_grado = grado;
            ciudad_mas_aislada = ciudad;
        }
    }

    cout << "Ciudad más aislada: " << ciudad_mas_aislada << endl;
    cout << "Grado de " << ciudad_mas_aislada << ": " << menor_grado << endl;

    cout << endl;

    string ciudad_mas_conectada = vertices[0];
    size_t mayor_grado =  g.get_neighbors(vertices[0]).size();

    for (const string& ciudad : vertices) {
        size_t grado = g.get_neighbors(ciudad).size();

        if (grado > mayor_grado) {
            mayor_grado = grado;
            ciudad_mas_conectada = ciudad;
        }
    }

    cout << "Ciudad más conectada: " << ciudad_mas_conectada << endl;
    cout << "Grado de " << ciudad_mas_conectada << ": " << mayor_grado << endl;

    cout << endl;

    // Quinto Análisis: Comparar el camino con menos rutas (BFS) con el camino más corto en km entre dos ciudades lejanas.

    vector<string> camino_con_BFS = caminoBFS(g, "Buenos Aires", "Comodoro Rivadavia");
    vector<string> camino_con_Dijkstra = caminoDijkstra(g, vertices, indice_vertice, indice_vertice.at("Buenos Aires"), indice_vertice.at("Comodoro Rivadavia"));

    size_t cantidad_aristas_bfs = camino_con_BFS.size() - 1;
    size_t cantidad_aristas_dijkstra = camino_con_Dijkstra.size() - 1;

    int distancia_BFS = 0;

    for (size_t i = 0; i + 1 < camino_con_BFS.size(); i++) {

        const string ciudad_1 = camino_con_BFS[i];
        const string ciudad_2 = camino_con_BFS[i + 1];

        if (g.edge_exists(ciudad_1, ciudad_2) == false) {
            cout << "No se ha encotrado la arista entre " << ciudad_1 << " y " << ciudad_2 << endl;
            continue;
        }

        auto peso = g.get_edge_data(ciudad_1, ciudad_2);

        if (peso.has_value() == false) {
            cout << "La arista existe pero no tiene un peso asociado"  << endl;
            continue;
        }

        distancia_BFS += peso.value();
    }

    int distancia_Dijkstra = 0;

    for (size_t i = 0; i + 1 < camino_con_Dijkstra.size(); i++) {

        const string& ciudad_1 = camino_con_Dijkstra[i];
        const string& ciudad_2 = camino_con_Dijkstra[i + 1];

        if (g.edge_exists(ciudad_1, ciudad_2) == false) {
            cout << "No se ha encotrado la arista entre " << ciudad_1 << " y " << ciudad_2 << endl;
            continue;
        }

        auto peso = g.get_edge_data(ciudad_1, ciudad_2);

        if (peso.has_value() == false) {
            cout << "La arista existe pero no tiene un peso asociado"  << endl;
            continue;
        }

        distancia_Dijkstra += peso.value();
    }

    cout << "Comparación de caminos" << endl;
    cout << "======================" << endl;

    cout << endl;

    cout << "Cantidad de rutas (BFS) entre Buenos Aires y Comodoro Rivadavia: " << cantidad_aristas_bfs << endl;
    cout << "Cantidad de rutas (Dijkstra) entre Buenos Aires y Comodoro Rivadavia: " << cantidad_aristas_dijkstra << endl;

    cout << endl;

    cout << "Distancia Total (BFS) entre Buenos Aires y Comodoro Rivadavia: " << distancia_BFS << " km." << endl;
    cout << endl;
    cout << "Camino encontrado por BFS: " << endl;

    for (size_t i = 0; i < camino_con_BFS.size(); i++) {
        cout << camino_con_BFS[i];

        if (i + 1 < camino_con_BFS.size()) {
            cout << " -> ";
        }
    }

    cout << endl;
    cout << endl;

    cout << "Distancia Total (Dijkstra) entre Buenos Aires y Comodoro Rivadavia: " << distancia_Dijkstra << " km." << endl;
    cout << endl;
    cout << "Camino encontrado por Dijsktra: " << endl;

    for (size_t i = 0; i < camino_con_Dijkstra.size(); i++) {
        cout << camino_con_Dijkstra[i];

        if (i + 1 < camino_con_Dijkstra.size()) {
            cout << " -> ";
        }
    }

    cout << endl;

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

    vector<string> vertices = g.get_vertices();

    size_t buenos_aires = vertices.size();
    size_t la_quiaca = vertices.size();
    size_t ushuaia = vertices.size();

    for (size_t i = 0; i < vertices.size(); i++) {

        if (vertices[i] == "Buenos Aires") {
            buenos_aires = i;
        }

        if (vertices[i] == "La Quiaca") {
            la_quiaca = i;
        }

        if (vertices[i] == "Ushuaia") {
            ushuaia = i;
        }
    }

    if (buenos_aires < vertices.size() && la_quiaca < vertices.size() && ushuaia < vertices.size()) {

        cout << "Distancia Buenos Aires - La Quiaca: " << dist[buenos_aires][la_quiaca] << " km." << endl;

        cout << "Distancia Buenos Aires - Ushuaia: " << dist[buenos_aires][ushuaia] << " km." << endl;
    }

    else {
        cout << "No se han encontrado las ciudades solicitadas." << endl;
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
    analisisLibre(g, dist);

    return 0;
}