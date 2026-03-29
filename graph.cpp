#include "graph.h"
 
/// @brief Constroi um grafo com n nos e listas de adjacencia vazias.
Graph::Graph(int n) : numVertices(n), adj(n) {}
 
/// @brief Adiciona aresta dirigida de from para to com a capacidade dada.
/// Adiciona tambem a aresta inversa com capacidade zero para suporte ao algoritmo de fluxo.
/// @complexity O(1)
void Graph::addEdge(int from, int to, int capacity) {
    Edge forward(to, capacity, (int) adj[to].size());
    Edge backward(from, 0, (int) adj[from].size());
    adj[from].push_back(forward);
    adj[to].push_back(backward);
}
 
/// @brief Calcula o fluxo maximo entre source e sink usando BFS para encontrar
/// o caminho aumentante mais curto em cada iteracao (Edmonds-Karp).
/// @complexity O(V * E^2) onde V = nos, E = arestas
int Graph::edmondsKarp(int source, int sink) {
    int maxFlow = 0;
 
    while (true) {
        std::vector<int> parent(numVertices, -1);
        std::vector<int> parentEdge(numVertices, -1);
        std::queue<int> q;
 
        q.push(source);
        parent[source] = source;
 
        // BFS
        while (!q.empty() && parent[sink] == -1) {
            int u = q.front();
            q.pop();
 
            for (int i = 0; i < (int) adj[u].size(); i++) {
                Edge &e = adj[u][i];
                if (parent[e.to] == -1 && e.capacity > e.flow) {
                    parent[e.to] = u;
                    parentEdge[e.to] = i;
                    q.push(e.to);
                    if (e.to == sink) break;
                }
            }
        }
 
        if (parent[sink] == -1) break;
 
        int pathFlow = std::numeric_limits<int>::max();
        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            Edge &e = adj[u][parentEdge[v]];
            pathFlow = std::min(pathFlow, e.capacity - e.flow);
        }
 
        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            Edge &e = adj[u][parentEdge[v]];
            e.flow += pathFlow;
            adj[e.to][e.rev].flow -= pathFlow;
        }
 
        maxFlow += pathFlow;
    }
 
    return maxFlow;
}
 
/// @brief Imprime todas as arestas com capacidade positiva no formato "u -> v | cap=C flow=F".
void Graph::printGraph() const {
    for (int u = 0; u < numVertices; u++) {
        for (const Edge &e : adj[u]) {
            if (e.capacity > 0)
                std::cout << u << " -> " << e.to
                          << " | cap=" << e.capacity
                          << " flow=" << e.flow << std::endl;
        }
    }
}