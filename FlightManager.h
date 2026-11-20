#ifndef FLIGHT_MANAGER_H
#define FLIGHT_MANAGER_H
#include "Models.h"
#include <list>
#include <string>
#include <vector>
class FlightManager {
private:
    struct Node {
        Flight* flight;
        Node *left, *right;

        explicit Node(Flight* f)
            : flight(f), left(nullptr), right(nullptr) {}
    };
    std::list<Flight> flights;
    Node* root = nullptr;
    Node* insertNode(Node* node, Flight* f);
    Flight* searchNode(Node* node, const std::string& code) const;
    void destroy(Node* node);
    void mergeSort(std::vector<Flight*>& a, int l, int r) const;
    void merge(std::vector<Flight*>& a, int l, int m, int r) const;
public:
    FlightManager() = default;
    ~FlightManager();
    FlightManager(const FlightManager&) = delete;
    FlightManager& operator=(const FlightManager&) = delete;
    Flight* find(const std::string& code) const;
    bool add(const Flight& flight);
    void display() const;
    void sortAndDisplay() const;
    void allocateGate();
    void updateDelay();
    const std::list<Flight>& all() const {
        return flights;
    }
    void load();
    void save() const;
};
#endif
