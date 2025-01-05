#include <iostream>
#include <unordered_map>
#include <queue>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <algorithm>

using namespace std;

struct Admin {
    string name;
    int password;
};

struct WasteCategory {
    string name;
    string description;
    int defaultRiskLevel;
    vector<string> disposalMethods;
};

const vector<WasteCategory> WASTE_CATEGORIES = {
    {"RADIOACTIVE", "Nuclear waste, radioactive materials", 10, {"Underground geological repository disposal", "Decay storage in lead-lined containers"}},
    {"INFECTIOUS", "Medical waste, biological materials", 9, {"Autoclave sterilization followed by incineration", "Chemical disinfection and secure landfill disposal"}},
    {"TOXIC", "Pesticides, heavy metals, poisonous materials", 8, {"Chemical treatment and neutralization", "Secure landfill with chemical stabilization"}},
    {"REACTIVE", "Explosive materials, unstable compounds", 8, {"Controlled detonation in designated areas", "Chemical stabilization followed by secure disposal"}},
    {"CORROSIVE", "Acids, batteries, cleaning chemicals", 7, {"Neutralization followed by wastewater treatment", "Recovery and recycling of acid/base components"}},
    {"IGNITABLE", "Flammable liquids, solvents, oils", 6, {"Controlled incineration at licensed facilities", "Fuel blending for energy recovery"}},
    {"CHEMICAL", "Laboratory chemicals, industrial compounds", 5, {"Chemical oxidation or reduction treatment", "Solidification and stabilization before disposal"}},
    {"E-WASTE", "Electronic equipment, circuit boards", 4, {"Component separation and recycling", "Precious metal recovery and safe disposal"}}
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
};

void logAction(const string &action) {
    ofstream logFile("hazardous_waste_log.txt", ios::app);
    if (logFile.is_open()) {
        time_t now = time(nullptr);
        logFile << ctime(&now) << " - " << action << "\n";
        logFile.close();
    } else {
        cerr << "Unable to open log file.\n";
    }
}

struct HazardousItem {
    string name;
    string category;
    string disposalMethod;
    int riskLevel;

    HazardousItem(string n, string c, string d, int r)
        : name(n), category(c), disposalMethod(d), riskLevel(r) {}
};

struct CompareRisk {
    bool operator()(const HazardousItem &a, const HazardousItem &b) {
        return a.riskLevel < b.riskLevel;
    }
};

class AdminFunctions {
private:
    unordered_map<string, string> disposalMethods;
    unordered_map<string, int> itemInventory;
    priority_queue<HazardousItem, vector<HazardousItem>, CompareRisk> disposalQueue;

    void saveDisposalMethodsToFile() {
        ofstream outFile("disposal_methods.txt");
        if (outFile.is_open()) {
            for (const auto &pair : disposalMethods) {
                outFile << pair.first << "," << pair.second << "\n";
            }
            outFile.close();
        } else {
            cerr << "Unable to open file for saving disposal methods.\n";
        }
    }

    void loadDisposalMethodsFromFile() {
        ifstream inFile("disposal_methods.txt");
        if (inFile.is_open()) {
            string line;
            while (getline(inFile, line)) {
                size_t pos = line.find(',');
                string item = line.substr(0, pos);
                string method = line.substr(pos + 1);
                disposalMethods[item] = method;
            }
            inFile.close();
        } else {
            cerr << "Unable to open file for loading disposal methods.\n";
        }
    }

    void saveInventoryToFile() {
        ofstream outFile("inventory.txt");
        if (outFile.is_open()) {
            for (const auto &pair : itemInventory) {
                outFile << pair.first << "," << pair.second << "\n";
            }
            outFile.close();
        } else {
            cerr << "Unable to open file for saving inventory.\n";
        }
    }

    void loadInventoryFromFile() {
        ifstream inFile("inventory.txt");
        if (inFile.is_open()) {
            string line;
            while (getline(inFile, line)) {
                size_t pos = line.find(',');
                string item = line.substr(0, pos);
                int count = stoi(line.substr(pos + 1));
                itemInventory[item] = count;
            }
            inFile.close();
        } else {
            cerr << "Unable to open file for loading inventory.\n";
        }
    }

    void saveDisposalQueueToFile() {
        ofstream outFile("disposal_queue.txt");
        if (outFile.is_open()) {
            auto tempQueue = disposalQueue;
            while (!tempQueue.empty()) {
                HazardousItem item = tempQueue.top();
                tempQueue.pop();
                outFile << item.name << "," << item.category << "," << item.disposalMethod << "," << item.riskLevel << "\n";
            }
            outFile.close();
        } else {
            cerr << "Unable to open file for saving disposal queue.\n";
        }
    }

    void loadDisposalQueueFromFile() {
        ifstream inFile("disposal_queue.txt");
        if (inFile.is_open()) {
            string line;
            while (getline(inFile, line)) {
                size_t pos1 = line.find(',');
                size_t pos2 = line.find(',', pos1 + 1);
                size_t pos3 = line.find(',', pos2 + 1);
                string name = line.substr(0, pos1);
                string category = line.substr(pos1 + 1, pos2 - pos1 - 1);
                string method = line.substr(pos2 + 1, pos3 - pos2 - 1);
                int riskLevel = stoi(line.substr(pos3 + 1));
                disposalQueue.emplace(name, category, method, riskLevel);
            }
            inFile.close();
        } else {
            cerr << "Unable to open file for loading disposal queue.\n";
        }
    }

public:
    AdminFunctions() {
        loadDisposalMethodsFromFile();
        loadInventoryFromFile();
        loadDisposalQueueFromFile();
    }

    ~AdminFunctions() {
        saveDisposalMethodsToFile();
        saveInventoryToFile();
        saveDisposalQueueToFile();
    }

    void displayWasteCategories() {
        cout << "\nPredefined Hazardous Waste Categories:\n";
        cout << string(100, '=') << "\n";
        cout << setw(15) << left << "Category"
             << setw(45) << left << "| Description"
             << setw(15) << left << "| Risk Level"
             << "| Recommended Disposal Methods\n";
        cout << string(100, '=') << "\n";

        for (const auto &category : WASTE_CATEGORIES) {
            cout << setw(15) << left << category.name
                 << "| " << setw(43) << left << category.description
                 << "| " << setw(13) << left << category.defaultRiskLevel
                 << "| 1. " << category.disposalMethods[0] << "\n";
            cout << setw(15) << " "
                 << "| " << setw(43) << " "
                 << "| " << setw(13) << " "
                 << "| 2. " << category.disposalMethods[1] << "\n";
            cout << string(100, '-') << "\n";
        }
    }

    vector<string> getRecommendedDisposalMethods(const string &category) {
        for (const auto &cat : WASTE_CATEGORIES) {
            if (cat.name == category) {
                return cat.disposalMethods;
            }
        }
        return vector<string>();
    }

    int getDefaultRiskLevel(const string &category) {
        for (const auto &cat : WASTE_CATEGORIES) {
            if (cat.name == category) {
                return cat.defaultRiskLevel;
            }
        }
        return 5;
    }

    void addDisposalMethod(const string &item, const string &method) {
        disposalMethods[item] = method;
        cout << "Admin: Added disposal method for " << item << " -> " << method << "\n";
        logAction("Added disposal method for " + item + " -> " + method);
    }

    void updateInventory(const string &item, int count) {
        itemInventory[item] += count;
        cout << "Admin: Updated inventory for " << item << " by " << count << " units. Total: " << itemInventory[item] << "\n";
        logAction("Updated inventory for " + item + " by " + to_string(count) + " units");
    }

    void displayDisposalMethods() {
        cout << "\nDisposal Methods:\n";
        for (const auto &pair : disposalMethods) {
            cout << "Item: " << pair.first << " -> Method: " << pair.second << "\n";
        }
    }

    void displayInventory() {
        cout << "\nItem Inventory:\n";
        for (const auto &pair : itemInventory) {
            cout << "Item: " << pair.first << " -> Quantity: " << pair.second << "\n";
        }
    }

    void enqueueHazardousItem(const string &name, const string &category, int riskLevel) {
        string disposalMethod = disposalMethods[name];
        if (disposalMethod.empty()) {
            auto recommendedMethods = getRecommendedDisposalMethods(category);
            if (!recommendedMethods.empty()) {
                cout << "\nRecommended disposal methods for " << category << ":\n";
                for (size_t i = 0; i < recommendedMethods.size(); ++i) {
                    cout << i + 1 << ". " << recommendedMethods[i] << "\n";
                }
            }
        }
        disposalQueue.emplace(name, category, disposalMethod, riskLevel);
        cout << "Admin: Enqueued hazardous item: " << name << " with risk level: " << riskLevel << "\n";
        logAction("Enqueued hazardous item: " + name + " with risk level: " + to_string(riskLevel));
    }

    void viewDisposalQueue() {
        cout << "\nPending Disposal Queue:\n";
        auto tempQueue = disposalQueue;
        while (!tempQueue.empty()) {
            HazardousItem item = tempQueue.top();
            tempQueue.pop();
            cout << "Item: " << item.name
                 << " | Category: " << item.category
                 << " | Risk Level: " << item.riskLevel << "\n";
        }
    }
};

void adminMenu(AdminFunctions &admin) {
    int choice;
    do {
        cout << "\nAdmin Menu:\n";
        cout << "1. Add Disposal Method\n";
        cout << "2. Update Inventory\n";
        cout << "3. Display Disposal Methods\n";
        cout << "4. Display Inventory\n";
        cout << "5. Enqueue Hazardous Item\n";
        cout << "6. View Disposal Queue\n";
        cout << "7. View Waste Categories\n";
        cout << "8. Logout\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1: {
            string item, method;
            cin.ignore();
            cout << "Enter item name: ";
            getline(cin, item);

            admin.displayWasteCategories();

            cout << "Enter disposal method: ";
            getline(cin, method);
            admin.addDisposalMethod(item, method);
            break;
        }
        case 2: {
            string item;
            int count;
            cin.ignore();
            cout << "Enter item name: ";
            getline(cin, item);
            cout << "Enter count: ";
            cin >> count;
            admin.updateInventory(item, count);
            break;
        }
        case 3:
            admin.displayDisposalMethods();
            break;
        case 4:
            admin.displayInventory();
            break;
        case 5: {
            string name, category;
            int riskLevel;
            cin.ignore();

            admin.displayWasteCategories();

            cout << "Enter item name: ";
            getline(cin, name);
            cout << "Enter category (from the list above): ";
            getline(cin, category);

            bool validCategory = false;
            int defaultRisk = admin.getDefaultRiskLevel(category);

            for (const auto &cat : WASTE_CATEGORIES) {
                if (cat.name == category) {
                    validCategory = true;
                    break;
                }
            }

            if (!validCategory) {
                cout << "Warning: Entered category doesn't match predefined categories.\n";
                cout << "Proceeding with default risk level of 5.\n";
            } else {
                cout << "Default risk level for " << category << " is " << defaultRisk << "\n";
            }

            cout << "Enter risk level (1-10) [Press Enter to use default " << defaultRisk << "]: ";
            string riskInput;
            getline(cin, riskInput);

            if (riskInput.empty()) {
                riskLevel = defaultRisk;
            } else {
                riskLevel = stoi(riskInput);
                if (riskLevel < 1 || riskLevel > 10) {
                    cout << "Invalid risk level. Using default risk level of " << defaultRisk << "\n";
                    riskLevel = defaultRisk;
                }
            }

            admin.enqueueHazardousItem(name, category, riskLevel);
            break;
        }
        case 6:
            admin.viewDisposalQueue();
            break;
        case 7:
            admin.displayWasteCategories();
            break;
        case 8:
            cout << "Logging out...\n";
            break;
        default:
            cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 8);
}

void userMenu(AdminFunctions &admin) {
    int choice;
    do {
        cout << "\nUser Menu:\n";
        cout << "1. Display Disposal Methods\n";
        cout << "2. Display Inventory\n";
        cout << "3. View Disposal Queue\n";
        cout << "4. View Waste Categories\n";
        cout << "5. Logout\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            admin.displayDisposalMethods();
            break;
        case 2:
            admin.displayInventory();
            break;
        case 3:
            admin.viewDisposalQueue();
            break;
        case 4:
            admin.displayWasteCategories();
            break;
        case 5:
            cout << "Logging out...\n";
            return;
        default:
            cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 5);
}

int main() {
    LoginSystem loginSystem;
    AdminFunctions admin;

    int choice;
    do {
        cout << "\nMain Menu:\n";
        cout << "1. Admin Login\n";
        cout << "2. User Login\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            if (loginSystem.adminLogin()) {
                adminMenu(admin);
            }
            break;
        case 2:
            cout << "User Login Successful!\n";
            userMenu(admin);
            break;
        case 3:
            cout << "Exiting program...\n";
            break;
        default:
            cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 3);

    cout << "\nThank you for using the system.\n";
    return 0;
}
