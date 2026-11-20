#include "AirportGraph.h"
#include <iostream>
#include <queue>
#include <climits>
#include <algorithm>
using namespace std;
int AirportGraph::indexOf(const string& airport) const {
    for (int i = 0; i < (int)airports.size(); i++) {
        if (airports[i] == airport)
            return i;
    }

    return -1;
}

void AirportGraph::addAirport(const string& airport) {
    if (airport.empty() || indexOf(airport) != -1)
        return;

    airports.push_back(airport);

    for (auto& row : adj)
        row.push_back(0);

    adj.push_back(vector<int>(airports.size(), 0));
}

void AirportGraph::addRoute(
    const string& from, const string& to, int distance) {

    if (distance <= 0 || from == to) {
        cout << "Invalid route.\n";
        return;
    }

    int u = indexOf(from);
    int v = indexOf(to);

    if (u == -1 || v == -1) {
        cout << "Airport not found. Add both airports first.\n";
        return;
    }

    adj[u][v] = distance;
    adj[v][u] = distance;

    cout << "Route added successfully.\n";
}

void AirportGraph::displayGraph() const {
    if (airports.empty()) {
        cout << "No airports available.\n";
        return;
    }

    cout << "\nAirport Route Network:\n";

    for (int i = 0; i < (int)airports.size(); i++) {
        cout << airports[i] << " -> ";

        bool found = false;

        for (int j = 0; j < (int)airports.size(); j++) {
            if (adj[i][j] > 0) {
                cout << airports[j] << " ("
                     << adj[i][j] << " km)  ";
                found = true;
            }
        }

        if (!found)
            cout << "No connected routes";

        cout << '\n';
    }
}

void AirportGraph::bfs(const string& start) const {
    int source = indexOf(start);

    if (source == -1) {
        cout << "Starting airport not found.\n";
        return;
    }

    vector<bool> visited(airports.size(), false);
    queue<int> q;

    visited[source] = true;
    q.push(source);

    cout << "BFS traversal: ";

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        cout << airports[u] << " ";

        for (int v = 0; v < (int)airports.size(); v++) {
            if (adj[u][v] > 0 && !visited[v]) {
                visited[v] = true;
                q.push(v);
            }
        }
    }

    cout << '\n';
}

void AirportGraph::dijkstra(const string& start) const {
    int source = indexOf(start);

    if (source == -1) {
        cout << "Starting airport not found.\n";
        return;
    }

    int n = (int)airports.size();

    vector<int> dist(n, INT_MAX);
    vector<bool> visited(n, false);

    dist[source] = 0;

    for (int count = 0; count < n; count++) {
        int u = -1;

        for (int i = 0; i < n; i++) {
            if (!visited[i] &&
                (u == -1 || dist[i] < dist[u]))
                u = i;
        }

        if (u == -1 || dist[u] == INT_MAX)
            break;

        visited[u] = true;

        for (int v = 0; v < n; v++) {
            if (adj[u][v] > 0 && !visited[v] &&
                dist[u] <= INT_MAX - adj[u][v] &&
                dist[u] + adj[u][v] < dist[v]) {

                dist[v] = dist[u] + adj[u][v];
            }
        }
    }

    cout << "\nShortest distances from " << start << ":\n";

    for (int i = 0; i < n; i++) {
        cout << airports[i] << " : ";

        if (dist[i] == INT_MAX)
            cout << "Unreachable\n";
        else
            cout << dist[i] << " km\n";
    }
}
