#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <set>
#include <fstream>
#include <sstream>
using namespace std;

// Utility Function for Printing Separator Lines
void printLine() {
    cout << "=====================================" << endl;
}

// Class Definitions

// Donation Class to Store Donation Details
class Donation {
public:
    int id;
    string foodType;
    int quantity;
    string location;
    string expirationDate;
    string notes;
    bool isRequested;

    Donation(int id, string foodType, int quantity, string location, string expirationDate, string notes)
        : id(id), foodType(foodType), quantity(quantity), location(location), expirationDate(expirationDate), notes(notes), isRequested(false) {}

    void display() const {
        cout << "ID: " << id << ", Food Type: " << foodType
             << ", Quantity: " << quantity << ", Location: " << location
             << ", Expiration Date: " << expirationDate
             << ", Notes: " << notes
             << (isRequested ? " (Requested)" : "") << endl;
    }
};

// Recipient Class to Store Recipient Details
class Recipient {
public:
    int id;
    string name;
    string location;
    string preferences;

    Recipient(int id, string name, string location, string preferences)
        : id(id), name(name), location(location), preferences(preferences) {}

    void display() {
        cout << "ID: " << id << ", Name: " << name << ", Location: " << location
             << ", Preferences: " << preferences << endl;
    }
};

// Data Storage
vector<Donation> donations;
vector<Recipient> recipients;
unordered_map<int, vector<int>> matches; // Maps donation IDs to recipient IDs
set<int> donationIds;  // To ensure donation IDs are unique
set<int> recipientIds; // To ensure recipient IDs are unique

// Function Declarations
void saveDonationsToFile(); // Declare saveDonationsToFile function

// Add Donation Function
void addDonation() {
    int id, quantity;
    string foodType, location, expirationDate, notes;

    cout << "Enter Donation ID: ";
    cin >> id;
    if (donationIds.count(id)) {
        cout << "Error: Donation ID already exists!" << endl;
        return;
    }

    cout << "Enter Food Type: ";
    cin.ignore();
    getline(cin, foodType);
    cout << "Enter Quantity: ";
    cin >> quantity;
    cout << "Enter Location: ";
    cin.ignore();
    getline(cin, location);
    cout << "Enter Expiration Date: ";
    getline(cin, expirationDate);
    cout << "Enter Additional Notes: ";
    getline(cin, notes);

    donations.push_back(Donation(id, foodType, quantity, location, expirationDate, notes));
    donationIds.insert(id);
    cout << "Donation added successfully!" << endl;
}

// Add Recipient Function
void addRecipient() {
    int id;
    string name, location, preferences;

    cout << "Enter Recipient ID: ";
    cin >> id;
    if (recipientIds.count(id)) {
        cout << "Error: Recipient ID already exists!" << endl;
        return;
    }

    cout << "Enter Name: ";
    cin.ignore();
    getline(cin, name);
    cout << "Enter Location: ";
    getline(cin, location);
    cout << "Enter Preferences: ";
    getline(cin, preferences);

    recipients.push_back(Recipient(id, name, location, preferences));
    recipientIds.insert(id);
    cout << "Recipient added successfully!" << endl;
}

// Generate Reports
void generateReports() {
    cout << "Donation Statistics:" << endl;
    cout << "Total Donations: " << donations.size() << endl;
    cout << "Total Recipients: " << recipients.size() << endl;

    for (const auto &donation : donations) {
        cout << "Donation ID: " << donation.id << " -> Recipients: ";
        if (matches.count(donation.id)) {
            for (const auto &recipientId : matches[donation.id]) {
                cout << recipientId << " ";
            }
        }
        cout << endl;
    }
}

// Search Donations by Location
void searchDonationsByLocation() {
    string location;
    cout << "Enter location to search donations: ";
    cin.ignore();
    getline(cin, location);

    bool found = false;
    for (const auto &donation : donations) {
        if (donation.location.find(location) != string::npos) {
            donation.display();
            found = true;
        }
    }

    if (!found) {
        cout << "No donations found for the specified location!" << endl;
    }
}

// Search Donations by Keyword
void searchDonationsByKeyword() {
    string keyword;
    cout << "Enter keyword to search donations: ";
    cin.ignore();
    getline(cin, keyword);

    bool found = false;
    for (const auto &donation : donations) {
        if (donation.foodType.find(keyword) != string::npos || donation.location.find(keyword) != string::npos) {
            donation.display();
            found = true;
        }
    }

    if (!found) {
        cout << "No donations found for the specified keyword!" << endl;
    }
}

// Logout Function
void logout() {
    cout << "Saving resources before exiting..." << endl;
    saveDonationsToFile();
    cout << "Thank you for using the platform! Goodbye!" << endl;
}

// Save Donations to File
void saveDonationsToFile() {
    ofstream outFile("donations.txt");
    if (!outFile) {
        cout << "Error opening file!" << endl;
        return;
    }

    for (const auto &donation : donations) {
        outFile << donation.id << "," << donation.foodType << "," << donation.quantity << ","
                << donation.location << "," << donation.expirationDate << "," << donation.notes << endl;
    }

    cout << "Donations saved to file!" << endl;
    outFile.close();
}

// Load Donations from File
void loadDonationsFromFile() {
    ifstream inFile("donations.txt");
    if (!inFile) {
        cout << "Error opening file!" << endl;
        return;
    }

    donations.clear();
    donationIds.clear();
    string line;
    while (getline(inFile, line)) {
        stringstream ss(line);
        string id, foodType, quantity, location, expirationDate, notes;
        getline(ss, id, ',');
        getline(ss, foodType, ',');
        getline(ss, quantity, ',');
        getline(ss, location, ',');
        getline(ss, expirationDate, ',');
        getline(ss, notes, ',');

        int donationId = stoi(id);
        donations.push_back(Donation(donationId, foodType, stoi(quantity), location, expirationDate, notes));
        donationIds.insert(donationId);
    }

    cout << "Donations loaded from file!" << endl;
    inFile.close();
}

// Main Menu
void menu() {
    int choice;
    do {
        printLine();
        cout << "Food Waste Redistribution Platform" << endl;
        printLine();
        cout << "1. Add Donation\n2. Add Recipient\n3. Generate Reports\n4. Search Donations by Keyword\n5. Search Donations by Location\n6. Logout" << endl;
        printLine();
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                addDonation();
                break;
            case 2:
                addRecipient();
                break;
            case 3:
                generateReports();
                break;
            case 4:
                searchDonationsByKeyword();
                break;
            case 5:
                searchDonationsByLocation();
                break;
            case 6:
                logout();
                break;
            default:
                cout << "Invalid choice!" << endl;
        }
    } while (choice != 6);
}

// Main Function
int main() {
    cout << "Welcome to the Food Waste Redistribution Platform!\n";
    cout << "Initializing system resources...\n";

    // Load existing data
    loadDonationsFromFile();

    // Start menu-driven interaction
    menu();

    return 0;
}
