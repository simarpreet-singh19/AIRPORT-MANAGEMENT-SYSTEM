#ifndef PASSENGER_BOOKING_H
#define PASSENGER_BOOKING_H
#include "Models.h"
#include "FlightManager.h"
#include <list>
#include <queue>
#include <stack>
#include <vector>
#include <string>
class PassengerBookingManager {
private:
    std::list<Passenger> passengers;
    std::vector<Booking> bookings;

    std::stack<int> undoStack;
    std::queue<int> normalQueue;

    struct PriorityItem {
        int score;
        int sequence;
        int id;
    };

    struct Compare {
        bool operator()(const PriorityItem& a,
                        const PriorityItem& b) const {
            if (a.score != b.score)
                return a.score < b.score;

            return a.sequence > b.sequence;
        }
    };

    std::priority_queue<
        PriorityItem,
        std::vector<PriorityItem>,
        Compare
    > priorityQueue;

    int nextPassengerId = 1001;
    int nextBookingId = 5001;
    int nextSequence = 1;

    Passenger* passenger(int id);
    Booking* booking(int id);

    void releaseSeat(const Booking& b, FlightManager& flights);

public:
    void registerPassenger();
    void displayPassengers() const;

    void bookTicket(FlightManager& flights);
    void cancelTicket(FlightManager& flights);
    void undoBooking(FlightManager& flights);

    void checkIn();
    void addToBoarding();
    void processBoarding();

    void displayBookings() const;

    void load(FlightManager& flights);
    void save() const;
};

#endif
