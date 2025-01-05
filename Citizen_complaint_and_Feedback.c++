#include <iostream>
#include <queue>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <iomanip>
#include <fstream>

using namespace std;

// Struct for Complaint Details
struct Complaint {
    int priority;
    string citizenName;
    string area;
    string description;
    string status; // Pending, In Progress, or Resolved
    string resolvedByAdmin; // Admin who resolved the complaint

    bool operator<(const Complaint &other) const {
        return priority < other.priority; // Higher priority complaints come first
    }
};

// Complaint System Class
class ComplaintSystem {
private:
    map<string, priority_queue<Complaint>> areaComplaints; // Categorized by area
    map<string, string> userComplaints;                    // Track user complaints
    vector<Complaint> resolvedComplaints;                  // Resolved complaints for stats
    map<string, int> adminResolvedCount;                   // Track number of complaints resolved by each admin
    int totalComplaintsSubmitted;
    int totalComplaintsResolved;

    // Helper: Validate Area
    bool validateArea(const string &area) {
        if (areaComplaints.find(area) == areaComplaints.end() || areaComplaints[area].empty()) {
            cout << "\nInvalid area or no complaints in the area.\n";
            return false;
        }
        return true;
    }

    // Helper: Display Complaint
    void displayComplaint(const Complaint &complaint) {
        cout << "Priority: " << complaint.priority
             << ", Citizen: " << complaint.citizenName
             << ", Area: " << complaint.area
             << ", Status: " << complaint.status
             << "\nDescription: " << complaint.description << "\n";
        if (complaint.status == "Resolved") {
            cout << "Resolved by: " << complaint.resolvedByAdmin << "\n";
        }
    }

    // Helper: Save complaints to a file
    void saveComplaintsToFile() {
        ofstream outFile("complaints.txt");
        if (outFile.is_open()) {
            outFile << totalComplaintsSubmitted << " " << totalComplaintsResolved << "\n";
            for (const auto &entry : areaComplaints) {
                const auto &area = entry.first;
                const auto &complaints = entry.second;
                priority_queue<Complaint> tempQueue = complaints;

                while (!tempQueue.empty()) {
                    const Complaint &complaint = tempQueue.top();
                    outFile << complaint.priority << " " << complaint.citizenName << " "
                            << complaint.area << " " << complaint.description << " "
                            << complaint.status << " " << complaint.resolvedByAdmin << "\n";
                    tempQueue.pop();
                }
            }
            outFile.close();
        } else {
            cerr << "Unable to open file for saving complaints.\n";
        }

        ofstream adminOutFile("admin_resolved.txt");
        if (adminOutFile.is_open()) {
            for (const auto &entry : adminResolvedCount) {
                adminOutFile << entry.first << " " << entry.second << "\n";
            }
            adminOutFile.close();
        } else {
            cerr << "Unable to open file for saving admin resolved counts.\n";
        }
    }

    // Helper: Load complaints from a file
    void loadComplaintsFromFile() {
        ifstream inFile("complaints.txt");
        if (inFile.is_open()) {
            inFile >> totalComplaintsSubmitted >> totalComplaintsResolved;
            inFile.ignore();
            while (!inFile.eof()) {
                Complaint complaint;
                inFile >> complaint.priority;
                inFile.ignore();
                getline(inFile, complaint.citizenName, ' ');
                getline(inFile, complaint.area, ' ');
                getline(inFile, complaint.description, ' ');
                getline(inFile, complaint.status, ' ');
                getline(inFile, complaint.resolvedByAdmin);
                if (!inFile.fail()) {
                    areaComplaints[complaint.area].push(complaint);
                    userComplaints[complaint.citizenName] = complaint.area;
                }
            }
            inFile.close();
        } else {
            cerr << "Unable to open file for loading complaints.\n";
        }

        ifstream adminInFile("admin_resolved.txt");
        if (adminInFile.is_open()) {
            string adminName;
            int resolvedCount;
            while (adminInFile >> adminName >> resolvedCount) {
                adminResolvedCount[adminName] = resolvedCount;
            }
            adminInFile.close();
        } else {
            cerr << "Unable to open file for loading admin resolved counts.\n";
        }
    }

public:
    ComplaintSystem() : totalComplaintsSubmitted(0), totalComplaintsResolved(0) {
        loadComplaintsFromFile();
    }

    ~ComplaintSystem() {
        saveComplaintsToFile();
    }

    // Submit a new complaint
    void submitComplaint(const string &citizenName, const string &area, const string &description) {
        if (userComplaints.find(citizenName) != userComplaints.end()) {
            cout << "\nYou have already registered a complaint. Only one complaint per user is allowed.\n";
            return;
        }

        Complaint newComplaint = {0, citizenName, area, description, "Pending", ""};
        areaComplaints[area].push(newComplaint);
        userComplaints[citizenName] = area;
        totalComplaintsSubmitted++;

        cout << "\nComplaint submitted successfully in area: " << area << "!\n";
    }

    // Resolve the highest-priority complaint
    void resolveComplaint(const string &area, const string &adminName) {
        if (!validateArea(area))
            return;

        Complaint topComplaint = areaComplaints[area].top();
        areaComplaints[area].pop();
        topComplaint.status = "Resolved";
        topComplaint.resolvedByAdmin = adminName;
        resolvedComplaints.push_back(topComplaint);
        userComplaints.erase(topComplaint.citizenName);
        totalComplaintsResolved++;
        adminResolvedCount[adminName]++;

        cout << "\nResolving complaint in area: " << area << ":\n";
        displayComplaint(topComplaint);
    }

    // Update complaint status
    void updateComplaintStatus(const string &citizenName, const string &newStatus) {
        auto it = userComplaints.find(citizenName);
        if (it == userComplaints.end()) {
            cout << "\nNo complaint found for citizen: " << citizenName << "\n";
            return;
        }

        string area = it->second;
        priority_queue<Complaint> tempQueue;
        bool updated = false;

        while (!areaComplaints[area].empty()) {
            Complaint complaint = areaComplaints[area].top();
            areaComplaints[area].pop();

            if (complaint.citizenName == citizenName) {
                complaint.status = newStatus;
                updated = true;
            }

            tempQueue.push(complaint);
        }

        areaComplaints[area] = tempQueue;

        if (updated) {
            cout << "\nStatus updated successfully for citizen: " << citizenName << "\n";
        } else {
            cout << "\nFailed to update status.\n";
        }
    }

    // View all complaints by area
    void viewComplaints(const string &area) {
        if (!validateArea(area))
            return;

        priority_queue<Complaint> tempQueue = areaComplaints[area];
        cout << "\nComplaints in area: " << area << ":\n";

        while (!tempQueue.empty()) {
            Complaint complaint = tempQueue.top();
            tempQueue.pop();
            displayComplaint(complaint);
        }
    }

    // View complaints summary
    void viewSummary() {
        cout << "\nSystem Summary:\n";
        cout << "Total Complaints Submitted: " << totalComplaintsSubmitted << "\n";
        cout << "Total Complaints Resolved: " << totalComplaintsResolved << "\n";
        cout << "Pending Complaints: " << totalComplaintsSubmitted - totalComplaintsResolved << "\n\n";
    }

    // View end-of-month statistics
    void viewMonthlyStats() {
        cout << "\nEnd-of-Month Statistics:\n";
        cout << "Total Complaints Resolved: " << resolvedComplaints.size() << "\n";

        map<string, int> areaStats;
        for (const auto &complaint : resolvedComplaints) {
            areaStats[complaint.area]++;
        }

        cout << "Complaints Resolved by Area:\n";
        for (const auto &entry : areaStats) {
            cout << "Area: " << entry.first << ", Resolved: " << entry.second << "\n";
        }
    }

    // Display all active complaints in the system
    void displayAllActiveComplaints() {
        cout << "\nActive Complaints:\n";
        bool hasActiveComplaints = false;

        for (const auto &entry : areaComplaints) {
            const string &area = entry.first;
            const auto &complaints = entry.second;

            if (!complaints.empty()) {
                hasActiveComplaints = true;
                priority_queue<Complaint> tempQueue = complaints;
                while (!tempQueue.empty()) {
                    const Complaint &complaint = tempQueue.top();
                    cout << "Area: " << area << ", Citizen: " << complaint.citizenName << ", Priority: " << complaint.priority << ", Description: " << complaint.description << "\n";
                    tempQueue.pop();
                }
            }
        }

        if (!hasActiveComplaints) {
            cout << "0\n";
        }
    }

    // Display number of complaints resolved by each admin
    void displayAdminResolutionCounts() {
        cout << "\nComplaints Resolved by Each Admin:\n";
        for (const auto &entry : adminResolvedCount) {
            cout << "Admin: " << entry.first << ", Resolved: " << entry.second << "\n";
        }
    }
};

// Structure to store admin credentials
struct Admin {
    string name;
    int password; // 5-digit positive integer password
};

class LoginSystem {
private:
    Admin admins[4];
    int adminCount;
    string loggedInAdmin;

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
                loggedInAdmin = adminName;
                return true;
            }
        }

        cout << "Invalid Admin ID or Password.\n";
        return false;
    }

    void userLogin() {
        cout << "Welcome, User! You are logged in.\n";
    }

    string getLoggedInAdmin() const {
        return loggedInAdmin;
    }
};

void displayAdminMenu() {
    cout << "\nAdmin Menu\n";
    cout << "1. Resolve Complaint\n";
    cout << "2. Update Complaint Status\n";
    cout << "3. View Complaints by Area\n";
    cout << "4. View Monthly Statistics\n";
    cout << "5. View System Summary\n";
    cout << "6. Display All Active Complaints\n";
    cout << "7. Display Admin Resolution Counts\n";
    cout << "8. Exit\n";
    cout << "Enter your choice: ";
}

void displayUserMenu() {
    cout << "\nUser Menu\n";
    cout << "1. Submit Complaint\n";
    cout << "2. View Monthly Statistics\n";
    cout << "3. View System Summary\n";
    cout << "4. Exit\n";
    cout << "Enter your choice: ";
}

int main() {
    LoginSystem loginSystem;
    ComplaintSystem complaintSystem;
    int loginChoice;

    while (true) {
        cout << "\nLogin Page\n";
        cout << "1. Admin Login\n";
        cout << "2. User Login\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> loginChoice;

        if (loginChoice == 3) {
            cout << "Exiting the system. Goodbye!\n";
            break;
        }

        if (loginChoice == 1) {
            if (loginSystem.adminLogin()) {
                int choice;
                do {
                    displayAdminMenu();
                    cin >> choice;

                    switch (choice) {
                    case 1: {
                        string area;
                        cout << "Enter the area to resolve a complaint: ";
                        cin.ignore();
                        getline(cin, area);
                        complaintSystem.resolveComplaint(area, loginSystem.getLoggedInAdmin());
                        break;
                    }
                    case 2: {
                        string name, status;
                        cout << "Enter the citizen's name: ";
                        cin.ignore();
                        getline(cin, name);
                        cout << "Enter new status (Pending/In Progress/Resolved): ";
                        getline(cin, status);
                        complaintSystem.updateComplaintStatus(name, status);
                        break;
                    }
                    case 3: {
                        string area;
                        cout << "Enter the area to view complaints: ";
                        cin.ignore();
                        getline(cin, area);
                        complaintSystem.viewComplaints(area);
                        break;
                    }
                    case 4:
                        complaintSystem.viewMonthlyStats();
                        break;
                    case 5:
                        complaintSystem.viewSummary();
                        break;
                    case 6:
                        complaintSystem.displayAllActiveComplaints();
                        break;
                    case 7:
                        complaintSystem.displayAdminResolutionCounts();
                        break;
                    case 8:
                        cout << "Exiting Admin menu.\n";
                        break;
                    default:
                        cout << "Invalid choice. Please try again.\n";
                    }
                } while (choice != 8);
            }
        } else if (loginChoice == 2) {
            loginSystem.userLogin();
            int choice;
            do {
                displayUserMenu();
                cin >> choice;

                switch (choice) {
                case 1: {
                    string name, area, description;
                    cout << "Enter your name: ";
                    cin.ignore();
                    getline(cin, name);
                    cout << "Enter your area: ";
                    getline(cin, area);
                    cout << "Enter complaint description: ";
                    getline(cin, description);
                    complaintSystem.submitComplaint(name, area, description);
                    break;
                }
                case 2:
                    complaintSystem.viewMonthlyStats();
                    break;
                case 3:
                    complaintSystem.viewSummary();
                    break;
                case 4:
                    cout << "Exiting User menu.\n";
                    break;
                default:
                    cout << "Invalid choice. Please try again.\n";
                }
            } while (choice != 4);
        } else {
            cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}
