/*name: Ebad Ali Siddiqui
    section: 2C
    roll no: 24k-0882*/
#include <iostream>
#include <string>
#include <fstream>
#include <exception>

using namespace std;

const int MAX_USERS = 100;
const int MAX_VEHICLES = 50;
const int MAX_BOOKINGS = 200;
const int MAX_SEATS = 60;

class BookingException : public exception
{
    string message;

public:
    BookingException(string msg) : message(msg) {}
    const char *what() const throw()
    {
        return message.c_str();
    }
};

class Route
{
public:
    string start, end;
    double distance;

    Route(string s = "", string e = "", double d = 0) : start(s), end(e), distance(d) {}

    bool isLong(double threshold)
    {
        return distance > threshold;
    }
};

class Driver
{
public:
    string name, license;
    Driver(string n = "", string l = "") : name(n), license(l) {}
};

class User
{
protected:
    string name, id;
    bool hasPaid;
    int lastBookingMonth;

public:
    User(string n = "", string i = "") : name(n), id(i), hasPaid(false), lastBookingMonth(-1) {}
    virtual string getRole() const = 0;
    virtual double getBaseFare() const = 0;

    string getId() const { return id; }
    string getName() const { return name; }
    bool paid() const { return hasPaid; }
    void makePayment() { hasPaid = true; }

    void setBookingMonth(int month) { lastBookingMonth = month; }
    bool alreadyBookedThisMonth(int month) const { return lastBookingMonth == month; }

    virtual void save(ofstream &out) const
    {
        out << name << " " << id << " " << hasPaid << " " << lastBookingMonth << "\n";
    }
};

class Student : public User
{
public:
    Student(string n = "", string i = "") : User(n, i) {}
    string getRole() const { return "Student"; }
    double getBaseFare() const { return 5000; }
    void save(ofstream &out) const
    {
        out << "Student ";
        User::save(out);
    }
};

class Faculty : public User
{
public:
    Faculty(string n = "", string i = "") : User(n, i) {}
    string getRole() const { return "Faculty"; }
    double getBaseFare() const { return 8000; }
    void save(ofstream &out) const
    {
        out << "Faculty ";
        User::save(out);
    }
};

class Seat
{
public:
    int seatNo;
    bool isBooked;
    string userId;
    string role;

    Seat(int num = 0, string r = "") : seatNo(num), role(r), isBooked(false), userId("") {}
};

class Vehicle
{
public:
    string id;
    Driver driver;
    Route route;
    bool isAC;
    string transporter;
    Seat seats[MAX_SEATS];
    int seatCount;

    Vehicle(string i = "", Driver d = Driver(), Route r = Route(), bool ac = false, string t = "", int sc = 0, string role = "")
        : id(i), driver(d), route(r), isAC(ac), transporter(t), seatCount(sc)
    {
        for (int i = 0; i < seatCount; i++)
        {
            seats[i] = Seat(i + 1, role);
        }
    }

    Seat *findAvailableSeat(string role)
    {
        for (int i = 0; i < seatCount; i++)
        {
            if (!seats[i].isBooked && seats[i].role == role)
            {
                return &seats[i];
            }
        }
        return nullptr;
    }

    string getId() const { return id; }
    bool isAirConditioned() const { return isAC; }

    void save(ofstream &out) const
    {
        out << id << " " << driver.name << " " << driver.license << " "
            << route.start << " " << route.end << " " << route.distance << " "
            << isAC << " " << transporter << " " << seatCount << "\n";
    }
};

class Booking
{
public:
    User *user;
    Vehicle *vehicle;
    Seat *seat;
    int month;

    Booking(User *u = nullptr, Vehicle *v = nullptr, Seat *s = nullptr, int m = 1) : user(u), vehicle(v), seat(s), month(m) {}

    double calculateFare() const
    {
        double fare = user->getBaseFare();
        if (vehicle->isAirConditioned())
            fare += 2000;
        return fare;
    }

    void print() const
    {
        cout << "User: " << user->getName() << " (" << user->getRole() << ")\n"
             << "Vehicle: " << vehicle->getId() << ", Seat: " << seat->seatNo
             << ", Month: " << month << ", Fare: " << calculateFare() << "\n";
    }

    void save(ofstream &out) const
    {
        out << user->getId() << " " << vehicle->getId() << " " << seat->seatNo << " " << month << "\n";
    }
};

class TransportManager
{
    User *users[MAX_USERS];
    Vehicle *vehicles[MAX_VEHICLES];
    Booking *bookings[MAX_BOOKINGS];
    int userCount = 0, vehicleCount = 0, bookingCount = 0;

public:
    void registerUser(User *user)
    {
        if (userCount < MAX_USERS)
            users[userCount++] = user;
    }

    void addVehicle(Vehicle *vehicle)
    {
        if (vehicleCount < MAX_VEHICLES)
            vehicles[vehicleCount++] = vehicle;
    }

    void makePayment(string userId)
    {
        User *user = findUserById(userId);
        if (!user)
            throw BookingException("User not found.");
        user->makePayment();
    }

    void bookSeat(string userId, string vehicleId, int month)
    {
        User *user = findUserById(userId);
        if (!user)
            throw BookingException("User not found.");
        if (!user->paid())
            throw BookingException("Payment not completed.");
        if (user->alreadyBookedThisMonth(month))
            throw BookingException("User already booked this month.");

        Vehicle *vehicle = findVehicleById(vehicleId);
        if (!vehicle)
            throw BookingException("Vehicle not found.");

        Seat *seat = vehicle->findAvailableSeat(user->getRole());
        if (!seat)
            throw BookingException("No available seat for this role.");

        seat->isBooked = true;
        seat->userId = user->getId();
        user->setBookingMonth(month);
        bookings[bookingCount++] = new Booking(user, vehicle, seat, month);
    }

    void showBookings() const
    {
        for (int i = 0; i < bookingCount; i++)
        {
            bookings[i]->print();
        }
    }

    void saveAll()
    {
        ofstream userFile("users.txt"), vehicleFile("vehicles.txt"), bookingFile("bookings.txt");
        for (int i = 0; i < userCount; i++)
            users[i]->save(userFile);
        for (int i = 0; i < vehicleCount; i++)
            vehicles[i]->save(vehicleFile);
        for (int i = 0; i < bookingCount; i++)
            bookings[i]->save(bookingFile);
        userFile.close();
        vehicleFile.close();
        bookingFile.close();
    }

private:
    User *findUserById(string id)
    {
        for (int i = 0; i < userCount; i++)
        {
            if (users[i]->getId() == id)
                return users[i];
        }
        return nullptr;
    }

    Vehicle *findVehicleById(string id)
    {
        for (int i = 0; i < vehicleCount; i++)
        {
            if (vehicles[i]->getId() == id)
                return vehicles[i];
        }
        return nullptr;
    }
};

int main()
{
    TransportManager manager;

    Route route1("Model Town", "FAST", 25);
    Driver driver1("Ali Khan", "LIC001");
    Driver driver2("Zulfiqar", "LIC002");

    // Adding vehicles
    manager.addVehicle(new Vehicle("V1", driver1, route1, false, "Nadeem", 52, "Student"));
    manager.addVehicle(new Vehicle("V2", driver2, route1, true, "Zulfiqar", 32, "Faculty"));

    // Registering users
    manager.registerUser(new Student("Ahmed", "S123"));
    manager.registerUser(new Faculty("Dr. Farah", "F456"));

    // Making payments
    manager.makePayment("S123");
    manager.makePayment("F456");

    // Booking seats for May (month = 5)
    try
    {
        manager.bookSeat("S123", "V1", 5);
        manager.bookSeat("F456", "V2", 5);
    }
    catch (BookingException &e)
    {
        cout << "Error: " << e.what() << endl;
    }

    // Showing bookings
    manager.showBookings();

    // Saving data files
    manager.saveAll();

    return 0;
}
