#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <ctime>

using namespace std;

// Structure for compost parameters
struct CompostData {
    double temperature;
    double moisture;
    string stage;
    string timeStamp;
};

// Structure for admin credentials
struct Admin {
    string name;
    int password;
};

// Heap class for parameter monitoring
class CompostHeap {
private:
    vector<double> heap;

    void heapifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (heap[parent] < heap[index]) {
                swap(heap[parent], heap[index]);
                index = parent;
            } else
                break;
        }
    }

    void heapifyDown(int index) {
        int size = heap.size();
        while (true) {
            int largest = index;
            int left = 2 * index + 1;
            int right = 2 * index + 2;

            if (left < size && heap[left] > heap[largest])
                largest = left;
            if (right < size && heap[right] > heap[largest])
                largest = right;

            if (largest != index) {
                swap(heap[index], heap[largest]);
                index = largest;
            } else
                break;
        }
    }

public:
    void insert(double value) {
        heap.push_back(value);
        heapifyUp(heap.size() - 1);
    }

    double getMax() {
        if (heap.empty())
            return -1;
        return heap[0];
    }

    void removeMax() {
        if (heap.empty())
            return;
        heap[0] = heap.back();
        heap.pop_back();
        if (!heap.empty())
            heapifyDown(0);
    }
};

class SmartCompostSystem {
private:
    vector<CompostData> compostHistory;
    CompostHeap temperatureHeap;
    CompostHeap moistureHeap;

    void saveCompostDataToFile() {
        ofstream outFile("compost_data.txt");
        if (outFile.is_open()) {
            for (const auto &data : compostHistory) {
                outFile << data.temperature << "," << data.moisture << "," << data.stage << "," << data.timeStamp << "\n";
            }
            outFile.close();
        } else {
            cerr << "Unable to open file for saving compost data.\n";
        }
    }

    void loadCompostDataFromFile() {
        ifstream inFile("compost_data.txt");
        if (inFile.is_open()) {
            string line;
            while (getline(inFile, line)) {
                size_t pos1 = line.find(',');
                size_t pos2 = line.find(',', pos1 + 1);
                size_t pos3 = line.find(',', pos2 + 1);
                double temp = stod(line.substr(0, pos1));
                double moisture = stod(line.substr(pos1 + 1, pos2 - pos1 - 1));
                string stage = line.substr(pos2 + 1, pos3 - pos2 - 1);
                string timeStamp = line.substr(pos3 + 1);

                compostHistory.push_back({temp, moisture, stage, timeStamp});
                temperatureHeap.insert(temp);
                moistureHeap.insert(moisture);
            }
            inFile.close();
        } else {
            cerr << "Unable to open file for loading compost data.\n";
        }
    }

public:
    SmartCompostSystem() {
        loadCompostDataFromFile();
    }

    ~SmartCompostSystem() {
        saveCompostDataToFile();
    }

    // Admin Functions
    void addCompostData(double temp, double moisture, string stage) {
        CompostData data;
        data.temperature = temp;
        data.moisture = moisture;
        data.stage = stage;
        data.timeStamp = getCurrentTime();

        compostHistory.push_back(data);
        temperatureHeap.insert(temp);
        moistureHeap.insert(moisture);

        cout << "Data added successfully!\n";
    }

    void viewAllData() {
        cout << "\nComplete Compost History:\n";
        cout << setw(15) << "Temperature" << setw(15) << "Moisture"
             << setw(20) << "Stage" << setw(25) << "Timestamp\n";
        cout << string(75, '-') << "\n";

        for (const auto &data : compostHistory) {
            cout << setw(15) << data.temperature
                 << setw(15) << data.moisture
                 << setw(20) << data.stage
                 << setw(25) << data.timeStamp << "\n";
        }
    }

    // User Functions
    void checkCurrentStatus() {
        if (compostHistory.empty()) {
            cout << "No compost data available.\n";
            return;
        }

        auto latest = compostHistory.back();
        cout << "\nCurrent Compost Status:\n";
        cout << "Temperature: " << latest.temperature << "°C\n";
        cout << "Moisture: " << latest.moisture << "%\n";
        cout << "Stage: " << latest.stage << "\n";

        // Provide recommendations
        if (latest.temperature > 65)
            cout << "Warning: Temperature too high! Consider adding brown materials.\n";
        else if (latest.temperature < 45)
            cout << "Warning: Temperature too low! Consider adding green materials.\n";

        if (latest.moisture > 60)
            cout << "Warning: Too wet! Add dry brown materials.\n";
        else if (latest.moisture < 40)
            cout << "Warning: Too dry! Add water or green materials.\n";
    }

    void searchStage(string stage) {
        cout << "\nSearching for stage: " << stage << "\n";
        bool found = false;

        for (const auto &data : compostHistory) {
            if (data.stage == stage) {
                cout << "Found entry:\n";
                cout << "Temperature: " << data.temperature << "°C\n";
                cout << "Moisture: " << data.moisture << "%\n";
                cout << "Timestamp: " << data.timeStamp << "\n\n";
                found = true;
            }
        }

        if (!found)
            cout << "No entries found for stage: " << stage << "\n";
    }

private:
    string getCurrentTime() {
        time_t now = time(0);
        string dt = ctime(&now);
        return dt.substr(0, dt.length() - 1);
    }
};

class LoginSystem {
private:
    Admin admins[4];
    int adminCount;
    SmartCompostSystem compostSystem;

public:
    LoginSystem() {
        adminCount = 4;
        admins[0] = {"adarsh", 12345};
        admins[1] = {"bhargavi", 54321};
        admins[2] = {"shreyas", 67890};
        admins[3] = {"shubham", 67890};
    }

    void adminMenu() {
        while (true) {
            cout << "\nAdmin Menu\n";
            cout << "1. Add Compost Data\n";
            cout << "2. View All Data\n";
            cout << "3. Check Current Status\n";
            cout << "4. Search by Stage\n";
            cout << "5. Logout\n";
            cout << "Enter choice: ";

            int choice;
            cin >> choice;

            switch (choice) {
            case 1: {
                double temp, moisture;
                string stage;
                cout << "Enter temperature (°C): ";
                cin >> temp;
                cout << "Enter moisture (%): ";
                cin >> moisture;
                cout << "Enter stage (Initial/Active/Curing/Mature): ";
                cin >> stage;
                compostSystem.addCompostData(temp, moisture, stage);
                break;
            }
            case 2:
                compostSystem.viewAllData();
                break;
            case 3:
                compostSystem.checkCurrentStatus();
                break;
            case 4: {
                string stage;
                cout << "Enter stage to search: ";
                cin >> stage;
                compostSystem.searchStage(stage);
                break;
            }
            case 5:
                return;
            default:
                cout << "Invalid choice!\n";
            }
        }
    }

    void userMenu() {
        while (true) {
            cout << "\nUser Menu\n";
            cout << "1. Check Current Status\n";
            cout << "2. Search by Stage\n";
            cout << "3. Logout\n";
            cout << "Enter choice: ";

            int choice;
            cin >> choice;

            switch (choice) {
            case 1:
                compostSystem.checkCurrentStatus();
                break;
            case 2: {
                string stage;
                cout << "Enter stage to search: ";
                cin >> stage;
                compostSystem.searchStage(stage);
                break;
            }
            case 3:
                return;
            default:
                cout << "Invalid choice!\n";
            }
        }
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

int main() {
    LoginSystem loginSystem;

    while (true) {
        cout << "\nSmart Composting System\n";
        cout << "1. Admin Login\n";
        cout << "2. User Login\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";

        int choice;
        cin >> choice;

        switch (choice) {
        case 1:
            if (loginSystem.adminLogin()) {
                loginSystem.adminMenu();
            }
            break;
        case 2:
            loginSystem.userLogin();
            loginSystem.userMenu();
            break;
        case 3:
            cout << "Thank you for using Smart Composting System!\n";
            return 0;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    }
    return 0;
}
