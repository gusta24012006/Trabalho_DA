#include "graph.h"

Graph::Graph(int n) : numVertices(n), adj(n) {}

void Graph::addEdge(int from, int to, int capacity) {
    Edge forward(to, capacity, (int) adj[to].size());
    Edge backward(from, 0, (int) adj[from].size());
    adj[from].push_back(forward);
    adj[to].push_back(backward);
}

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
