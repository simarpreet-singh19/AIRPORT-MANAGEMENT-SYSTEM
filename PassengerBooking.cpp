#include "PassengerBooking.h"
#include "Utils.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
using namespace std;
Passenger* PassengerBookingManager::passenger(int id) {
    for (auto& p : passengers)
        if (p.id == id)
            return &p;

    return nullptr;
}

Booking* PassengerBookingManager::booking(int id) {
    for (auto& b : bookings)
        if (b.id == id)
            return &b;

    return nullptr;
}

void PassengerBookingManager::releaseSeat(
    const Booking& b, FlightManager& flights) {

    Flight* f = flights.find(b.flightCode);

    if (f && b.seatNumber >= 1 && b.seatNumber <= SEATS_PER_FLIGHT)
        f->seats[b.seatNumber - 1] = false;
}

void PassengerBookingManager::registerPassenger() {
    Passenger p;

    p.name = cleanField(readLine("Passenger name: "));
    p.phone = cleanField(readLine("Phone number: "));

    if (p.name.empty() || p.phone.empty()) {
        cout << "Name and phone are required.\n";
        return;
    }

    p.id = nextPassengerId++;
    passengers.push_back(p);

    cout << "Passenger registered. ID: " << p.id << '\n';
}

void PassengerBookingManager::displayPassengers() const {
    if (passengers.empty()) {
        cout << "No passengers registered.\n";
        return;
    }

    cout << "\nPassenger ID | Name | Phone\n";

    for (const auto& p : passengers)
        cout << p.id << " | " << p.name
             << " | " << p.phone << '\n';
}

void PassengerBookingManager::bookTicket(FlightManager& flights) {
    int pid = readInt("Passenger ID: ", 1, 100000000);

    if (!passenger(pid)) {
        cout << "Passenger not found.\n";
        return;
    }

    string code = readLine("Flight code: ");
    Flight* f = flights.find(code);

    if (!f) {
        cout << "Flight not found.\n";
        return;
    }

    if (!f->availableSeats()) {
        cout << "No seats available.\n";
        return;
    }

    cout << "Available seats: ";

    for (int i = 0; i < SEATS_PER_FLIGHT; i++)
        if (!f->seats[i])
            cout << i + 1 << ' ';

    cout << '\n';

    int seat = readInt("Choose seat: ", 1, SEATS_PER_FLIGHT);

    if (f->seats[seat - 1]) {
        cout << "Seat already booked.\n";
        return;
    }

    f->seats[seat - 1] = true;

    Booking b;
    b.id = nextBookingId++;
    b.passengerId = pid;
    b.flightCode = code;
    b.seatNumber = seat;

    bookings.push_back(b);
    undoStack.push(b.id);

    cout << "Ticket booked. Booking ID: " << b.id << '\n';
}

void PassengerBookingManager::cancelTicket(FlightManager& flights) {
    int id = readInt("Booking ID: ", 1, 100000000);
    Booking* b = booking(id);

    if (!b || !b->active || b->boarded) {
        cout << "Active cancellable booking not found.\n";
        return;
    }

    releaseSeat(*b, flights);
    b->active = false;

    cout << "Booking cancelled and seat released.\n";
}

void PassengerBookingManager::undoBooking(FlightManager& flights) {
    while (!undoStack.empty()) {
        int id = undoStack.top();
        undoStack.pop();

        Booking* b = booking(id);

        if (b && b->active && !b->checkedIn && !b->boarded) {
            releaseSeat(*b, flights);
            b->active = false;

            cout << "Latest eligible booking undone using Stack.\n";
            return;
        }
    }

    cout << "No eligible booking to undo.\n";
}

void PassengerBookingManager::checkIn() {
    int id = readInt("Booking ID: ", 1, 100000000);
    Booking* b = booking(id);

    if (!b || !b->active) {
        cout << "Active booking not found.\n";
        return;
    }

    if (b->checkedIn) {
        cout << "Already checked in.\n";
        return;
    }

    b->checkedIn = true;
    cout << "Check-in completed.\n";
}

void PassengerBookingManager::addToBoarding() {
    int id = readInt("Booking ID: ", 1, 100000000);
    Booking* b = booking(id);

    if (!b || !b->active || !b->checkedIn || b->boarded) {
        cout << "Active checked-in booking required.\n";
        return;
    }

    if (b->boardingType != BoardingType::NONE) {
        cout << "Already queued.\n";
        return;
    }

    cout << "1. Normal queue\n2. Priority queue\n";

    int choice = readInt("Choice: ", 1, 2);
    b->sequence = nextSequence++;

    if (choice == 1) {
        b->boardingType = BoardingType::NORMAL;
        normalQueue.push(id);
    }
    else {
        b->priorityScore = readInt("Priority score (1-5): ", 1, 5);
        b->boardingType = BoardingType::PRIORITY;

        priorityQueue.push({
            b->priorityScore, b->sequence, id
        });
    }

    cout << "Added to boarding queue.\n";
}

void PassengerBookingManager::processBoarding() {
    while (!priorityQueue.empty()) {
        auto item = priorityQueue.top();
        priorityQueue.pop();

        Booking* b = booking(item.id);

        if (b && b->active && b->checkedIn &&
            !b->boarded && b->boardingType == BoardingType::PRIORITY) {

            b->boarded = true;
            cout << "Priority booking " << b->id << " boarded.\n";
            return;
        }
    }

    while (!normalQueue.empty()) {
        int id = normalQueue.front();
        normalQueue.pop();

        Booking* b = booking(id);

        if (b && b->active && b->checkedIn &&
            !b->boarded && b->boardingType == BoardingType::NORMAL) {

            b->boarded = true;
            cout << "Normal booking " << b->id
                 << " boarded in FIFO order.\n";
            return;
        }
    }

    cout << "No eligible passengers waiting.\n";
}

void PassengerBookingManager::displayBookings() const {
    if (bookings.empty()) {
        cout << "No bookings recorded.\n";
        return;
    }

    cout << "\nBooking | Passenger | Flight | Seat | Status | Check-in | Boarded\n";

    for (const auto& b : bookings) {
        cout << b.id << " | " << b.passengerId
             << " | " << b.flightCode
             << " | " << b.seatNumber
             << " | " << (b.active ? "Active" : "Cancelled")
             << " | " << (b.checkedIn ? "Yes" : "No")
             << " | " << (b.boarded ? "Yes" : "No") << '\n';
    }
}

void PassengerBookingManager::load(FlightManager& flights) {
    ifstream pin("passengers.txt");
    string line;

    while (getline(pin, line)) {
        stringstream ss(line);
        string id, name, phone;

        if (!getline(ss, id, '|') ||
            !getline(ss, name, '|') ||
            !getline(ss, phone))
            continue;

        try {
            Passenger p{stoi(id), name, phone};
            passengers.push_back(p);
            nextPassengerId = max(nextPassengerId, p.id + 1);
        }
        catch (...) {}
    }

    ifstream bin("bookings.txt");

    while (getline(bin, line)) {
        stringstream ss(line);
        vector<string> x;
        string v;

        while (getline(ss, v, '|'))
            x.push_back(v);

        if (x.size() != 10)
            continue;

        try {
            Booking b;

            b.id = stoi(x[0]);
            b.passengerId = stoi(x[1]);
            b.flightCode = x[2];
            b.seatNumber = stoi(x[3]);

            b.active = (x[4] == "1");
            b.checkedIn = (x[5] == "1");
            b.boarded = (x[6] == "1");

            int type = stoi(x[7]);

            if (type < 0 || type > 2)
                continue;

            b.boardingType = static_cast<BoardingType>(type);
            b.priorityScore = stoi(x[8]);
            b.sequence = stoi(x[9]);

            if (!passenger(b.passengerId) ||
                !flights.find(b.flightCode) ||
                b.seatNumber < 1 ||
                b.seatNumber > SEATS_PER_FLIGHT)
                continue;

            if (b.active)
                flights.find(b.flightCode)->seats[b.seatNumber - 1] = true;

            bookings.push_back(b);

            nextBookingId = max(nextBookingId, b.id + 1);
            nextSequence = max(nextSequence, b.sequence + 1);

            if (b.active && !b.boarded) {
                if (!b.checkedIn)
                    undoStack.push(b.id);
                else if (b.boardingType == BoardingType::NORMAL)
                    normalQueue.push(b.id);
                else if (b.boardingType == BoardingType::PRIORITY)
                    priorityQueue.push({
                        b.priorityScore, b.sequence, b.id
                    });
            }
        }
        catch (...) {}
    }
}

void PassengerBookingManager::save() const {
    ofstream p("passengers.txt");

    for (const auto& x : passengers)
        p << x.id << '|' << x.name << '|' << x.phone << '\n';

    ofstream b("bookings.txt");

    for (const auto& x : bookings) {
        b << x.id << '|' << x.passengerId << '|'
          << x.flightCode << '|' << x.seatNumber << '|'
          << x.active << '|' << x.checkedIn << '|'
          << x.boarded << '|'
          << static_cast<int>(x.boardingType) << '|'
          << x.priorityScore << '|' << x.sequence << '\n';
    }
}
