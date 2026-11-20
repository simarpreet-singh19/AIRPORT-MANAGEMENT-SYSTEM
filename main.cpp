#include "Models.h"
#include "Utils.h"
#include "FlightManager.h"
#include "PassengerBooking.h"
#include "AirportGraph.h"
#include <iostream>
#include <limits>
using namespace std;
void addFlight(FlightManager& flights) {
    Flight f;

    f.code = cleanField(readLine("Flight code: "));
    f.origin = cleanField(readLine("Origin airport: "));
    f.destination = cleanField(readLine("Destination airport: "));
    f.date = readLine("Date (DD-MM-YYYY): ");
    f.departure = readLine("Departure time (HH:MM): ");

    if (f.code.empty() || f.origin.empty() ||
        f.destination.empty() || f.origin == f.destination) {
        cout << "Invalid flight details.\n";
        return;
    }

    if (!validDate(f.date) || !validTime(f.departure)) {
        cout << "Invalid date or time format.\n";
        return;
    }

    if (flights.add(f))
        cout << "Flight added successfully.\n";
    else
        cout << "Flight code already exists.\n";
}

void addAirport(AirportGraph& graph) {
    string name = cleanField(readLine("Airport name: "));

    if (name.empty()) {
        cout << "Airport name cannot be empty.\n";
        return;
    }

    graph.addAirport(name);
    cout << "Airport added if it did not already exist.\n";
}

void addRoute(AirportGraph& graph) {
    string from = cleanField(readLine("From airport: "));
    string to = cleanField(readLine("To airport: "));
    int distance = readInt("Distance in km: ", 1, 100000);

    graph.addRoute(from, to, distance);
}

void showMenu() {
    cout << "\n========================================\n";
    cout << "       AEROFLOW AIRPORT MANAGEMENT\n";
    cout << "========================================\n";

    cout << "1.  Add Flight\n";
    cout << "2.  Display All Flights\n";
    cout << "3.  Search Flight using BST\n";
    cout << "4.  Sort Flights using Merge Sort\n";
    cout << "5.  Allocate Gate\n";
    cout << "6.  Update Flight Delay\n";
    cout << "7.  Register Passenger\n";
    cout << "8.  Display Passengers\n";
    cout << "9.  Book Ticket\n";
    cout << "10. Cancel Ticket\n";
    cout << "11. Undo Latest Booking\n";
    cout << "12. Passenger Check-in\n";
    cout << "13. Add to Boarding Queue\n";
    cout << "14. Process Boarding\n";
    cout << "15. Display Bookings\n";
    cout << "16. Add Airport to Network\n";
    cout << "17. Add Airport Route\n";
    cout << "18. Display Airport Graph\n";
    cout << "19. BFS Traversal\n";
    cout << "20. Dijkstra Shortest Path\n";
    cout << "0.  Save and Exit\n";

    cout << "========================================\n";
}

int main() {
    FlightManager flights;
    PassengerBookingManager passengers;
    AirportGraph graph;

    flights.load();
    passengers.load(flights);

    cout << "\nWelcome to AeroFlow Airport Management System!\n";

    bool running = true;

    while (running) {
        showMenu();

        int choice = readInt("Enter your choice: ", 0, 20);

        switch (choice) {
            case 1:
                addFlight(flights);
                break;

            case 2:
                flights.display();
                break;

            case 3: {
                string code = readLine("Enter flight code: ");
                Flight* f = flights.find(code);

                if (f) {
                    cout << "\nFlight found:\n";
                    cout << "Code: " << f->code << '\n';
                    cout << "Route: " << f->origin
                         << " -> " << f->destination << '\n';
                    cout << "Date: " << f->date << '\n';
                    cout << "Departure: "
                         << f->effectiveDeparture() << '\n';
                    cout << "Gate: "
                         << (f->gate ? to_string(f->gate) : "Not allocated")
                         << '\n';
                    cout << "Available seats: "
                         << f->availableSeats() << '\n';
                }
                else {
                    cout << "Flight not found.\n";
                }
                break;
            }

            case 4:
                flights.sortAndDisplay();
                break;

            case 5:
                flights.allocateGate();
                break;

            case 6:
                flights.updateDelay();
                break;

            case 7:
                passengers.registerPassenger();
                break;

            case 8:
                passengers.displayPassengers();
                break;

            case 9:
                passengers.bookTicket(flights);
                break;

            case 10:
                passengers.cancelTicket(flights);
                break;

            case 11:
                passengers.undoBooking(flights);
                break;

            case 12:
                passengers.checkIn();
                break;

            case 13:
                passengers.addToBoarding();
                break;

            case 14:
                passengers.processBoarding();
                break;

            case 15:
                passengers.displayBookings();
                break;

            case 16:
                addAirport(graph);
                break;

            case 17:
                addRoute(graph);
                break;

            case 18:
                graph.displayGraph();
                break;

            case 19: {
                string start = cleanField(readLine("Starting airport: "));
                graph.bfs(start);
                break;
            }

            case 20: {
                string start = cleanField(readLine("Starting airport: "));
                graph.dijkstra(start);
                break;
            }

            case 0:
                flights.save();
                passengers.save();
                cout << "Data saved successfully.\n";
                cout << "Thank you for using AeroFlow!\n";
                running = false;
                break;
        }
    }

    return 0;
}
