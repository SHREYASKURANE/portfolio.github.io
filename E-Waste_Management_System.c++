#include <iostream>
#include <vector>
#include <map>
#include <limits>
#include <string>
#include <algorithm>
#include <set>
#include <fstream>

using namespace std;

// Admin Structure
struct Admin {
    string name;
    int password;
};

// E-Waste Collection Center Structure
class CollectionCenter {
public:
    string id;
    string location;
    float eWasteQuantity; // Changed to float

    CollectionCenter(string id, string loc, float qty) // Changed parameter type
        : id(id), location(loc), eWasteQuantity(qty) {}
};

// AVL Node for Collection Center
class AVLNode {
public:
    CollectionCenter center;
    AVLNode *left;
    AVLNode *right;
    int height;

    AVLNode(CollectionCenter c)
        : center(c), left(nullptr), right(nullptr), height(1) {}
};

// AVL Tree for Managing Collection Centers
class AVLTree {
private:
    AVLNode *root;
    set<pair<string, string>> centerKeys; // To track (ID, Location) pairs

    int getHeight(AVLNode *node) {
        return node ? node->height : 0;
    }

    int getBalance(AVLNode *node) {
        return node ? getHeight(node->left) - getHeight(node->right) : 0;
    }

    AVLNode *rotateRight(AVLNode *y) {
        AVLNode *x = y->left;
        AVLNode *T2 = x->right;

        x->right = y;
        y->left = T2;

        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;

        return x;
    }

    AVLNode *rotateLeft(AVLNode *x) {
        AVLNode *y = x->right;
        AVLNode *T2 = y->left;

        y->left = x;
        x->right = T2;

        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;

        return y;
    }

    AVLNode *insert(AVLNode *node, CollectionCenter center) {
        if (!node)
            return new AVLNode(center);

        if (center.id < node->center.id)
            node->left = insert(node->left, center);
        else if (center.id > node->center.id || (center.id == node->center.id && center.location != node->center.location))
            node->right = insert(node->right, center);
        else
            return node;

        node->height = max(getHeight(node->left), getHeight(node->right)) + 1;

        int balance = getBalance(node);

        if (balance > 1 && center.id < node->left->center.id)
            return rotateRight(node);

        if (balance < -1 && center.id > node->right->center.id)
            return rotateLeft(node);

        if (balance > 1 && center.id > node->left->center.id) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }

        if (balance < -1 && center.id < node->right->center.id) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    AVLNode *getMinValueNode(AVLNode *node) {
        AVLNode *current = node;
        while (current && current->left)
            current = current->left;
        return current;
    }

    AVLNode *deleteNode(AVLNode *root, string id) {
        if (!root)
            return root;

        if (id < root->center.id)
            root->left = deleteNode(root->left, id);
        else if (id > root->center.id)
            root->right = deleteNode(root->right, id);
        else {
            centerKeys.erase({root->center.id, root->center.location});

            if (!root->left || !root->right) {
                AVLNode *temp = root->left ? root->left : root->right;
                if (!temp) {
                    temp = root;
                    root = nullptr;
                } else {
                    *root = *temp;
                }
                delete temp;
            } else {
                AVLNode *temp = getMinValueNode(root->right);
                root->center = temp->center;
                root->right = deleteNode(root->right, temp->center.id);
            }
        }

        if (!root)
            return root;

        root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
        int balance = getBalance(root);

        if (balance > 1 && getBalance(root->left) >= 0)
            return rotateRight(root);
        if (balance > 1 && getBalance(root->left) < 0) {
            root->left = rotateLeft(root->left);
            return rotateRight(root);
        }
        if (balance < -1 && getBalance(root->right) <= 0)
            return rotateLeft(root);
        if (balance < -1 && getBalance(root->right) > 0) {
            root->right = rotateRight(root->right);
            return rotateLeft(root);
        }

        return root;
    }

    void inOrderTraversal(AVLNode *node) {
        if (node) {
            inOrderTraversal(node->left);
            cout << "Center ID: " << node->center.id << ", Location: " << node->center.location
                 << ", E-Waste Quantity: " << node->center.eWasteQuantity << " kg\n";
            inOrderTraversal(node->right);
        }
    }

public:
    AVLTree() : root(nullptr) {
        loadCentersFromFile();
    }

    ~AVLTree() {
        saveCentersToFile();
    }

    bool addCenter(CollectionCenter center) {
        auto key = make_pair(center.id, center.location);
        if (centerKeys.find(key) != centerKeys.end()) {
            AVLNode *current = root;
            while (current) {
                if (current->center.id == center.id && current->center.location == center.location) {
                    current->center.eWasteQuantity += center.eWasteQuantity;
                    cout << "Center ID and Location already exist. Adding E-Waste Quantity.\n";
                    return true;
                }
                current = (center.id < current->center.id) ? current->left : current->right;
            }
            return false;
        }
        centerKeys.insert(key);
        root = insert(root, center);
        cout << "Collection Center added successfully.\n";
        return true;
    }

    void removeCenter(string id) {
        if (centerKeys.find({id, ""}) == centerKeys.end()) {
            cout << "Error: Center ID does not exist.\n";
            return;
        }
        root = deleteNode(root, id);
    }

    void displayCenters() {
        if (!root) {
            cout << "No collection centers available.\n";
            return;
        }
        inOrderTraversal(root);
    }

    bool modifyCenter(string id, string newLocation, float newQuantity) {
        AVLNode *current = root;
        while (current) {
            if (current->center.id == id) {
                current->center.location = newLocation;
                current->center.eWasteQuantity = newQuantity;
                cout << "Collection center details updated successfully.\n";
                return true;
            }
            current = (id < current->center.id) ? current->left : current->right;
        }
        cout << "Error: Collection center not found.\n";
        return false;
    }

    void saveCentersToFile() {
        ofstream outFile("centers.txt");
        if (outFile.is_open()) {
            saveCenters(outFile, root);
            outFile.close();
        } else {
            cerr << "Unable to open file for saving centers.\n";
        }
    }

    void loadCentersFromFile() {
        ifstream inFile("centers.txt");
        if (inFile.is_open()) {
            string id, location;
            float quantity;
            while (inFile >> id >> location >> quantity) {
                CollectionCenter center(id, location, quantity);
                addCenter(center);
            }
            inFile.close();
        } else {
            cerr << "Unable to open file for loading centers.\n";
        }
    }

private:
    void saveCenters(ofstream &outFile, AVLNode *node) {
        if (node) {
            saveCenters(outFile, node->left);
            outFile << node->center.id << " " << node->center.location << " " << node->center.eWasteQuantity << "\n";
            saveCenters(outFile, node->right);
        }
    }
};

// Graph for Routes between Centers
class Graph {
private:
    map<string, vector<pair<string, double>>> adjList;
    map<string, string> locationMap; // Maps ID to Location

public:
    void addEdge(string u, string v, double weight) {
        if (u == v) {
            return; // Ignore routes where the two centers are the same
        }
        adjList[u].push_back({v, weight});
        adjList[v].push_back({u, weight});
        saveRoutesToFile();
    }

    void displayGraph() {
        for (const auto &node : adjList) {
            cout << "Location " << locationMap[node.first] << " is connected to:\n";
            for (const auto &neighbor : node.second) {
                cout << "  -> " << locationMap[neighbor.first] << " (Distance: " << neighbor.second << " km)\n";
            }
        }
    }

    void shortestPath(string start, string end) {
        map<string, double> dist;
        map<string, string> parent;
        map<string, bool> visited;

        for (auto &node : adjList) {
            dist[node.first] = numeric_limits<double>::max();
            visited[node.first] = false;
        }
        dist[start] = 0;

        for (size_t i = 0; i < adjList.size(); ++i) {
            string minNode;
            double minDist = numeric_limits<double>::max();

            for (auto &node : dist) {
                if (!visited[node.first] && node.second < minDist) {
                    minDist = node.second;
                    minNode = node.first;
                }
            }

            if (minNode.empty())
                break;
            visited[minNode] = true;

            for (auto &neighbor : adjList[minNode]) {
                if (dist[minNode] + neighbor.second < dist[neighbor.first]) {
                    dist[neighbor.first] = dist[minNode] + neighbor.second;
                    parent[neighbor.first] = minNode;
                }
            }
        }

        if (dist[end] == numeric_limits<double>::max()) {
            cout << "No path exists from " << locationMap[start] << " to " << locationMap[end] << ".\n";
            return;
        }

        cout << "Shortest path from " << locationMap[start] << " to " << locationMap[end] << ": ";
        string current = end;
        vector<string> path;
        while (current != start) {
            path.push_back(current);
            current = parent[current];
        }
        path.push_back(start);
        reverse(path.begin(), path.end());
        for (const auto &node : path) {
            cout << locationMap[node] << " ";
        }
        cout << "\nTotal Distance: " << dist[end] << " km\n";
    }

    void modifyRoute(string u, string v, double newWeight) {
        bool routeFound = false;
        for (auto &neighbor : adjList[u]) {
            if (neighbor.first == v) {
                neighbor.second = newWeight;
                routeFound = true;
                break;
            }
        }
        for (auto &neighbor : adjList[v]) {
            if (neighbor.first == u) {
                neighbor.second = newWeight;
                break;
            }
        }
        if (routeFound) {
            cout << "Route between " << locationMap[u] << " and " << locationMap[v] << " updated successfully.\n";
            saveRoutesToFile();
        } else {
            cout << "Error: Route not found.\n";
        }
    }

    void addLocation(string id, string location) {
        locationMap[id] = location;
    }

    void saveRoutesToFile() {
        ofstream outFile("routes.txt");
        if (outFile.is_open()) {
            for (const auto &node : adjList) {
                for (const auto &neighbor : node.second) {
                    outFile << node.first << " " << neighbor.first << " " << neighbor.second << "\n";
                }
            }
            outFile.close();
        } else {
            cerr << "Unable to open file for saving routes.\n";
        }
    }

    void loadRoutesFromFile() {
        ifstream inFile("routes.txt");
        if (inFile.is_open()) {
            string u, v;
            double weight;
            while (inFile >> u >> v >> weight) {
                addEdge(u, v, weight);
            }
            inFile.close();
        } else {
            cerr << "Unable to open file for loading routes.\n";
        }
    }
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

    bool authenticate(string name, int password) {
        for (int i = 0; i < adminCount; i++) {
            if (admins[i].name == name && admins[i].password == password) {
                loggedInAdmin = name;
                return true;
            }
        }
        return false;
    }

    string getLoggedInAdmin() const {
        return loggedInAdmin;
    }
};

// Function to display the admin menu and handle admin operations
void adminMenu(AVLTree &centerManager, Graph &routeManager, const string &loggedInAdmin) {
    int choice;
    do {
        cout << "\nAdmin Menu:\n";
        cout << "1. Add Collection Center\n";
        cout << "2. Remove Collection Center\n";
        cout << "3. Display Collection Centers\n";
        cout << "4. Modify Collection Center\n";
        cout << "5. Add Route between Centers\n";
        cout << "6. Display Routes\n";
        cout << "7. Find Shortest Path between Centers\n";
        cout << "8. Modify Route\n";
        cout << "9. Logout\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1: {
            string id, location;
            float quantity;
            cout << "Enter Center ID: ";
            cin >> id;
            cout << "Enter Location: ";
            cin >> location;
            cout << "Enter E-Waste Quantity (kg): ";
            cin >> quantity;

            CollectionCenter center(id, location, quantity);
            if (centerManager.addCenter(center)) {
                routeManager.addLocation(id, location);
            }
            break;
        }
        case 2: {
            string id;
            cout << "Enter Center ID to remove: ";
            cin >> id;
            centerManager.removeCenter(id);
            break;
        }
        case 3:
            centerManager.displayCenters();
            break;
        case 4: {
            string id, newLocation;
            float newQuantity;
            cout << "Enter Center ID to modify: ";
            cin >> id;
            cout << "Enter New Location: ";
            cin >> newLocation;
            cout << "Enter New E-Waste Quantity (kg): ";
            cin >> newQuantity;
            centerManager.modifyCenter(id, newLocation, newQuantity);
            break;
        }
        case 5: {
            string u, v;
            double weight;
            cout << "Enter Center ID 1: ";
            cin >> u;
            cout << "Enter Center ID 2: ";
            cin >> v;
            cout << "Enter Distance between them (in km): ";
            cin >> weight;

            routeManager.addEdge(u, v, weight);
            break;
        }
        case 6:
            routeManager.displayGraph();
            break;
        case 7: {
            string start, end;
            cout << "Enter Start Center ID: ";
            cin >> start;
            cout << "Enter End Center ID: ";
            cin >> end;
            routeManager.shortestPath(start, end);
            break;
        }
        case 8: {
            string u, v;
            double newWeight;
            cout << "Enter Center ID 1: ";
            cin >> u;
            cout << "Enter Center ID 2: ";
            cin >> v;
            cout << "Enter New Distance between them (in km): ";
            cin >> newWeight;
            routeManager.modifyRoute(u, v, newWeight);
            break;
        }
        case 9:
            cout << "Logging out...\n";
            return;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 9);
}

// Function to display the user menu and handle user operations
void userMenu(AVLTree &centerManager, Graph &routeManager) {
    int choice;
    do {
        cout << "\nUser Menu:\n";
        cout << "1. Display Collection Centers\n";
        cout << "2. Display Routes\n";
        cout << "3. Find Shortest Path between Centers\n";
        cout << "4. Logout\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            centerManager.displayCenters();
            break;
        case 2:
            routeManager.displayGraph();
            break;
        case 3: {
            string start, end;
            cout << "Enter Start Center ID: ";
            cin >> start;
            cout << "Enter End Center ID: ";
            cin >> end;
            routeManager.shortestPath(start, end);
            break;
        }
        case 4:
            cout << "Logging out...\n";
            return;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 4);
}

// Main Function
int main() {
    AVLTree centerManager;
    Graph routeManager;
    LoginSystem loginSystem;

    cout << "Welcome to E-Waste Management System\n";

    int loginChoice;
    do {
        cout << "\nLogin Menu:\n";
        cout << "1. Admin Login\n";
        cout << "2. User Login\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> loginChoice;

        switch (loginChoice) {
        case 1: {
            string username;
            int password;
            cout << "Enter Admin Username: ";
            cin >> username;
            cout << "Enter Admin Password: ";
            cin >> password;

            if (loginSystem.authenticate(username, password)) {
                cout << "Login Successful!\n";
                adminMenu(centerManager, routeManager, loginSystem.getLoggedInAdmin());
            } else {
                cout << "Authentication Failed. Please try again.\n";
            }
            break;
        }
        case 2:
            cout << "User Login Successful!\n";
            userMenu(centerManager, routeManager);
            break;
        case 3:
            cout << "Exiting the system. Goodbye!\n";
            break;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    } while (loginChoice != 3);

    return 0;
}
