#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <queue>
#include <limits>
#include <iostream>

struct Edge {
    int to;           // nó destino
    int capacity;     // capacidade da aresta
    int flow;         // fluxo atual
    int rev;          // índice da aresta inversa na lista do destino

    Edge(int to, int capacity, int rev)
            : to(to), capacity(capacity), flow(0), rev(rev) {}
};

class Graph {
private:
    int numVertices;
    std::vector<std::vector<Edge>> adj; // lista de adjacências

public:
    explicit Graph(int n);

    void addEdge(int from, int to, int capacity);

    int edmondsKarp(int source, int sink);

    void printGraph() const;

    const std::vector<std::vector<Edge>>& getAdj() const { return adj; }

};

#endif // GRAPH_H
