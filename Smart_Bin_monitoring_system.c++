#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <string>
#include <algorithm> // For remove function
#include <fstream>

using namespace std;

// Structure to store admin credentials
struct Admin {
    string name;
    int password; // 5-digit positive integer password
};

class LoginSystem {
private:
    vector<Admin> admins; // Vector to store admin credentials

public:
    LoginSystem() {
        // Prepopulate admin credentials
        admins.push_back({"adarsh", 12345});
        admins.push_back({"bhargavi", 54321});
        admins.push_back({"shreyas", 67890});
        admins.push_back({"shubham", 67890});
    }

    // Function to authenticate an admin
    bool adminLogin() {
        string adminName;
        int password;

        cout << "Enter Admin ID (Name): ";
        cin.ignore();
        getline(cin, adminName);

        cout << "Enter Password (5-digit positive integer): ";
        cin >> password;

        // Validate password format
        if (password < 10000 || password > 99999) {
            cout << "Invalid Password. Must be a 5-digit positive integer.\n";
            return false;
        }

        // Check credentials in the array
        for (const Admin &admin : admins) {
            if (admin.name == adminName && admin.password == password) {
                cout << "Admin login successful!\n";
                return true;
            }
        }

        cout << "Invalid Admin ID or Password.\n";
        return false;
    }

    // Function for user login (No authentication required in this case)
    void userLogin() {
        cout << "Welcome, User! You are logged in.\n";
    }
};

class BinMonitoringSystem {
private:
    struct Bin {
        int id;
        string location;
        double fillLevel;
        bool needsCollection;
        bool underMaintenance;
    };

    vector<Bin> bins;
    map<string, vector<int>> locationBins;
    queue<int> collectionQueue;

    const double FILL_THRESHOLD = 80.0;
    bool isAdminLoggedIn;

    void saveBinsToFile() {
        ofstream outFile("bins.txt");
        if (outFile.is_open()) {
            for (const auto &bin : bins) {
                outFile << bin.id << "," << bin.location << "," << bin.fillLevel << "," << bin.needsCollection << "," << bin.underMaintenance << "\n";
            }
            outFile.close();
        } else {
            cerr << "Unable to open file for saving bins.\n";
        }
    }

    void loadBinsFromFile() {
        ifstream inFile("bins.txt");
        if (inFile.is_open()) {
            string line;
            while (getline(inFile, line)) {
                size_t pos1 = line.find(',');
                size_t pos2 = line.find(',', pos1 + 1);
                size_t pos3 = line.find(',', pos2 + 1);
                size_t pos4 = line.find(',', pos3 + 1);

                int id = stoi(line.substr(0, pos1));
                string location = line.substr(pos1 + 1, pos2 - pos1 - 1);
                double fillLevel = stod(line.substr(pos2 + 1, pos3 - pos2 - 1));
                bool needsCollection = stoi(line.substr(pos3 + 1, pos4 - pos3 - 1));
                bool underMaintenance = stoi(line.substr(pos4 + 1));

                bins.push_back({id, location, fillLevel, needsCollection, underMaintenance});
                locationBins[location].push_back(id);
                if (needsCollection) {
                    collectionQueue.push(id);
                }
            }
            inFile.close();
        } else {
            cerr << "Unable to open file for loading bins.\n";
        }
    }

public:
    BinMonitoringSystem() : isAdminLoggedIn(false) {
        loadBinsFromFile();
    }

    ~BinMonitoringSystem() {
        saveBinsToFile();
    }

    void setAdminLoginStatus(bool status) {
        isAdminLoggedIn = status;
    }

    bool getAdminLoginStatus() const {
        return isAdminLoggedIn;
    }

    // Admin Functions
    void addBin() {
        if (!isAdminLoggedIn) {
            cout << "Access denied. Admin privileges required.\n";
            return;
        }

        Bin bin;
        cout << "Enter bin ID: ";
        cin >> bin.id;

        // Check for duplicate bin ID
        for (const Bin &existingBin : bins) {
            if (existingBin.id == bin.id) {
                cout << "Error: Bin ID already exists.\n";
                return;
            }
        }

        cout << "Enter location: ";
        cin.ignore();
        getline(cin, bin.location);
        bin.fillLevel = 0.0;
        bin.needsCollection = false;
        bin.underMaintenance = false;

        bins.push_back(bin);
        locationBins[bin.location].push_back(bin.id);
        cout << "Bin added successfully!\n";
    }

    void editBinDetails() {
        if (!isAdminLoggedIn) {
            cout << "Access denied. Admin privileges required.\n";
            return;
        }

        int binId;
        cout << "Enter bin ID to edit: ";
        cin >> binId;

        for (Bin &bin : bins) {
            if (bin.id == binId) {
                cout << "Enter new location: ";
                cin.ignore();
                getline(cin, bin.location);
                cout << "Bin details updated successfully!\n";
                return;
            }
        }
        cout << "Bin not found.\n";
    }

    void deleteBin() {
        if (!isAdminLoggedIn) {
            cout << "Access denied. Admin privileges required.\n";
            return;
        }

        int binId;
        cout << "Enter bin ID to delete: ";
        cin >> binId;

        for (auto it = bins.begin(); it != bins.end(); ++it) {
            if (it->id == binId) {
                locationBins[it->location].erase(
                    remove(locationBins[it->location].begin(), locationBins[it->location].end(), binId),
                    locationBins[it->location].end());
                bins.erase(it);
                cout << "Bin deleted successfully!\n";
                return;
            }
        }
        cout << "Bin not found.\n";
    }

    void updateBinStatus() {
        if (!isAdminLoggedIn) {
            cout << "Access denied. Admin privileges required.\n";
            return;
        }

        int binId;
        cout << "Enter bin ID: ";
        cin >> binId;

        for (Bin &bin : bins) {
            if (bin.id == binId) {
                cout << "Enter new fill level (0-100): ";
                cin >> bin.fillLevel;

                if (bin.fillLevel >= FILL_THRESHOLD && !bin.needsCollection) {
                    bin.needsCollection = true;
                    collectionQueue.push(bin.id);
                    cout << "Alert: Bin needs collection!\n";
                }
                return;
            }
        }
        cout << "Bin not found.\n";
    }

    void viewBinStatus() const {
        if (bins.empty()) {
            cout << "No bins available.\n";
            return;
        }

        cout << "\n=== Bin Status ===\n";
        for (const Bin &bin : bins) {
            cout << "Bin " << bin.id << " at " << bin.location << ":\n";
            cout << "Fill Level: " << bin.fillLevel << "%\n";
            cout << "Status: " << (bin.needsCollection ? "Needs collection" : "OK") << "\n";
            cout << "Maintenance: " << (bin.underMaintenance ? "Under Maintenance" : "Operational") << "\n\n";
        }
    }

    void showMenu() const {
        if (isAdminLoggedIn) {
            cout << "\n=== Admin Menu ===\n";
            cout << "1. Add Bin\n";
            cout << "2. Update Bin Status\n";
            cout << "3. Edit Bin Details\n";
            cout << "4. Delete Bin\n";
            cout << "5. View Bin Status\n";
            cout << "6. Logout\n";
        } else {
            cout << "\n=== User Menu ===\n";
            cout << "1. View Bin Status\n";
            cout << "2. Logout\n";
        }
    }
};

int main() {
    LoginSystem loginSystem;
    BinMonitoringSystem system;

    while (true) {
        int choice;
        cout << "\n=== Main Menu ===\n";
        cout << "1. Admin Login\n";
        cout << "2. User Login\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 3) {
            cout << "Exiting the system. Goodbye!\n";
            break;
        }

        if (choice == 1) {
            if (loginSystem.adminLogin()) {
                system.setAdminLoginStatus(true);
            } else {
                continue;
            }
        } else if (choice == 2) {
            loginSystem.userLogin();
            system.setAdminLoginStatus(false);
        } else {
            cout << "Invalid choice. Try again.\n";
            continue;
        }

        while (true) {
            system.showMenu();
            cout << "Enter choice: ";
            cin >> choice;

            if ((choice == 6 && system.getAdminLoginStatus()) || (choice == 2 && !system.getAdminLoginStatus())) {
                break;
            }

            switch (choice) {
            case 1:
                if (system.getAdminLoginStatus()) {
                    system.addBin();
                } else {
                    system.viewBinStatus();
                }
                break;
            case 2:
                if (system.getAdminLoginStatus()) {
                    system.updateBinStatus();
                } else {
                    cout << "Invalid choice for users.\n";
                }
                break;
            case 3:
                if (system.getAdminLoginStatus()) {
                    system.editBinDetails();
                } else {
                    cout << "Invalid choice.\n";
                }
                break;
            case 4:
                if (system.getAdminLoginStatus()) {
                    system.deleteBin();
                } else {
                    cout << "Invalid choice.\n";
                }
                break;
            case 5:
                system.viewBinStatus();
                break;
            default:
                cout << "Invalid choice.\n";
            }
        }
    }

    return 0;
}
