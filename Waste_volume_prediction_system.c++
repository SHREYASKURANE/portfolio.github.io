#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <map>
#include <algorithm>
#include <fstream>

using namespace std;

// Structure for waste volume data
struct WasteData {
    string date;
    string location;
    double volume;

    WasteData(string d, string l, double v) : date(d), location(l), volume(v) {}
};

// Trie node for location-based searching
struct TrieNode {
    map<char, TrieNode *> children;
    bool isEndOfWord;
    vector<WasteData *> locationData;

    TrieNode() : isEndOfWord(false) {}
};

// Structure to store admin credentials (from your login system)
struct Admin {
    string name;
    int password;
};

// Waste Volume Prediction System class
class WastePredictionSystem {
private:
    vector<WasteData> historicalData;
    TrieNode *locationTrie;
    const int NUM_BUCKETS = 10;

    // Initialize Trie node
    TrieNode *createNode() {
        return new TrieNode();
    }

    // Insert location into Trie
    void insertLocation(string location, WasteData *data) {
        TrieNode *current = locationTrie;

        for (char c : location) {
            if (current->children.find(c) == current->children.end()) {
                current->children[c] = createNode();
            }
            current = current->children[c];
        }

        current->isEndOfWord = true;
        current->locationData.push_back(data);
    }

    // Bucket Sort implementation
    void bucketSort(vector<WasteData> &data) {
        if (data.empty())
            return;

        vector<vector<WasteData>> buckets(NUM_BUCKETS);

        // Find range of volumes
        double maxVol = data[0].volume;
        double minVol = data[0].volume;
        for (const auto &entry : data) {
            maxVol = max(maxVol, entry.volume);
            minVol = min(minVol, entry.volume);
        }

        if (maxVol == minVol) {
            // All volumes are the same; no need to sort
            return;
        }

        // Distribute into buckets
        for (const auto &entry : data) {
            int bucketIndex = (int)((entry.volume - minVol) * NUM_BUCKETS / (maxVol - minVol + 1));
            bucketIndex = min(bucketIndex, NUM_BUCKETS - 1);
            buckets[bucketIndex].push_back(entry);
        }

        // Sort individual buckets
        for (auto &bucket : buckets) {
            sort(bucket.begin(), bucket.end(),
                 [](const WasteData &a, const WasteData &b) {
                     return a.volume < b.volume;
                 });
        }

        // Concatenate all buckets
        data.clear();
        for (const auto &bucket : buckets) {
            data.insert(data.end(), bucket.begin(), bucket.end());
        }
    }

    void saveWasteDataToFile() {
        ofstream outFile("waste_data.txt");
        if (outFile.is_open()) {
            for (const auto &data : historicalData) {
                outFile << data.date << "," << data.location << "," << data.volume << "\n";
            }
            outFile.close();
        } else {
            cerr << "Unable to open file for saving waste data.\n";
        }
    }

    void loadWasteDataFromFile() {
        ifstream inFile("waste_data.txt");
        if (inFile.is_open()) {
            string line;
            while (getline(inFile, line)) {
                size_t pos1 = line.find(',');
                size_t pos2 = line.find(',', pos1 + 1);
                string date = line.substr(0, pos1);
                string location = line.substr(pos1 + 1, pos2 - pos1 - 1);
                double volume = stod(line.substr(pos2 + 1));

                WasteData newData(date, location, volume);
                historicalData.push_back(newData);
                insertLocation(location, &historicalData.back());
            }
            inFile.close();
        } else {
            cerr << "Unable to open file for loading waste data.\n";
        }
    }

public:
    WastePredictionSystem() {
        locationTrie = createNode();
        loadWasteDataFromFile();
    }

    ~WastePredictionSystem() {
        saveWasteDataToFile();
    }

    // Add new waste data
    void addWasteData(string date, string location, double volume) {
        WasteData newData(date, location, volume);
        historicalData.push_back(newData);
        insertLocation(location, &historicalData.back());
        saveWasteDataToFile();
    }

    // Generate prediction for a location
    vector<double> predictWasteVolume(string location) {
        vector<double> predictions;
        TrieNode *current = locationTrie;

        // Find location in Trie
        for (char c : location) {
            if (current->children.find(c) == current->children.end()) {
                cout << "No data available for this location.\n";
                return predictions;
            }
            current = current->children[c];
        }

        if (!current->isEndOfWord) {
            cout << "Location not found.\n";
            return predictions;
        }

        // Access location history
        vector<WasteData *> &locationHistory = current->locationData;
        if (locationHistory.empty()) {
            cout << "No historical data for the location.\n";
            return predictions;
        }

        // Sort historical data
        vector<WasteData> sortedData;
        for (auto *data : locationHistory) {
            if (data) {
                sortedData.push_back(*data);
            }
        }

        if (sortedData.empty()) {
            cout << "Error: No valid historical data found.\n";
            return predictions;
        }

        bucketSort(sortedData);

        // Calculate predictions for the next 3 months
        double avgVolume = 0;
        for (const auto &data : sortedData) {
            avgVolume += data.volume;
        }
        avgVolume /= sortedData.size();

        // Linear trend prediction
        double trend = 0;
        if (sortedData.size() > 1) {
            trend = (sortedData.back().volume - sortedData.front().volume) / (sortedData.size() - 1);
        }

        for (int i = 1; i <= 3; ++i) {
            predictions.push_back(max(0.0, avgVolume + trend * i)); // Ensure non-negative predictions
        }

        return predictions;
    }

    // Generate forecast report
    void generateForecastReport(string location) {
        vector<double> predictions = predictWasteVolume(location);
        if (predictions.empty()) {
            return;
        }

        cout << "\nWaste Volume Forecast Report for " << location << "\n";
        cout << "----------------------------------------\n";
        cout << "Predictions for the next 3 months:\n\n";

        for (int i = 0; i < predictions.size(); i++) {
            cout << "Month " << (i + 1) << ": " << fixed << setprecision(2)
                 << predictions[i] << " tons\n";
        }

        cout << "\nNote: Predictions are based on historical data and trends.\n";
    }
};

class LoginSystem {
private:
    Admin admins[4];
    int adminCount;
    WastePredictionSystem predictionSystem;

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

        for (int i = 0; i < static_cast<int>(adminCount); ++i) {
            if (admins[i].name == adminName && admins[i].password == password) {
                cout << "Admin login successful!\n";
                return true;
            }
        }
        cout << "Invalid Admin ID or Password.\n";
        return false;
    }

    void adminDashboard() {
        while (true) {
            cout << "\nAdmin Dashboard\n";
            cout << "1. Add New Waste Data\n";
            cout << "2. Generate Forecast Report\n";
            cout << "3. Logout\n";
            cout << "Enter choice: ";

            int choice;
            cin >> choice;

            switch (choice) {
            case 1: {
                string date, location;
                double volume;

                cout << "Enter date (DD-MM-YYYY): ";
                cin >> date;
                cout << "Enter location: ";
                cin.ignore();
                getline(cin, location);
                cout << "Enter waste volume (tons): ";
                cin >> volume;

                predictionSystem.addWasteData(date, location, volume);
                cout << "Data added successfully!\n";
                break;
            }
            case 2: {
                string location;
                cout << "Enter location for forecast: ";
                cin.ignore();
                getline(cin, location);
                predictionSystem.generateForecastReport(location);
                break;
            }
            case 3:
                return;
            default:
                cout << "Invalid choice!\n";
            }
        }
    }

    void userDashboard() {
        while (true) {
            cout << "\nUser Dashboard\n";
            cout << "1. View Forecast Report\n";
            cout << "2. Logout\n";
            cout << "Enter choice: ";

            int choice;
            cin >> choice;

            switch (choice) {
            case 1: {
                string location;
                cout << "Enter location for forecast: ";
                cin.ignore();
                getline(cin, location);
                predictionSystem.generateForecastReport(location);
                break;
            }
            case 2:
                return;
            default:
                cout << "Invalid choice!\n";
            }
        }
    }

    void userLogin() {
        cout << "Welcome, User! You are logged in.\n";
        userDashboard();
    }
};

int main() {
    LoginSystem loginSystem;
    while (true) {
        cout << "\nLogin Page\n";
        cout << "1. Admin Login\n";
        cout << "2. User Login\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";

        int choice;
        cin >> choice;

        switch (choice) {
        case 1:
            if (loginSystem.adminLogin()) {
                loginSystem.adminDashboard();
            }
            break;
        case 2:
            loginSystem.userLogin();
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
