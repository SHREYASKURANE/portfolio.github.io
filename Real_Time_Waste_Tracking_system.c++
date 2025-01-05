#include <iostream>
#include <string>
#include <limits.h>
#include <vector>
#include <algorithm>
#include <stack>
#include <fstream>

using namespace std;

// Node structure for the graph
struct Node {
    int id;
    string name;
    Node(int id, string name) : id(id), name(name) {}
};

// Edge structure for the graph
struct Edge {
    int from;
    int to;
    int weight;
    Edge(int from, int to, int weight) : from(from), to(to), weight(weight) {}
    Edge() : from(0), to(0), weight(0) {}
};

// Graph class with improved error handling
class Graph {
private:
    vector<string> nodeNames;
    vector<vector<Edge>> adjList;
    int nodeCount;

public:
    Graph() : nodeCount(0) {
        loadGraphFromFile();
    }

    ~Graph() {
        saveGraphToFile();
    }

    void setNodeCount(int count) {
        nodeCount = count;
        nodeNames.resize(count);
        adjList.resize(count);
    }

    bool addNode(int index, const string &name) {
        if (index < 0 || index >= nodeCount)
            return false;
        nodeNames[index] = name;
        return true;
    }

    bool addEdge(const string &fromName, const string &toName, int weight) {
        int from = getNodeIndex(fromName);
        int to = getNodeIndex(toName);

        if (from == -1 || to == -1 || weight < 0)
            return false;

        // Add edge in both directions (undirected graph)
        adjList[from].push_back(Edge(from, to, weight));
        adjList[to].push_back(Edge(to, from, weight));
        return true;
    }

    bool deleteNode(int nodeIndex) {
        if (nodeIndex < 0 || nodeIndex >= nodeCount)
            return false;

        // Remove the node and adjust all other nodes
        nodeNames.erase(nodeNames.begin() + nodeIndex);
        adjList.erase(adjList.begin() + nodeIndex);

        // Update remaining edges
        for (auto &edges : adjList) {
            edges.erase(
                remove_if(edges.begin(), edges.end(),
                          [nodeIndex](const Edge &e) {
                              return e.to == nodeIndex || e.from == nodeIndex;
                          }),
                edges.end());

            // Update indices for remaining edges
            for (Edge &e : edges) {
                if (e.to > nodeIndex)
                    e.to--;
                if (e.from > nodeIndex)
                    e.from--;
            }
        }

        nodeCount--;
        return true;
    }

    bool modifyNode(int nodeIndex, const string &newName) {
        if (nodeIndex < 0 || nodeIndex >= nodeCount)
            return false;
        nodeNames[nodeIndex] = newName;
        return true;
    }

    bool deleteEdge(const string &fromName, const string &toName) {
        int from = getNodeIndex(fromName);
        int to = getNodeIndex(toName);

        if (from == -1 || to == -1)
            return false;

        // Remove edge in both directions
        auto removeEdge = [](vector<Edge> &edges, int from, int to) {
            edges.erase(
                remove_if(edges.begin(), edges.end(),
                          [from, to](const Edge &e) {
                              return e.from == from && e.to == to;
                          }),
                edges.end());
        };

        removeEdge(adjList[from], from, to);
        removeEdge(adjList[to], to, from);
        return true;
    }

    bool modifyEdge(const string &fromName, const string &toName, int newWeight) {
        if (newWeight < 0)
            return false;

        int from = getNodeIndex(fromName);
        int to = getNodeIndex(toName);

        if (from == -1 || to == -1)
            return false;

        // Modify edge in both directions
        auto modifyEdgeWeight = [newWeight](vector<Edge> &edges, int from, int to) {
            for (Edge &e : edges) {
                if (e.from == from && e.to == to) {
                    e.weight = newWeight;
                    return true;
                }
            }
            return false;
        };

        return modifyEdgeWeight(adjList[from], from, to) &&
               modifyEdgeWeight(adjList[to], to, from);
    }

    const vector<Edge> &getEdges(int node) const {
        return adjList[node];
    }

    int getNodeCount() const { return nodeCount; }

    const string &getNodeName(int index) const {
        return nodeNames[index];
    }

    int getNodeIndex(const string &name) const {
        for (int i = 0; i < nodeCount; ++i) {
            if (nodeNames[i] == name)
                return i;
        }
        return -1;
    }

    void displayGraph() {
        cout << "\nCollection Points and Routes:\n";
        for (int i = 0; i < nodeCount; ++i) {
            cout << "Collection Point " << nodeNames[i] << " (ID: " << i << ")\n";
            for (const Edge &e : adjList[i]) {
                cout << "  Route to " << nodeNames[e.to] << " with distance " << e.weight << "\n";
            }
        }
    }

    void saveGraphToFile() {
        ofstream outFile("graph.txt");
        if (outFile.is_open()) {
            outFile << nodeCount << "\n";
            for (const auto &name : nodeNames) {
                outFile << name << "\n";
            }
            for (const auto &edges : adjList) {
                for (const auto &edge : edges) {
                    outFile << edge.from << " " << edge.to << " " << edge.weight << "\n";
                }
            }
            outFile.close();
        } else {
            cerr << "Unable to open file for saving graph.\n";
        }
    }

    void loadGraphFromFile() {
        ifstream inFile("graph.txt");
        if (inFile.is_open()) {
            inFile >> nodeCount;
            inFile.ignore();
            nodeNames.resize(nodeCount);
            adjList.resize(nodeCount);
            for (auto &name : nodeNames) {
                getline(inFile, name);
            }
            int from, to, weight;
            while (inFile >> from >> to >> weight) {
                adjList[from].push_back(Edge(from, to, weight));
            }
            inFile.close();
        } else {
            cerr << "Unable to open file for loading graph.\n";
        }
    }
};

// Dijkstra's Algorithm implementation with Path Tracking
void dijkstra(const Graph &graph, const string &startName) {
    int nodeCount = graph.getNodeCount();
    if (nodeCount == 0) {
        cout << "No collection points exist in the system.\n";
        return;
    }

    int startNode = graph.getNodeIndex(startName);
    if (startNode == -1) {
        cout << "Starting collection point not found.\n";
        return;
    }

    vector<int> distances(nodeCount, INT_MAX);
    vector<bool> visited(nodeCount, false);
    vector<int> previous(nodeCount, -1); // To track the shortest path
    distances[startNode] = 0;

    for (int i = 0; i < nodeCount; i++) {
        int minDist = INT_MAX;
        int currentNode = -1;

        // Find unvisited node with minimum distance
        for (int j = 0; j < nodeCount; j++) {
            if (!visited[j] && distances[j] < minDist) {
                minDist = distances[j];
                currentNode = j;
            }
        }

        if (currentNode == -1)
            break;
        visited[currentNode] = true;

        // Update distances through current node
        const vector<Edge> &edges = graph.getEdges(currentNode);
        for (const Edge &edge : edges) {
            if (!visited[edge.to]) {
                int newDist = distances[currentNode] + edge.weight;
                if (newDist < distances[edge.to]) {
                    distances[edge.to] = newDist;
                    previous[edge.to] = currentNode;
                }
            }
        }
    }

    // Display results
    cout << "\nShortest paths from collection point " << startName << ":\n";
    int shortestDist = INT_MAX;
    int targetNode = -1;
    for (int i = 0; i < nodeCount; i++) {
        if (i != startNode) {
            const string &nodeName = graph.getNodeName(i);
            if (distances[i] == INT_MAX) {
                cout << "Collection point " << nodeName << " is unreachable.\n";
            } else {
                cout << "Collection point " << nodeName << " - Distance: " << distances[i] << "\n";
                if (distances[i] < shortestDist) {
                    shortestDist = distances[i];
                    targetNode = i;
                }
            }
        }
    }

    // Display the shortest path
    if (targetNode != -1) {
        cout << "\nShortest path to collection point " << graph.getNodeName(targetNode) << " (Distance: " << shortestDist << "):\n";

        stack<int> path;
        int currentNode = targetNode;
        while (currentNode != -1) {
            path.push(currentNode);
            currentNode = previous[currentNode];
        }

        while (!path.empty()) {
            cout << graph.getNodeName(path.top()) << " ";
            path.pop();
        }
        cout << endl;
    }
}

// Admin credentials structure
struct Admin {
    string name;
    int password;
    Admin(string n, int p) : name(n), password(p) {}
};

// Login system class
class LoginSystem {
private:
    vector<Admin> admins;

public:
    LoginSystem() {
        admins = {
            Admin("adarsh", 12345),
            Admin("bhargavi", 54321),
            Admin("shreyas", 67890),
            Admin("shubham", 98765)};
    }

    bool adminLogin() {
        string adminName;
        int password;

        cout << "Enter Admin ID (Name): ";
        cin.ignore();
        getline(cin, adminName);

        cout << "Enter Password (5-digit positive integer): ";
        cin >> password;

        if (cin.fail() || password < 10000 || password > 99999) {
            cin.clear();
            cin.ignore(INT_MAX, '\n');
            cout << "Invalid password format. Must be a 5-digit positive integer.\n";
            return false;
        }

        for (const Admin &admin : admins) {
            if (admin.name == adminName && admin.password == password) {
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

// Menu display functions
void displayMainMenu() {
    cout << "\nLogin Page\n"
         << "1. Admin Login\n"
         << "2. User Login\n"
         << "3. Exit\n"
         << "Enter your choice: ";
}

void displayAdminMenu() {
    cout << "\nAdmin Menu:\n"
         << "1. Add collection points and their names\n"
         << "2. Modify collection point name\n"
         << "3. Delete collection point\n"
         << "4. Add routes (edges)\n"
         << "5. Modify route (edge)\n"
         << "6. Delete route (edge)\n"
         << "7. Display collection points and routes\n"
         << "8. Exit\n"
         << "Enter your choice: ";
}

void displayUserMenu() {
    cout << "\nUser Menu:\n"
         << "1. Optimize routes to find their shortest path\n"
         << "2. Exit\n"
         << "Enter your choice: ";
}

// Input validation function
template <typename T>
bool getValidInput(T &input, const string &prompt) {
    cout << prompt;
    cin >> input;

    if (cin.fail()) {
        cin.clear();
        cin.ignore(INT_MAX, '\n');
        cout << "Invalid input. Please try again.\n";
        return false;
    }
    return true;
}

int main() {
    LoginSystem loginSystem;
    Graph cityGraph;

    while (true) {
        displayMainMenu();
        int choice;

        if (!getValidInput(choice, ""))
            continue;

        switch (choice) {
        case 1: // Admin Login
            if (loginSystem.adminLogin()) {
                int adminChoice;
                do {
                    displayAdminMenu();
                    if (!getValidInput(adminChoice, ""))
                        continue;

                    switch (adminChoice) {
                    case 1: { // Add collection points
                        int numNodes;
                        cout << "Enter the number of collection points: ";
                        if (getValidInput(numNodes, "") && numNodes > 0) {
                            cityGraph.setNodeCount(numNodes);
                            cin.ignore();
                            for (int i = 0; i < numNodes; ++i) {
                                string name;
                                cout << "Enter name for collection point " << i + 1 << ": ";
                                getline(cin, name);
                                if (!cityGraph.addNode(i, name)) {
                                    cout << "Failed to add collection point.\n";
                                }
                            }
                        }
                        break;
                    }

                    case 2: { // Modify collection point name
                        int nodeIndex;
                        string newName;
                        cout << "Enter collection point index to modify: ";
                        if (getValidInput(nodeIndex, "")) {
                            cin.ignore();
                            cout << "Enter new name: ";
                            getline(cin, newName);
                            if (!cityGraph.modifyNode(nodeIndex, newName)) {
                                cout << "Failed to modify collection point.\n";
                            }
                        }
                        break;
                    }

                    case 3: { // Delete collection point
                        int nodeIndex;
                        cout << "Enter collection point index to delete: ";
                        if (getValidInput(nodeIndex, "")) {
                            if (!cityGraph.deleteNode(nodeIndex)) {
                                cout << "Failed to delete collection point.\n";
                            }
                        }
                        break;
                    }

                    case 4: { // Add route
                        string fromName, toName;
                        int weight;
                        cin.ignore();
                        cout << "Enter source collection point name: ";
                        getline(cin, fromName);
                        cout << "Enter destination collection point name: ";
                        getline(cin, toName);
                        cout << "Enter distance: ";
                        if (getValidInput(weight, "")) {
                            if (!cityGraph.addEdge(fromName, toName, weight)) {
                                cout << "Failed to add route.\n";
                            }
                        }
                        break;
                    }

                    case 5: { // Modify route
                        string fromName, toName;
                        int newWeight;
                        cin.ignore();
                        cout << "Enter source collection point name: ";
                        getline(cin, fromName);
                        cout << "Enter destination collection point name: ";
                        getline(cin, toName);
                        cout << "Enter new distance: ";
                        if (getValidInput(newWeight, "")) {
                            if (!cityGraph.modifyEdge(fromName, toName, newWeight)) {
                                cout << "Failed to modify route.\n";
                            }
                        }
                        break;
                    }

                    case 6: { // Delete route
                        string fromName, toName;
                        cin.ignore();
                        cout << "Enter source collection point name: ";
                        getline(cin, fromName);
                        cout << "Enter destination collection point name: ";
                        getline(cin, toName);
                        if (!cityGraph.deleteEdge(fromName, toName)) {
                            cout << "Failed to delete route.\n";
                        }
                        break;
                    }

                    case 7: { // Display collection points and routes
                        cityGraph.displayGraph();
                        break;
                    }

                    case 8:
                        break;
                    default:
                        cout << "Invalid choice.\n";
                    }
                } while (adminChoice != 8);
            }
            break;

        case 2: // User Login
            loginSystem.userLogin();
            {
                string startNode;
                cout << "Enter collection point to start from: ";
                cin.ignore();
                getline(cin, startNode);
                dijkstra(cityGraph, startNode);
            }
            break;

        case 3:
            return 0;

        default:
            cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}
