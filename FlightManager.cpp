#include "FlightManager.h"
#include "Utils.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <tuple>
using namespace std;
FlightManager::Node* FlightManager::insertNode(Node* node, Flight* f) {
    if (!node) return new Node(f);

    if (f->code < node->flight->code)
        node->left = insertNode(node->left, f);
    else if (f->code > node->flight->code)
        node->right = insertNode(node->right, f);

    return node;
}

Flight* FlightManager::searchNode(Node* node, const string& code) const {
    if (!node) return nullptr;

    if (code == node->flight->code)
        return node->flight;

    return code < node->flight->code
        ? searchNode(node->left, code)
        : searchNode(node->right, code);
}

void FlightManager::destroy(Node* node) {
    if (!node) return;

    destroy(node->left);
    destroy(node->right);
    delete node;
}

FlightManager::~FlightManager() {
    destroy(root);
}

Flight* FlightManager::find(const string& code) const {
    return searchNode(root, code);
}

bool FlightManager::add(const Flight& f) {
    if (f.code.empty() || find(f.code))
        return false;

    flights.push_back(f);
    root = insertNode(root, &flights.back());

    return true;
}

void FlightManager::display() const {
    if (flights.empty()) {
        cout << "No flights available.\n";
        return;
    }

    cout << "\n" << left << setw(12) << "Flight"
         << setw(9) << "From"
         << setw(9) << "To"
         << setw(14) << "Date"
         << setw(10) << "Time"
         << setw(7) << "Gate"
         << "Seats\n";

    cout << string(75, '-') << '\n';

    for (const auto& f : flights)
        cout << left << setw(12) << f.code
             << setw(9) << f.origin
             << setw(9) << f.destination
             << setw(14) << f.date
             << setw(10) << f.effectiveDeparture()
             << setw(7) << (f.gate ? to_string(f.gate) : "-")
             << f.availableSeats() << "/" << SEATS_PER_FLIGHT << '\n';
}

static tuple<int, int, int> dateKey(const string& date) {
    return {
        stoi(date.substr(6, 4)),
        stoi(date.substr(3, 2)),
        stoi(date.substr(0, 2))
    };
}

void FlightManager::merge(vector<Flight*>& a, int l, int m, int r) const {
    vector<Flight*> temp;

    int i = l, j = m + 1;

    auto before = [](Flight* x, Flight* y) {
        auto dx = dateKey(x->date);
        auto dy = dateKey(y->date);

        if (dx != dy)
            return dx < dy;

        return x->effectiveDeparture() < y->effectiveDeparture();
    };

    while (i <= m && j <= r)
        temp.push_back(before(a[i], a[j]) ? a[i++] : a[j++]);

    while (i <= m)
        temp.push_back(a[i++]);

    while (j <= r)
        temp.push_back(a[j++]);

    for (int k = 0; k < (int)temp.size(); ++k)
        a[l + k] = temp[k];
}

void FlightManager::mergeSort(vector<Flight*>& a, int l, int r) const {
    if (l >= r) return;

    int m = l + (r - l) / 2;

    mergeSort(a, l, m);
    mergeSort(a, m + 1, r);
    merge(a, l, m, r);
}

void FlightManager::sortAndDisplay() const {
    vector<Flight*> ordered;

    for (const auto& f : flights)
        ordered.push_back(const_cast<Flight*>(&f));

    if (ordered.empty()) {
        cout << "No flights to sort.\n";
        return;
    }

    mergeSort(ordered, 0, (int)ordered.size() - 1);

    cout << "\nFlights sorted using Merge Sort:\n";

    for (const Flight* f : ordered)
        cout << f->code << " | " << f->date << " | "
             << f->effectiveDeparture() << " | "
             << f->origin << " -> " << f->destination << '\n';
}

void FlightManager::allocateGate() {
    string code = readLine("Flight code: ");

    Flight* target = find(code);

    if (!target) {
        cout << "Flight not found.\n";
        return;
    }

    int targetMin =
        stoi(target->effectiveDeparture().substr(0, 2)) * 60 +
        stoi(target->effectiveDeparture().substr(3, 2));

    for (int gate = 1; gate <= GATE_COUNT; ++gate) {
        bool free = true;

        for (const auto& other : flights) {
            if (other.code == code || other.gate != gate ||
                other.date != target->date)
                continue;

            int otherMin =
                stoi(other.effectiveDeparture().substr(0, 2)) * 60 +
                stoi(other.effectiveDeparture().substr(3, 2));

            if (abs(targetMin - otherMin) < 90) {
                free = false;
                break;
            }
        }

        if (free) {
            target->gate = gate;
            cout << "Gate " << gate << " allocated.\n";
            return;
        }
    }

    cout << "No gate available under the 90-minute separation rule.\n";
}

void FlightManager::updateDelay() {
    string code = readLine("Flight code: ");

    Flight* f = find(code);

    if (!f) {
        cout << "Flight not found.\n";
        return;
    }

    string t = readLine("Revised departure (HH:MM): ");

    if (!validTime(t)) {
        cout << "Invalid time.\n";
        return;
    }

    f->revisedDeparture = t;
    f->delayed = true;

    cout << "Flight time updated.\n";
}

void FlightManager::load() {
    ifstream in("flights.txt");
    string line;

    while (getline(in, line)) {
        stringstream ss(line);
        vector<string> x;
        string v;

        while (getline(ss, v, '|'))
            x.push_back(v);

        if (x.size() != 8 + SEATS_PER_FLIGHT)
            continue;

        Flight f;

        f.code = x[0];
        f.origin = x[1];
        f.destination = x[2];
        f.date = x[3];
        f.departure = x[4];

        try {
            f.gate = stoi(x[5]);
            f.delayed = (x[6] == "1");
            f.revisedDeparture = x[7];

            if (!validDate(f.date) ||
                !validTime(f.departure) ||
                (f.delayed && !validTime(f.revisedDeparture)))
                continue;

            for (int i = 0; i < SEATS_PER_FLIGHT; i++)
                f.seats[i] = (x[8 + i] == "1");

            add(f);
        }
        catch (...) {}
    }
}

void FlightManager::save() const {
    ofstream out("flights.txt");

    for (const auto& f : flights) {
        out << f.code << '|' << f.origin << '|'
            << f.destination << '|' << f.date << '|'
            << f.departure << '|' << f.gate << '|'
            << f.delayed << '|' << f.revisedDeparture;

        for (bool seat : f.seats)
            out << '|' << seat;

        out << '\n';
    }
}
