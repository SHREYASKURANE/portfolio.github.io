#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>

using namespace std;

// Structure to store admin credentials
struct Admin {
    string name;
    int password;
};

// Structure to store waste records
struct WasteRecord {
    string source;
    double quantity;
    string status;
};

class WasteTrackingSystem {
private:
    // Array of admin credentials
    Admin admins[4];
    int adminCount;
    vector<WasteRecord> records;

    // Function to add a waste record
    void addWasteRecord() {
        string source;
        double quantity;
        string status;

        cout << "\nEnter Source of Waste (e.g., Company, Farm, Household): ";
        cin.ignore(); // Clear the input buffer
        getline(cin, source);

        cout << "Enter Quantity of Waste (kg): ";
        cin >> quantity;

        cout << "Enter Processing Status (e.g., Collected, Recycled, Disposed): ";
        cin.ignore(); // Clear the input buffer
        getline(cin, status);

        // Add the record
        records.push_back({source, quantity, status});
        saveRecordsToFile();

        cout << "\nRecord Added Successfully!\n";
        cout << "Source: " << source << "\n";
        cout << "Quantity: " << quantity << " kg\n";
        cout << "Status: " << status << "\n";
    }

    // Function to display all waste records
    void displayRecords() {
        if (records.empty()) {
            cout << "\nNo records to display.\n";
            return;
        }

        // Map to store aggregated records (source+status -> total quantity)
        map<pair<string, string>, double> aggregatedRecords;

        // Aggregate quantities for same source and status
        for (const auto &record : records) {
            pair<string, string> key = {record.source, record.status};
            aggregatedRecords[key] += record.quantity;
        }

        // Display records grouped by source and status
        cout << "\nDisplaying aggregated waste records:\n";
        cout << "--------------------------------\n";

        string currentSource = "";
        for (const auto &record : aggregatedRecords) {
            // Print source header if it's a new source
            if (currentSource != record.first.first) {
                currentSource = record.first.first;
                cout << "\nSource: " << currentSource << "\n";
                cout << "--------------------------------\n";
            }

            cout << "Status: " << record.first.second << "\n";
            cout << "Total Quantity: " << record.second << " kg\n";
            cout << "--------------------------------\n";
        }
    }

    // Function to save records to a file
    void saveRecordsToFile() {
        ofstream outFile("waste_records.txt");
        if (outFile.is_open()) {
            for (const auto &record : records) {
                outFile << record.source << "," << record.quantity << "," << record.status << "\n";
            }
            outFile.close();
        } else {
            cerr << "Unable to open file for saving records.\n";
        }
    }

    // Function to load records from a file
    void loadRecordsFromFile() {
        ifstream inFile("waste_records.txt");
        if (inFile.is_open()) {
            string line;
            while (getline(inFile, line)) {
                size_t pos1 = line.find(',');
                size_t pos2 = line.find(',', pos1 + 1);
                string source = line.substr(0, pos1);
                double quantity = stod(line.substr(pos1 + 1, pos2 - pos1 - 1));
                string status = line.substr(pos2 + 1);
                records.push_back({source, quantity, status});
            }
            inFile.close();
        } else {
            cerr << "Unable to open file for loading records.\n";
        }
    }

public:
    WasteTrackingSystem() {
        // Prepopulate admin credentials
        adminCount = 4;
        admins[0] = {"adarsh", 12345};
        admins[1] = {"bhargavi", 54321};
        admins[2] = {"shreyas", 67890};
        admins[3] = {"shubham", 67890};
        loadRecordsFromFile();
    }

    // Function to authenticate an admin
    bool adminLogin() {
        string adminName;
        int password;

        cout << "Enter Admin ID (Name): ";
        cin.ignore(); // Clear newline from previous input
        getline(cin, adminName);

        cout << "Enter Password (5-digit positive integer): ";
        cin >> password;

        // Validate password format
        if (password < 10000 || password > 99999) {
            cout << "Invalid Password. Must be a 5-digit positive integer.\n";
            return false;
        }

        // Check credentials in the array
        for (int i = 0; i < adminCount; ++i) {
            if (admins[i].name == adminName && admins[i].password == password) {
                cout << "Admin login successful!\n";
                return true;
            }
        }

        cout << "Invalid Admin ID or Password.\n";
        return false;
    }

    // Function to handle admin menu
    void adminMenu() {
        int choice;
        do {
            cout << "\nAdmin Menu:\n";
            cout << "1. Add Waste Record\n";
            cout << "2. Display Records\n";
            cout << "3. Logout\n";
            cout << "Enter your choice: ";
            cin >> choice;

            switch (choice) {
            case 1:
                addWasteRecord();
                break;
            case 2:
                displayRecords();
                break;
            case 3:
                cout << "Logging out from admin account...\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
            }
        } while (choice != 3);
    }

    // Function to handle user menu
    void userMenu() {
        cout << "Welcome, User! You are logged in.\n";
        int choice;
        do {
            cout << "\nUser Menu:\n";
            cout << "1. Display Records\n";
            cout << "2. Logout\n";
            cout << "Enter your choice: ";
            cin >> choice;

            switch (choice) {
            case 1:
                displayRecords();
                break;
            case 2:
                cout << "Logging out from user account...\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
            }
        } while (choice != 2);
    }
};

int main() {
    WasteTrackingSystem system;

    while (true) {
        int choice;
        cout << "\nWaste Tracking System\n";
        cout << "1. Admin Login\n";
        cout << "2. User Login\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            if (system.adminLogin()) {
                system.adminMenu();
            }
            break;
        case 2:
            system.userMenu();
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
