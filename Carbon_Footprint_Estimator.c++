#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
using namespace std;

const int MAX_CITIES = 100;
const double NO_PATH = -1;

// Structure for admin credentials
struct Admin {
    string name;
    int password;
};

struct Edge {
    string source;
    string target;
    double weight;
    double carbonEmission;
};

struct CityEmission {
    string cityName;
    double totalEmission;
};

class WasteTransportSystem {
private:
    Admin admins[4];
    int adminCount;
    bool isInitialized;
    int nodes;
    string cityNames[MAX_CITIES];
    int adjacencyList[MAX_CITIES][MAX_CITIES];
    vector<Edge> edges;
    vector<CityEmission> cityEmissions;

    double calculateCarbonEmission(double distance, string wasteType) {
        double baseFactor = 2.6;
        double wasteMultiplier = 1.0;
        if (wasteType == "Plastic")
            wasteMultiplier = 1.2;
        else if (wasteType == "Metal")
            wasteMultiplier = 1.5;
        else if (wasteType == "Organic")
            wasteMultiplier = 0.8;
        else if (wasteType == "Electronic")
            wasteMultiplier = 1.7;
        return distance * baseFactor * wasteMultiplier;
    }

public:
    WasteTransportSystem() : nodes(0), isInitialized(false) {
        for (int i = 0; i < MAX_CITIES; i++) {
            for (int j = 0; j < MAX_CITIES; j++) {
                adjacencyList[i][j] = -1;
            }
        }

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

    void initializeSystem() {
        if (!isInitialized) {
            cout << "Enter number of cities: ";
            cin >> nodes;

            cout << "Enter the names of the cities:\n";
            for (int i = 0; i < nodes; i++) {
                string cityName;
                cout << "City " << i + 1 << ": ";
                cin >> cityName;
                cityNames[i] = cityName;
                cityEmissions.push_back({cityName, 0.0});
            }
            isInitialized = true;
            saveSystemState();
        }
    }

    void addRoute(const string &source, const string &target, double distance, string wasteType) {
        int srcIndex = getCityIndex(source);
        int tgtIndex = getCityIndex(target);
        double emission = calculateCarbonEmission(distance, wasteType);

        edges.push_back({source, target, distance, emission});
        adjacencyList[srcIndex][tgtIndex] = 1;

        cityEmissions[srcIndex].totalEmission += emission;
        cityEmissions[tgtIndex].totalEmission += emission;

        saveSystemState();
    }

    void displayRoutes() {
        if (edges.empty()) {
            cout << "\nNo routes available.\n";
            return;
        }

        cout << "\nRoute Network:\n";
        cout << "----------------------------------------\n";
        for (const Edge &edge : edges) {
            cout << "Route: " << edge.source << " -> " << edge.target << "\n";
            cout << "Distance: " << edge.weight << " km\n";
            cout << "Carbon Emission: " << edge.carbonEmission << " kg CO2\n";
            cout << "----------------------------------------\n";
        }
    }

    void displayEmissionsByCity() {
        if (cityEmissions.empty()) {
            cout << "\nNo emission data available.\n";
            return;
        }

        sort(cityEmissions.begin(), cityEmissions.end(),
             [](const CityEmission &a, const CityEmission &b) {
                 return a.totalEmission > b.totalEmission;
             });

        cout << "\nCarbon Emissions by City (Sorted):\n";
        cout << "----------------------------------------\n";
        for (const CityEmission &city : cityEmissions) {
            if (city.totalEmission > 0) {
                cout << "City: " << city.cityName << "\n";
                cout << "Total Emissions: " << city.totalEmission << " kg CO2\n";
                cout << "----------------------------------------\n";
            }
        }
    }

    vector<double> calculateShortestPaths(const string &startCity) {
        int startIndex = getCityIndex(startCity);
        vector<double> distances(nodes, NO_PATH);
        distances[startIndex] = 0;

        for (int i = 0; i < nodes - 1; i++) {
            for (const Edge &edge : edges) {
                int srcIndex = getCityIndex(edge.source);
                int tgtIndex = getCityIndex(edge.target);
                if (distances[srcIndex] != NO_PATH &&
                    (distances[tgtIndex] == NO_PATH ||
                     distances[srcIndex] + edge.weight < distances[tgtIndex])) {
                    distances[tgtIndex] = distances[srcIndex] + edge.weight;
                }
            }
        }

        return distances;
    }

    int getCityIndex(const string &cityName) {
        for (int i = 0; i < nodes; i++) {
            if (cityNames[i] == cityName)
                return i;
        }
        return -1;
    }

    string getCityName(int index) {
        return cityNames[index];
    }

    int getNumCities() {
        return nodes;
    }

    bool isSystemInitialized() {
        return isInitialized;
    }

    void saveSystemState() {
        ofstream outFile("system_state.txt");
        if (outFile.is_open()) {
            outFile << nodes << "\n";
            for (int i = 0; i < nodes; i++) {
                outFile << cityNames[i] << "\n";
            }
            for (const Edge &edge : edges) {
                outFile << edge.source << " " << edge.target << " " << edge.weight << " " << edge.carbonEmission << "\n";
            }
            outFile.close();
        } else {
            cerr << "Unable to open file for saving system state.\n";
        }
    }

    void loadSystemState() {
        ifstream inFile("system_state.txt");
        if (inFile.is_open()) {
            inFile >> nodes;
            inFile.ignore();
            for (int i = 0; i < nodes; i++) {
                getline(inFile, cityNames[i]);
                cityEmissions.push_back({cityNames[i], 0.0});
            }
            Edge edge;
            while (inFile >> edge.source >> edge.target >> edge.weight >> edge.carbonEmission) {
                edges.push_back(edge);
                int srcIndex = getCityIndex(edge.source);
                int tgtIndex = getCityIndex(edge.target);
                adjacencyList[srcIndex][tgtIndex] = 1;
                cityEmissions[srcIndex].totalEmission += edge.carbonEmission;
                cityEmissions[tgtIndex].totalEmission += edge.carbonEmission;
            }
            inFile.close();
            isInitialized = true;
        } else {
            cerr << "Unable to open file for loading system state.\n";
        }
    }
};

void handleAdminMenu(WasteTransportSystem &system) {
    while (true) {
        cout << "\nAdmin Menu:\n";
        cout << "1. Add New Route\n";
        cout << "2. Display All Routes\n";
        cout << "3. Calculate Shortest Paths\n";
        cout << "4. Display Emissions by City\n";
        cout << "5. Logout\n";
        cout << "Enter choice: ";

        int choice;
        cin >> choice;

        switch (choice) {
        case 1: {
            string source, target, wasteType;
            double distance;
            cout << "Enter source city: ";
            cin >> source;
            cout << "Enter target city: ";
            cin >> target;
            cout << "Enter distance (km): ";
            cin >> distance;
            cout << "Enter waste type (Plastic/Metal/Organic/Electronic): ";
            cin >> wasteType;
            system.addRoute(source, target, distance, wasteType);
            cout << "Route added successfully!\n";
            break;
        }
        case 2:
            system.displayRoutes();
            break;
        case 3: {
            string startCity;
            cout << "Enter starting city: ";
            cin >> startCity;
            vector<double> distances = system.calculateShortestPaths(startCity);
            cout << "\nShortest Paths from " << startCity << ":\n";
            for (int i = 0; i < system.getNumCities(); i++) {
                cout << "To " << system.getCityName(i) << ": ";
                if (distances[i] == NO_PATH)
                    cout << "No route available\n";
                else
                    cout << distances[i] << " km\n";
            }
            break;
        }
        case 4:
            system.displayEmissionsByCity();
            break;
        case 5:
            cout << "Logging out from admin account...\n";
            return;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    }
}

void handleUserMenu(WasteTransportSystem &system) {
    while (true) {
        cout << "\nUser Menu:\n";
        cout << "1. Display All Routes\n";
        cout << "2. Calculate Shortest Paths\n";
        cout << "3. Display Emissions by City\n";
        cout << "4. Logout\n";
        cout << "Enter choice: ";

        int choice;
        cin >> choice;

        switch (choice) {
        case 1:
            system.displayRoutes();
            break;
        case 2: {
            string startCity;
            cout << "Enter starting city: ";
            cin >> startCity;
            vector<double> distances = system.calculateShortestPaths(startCity);
            cout << "\nShortest Paths from " << startCity << ":\n";
            for (int i = 0; i < system.getNumCities(); i++) {
                cout << "To " << system.getCityName(i) << ": ";
                if (distances[i] == NO_PATH)
                    cout << "No route available\n";
                else
                    cout << distances[i] << " km\n";
            }
            break;
        }
        case 3:
            system.displayEmissionsByCity();
            break;
        case 4:
            cout << "Logging out from user account...\n";
            return;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    }
}

int main() {
    WasteTransportSystem system;

    // Load the system state from the file
    system.loadSystemState();

    bool adminLoggedIn = false;

    while (true) {
        cout << "\nWaste Transport System\n";
        cout << "1. Admin Login\n";
        cout << "2. User Login\n";
        cout << "3. Exit\n";
        cout << "Enter choice: ";

        int choice;
        cin >> choice;

        switch (choice) {
        case 1:
            if (!system.isSystemInitialized()) {
                if (system.adminLogin()) {
                    adminLoggedIn = true;
                    system.initializeSystem();
                    handleAdminMenu(system);
                }
            } else {
                if (system.adminLogin()) {
                    handleAdminMenu(system);
                }
            }
            break;
        case 2:
            if (!system.isSystemInitialized()) {
                cout << "System not initialized. Please contact admin.\n";
            } else {
                system.userLogin();
                handleUserMenu(system);
            }
            break;
        case 3:
            cout << "Exiting system. Goodbye!\n";
            return 0;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}
