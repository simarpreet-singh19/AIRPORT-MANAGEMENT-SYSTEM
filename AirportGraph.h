#ifndef AIRPORT_GRAPH_H
#define AIRPORT_GRAPH_H
#include <string>
#include <vector>
class AirportGraph {
private:
    std::vector<std::string> airports;
    std::vector<std::vector<int>> adj;

    int indexOf(const std::string& airport) const;
public:
    void addAirport(const std::string& airport);
    void addRoute(const std::string& from,
                  const std::string& to,
                  int distance);

    void displayGraph() const;
    void bfs(const std::string& start) const;
    void dijkstra(const std::string& start) const;
};
#endif
