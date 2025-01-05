#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iomanip>

using namespace std;

// Structure to store admin credentials
struct Admin {
    string name;
    int password; // 5-digit positive integer password
};

class LoginSystem {
private:
    Admin admins[4]; // Array of admin credentials
    int adminCount;

public:
    LoginSystem() {
        // Prepopulate admin credentials
        adminCount = 4;
        admins[0] = {"adarsh", 12345};
        admins[1] = {"bhargavi", 54321};
        admins[2] = {"shreyas", 67890};
        admins[3] = {"shubham", 67890};
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

    // Function for user login
    void userLogin() {
        cout << "Welcome, User! You are logged in.\n";
    }
};

// Smart Waste Segregation System class
class SmartWasteSystem {
private:
    struct WasteMap {
        string category;
        string items[10];
        int itemCount;
    };

    WasteMap wasteMap[10];
    int categoryCount;

    string recyclableItems[5] = {"Paper", "Glass", "Metal", "Plastic", "Cardboard"};
    string nonRecyclableItems[5] = {"Food Waste", "Styrofoam", "Ceramics", "Diapers", "Hazardous Waste"};

    void quickSort(string arr[], int low, int high) {
        if (low < high) {
            int pi = partition(arr, low, high);
            quickSort(arr, low, pi - 1);
            quickSort(arr, pi + 1, high);
        }
    }

    int partition(string arr[], int low, int high) {
        string pivot = arr[high];
        int i = low - 1;

        for (int j = low; j < high; ++j) {
            if (arr[j] < pivot) {
                ++i;
                swap(arr[i], arr[j]);
            }
        }
        swap(arr[i + 1], arr[high]);
        return i + 1;
    }

    void saveWasteDataToFile() {
        ofstream outFile("waste_data.txt");
        if (outFile.is_open()) {
            outFile << categoryCount << "\n";
            for (int i = 0; i < categoryCount; ++i) {
                outFile << wasteMap[i].category << "," << wasteMap[i].itemCount << "\n";
                for (int j = 0; j < wasteMap[i].itemCount; ++j) {
                    outFile << wasteMap[i].items[j] << "\n";
                }
            }
            outFile.close();
        } else {
            cerr << "Unable to open file for saving waste data.\n";
        }
    }

    void loadWasteDataFromFile() {
        ifstream inFile("waste_data.txt");
        if (inFile.is_open()) {
            inFile >> categoryCount;
            inFile.ignore();
            for (int i = 0; i < categoryCount; ++i) {
                getline(inFile, wasteMap[i].category, ',');
                inFile >> wasteMap[i].itemCount;
                inFile.ignore();
                for (int j = 0; j < wasteMap[i].itemCount; ++j) {
                    getline(inFile, wasteMap[i].items[j]);
                }
            }
            inFile.close();
        } else {
            cerr << "Unable to open file for loading waste data.\n";
        }
    }

public:
    SmartWasteSystem() : categoryCount(0) {
        loadWasteDataFromFile();
    }

    ~SmartWasteSystem() {
        saveWasteDataToFile();
    }

    void addWaste(string category, string item) {
        int index = findCategoryIndex(category);
        if (index == -1) {
            wasteMap[categoryCount].category = category;
            wasteMap[categoryCount].items[0] = item;
            wasteMap[categoryCount].itemCount = 1;
            categoryCount++;
        } else {
            wasteMap[index].items[wasteMap[index].itemCount++] = item;
        }
        saveWasteDataToFile();
    }

    void deleteWasteCategory(string category) {
        int index = findCategoryIndex(category);
        if (index == -1) {
            cout << "Category not found.\n";
            return;
        }

        for (int i = index; i < categoryCount - 1; ++i) {
            wasteMap[i] = wasteMap[i + 1];
        }
        categoryCount--;
        saveWasteDataToFile();

        cout << "Category \"" << category << "\" deleted successfully.\n";
    }

    void modifyWasteCategory(string oldCategory, string newCategory) {
        int index = findCategoryIndex(oldCategory);
        if (index == -1) {
            cout << "Category not found.\n";
            return;
        }

        wasteMap[index].category = newCategory;
        saveWasteDataToFile();
        cout << "Category \"" << oldCategory << "\" modified to \"" << newCategory << "\" successfully.\n";
    }

    void displayWasteCategories() {
        cout << "\nWaste Categories and Items:\n";
        for (int i = 0; i < categoryCount; ++i) {
            cout << wasteMap[i].category << ": ";
            quickSort(wasteMap[i].items, 0, wasteMap[i].itemCount - 1);
            for (int j = 0; j < wasteMap[i].itemCount; ++j) {
                cout << wasteMap[i].items[j] << " ";
            }
            cout << endl;
        }
    }

    void displayPredefinedCategories() {
        cout << "\nPredefined Waste Categories:\n";
        cout << left << setw(20) << "Recyclable" << "Non-Recyclable" << endl;
        cout << string(40, '=') << endl;
        for (int i = 0; i < 5; ++i) {
            cout << left << setw(20) << recyclableItems[i] << nonRecyclableItems[i] << endl;
        }
    }

private:
    int findCategoryIndex(string category) {
        for (int i = 0; i < categoryCount; ++i) {
            if (wasteMap[i].category == category) {
                return i;
            }
        }
        return -1;
    }
};

int main() {
    LoginSystem loginSystem;
    SmartWasteSystem wasteSystem;
    int choice;

    while (true) {
        cout << "\nLogin Page\n";
        cout << "1. Admin Login\n";
        cout << "2. User Login\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            if (loginSystem.adminLogin()) {
                cout << "Accessing Admin Dashboard...\n";
                int adminChoice;
                do {
                    cout << "\nAdmin Menu:\n";
                    cout << "1. Add Waste\n";
                    cout << "2. Display Waste Categories\n";
                    cout << "3. Display Predefined Categories\n";
                    cout << "4. Delete Waste Category\n";
                    cout << "5. Modify Waste Category\n";
                    cout << "6. Logout\n";
                    cout << "Enter your choice: ";
                    cin >> adminChoice;

                    switch (adminChoice) {
                    case 1: {
                        string category, item;
                        cout << "Enter waste category: ";
                        cin >> category;
                        cout << "Enter waste item: ";
                        cin >> item;
                        wasteSystem.addWaste(category, item);
                        break;
                    }
                    case 2:
                        wasteSystem.displayWasteCategories();
                        break;
                    case 3:
                        wasteSystem.displayPredefinedCategories();
                        break;
                    case 4: {
                        string category;
                        cout << "Enter the category to delete: ";
                        cin >> category;
                        wasteSystem.deleteWasteCategory(category);
                        break;
                    }
                    case 5: {
                        string oldCategory, newCategory;
                        cout << "Enter the category to modify: ";
                        cin >> oldCategory;
                        cout << "Enter the new category name: ";
                        cin >> newCategory;
                        wasteSystem.modifyWasteCategory(oldCategory, newCategory);
                        break;
                    }
                    case 6:
                        cout << "Logging out...\n";
                        break;
                    default:
                        cout << "Invalid choice. Please try again.\n";
                    }
                } while (adminChoice != 6);
            }
            break;
        case 2:
            loginSystem.userLogin();
            int userChoice;
            do {
                cout << "\nUser Menu:\n";
                cout << "1. Display Waste Categories\n";
                cout << "2. Display Predefined Categories\n";
                cout << "3. Logout\n";
                cout << "Enter your choice: ";
                cin >> userChoice;

                switch (userChoice) {
                case 1:
                    wasteSystem.displayWasteCategories();
                    break;
                case 2:
                    wasteSystem.displayPredefinedCategories();
                    break;
                case 3:
                    cout << "Logging out...\n";
                    break;
                default:
                    cout << "Invalid choice. Please try again.\n";
                }
            } while (userChoice != 3);
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
