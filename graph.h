#ifndef GRAPH_H
#define GRAPH_H
 
#include <vector>
#include <queue>
#include <limits>
#include <iostream>
 
/// @brief Aresta dirigida no grafo de fluxo.
struct Edge {
    int to;       ///< No destino
    int capacity; ///< Capacidade da aresta
    int flow;     ///< Fluxo atual
    int rev;      ///< Indice da aresta inversa na lista do destino
 
    Edge(int to, int capacity, int rev)
            : to(to), capacity(capacity), flow(0), rev(rev) {}
};
 
/// @brief Grafo dirigido com lista de adjacencias para calculo de fluxo maximo.
class Graph {
private:
    int numVertices;
    std::vector<std::vector<Edge>> adj; ///< Lista de adjacencias
 
public:
    /// @brief Constroi um grafo com n nos sem arestas.
    explicit Graph(int n);
 
    /// @brief Adiciona aresta dirigida de from para to com a capacidade dada e respetiva aresta inversa.
    void addEdge(int from, int to, int capacity);
 
    /// @brief Calcula o fluxo maximo entre source e sink pelo algoritmo de Edmonds-Karp.
    /// @complexity O(V * E^2) onde V = nos, E = arestas
    int edmondsKarp(int source, int sink);
 
    /// @brief Imprime todas as arestas com capacidade positiva.
    void printGraph() const;
 
    /// @brief Devolve a lista de adjacencias em modo de leitura.
    const std::vector<std::vector<Edge>>& getAdj() const { return adj; }
};
 
#endif // GRAPH_H
 