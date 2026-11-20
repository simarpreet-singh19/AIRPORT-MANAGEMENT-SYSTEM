
#ifndef MODELS_H
#define MODELS_H
#include <string>
#include <vector>
constexpr int SEATS_PER_FLIGHT = 20;
constexpr int GATE_COUNT = 5;
struct Flight {
    std::string code;
    std::string origin;
    std::string destination;
    std::string date;
    std::string departure;
    std::string revisedDeparture;
    int gate = 0;
    bool delayed = false;
    std::vector<bool> seats =
        std::vector<bool>(SEATS_PER_FLIGHT, false);
    int availableSeats() const {
        int count = 0;
        for (bool occupied : seats) {
            if (!occupied)
                ++count;
        }
        return count;
    }
    std::string effectiveDeparture() const {
        return delayed ? revisedDeparture : departure;
    }
};
struct Passenger {
    int id = 0;
    std::string name;
    std::string phone;
};
enum class BoardingType {
    NONE,
    NORMAL,
    PRIORITY
};
struct Booking {
    int id = 0;
    int passengerId = 0;
    int seatNumber = 0;
    std::string flightCode;
    bool active = true;
    bool checkedIn = false;
    bool boarded = false;
    BoardingType boardingType = BoardingType::NONE;
    int priorityScore = 0;
    int sequence = 0;
};
#endif
