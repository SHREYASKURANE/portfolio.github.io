#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <fstream>

using namespace std;

struct Admin {
    string name;
    int password;
};

struct WasteFactors {
    double baseRate;
    double scaleFactor;
    int attendeeImpact;
    double durationImpact;
};

struct Event {
    string name;
    float duration;
    string type;
    int attendees;
    double wasteForecast;

    Event(string n, float d, string t, int a, double w)
        : name(n), duration(d), type(t), attendees(a), wasteForecast(w) {}
};

class LoginSystem {
private:
    Admin admins[4];
    int adminCount;

public:
    LoginSystem() {
        adminCount = 4;
        admins[0] = {"adarsh", 12345};
        admins[1] = {"bhargavi", 54321};
        admins[2] = {"shreyas", 67890};
        admins[3] = {"shubham", 67890};
    }

    bool adminLogin() {
        string adminName;
        int password;

        cout << "Enter Admin ID (Name): ";
        cin.ignore();
        getline(cin, adminName);

        cout << "Enter Password (5-digit positive integer): ";
        cin >> password;

        if (password < 10000 || password > 99999) {
            cout << "Invalid Password. Must be a 5-digit positive integer.\n";
            return false;
        }

        for (int i = 0; i < adminCount; ++i) {
            if (admins[i].name == adminName && admins[i].password == password) {
                cout << "Admin login successful!\n";
                return true;
            }
        }

        cout << "Invalid Admin ID or Password.\n";
        return false;
    }

    void userLogin() {
        cout << "Welcome, User! You are logged in.\n";
    }
};

class ZeroWasteEventPlanner {
private:
    unordered_map<string, vector<string>> wasteCategories;
    unordered_map<string, WasteFactors> eventWasteFactors;
    vector<Event> events;

    double calculateWasteForecast(float duration, const string &type, int attendees) {
        WasteFactors factors = eventWasteFactors[type];

        double baseWaste = factors.baseRate * attendees * duration;
        double scaledWaste = baseWaste * factors.scaleFactor;
        double attendeeScaling = 1.0 + (log10(attendees) * factors.attendeeImpact / 1000.0);
        double durationFactor = 1.0 + (factors.durationImpact * log10(duration + 1));
        double totalWaste = scaledWaste * attendeeScaling * durationFactor;

        return round(totalWaste * 100) / 100;
    }

    void saveEventsToFile() {
        ofstream outFile("events.txt");
        if (outFile.is_open()) {
            for (const auto &event : events) {
                outFile << event.name << "," << event.duration << "," << event.type << "," << event.attendees << "," << event.wasteForecast << "\n";
            }
            outFile.close();
        } else {
            cerr << "Unable to open file for saving events.\n";
        }
    }

    void loadEventsFromFile() {
        ifstream inFile("events.txt");
        if (inFile.is_open()) {
            string line;
            while (getline(inFile, line)) {
                size_t pos1 = line.find(',');
                size_t pos2 = line.find(',', pos1 + 1);
                size_t pos3 = line.find(',', pos2 + 1);
                size_t pos4 = line.find(',', pos3 + 1);

                string name = line.substr(0, pos1);
                float duration = stof(line.substr(pos1 + 1, pos2 - pos1 - 1));
                string type = line.substr(pos2 + 1, pos3 - pos2 - 1);
                int attendees = stoi(line.substr(pos3 + 1, pos4 - pos3 - 1));
                double wasteForecast = stod(line.substr(pos4 + 1));

                events.emplace_back(name, duration, type, attendees, wasteForecast);
            }
            inFile.close();
        } else {
            cerr << "Unable to open file for loading events.\n";
        }
    }

public:
    ZeroWasteEventPlanner() {
        wasteCategories["Conference"] = {"Paper", "Plastic", "Electronic"};
        wasteCategories["Festival"] = {"Food", "Plastic", "Glass", "Decorations"};
        wasteCategories["Sports"] = {"Food", "Plastic", "Metal Cans"};
        wasteCategories["Concert"] = {"Paper", "Plastic", "Batteries"};
        wasteCategories["Exhibition"] = {"Paper", "Plastic", "Glass", "Electronics"};
        wasteCategories["Seminar"] = {"Paper", "Plastic", "Electronic"};

        eventWasteFactors["Conference"] = {0.15, 1.0, 5, 0.8};
        eventWasteFactors["Festival"] = {0.25, 1.2, 8, 0.9};
        eventWasteFactors["Sports"] = {0.18, 0.9, 6, 0.7};
        eventWasteFactors["Concert"] = {0.22, 1.1, 7, 0.85};
        eventWasteFactors["Exhibition"] = {0.20, 1.0, 6, 0.8};
        eventWasteFactors["Seminar"] = {0.12, 0.8, 4, 0.75};

        loadEventsFromFile();
    }

    ~ZeroWasteEventPlanner() {
        saveEventsToFile();
    }

    void planEvent(const string &name, float duration, string type, int attendees) {
        transform(type.begin(), type.end(), type.begin(), ::tolower);
        if (!type.empty()) {
            type[0] = toupper(type[0]);
        }

        if (wasteCategories.find(type) == wasteCategories.end()) {
            cout << "Invalid event type. Please choose a valid type.\n";
            return;
        }

        double wasteForecast = calculateWasteForecast(duration, type, attendees);
        events.emplace_back(name, duration, type, attendees, wasteForecast);

        cout << "\nPlanned Event: " << name << "\n";
        cout << "  Type: " << type << "\n";
        cout << "  Duration: " << fixed << setprecision(1) << duration << " hours\n";
        cout << "  Attendees: " << attendees << "\n";
        cout << "  Waste Forecast: " << fixed << setprecision(2) << wasteForecast << " kg\n";
        cout << "  Waste Categories: ";
        for (const auto &category : wasteCategories[type]) {
            cout << category << " ";
        }
        cout << "\n";
    }

    void displayEvents() {
        if (events.empty()) {
            cout << "\nNo events planned yet.\n";
            return;
        }

        cout << "\nPlanned Events:\n";
        for (const auto &event : events) {
            cout << "  Name: " << event.name << "\n"
                 << "    Duration: " << fixed << setprecision(1) << event.duration << " hours\n"
                 << "    Type: " << event.type << "\n"
                 << "    Attendees: " << event.attendees << "\n"
                 << "    Waste Forecast: " << fixed << setprecision(2) << event.wasteForecast << " kg\n";
        }
    }

    void displayWasteBreakdown() {
        if (events.empty()) {
            cout << "\nNo events to calculate waste breakdown.\n";
            return;
        }

        cout << "\nWaste Breakdown:\n";
        for (const auto &event : events) {
            cout << "Event: " << event.name << "\n";
            double categoryWaste = event.wasteForecast / wasteCategories[event.type].size();
            for (const auto &category : wasteCategories[event.type]) {
                cout << "  " << category << " Waste: " << fixed << setprecision(2)
                     << categoryWaste << " kg\n";
            }
            cout << "\n";
        }
    }
};

void adminDashboard(ZeroWasteEventPlanner &planner) {
    int adminChoice;
    do {
        cout << "\nAdmin Dashboard\n";
        cout << "1. Plan Event\n";
        cout << "2. View Events\n";
        cout << "3. View Waste Breakdown\n";
        cout << "4. Logout\n";
        cout << "Enter your choice: ";
        cin >> adminChoice;

        switch (adminChoice) {
        case 1: {
            string name, type;
            float duration;
            int attendees;
            cin.ignore();
            cout << "Enter event name: ";
            getline(cin, name);
            cout << "Enter event duration (hours): ";
            cin >> duration;
            cout << "Enter number of attendees: ";
            cin >> attendees;
            cin.ignore();
            cout << "Enter event type (Conference/Festival/Sports/Concert/Exhibition/Seminar): ";
            getline(cin, type);
            planner.planEvent(name, duration, type, attendees);
            break;
        }
        case 2:
            planner.displayEvents();
            break;
        case 3:
            planner.displayWasteBreakdown();
            break;
        case 4:
            cout << "Logging out...\n";
            break;
        default:
            cout << "Invalid choice. Try again.\n";
        }
    } while (adminChoice != 4);
}

void userDashboard(ZeroWasteEventPlanner &planner) {
    int userChoice;
    do {
        cout << "\nUser Dashboard\n";
        cout << "1. View Events\n";
        cout << "2. Logout\n";
        cout << "Enter your choice: ";
        cin >> userChoice;

        switch (userChoice) {
        case 1:
            planner.displayEvents();
            break;
        case 2:
            cout << "Logging out...\n";
            break;
        default:
            cout << "Invalid choice. Try again.\n";
        }
    } while (userChoice != 2);
}

int main() {
    LoginSystem loginSystem;
    ZeroWasteEventPlanner planner;

    while (true) {
        int choice;
        cout << "\nLogin Page\n";
        cout << "1. Admin Login\n";
        cout << "2. User Login\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            if (loginSystem.adminLogin()) {
                adminDashboard(planner);
            }
            break;
        case 2:
            loginSystem.userLogin();
            userDashboard(planner);
            break;
        case 3:
            cout << "Exiting the system. Goodbye!\n";
            return 0;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}
