#include <iostream>
#include <string>
#include <fstream>
using namespace std;

// Structure to store user credentials
struct Admin {
    string name;
    int password;
};

class LoginSystem {
private:
    Admin admins[4];
    int adminCount;

    void saveRecord(const string &wasteType, float quantity, float energyGenerated, float totalEnergy) {
        ofstream outFile("waste_records.txt", ios::app);
        if (outFile.is_open()) {
            outFile << wasteType << "," << quantity << "," << energyGenerated << "," << totalEnergy << "\n";
            outFile.close();
        } else {
            cerr << "Unable to open file for saving records.\n";
        }
    }

    void loadRecords() {
        ifstream inFile("waste_records.txt");
        if (inFile.is_open()) {
            string line;
            cout << "\nPast Waste Processing Records:\n";
            cout << "----------------------------\n";
            while (getline(inFile, line)) {
                size_t pos1 = line.find(',');
                size_t pos2 = line.find(',', pos1 + 1);
                size_t pos3 = line.find(',', pos2 + 1);

                string wasteType = line.substr(0, pos1);
                float quantity = stof(line.substr(pos1 + 1, pos2 - pos1 - 1));
                float energyGenerated = stof(line.substr(pos2 + 1, pos3 - pos2 - 1));
                float totalEnergy = stof(line.substr(pos3 + 1));

                cout << "Waste Type: " << wasteType << "\n";
                cout << "Quantity: " << quantity << " tons\n";
                cout << "Energy Generation Factor: " << energyGenerated << " MWh/ton\n";
                cout << "Total Energy: " << totalEnergy << " MWh\n";
                cout << "----------------------------\n";
            }
            inFile.close();
        } else {
            cerr << "No past records found.\n";
        }
    }

public:
    LoginSystem() {
        adminCount = 4;
        admins[0] = {"adarsh", 12345};
        admins[1] = {"bhargavi", 54321};
        admins[2] = {"shreyas", 67890};
        admins[3] = {"shubham", 67890};
    }

    void userLogin() {
        cout << "Welcome, User! You are logged in.\n";
        loadRecords();
        processWasteMenu();
    }

private:
    void processWasteMenu() {
        int choice;
        do {
            cout << "\nMenu:\n1. Process Waste\n2. Exit\nEnter choice: ";
            cin >> choice;

            switch (choice) {
            case 1: {
                string wasteType;
                float quantity; // Changed to float
                float energyGenerated;

                // Energy generated per ton for each waste type (MWh/ton)
                const float energyPerTonEwaste = 2.5;        // High due to metal content and complex materials
                const float energyPerTonAgricultural = 1.8;  // Moderate due to organic matter
                const float energyPerTonBiodegradable = 1.5; // Lower due to moisture content
                const float energyPerTonPlastic = 3.2;       // Highest due to petroleum-based materials
                const float energyPerTonMetal = 0.8;         // Lowest due to non-combustible nature

                cout << "\nSelect Waste Type:\n";
                cout << "1. E-Waste\n2. Agricultural\n3. Biodegradable\n4. Plastic\n5. Metal\nEnter choice: ";
                int wasteChoice;
                cin >> wasteChoice;

                switch (wasteChoice) {
                case 1:
                    wasteType = "E-Waste";
                    energyGenerated = energyPerTonEwaste;
                    break;
                case 2:
                    wasteType = "Agricultural";
                    energyGenerated = energyPerTonAgricultural;
                    break;
                case 3:
                    wasteType = "Biodegradable";
                    energyGenerated = energyPerTonBiodegradable;
                    break;
                case 4:
                    wasteType = "Plastic";
                    energyGenerated = energyPerTonPlastic;
                    break;
                case 5:
                    wasteType = "Metal";
                    energyGenerated = energyPerTonMetal;
                    break;
                default:
                    cout << "Invalid waste type!\n";
                    continue;
                }

                cout << "Enter Quantity of Waste (tons): ";
                cin >> quantity;

                // Calculate total energy generated
                float totalEnergy = quantity * energyGenerated;

                // Display results with detailed breakdown
                cout << "\nWaste Processing Report:\n";
                cout << "------------------------\n";
                cout << "Waste Type: " << wasteType << endl;
                cout << "Quantity Processed: " << quantity << " tons\n";
                cout << "Energy Generation Factor: " << energyGenerated << " MWh/ton\n";
                cout << "Total Energy Generated: " << totalEnergy << " MWh\n";

                // Save record to file
                saveRecord(wasteType, quantity, energyGenerated, totalEnergy);
                break;
            }
            case 2:
                cout << "Exiting to main menu...\n";
                break;
            default:
                cout << "Invalid choice!\n";
            }
        } while (choice != 2);
    }
};

int main() {
    LoginSystem loginSystem;
    while (true) {
        int choice;
        cout << "\nLogin Page\n";
        cout << "1. User Login\n";
        cout << "2. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            loginSystem.userLogin();
            break;
        case 2:
            cout << "Exiting the system. Goodbye!\n";
            return 0;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    }
    return 0;
}
