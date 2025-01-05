#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <queue>
#include <set>
#include <iomanip>
#include <limits>
#include <unordered_map>
#include <ctime>
#include <limits.h>
#include <stack>
#include <cmath>
#include <cctype>
#include <cstdlib> // For rand() and srand()
#include <cfloat>
#include <climits>
#include <regex>
#include <conio.h>

using namespace std;

// Optimized Waste Collection Network--------------------------------------------------------------------
// Node structure for the graph
struct Node
{
    int id;
    string name;
    Node(int id, string name) : id(id), name(name) {}
};

// Edge structure for the graph
struct Edge
{
    int from;
    int to;
    float weight;
    Edge(int from, int to, float weight) : from(from), to(to), weight(weight) {}
    Edge() : from(0), to(0), weight(0) {}
};

// Graph class with improved error handling
class Graph
{
private:
    vector<string> nodeNames;
    vector<vector<Edge>> adjList;
    int nodeCount;

    void saveGraphToFile()
    {
        ofstream outFile("graph.txt");
        if (outFile.is_open())
        {
            outFile << nodeCount << "\n";
            for (const auto &name : nodeNames)
            {
                outFile << name << "\n";
            }
            for (const auto &edges : adjList)
            {
                for (const auto &edge : edges)
                {
                    outFile << edge.from << " " << edge.to << " " << edge.weight << "\n";
                }
            }
            outFile.close();
        }
    }

    void loadGraphFromFile()
    {
        ifstream inFile("graph.txt");
        if (inFile.is_open())
        {
            inFile >> nodeCount;
            inFile.ignore();
            nodeNames.resize(nodeCount);
            adjList.resize(nodeCount);
            for (auto &name : nodeNames)
            {
                getline(inFile, name);
            }
            int from, to;
            float weight;
            while (inFile >> from >> to >> weight)
            {
                adjList[from].push_back(Edge(from, to, weight));
            }
            inFile.close();
        }
    }

public:
    Graph() : nodeCount(0)
    {
        loadGraphFromFile();
    }

    ~Graph()
    {
        saveGraphToFile();
    }

    void setNodeCount(int count)
    {
        nodeCount = count;
        nodeNames.resize(count);
        adjList.resize(count);
    }

    bool addNode(const string &name)
    {
        nodeNames.push_back(name);
        adjList.emplace_back();
        nodeCount++;
        saveGraphToFile();
        cout << "Collection Point added successfully.\n";
        return true;
    }

    bool addEdge(const string &fromName, const string &toName, float weight)
    {
        int from = getNodeIndex(fromName);
        int to = getNodeIndex(toName);

        if (from == -1 || to == -1 || weight < 0)
        {
            cout << "Failed to add Route.\n";
            return false;
        }

        // Add edge in both directions (undirected graph)
        adjList[from].push_back(Edge(from, to, weight));
        adjList[to].push_back(Edge(to, from, weight));

        saveGraphToFile();
        cout << "Route added successfully.\n";
        return true;
    }

    bool deleteNode(const string &name)
    {
        int nodeIndex = getNodeIndex(name);
        if (nodeIndex < 0 || nodeIndex >= nodeCount)
        {
            cout << "Failed to delete Collection Point.\n";
            return false;
        }

        // Remove the node and adjust all other nodes
        nodeNames.erase(nodeNames.begin() + nodeIndex);
        adjList.erase(adjList.begin() + nodeIndex);

        // Update remaining edges
        for (auto &edges : adjList)
        {
            edges.erase(
                remove_if(edges.begin(), edges.end(),
                          [nodeIndex](const Edge &e)
                          {
                              return e.to == nodeIndex || e.from == nodeIndex;
                          }),
                edges.end());

            // Update indices for remaining edges
            for (Edge &e : edges)
            {
                if (e.to > nodeIndex)
                {
                    e.to--;
                }

                if (e.from > nodeIndex)
                {
                    e.from--;
                }
            }
        }

        nodeCount--;
        cout << "Collection point '" << name << "' has been deleted successfully.\n";
        saveGraphToFile();
        return true;
    }

    bool modifyNode(const string &oldName, const string &newName)
    {
        int nodeIndex = getNodeIndex(oldName);
        if (nodeIndex < 0 || nodeIndex >= nodeCount)
        {
            cout << "Failed to modify Collection Point.\n";
            return false;
        }
        nodeNames[nodeIndex] = newName;
        cout << "Collection point '" << oldName << "' has been renamed to '" << newName << "' successfully.\n";
        saveGraphToFile();
        return true;
    }

    bool deleteEdge(const string &fromName, const string &toName)
    {
        int from = getNodeIndex(fromName);
        int to = getNodeIndex(toName);

        if (from == -1 || to == -1)
        {
            cout << "Failed to delete Route.\n";
            return false;
        }

        // Remove edge in both directions
        auto removeEdge = [](vector<Edge> &edges, int from, int to)
        {
            edges.erase(
                remove_if(edges.begin(), edges.end(),
                          [from, to](const Edge &e)
                          {
                              return e.from == from && e.to == to;
                          }),
                edges.end());
        };

        removeEdge(adjList[from], from, to);
        removeEdge(adjList[to], to, from);

        saveGraphToFile();
        cout << "Route deleted successfully.\n";
        return true;
    }

    bool modifyEdge(const string &fromName, const string &toName, float newWeight)
    {
        if (newWeight < 0)
        {
            cout << "Failed to modify Route.\n";
            return false;
        }

        int from = getNodeIndex(fromName);
        int to = getNodeIndex(toName);

        if (from == -1 || to == -1)
        {
            cout << "Failed to modify Route.\n";
            return false;
        }

        // Modify edge in both directions
        auto modifyEdgeWeight = [newWeight](vector<Edge> &edges, int from, int to)
        {
            for (Edge &e : edges)
            {
                if (e.from == from && e.to == to)
                {
                    e.weight = newWeight;
                    return true;
                }
            }
            return false;
        };

        bool result = modifyEdgeWeight(adjList[from], from, to) &&
                      modifyEdgeWeight(adjList[to], to, from);

        if (result)
        {
            saveGraphToFile();
            cout << "Route modified successfully.\n";
        }
        else
        {
            cout << "Failed to modify Route.\n";
        }
        return result;
    }

    const vector<Edge> &getEdges(int node) const
    {
        return adjList[node];
    }

    int getNodeCount() const
    {
        return nodeCount;
    }

    const string &getNodeName(int index) const
    {
        return nodeNames[index];
    }

    int getNodeIndex(const string &name) const
    {
        for (int i = 0; i < nodeCount; ++i)
        {
            if (nodeNames[i] == name)
                return i;
        }
        return -1;
    }

    void displayGraph()
    {
        cout << "\nCollection Points and Routes:\n";
        for (int i = 0; i < nodeCount; ++i)
        {
            cout << "Collection Point " << nodeNames[i] << " (ID: " << i << ")\n";
            for (const Edge &e : adjList[i])
            {
                cout << "  Route to " << nodeNames[e.to] << " with distance " << e.weight << "\n";
            }
        }
    }
};

// Dijkstra's Algorithm implementation with Path Tracking
void dijkstra(const Graph &cityGraph, const string &startName)
{
    int nodeCount = cityGraph.getNodeCount();
    if (nodeCount == 0)
    {
        cout << "No collection points exist in the system.\n";
        return;
    }

    int startNode = cityGraph.getNodeIndex(startName);
    if (startNode == -1)
    {
        cout << "Starting collection point not found.\n";
        return;
    }

    vector<float> distances(nodeCount, FLT_MAX);
    vector<bool> visited(nodeCount, false);
    vector<int> previous(nodeCount, -1); // To track the shortest path
    distances[startNode] = 0;

    for (int i = 0; i < nodeCount; i++)
    {
        float minDist = FLT_MAX;
        int currentNode = -1;

        // Find unvisited node with minimum distance
        for (int j = 0; j < nodeCount; j++)
        {
            if (!visited[j] && distances[j] < minDist)
            {
                minDist = distances[j];
                currentNode = j;
            }
        }

        if (currentNode == -1)
            break;
        visited[currentNode] = true;

        // Update distances through current node
        const vector<Edge> &edges = cityGraph.getEdges(currentNode);
        for (const Edge &edge : edges)
        {
            if (!visited[edge.to])
            {
                float newDist = distances[currentNode] + edge.weight;
                if (newDist < distances[edge.to])
                {
                    distances[edge.to] = newDist;
                    previous[edge.to] = currentNode;
                }
            }
        }
    }

    // Display results
    cout << "\nShortest paths from collection point " << startName << ":\n";
    float shortestDist = FLT_MAX;
    int targetNode = -1;
    for (int i = 0; i < nodeCount; i++)
    {
        if (i != startNode)
        {
            const string &nodeName = cityGraph.getNodeName(i);
            if (distances[i] == FLT_MAX)
            {
                cout << "Collection point " << nodeName << " is unreachable.\n";
            }
            else
            {
                cout << "Collection point " << nodeName << " - Distance: " << distances[i] << "\n";
                if (distances[i] < shortestDist)
                {
                    shortestDist = distances[i];
                    targetNode = i;
                }
            }
        }
    }

    // Display the shortest path
    if (targetNode != -1)
    {
        cout << "\nShortest path to collection point " << cityGraph.getNodeName(targetNode) << " (Distance: " << shortestDist << "):\n";

        stack<int> path;
        int currentNode = targetNode;
        while (currentNode != -1)
        {
            path.push(currentNode);
            currentNode = previous[currentNode];
        }

        while (!path.empty())
        {
            cout << cityGraph.getNodeName(path.top()) << " ";
            path.pop();
        }
        cout << endl;
    }
}

// Input validation function
template <typename T>
bool getValidInput(T &input, const string &prompt)
{
    cout << prompt;
    cin >> input;

    if (cin.fail())
    {
        cin.clear();
        cin.ignore(INT_MAX, '\n');
        cout << "Invalid input. Please try again.\n";
        return false;
    }
    return true;
}
//-------------------------------------------------------------------------------------------------------

// Recycled Product Marketplace---------------------------------------------------------------------------
//  Structure to represent a product
struct Product
{
    string name;
    string category;
    double price;

    Product(string n, string c, double p) : name(n), category(c), price(p) {}
};

class Recycled_Product_Marketplace
{
private:
    unordered_map<string, vector<Product>> productCatalog; // Maps categories to products

    void saveProductsToFile()
    {
        ofstream outFile("products.txt");
        if (outFile.is_open())
        {
            for (const auto &pair : productCatalog)
            {
                for (const auto &product : pair.second)
                {
                    outFile << product.category << "," << product.name << "," << product.price << "\n";
                }
            }
            outFile.close();
        }
    }

    void loadProductsFromFile()
    {
        ifstream inFile("products.txt");
        if (inFile.is_open())
        {
            string line;
            while (getline(inFile, line))
            {
                size_t pos1 = line.find(',');
                size_t pos2 = line.find(',', pos1 + 1);
                string category = line.substr(0, pos1);
                string name = line.substr(pos1 + 1, pos2 - pos1 - 1);
                double price = stod(line.substr(pos2 + 1));
                productCatalog[category].emplace_back(name, category, price);
            }
            inFile.close();
        }
    }

public:
    Recycled_Product_Marketplace()
    {
        loadProductsFromFile();
    }

    ~Recycled_Product_Marketplace()
    {
        saveProductsToFile();
    }

    void addProduct(const string &category, const Product &product)
    {
        productCatalog[category].push_back(product);
        cout << "Added product: " << product.name << " in category: " << category << "\n";
    }

    void displayProducts()
    {
        for (const auto &pair : productCatalog)
        {
            cout << "Category: " << pair.first << "\n";
            for (const auto &product : pair.second)
            {
                cout << "  Name: " << product.name
                     << ", Price: $" << product.price << "\n";
            }
        }
    }

    vector<Product> searchProducts(const string &category)
    {
        vector<Product> result;
        if (productCatalog.find(category) != productCatalog.end())
        {
            result = productCatalog[category];
        }
        return result;
    }

    void sortProductsByPrice(vector<Product> &products)
    {
        // Sort by price in ascending order
        sort(products.begin(), products.end(), [](const Product &a, const Product &b)
             { return a.price < b.price; });
    }
};
//-------------------------------------------------------------------------------------------------------

// Smart Bin Monitoring System----------------------------------------------------------------------------
class Bin_Monitoring_System
{
private:
    struct Bin
    {
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

    void saveBinsToFile()
    {
        ofstream outFile("bins.txt");
        if (outFile.is_open())
        {
            for (const auto &bin : bins)
            {
                outFile << bin.id << "," << bin.location << "," << bin.fillLevel << "," << bin.needsCollection << "," << bin.underMaintenance << "\n";
            }
            outFile.close();
        }
    }

    void loadBinsFromFile()
    {
        ifstream inFile("bins.txt");
        if (inFile.is_open())
        {
            string line;
            while (getline(inFile, line))
            {
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
                if (needsCollection)
                {
                    collectionQueue.push(id);
                }
            }
            inFile.close();
        }
    }

public:
    Bin_Monitoring_System()
    {
        loadBinsFromFile();
    }

    ~Bin_Monitoring_System()
    {
        saveBinsToFile();
    }

    // Admin Functions
    void addBin()
    {
        Bin bin;
        cout << "Enter bin ID: ";
        cin >> bin.id;

        // Check for duplicate bin ID
        for (const Bin &existingBin : bins)
        {
            if (existingBin.id == bin.id)
            {
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

    void editBinDetails()
    {

        int binId;
        cout << "Enter bin ID to edit: ";
        cin >> binId;

        for (Bin &bin : bins)
        {
            if (bin.id == binId)
            {
                cout << "Enter new location: ";
                cin.ignore();
                getline(cin, bin.location);
                cout << "Bin details updated successfully!\n";
                return;
            }
        }
        cout << "Bin not found.\n";
    }

    void deleteBin()
    {

        int binId;
        cout << "Enter bin ID to delete: ";
        cin >> binId;

        for (auto it = bins.begin(); it != bins.end(); ++it)
        {
            if (it->id == binId)
            {
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

    void updateBinStatus()
    {

        int binId;
        cout << "Enter bin ID: ";
        cin >> binId;

        for (Bin &bin : bins)
        {
            if (bin.id == binId)
            {
                cout << "Enter new fill level (0-100): ";
                cin >> bin.fillLevel;

                if (bin.fillLevel >= FILL_THRESHOLD && !bin.needsCollection)
                {
                    bin.needsCollection = true;
                    collectionQueue.push(bin.id);
                    cout << "Alert: Bin needs collection!\n";
                }
                return;
            }
        }
        cout << "Bin not found.\n";
    }

    void viewBinStatus() const
    {
        if (bins.empty())
        {
            cout << "No bins available.\n";
            return;
        }

        cout << "\n=== Bin Status ===\n";
        for (const Bin &bin : bins)
        {
            cout << "Bin " << bin.id << " at " << bin.location << ":\n";
            cout << "Fill Level: " << bin.fillLevel << "%\n";
            cout << "Status: " << (bin.needsCollection ? "Needs collection" : "OK") << "\n";
            cout << "Maintenance: " << (bin.underMaintenance ? "Under Maintenance" : "Operational") << "\n\n";
        }
    }
};
//-------------------------------------------------------------------------------------------------------

// Smart Composting System--------------------------------------------------------------------------------
//  Structure for compost parameters
struct CompostData
{
    double temperature;
    double moisture;
    string stage;
    string timeStamp;
};

// Heap class for parameter monitoring
class CompostHeap
{
private:
    vector<double> heap;

    void heapifyUp(int index)
    {
        while (index > 0)
        {
            int parent = (index - 1) / 2;
            if (heap[parent] < heap[index])
            {
                swap(heap[parent], heap[index]);
                index = parent;
            }
            else
            {
                break;
            }
        }
    }

    void heapifyDown(int index)
    {
        int size = heap.size();
        while (true)
        {
            int largest = index;
            int left = 2 * index + 1;
            int right = 2 * index + 2;

            if (left < size && heap[left] > heap[largest])
            {
                largest = left;
            }
            if (right < size && heap[right] > heap[largest])
            {
                largest = right;
            }

            if (largest != index)
            {
                swap(heap[index], heap[largest]);
                index = largest;
            }
            else
            {
                break;
            }
        }
    }

public:
    void insert(double value)
    {
        heap.push_back(value);
        heapifyUp(heap.size() - 1);
    }

    double getMax()
    {
        if (heap.empty())
            return -1;
        return heap[0];
    }

    void removeMax()
    {
        if (heap.empty())
            return;
        heap[0] = heap.back();
        heap.pop_back();
        if (!heap.empty())
            heapifyDown(0);
    }
};

class Smart_Compost_System
{
private:
    vector<CompostData> compostHistory;
    CompostHeap temperatureHeap;
    CompostHeap moistureHeap;

    void saveCompostDataToFile()
    {
        ofstream outFile("compost_data.txt");
        if (outFile.is_open())
        {
            for (const auto &data : compostHistory)
            {
                outFile << data.temperature << "," << data.moisture << "," << data.stage << "," << data.timeStamp << "\n";
            }
            outFile.close();
        }
    }

    void loadCompostDataFromFile()
    {
        ifstream inFile("compost_data.txt");
        if (inFile.is_open())
        {
            string line;
            while (getline(inFile, line))
            {
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
        }
    }

public:
    Smart_Compost_System()
    {
        loadCompostDataFromFile();
    }

    ~Smart_Compost_System()
    {
        saveCompostDataToFile();
    }

    // Admin Functions
    void addCompostData(double temp, double moisture, string stage)
    {
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

    void viewAllData()
    {
        cout << "\nComplete Compost History:\n";
        cout << setw(15) << "Temperature" << setw(15) << "Moisture"
             << setw(20) << "Stage" << setw(25) << "Timestamp\n";
        cout << string(75, '-') << "\n";

        for (const auto &data : compostHistory)
        {
            cout << setw(15) << data.temperature
                 << setw(15) << data.moisture
                 << setw(20) << data.stage
                 << setw(25) << data.timeStamp << "\n";
        }
    }

    // User Functions
    void checkCurrentStatus()
    {
        if (compostHistory.empty())
        {
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
        {
            cout << "Warning: Temperature too high! Consider adding brown materials.\n";
        }
        else if (latest.temperature < 45)
        {
            cout << "Warning: Temperature too low! Consider adding green materials.\n";
        }

        if (latest.moisture > 60)
        {
            cout << "Warning: Too wet! Add dry brown materials.\n";
        }
        else if (latest.moisture < 40)
        {
            cout << "Warning: Too dry! Add water or green materials.\n";
        }
    }

    void searchStage(string stage)
    {
        cout << "\nSearching for stage: " << stage << "\n";
        bool found = false;

        for (const auto &data : compostHistory)
        {
            if (data.stage == stage)
            {
                cout << "Found entry:\n";
                cout << "Temperature: " << data.temperature << "°C\n";
                cout << "Moisture: " << data.moisture << "%\n";
                cout << "Timestamp: " << data.timeStamp << "\n\n";
                found = true;
            }
        }

        if (!found)
        {
            cout << "No entries found for stage: " << stage << "\n";
        }
    }

private:
    string getCurrentTime()
    {
        time_t now = time(0);
        string dt = ctime(&now);
        return dt.substr(0, dt.length() - 1);
    }
};
//-------------------------------------------------------------------------------------------------------

// Smart Waste Segregation System-------------------------------------------------------------------------
// Function to convert a string to lowercase
string toLowerCase2(const string &str)
{
    string lowerStr = str;
    transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

// Smart Waste Segregation System class
class Smart_Waste_System
{
private:
    struct WasteMap
    {
        string category;
        vector<string> items;
    };

    vector<WasteMap> wasteMap;

    vector<string> recyclableItems = {"Paper", "Glass", "Metal", "Plastic", "Cardboard", "Used books", "Tires"};
    vector<string> nonRecyclableItems = {"Food Waste", "Styrofoam", "Ceramics", "Diapers", "Hazardous Waste", "Cigarette butts", "Broken mirrors"};
    vector<string> organicItems = {"Food scraps", "Coffee grounds and tea bags", "Eggshells", "Yard clippings and grass", "Fallen leaves", "Animal manure", "Stale bread"};
    vector<string> hazardousItems = {"Batteries", "Paints and solvents", "Pesticides", "Motor oil and lubricants", "Cleaning agents", "Mercury thermometers", "Herbicides"};
    vector<string> medicalItems = {"Used syringes and needles", "Bandages and dressings", "Expired medications", "Used gloves", "IV bags and tubing", "Pathological waste (tissue, blood)", "Test tubes"};

    void quickSort(vector<string> &arr, int low, int high)
    {
        if (low < high)
        {
            int pi = partition(arr, low, high);
            quickSort(arr, low, pi - 1);
            quickSort(arr, pi + 1, high);
        }
    }

    int partition(vector<string> &arr, int low, int high)
    {
        string pivot = arr[high];
        int i = low - 1;

        for (int j = low; j < high; ++j)
        {
            if (arr[j] < pivot)
            {
                ++i;
                swap(arr[i], arr[j]);
            }
        }
        swap(arr[i + 1], arr[high]);
        return i + 1;
    }

    void saveWasteDataToFile()
    {
        ofstream outFile("waste_data.txt");
        if (outFile.is_open())
        {
            outFile << wasteMap.size() << "\n";
            for (const auto &map : wasteMap)
            {
                outFile << map.category << "," << map.items.size() << "\n";
                for (const auto &item : map.items)
                {
                    outFile << item << "\n";
                }
            }
            outFile.close();
        }
    }

    void loadWasteDataFromFile()
    {
        ifstream inFile("waste_data.txt");
        if (inFile.is_open())
        {
            int categoryCount;
            inFile >> categoryCount;
            inFile.ignore();
            wasteMap.resize(categoryCount);
            for (int i = 0; i < categoryCount; ++i)
            {
                getline(inFile, wasteMap[i].category, ',');
                int itemCount;
                inFile >> itemCount;
                inFile.ignore();
                wasteMap[i].items.resize(itemCount);
                for (int j = 0; j < itemCount; ++j)
                {
                    getline(inFile, wasteMap[i].items[j]);
                }
            }
            inFile.close();
        }
    }

public:
    Smart_Waste_System()
    {
        loadWasteDataFromFile();
    }

    ~Smart_Waste_System()
    {
        saveWasteDataToFile();
    }

    void addWaste(const string &category, const string &item)
    {
        int index = findCategoryIndex(category);
        string lowerItem = toLowerCase2(item);
        if (index == -1)
        {
            wasteMap.push_back({category, {lowerItem}});
        }
        else
        {
            wasteMap[index].items.push_back(lowerItem);
        }
        saveWasteDataToFile();
    }

    void modifyWaste(const string &category, const string &oldItem, const string &newItem)
    {
        int index = findCategoryIndex(category);
        if (index == -1)
        {
            cout << "Category not found.\n";
            return;
        }

        auto &items = wasteMap[index].items;
        auto it = find(items.begin(), items.end(), toLowerCase2(oldItem));
        if (it != items.end())
        {
            *it = toLowerCase2(newItem);
            saveWasteDataToFile();
            cout << "Waste item \"" << oldItem << "\" modified to \"" << newItem << "\" successfully.\n";
        }
        else
        {
            cout << "Item not found in the category.\n";
        }
    }

    void deleteWaste(const string &category, const string &item)
    {
        int index = findCategoryIndex(category);
        if (index == -1)
        {
            cout << "Category not found.\n";
            return;
        }

        auto &items = wasteMap[index].items;
        auto it = find(items.begin(), items.end(), toLowerCase2(item));
        if (it != items.end())
        {
            items.erase(it);
            saveWasteDataToFile();
            cout << "Waste item \"" << item << "\" deleted successfully.\n";
        }
        else
        {
            cout << "Item not found in the category.\n";
        }
    }

    void deleteWasteCategory(const string &category)
    {
        int index = findCategoryIndex(category);
        if (index == -1)
        {
            cout << "Category not found.\n";
            return;
        }

        wasteMap.erase(wasteMap.begin() + index);
        saveWasteDataToFile();
        cout << "Category \"" << category << "\" deleted successfully.\n";
    }

    void modifyWasteCategory(const string &oldCategory, const string &newCategory)
    {
        int index = findCategoryIndex(oldCategory);
        if (index == -1)
        {
            cout << "Category not found.\n";
            return;
        }

        wasteMap[index].category = newCategory;
        saveWasteDataToFile();
        cout << "Category \"" << oldCategory << "\" modified to \"" << newCategory << "\" successfully.\n";
    }

    void displayWasteCategories()
    {
        cout << "\nWaste Categories and Items:\n";
        for (const auto &map : wasteMap)
        {
            cout << map.category << ": ";
            auto items = map.items;
            quickSort(items, 0, items.size() - 1);
            for (size_t j = 0; j < items.size(); ++j)
            {
                if (j > 0)
                    cout << ", ";
                cout << items[j];
            }
            cout << endl;
        }
    }

    void displayPredefinedCategories()
    {
        cout << "\nPredefined Waste Categories:\n";
        cout << left << setw(20) << "Recyclable" << "|" << setw(30) << "Non-Recyclable" << "|" << setw(30) << "Organic" << "|" << setw(30) << "Hazardous" << "|" << setw(30) << "Medical" << endl;
        cout << string(140, '=') << endl;
        for (size_t i = 0; i < recyclableItems.size(); ++i)
        {
            cout << left << setw(20) << recyclableItems[i] << "|"
                 << setw(30) << nonRecyclableItems[i] << "|"
                 << setw(30) << organicItems[i] << "|"
                 << setw(30) << hazardousItems[i] << "|"
                 << setw(30) << medicalItems[i] << endl;
        }
    }

private:
    int findCategoryIndex(const string &category) const
    {
        for (size_t i = 0; i < wasteMap.size(); ++i)
        {
            if (wasteMap[i].category == category)
            {
                return i;
            }
        }
        return -1;
    }
};
//-------------------------------------------------------------------------------------------------------

// Waste Disposal and Recycling System--------------------------------------------------------------------
//  Function to convert string to lowercase
void toLowercase(string &input)
{
    for (char &c : input)
    {
        if (c >= 'A' && c <= 'Z') // Check if the character is uppercase
        {
            c = c + ('a' - 'A'); // Convert to lowercase by adjusting ASCII value
        }
    }
}

// Node structure for linked list
struct ItemNode
{
    string name;
    ItemNode *next;
    ItemNode(const string &itemName) : name(itemName), next(nullptr) {}
};

// Recycling System
class Recycling_System
{
private:
    ItemNode *recyclableHead;
    ItemNode *nonRecyclableHead;

    void saveItemsToFile(const string &filename, ItemNode *head)
    {
        ofstream outFile(filename);
        if (outFile.is_open())
        {
            ItemNode *current = head;
            while (current)
            {
                outFile << current->name << "\n";
                current = current->next;
            }
            outFile.close();
        }
    }

    void loadItemsFromFile(const string &filename, ItemNode *&head)
    {
        ifstream inFile(filename);
        if (inFile.is_open())
        {
            string itemName;
            while (getline(inFile, itemName))
            {
                ItemNode *newItem = new ItemNode(itemName);
                newItem->next = head;
                head = newItem;
            }
            inFile.close();
        }
    }

public:
    Recycling_System() : recyclableHead(nullptr), nonRecyclableHead(nullptr)
    {
        loadItemsFromFile("recyclable_items.txt", recyclableHead);
        loadItemsFromFile("non_recyclable_items.txt", nonRecyclableHead);
    }

    ~Recycling_System()
    {
        saveItemsToFile("recyclable_items.txt", recyclableHead);
        saveItemsToFile("non_recyclable_items.txt", nonRecyclableHead);

        ItemNode *current = recyclableHead;
        while (current)
        {
            ItemNode *toDelete = current;
            current = current->next;
            delete toDelete;
        }
        current = nonRecyclableHead;
        while (current)
        {
            ItemNode *toDelete = current;
            current = current->next;
            delete toDelete;
        }
    }

    // Add a recyclable item to the linked list
    void addRecyclableItem(const string &itemName)
    {
        string lowerItemName = itemName;
        toLowercase(lowerItemName); // Convert item name to lowercase before storing

        // Check if the item already exists in non-recyclable items
        if (isNonRecyclable(lowerItemName) != "")
        {
            cout << itemName << " is already in non-recyclable items. Cannot add to recyclable items.\n";
            return;
        }

        // Add the item to recyclable items
        ItemNode *newItem = new ItemNode(lowerItemName);
        newItem->next = recyclableHead;
        recyclableHead = newItem;
        cout << itemName << " added to recyclable items.\n";
    }

    // Add a non-recyclable item to the linked list
    void addNonRecyclableItem(const string &itemName)
    {
        string lowerItemName = itemName;
        toLowercase(lowerItemName); // Convert item name to lowercase before storing

        // Check if the item already exists in recyclable items
        if (isRecyclable(lowerItemName) != "")
        {
            cout << itemName << " is already in recyclable items. Cannot add to non-recyclable items.\n";
            return;
        }

        // Add the item to non-recyclable items
        ItemNode *newItem = new ItemNode(lowerItemName);
        newItem->next = nonRecyclableHead;
        nonRecyclableHead = newItem;
        cout << itemName << " added to non-recyclable items.\n";
    }

    // Delete a recyclable item from the linked list
    bool deleteRecyclableItem(const string &itemName)
    {
        string lowerItemName = itemName;
        toLowercase(lowerItemName); // Convert item name to lowercase before searching

        ItemNode *current = recyclableHead;
        ItemNode *previous = nullptr;

        while (current)
        {
            if (current->name == lowerItemName)
            {
                if (previous)
                    previous->next = current->next;
                else
                    recyclableHead = current->next;

                delete current;
                cout << itemName << " deleted from recyclable items.\n";
                return true;
            }
            previous = current;
            current = current->next;
        }
        cout << itemName << " not found in recyclable items.\n";
        return false;
    }

    // Delete a non-recyclable item from the linked list
    bool deleteNonRecyclableItem(const string &itemName)
    {
        string lowerItemName = itemName;
        toLowercase(lowerItemName); // Convert item name to lowercase before searching

        ItemNode *current = nonRecyclableHead;
        ItemNode *previous = nullptr;

        while (current)
        {
            if (current->name == lowerItemName)
            {
                if (previous)
                    previous->next = current->next;
                else
                    nonRecyclableHead = current->next;

                delete current;
                cout << itemName << " deleted from non-recyclable items.\n";
                return true;
            }
            previous = current;
            current = current->next;
        }
        cout << itemName << " not found in non-recyclable items.\n";
        return false;
    }

    // Modify a recyclable item in the linked list
    bool modifyRecyclableItem(const string &oldName, const string &newName)
    {
        string lowerOldName = oldName;
        toLowercase(lowerOldName); // Convert old name to lowercase before searching
        string lowerNewName = newName;
        toLowercase(lowerNewName); // Convert new name to lowercase before storing

        ItemNode *current = recyclableHead;

        while (current)
        {
            if (current->name == lowerOldName)
            {
                current->name = lowerNewName;
                cout << oldName << " modified to " << newName << " in recyclable items.\n";
                return true;
            }
            current = current->next;
        }
        cout << oldName << " not found in recyclable items.\n";
        return false;
    }

    // Modify a non-recyclable item in the linked list
    bool modifyNonRecyclableItem(const string &oldName, const string &newName)
    {
        string lowerOldName = oldName;
        toLowercase(lowerOldName); // Convert old name to lowercase before searching
        string lowerNewName = newName;
        toLowercase(lowerNewName); // Convert new name to lowercase before storing

        ItemNode *current = nonRecyclableHead;

        while (current)
        {
            if (current->name == lowerOldName)
            {
                current->name = lowerNewName;
                cout << oldName << " modified to " << newName << " in non-recyclable items.\n";
                return true;
            }
            current = current->next;
        }
        cout << oldName << " not found in non-recyclable items.\n";
        return false;
    }

    // Display all recyclable items
    void displayRecyclableItems()
    {
        cout << "Recyclable Items:\n";
        if (!recyclableHead)
        {
            cout << "No recyclable items available.\n";
            return;
        }
        ItemNode *current = recyclableHead;
        while (current)
        {
            cout << "- " << current->name << "\n"; // Automatically in lowercase
            current = current->next;
        }
    }

    // Display all non-recyclable items
    void displayNonRecyclableItems()
    {
        cout << "Non-Recyclable Items:\n";
        if (!nonRecyclableHead)
        {
            cout << "No non-recyclable items available.\n";
            return;
        }
        ItemNode *current = nonRecyclableHead;
        while (current)
        {
            cout << "- " << current->name << "\n"; // Automatically in lowercase
            current = current->next;
        }
    }

    // Brute force search: Check if an item is recyclable
    string isRecyclable(const string &itemName)
    {
        string lowerItemName = itemName;
        toLowercase(lowerItemName); // Convert item name to lowercase before searching

        ItemNode *current = recyclableHead;
        while (current)
        {
            if (current->name == lowerItemName) // Match found
                return current->name;
            current = current->next;
        }
        return ""; // No match
    }

    // Brute force search: Check if an item is non-recyclable
    string isNonRecyclable(const string &itemName)
    {
        string lowerItemName = itemName;
        toLowercase(lowerItemName); // Convert item name to lowercase before searching

        ItemNode *current = nonRecyclableHead;
        while (current)
        {
            if (current->name == lowerItemName) // Match found
                return current->name;
            current = current->next;
        }
        return ""; // No match
    }
};
//-------------------------------------------------------------------------------------------------------

// Waste to Energy Conversion System----------------------------------------------------------------------
//  Class for waste processing
class Waste_Processing_System
{
public:
    void loadRecords()
    {
        ifstream inFile("waste_records.txt");
        if (inFile.is_open())
        {
            string line;
            cout << "\nPast Waste Processing Records:\n";
            cout << "----------------------------\n";
            while (getline(inFile, line))
            {
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
        }
        else
        {
            cerr << "No past records found.\n";
        }
    }

    void saveRecord(const string &wasteType, float quantity, float energyGenerated, float totalEnergy)
    {
        ofstream outFile("waste_records.txt", ios::app);
        if (outFile.is_open())
        {
            outFile << wasteType << "," << quantity << "," << energyGenerated << "," << totalEnergy << "\n";
            outFile.close();
        }
    }
};
//-------------------------------------------------------------------------------------------------------

// Waste Volume Prediction System-------------------------------------------------------------------------
//  Structure for waste volume data
struct WasteData
{
    string date;
    string location;
    double volume;

    WasteData(string d, string l, double v) : date(d), location(l), volume(v) {}
};

// Trie node for location-based searching
struct TrieNode
{
    map<char, TrieNode *> children;
    bool isEndOfWord;
    vector<WasteData *> locationData;

    TrieNode() : isEndOfWord(false) {}
};

// Waste Volume Prediction System class
class WastePredictionSystem
{
private:
    vector<WasteData> historicalData;
    TrieNode *locationTrie;
    const int NUM_BUCKETS = 10;

    // Initialize Trie node
    TrieNode *createNode()
    {
        return new TrieNode();
    }

    // Insert location into Trie
    void insertLocation(string location, WasteData *data)
    {
        TrieNode *current = locationTrie;

        for (char c : location)
        {
            if (current->children.find(c) == current->children.end())
            {
                current->children[c] = createNode();
            }
            current = current->children[c];
        }

        current->isEndOfWord = true;
        current->locationData.push_back(data);
    }

    // Bucket Sort implementation
    void bucketSort(vector<WasteData> &data)
    {
        if (data.empty())
            return;

        vector<vector<WasteData>> buckets(NUM_BUCKETS);

        // Find range of volumes
        double maxVol = data[0].volume;
        double minVol = data[0].volume;
        for (const auto &entry : data)
        {
            maxVol = max(maxVol, entry.volume);
            minVol = min(minVol, entry.volume);
        }

        if (maxVol == minVol)
        {
            // All volumes are the same; no need to sort
            return;
        }

        // Distribute into buckets
        for (const auto &entry : data)
        {
            int bucketIndex = (int)((entry.volume - minVol) * NUM_BUCKETS / (maxVol - minVol + 1));
            bucketIndex = min(bucketIndex, NUM_BUCKETS - 1);
            buckets[bucketIndex].push_back(entry);
        }

        // Sort individual buckets
        for (auto &bucket : buckets)
        {
            sort(bucket.begin(), bucket.end(),
                 [](const WasteData &a, const WasteData &b)
                 {
                     return a.volume < b.volume;
                 });
        }

        // Concatenate all buckets
        data.clear();
        for (const auto &bucket : buckets)
        {
            data.insert(data.end(), bucket.begin(), bucket.end());
        }
    }

    void saveWasteDataToFile()
    {
        ofstream outFile("waste_data.txt");
        if (outFile.is_open())
        {
            for (const auto &data : historicalData)
            {
                outFile << data.date << "," << data.location << "," << data.volume << "\n";
            }
            outFile.close();
        }
    }

    void loadWasteDataFromFile()
    {
        ifstream inFile("waste_data.txt");
        if (inFile.is_open())
        {
            string line;
            while (getline(inFile, line))
            {
                size_t pos1 = line.find(',');
                size_t pos2 = line.find(',', pos1 + 1);
                string date = line.substr(0, pos1);
                string location = line.substr(pos1 + 1, pos2 - pos1 - 1);
                double volume = stod(line.substr(pos2 + 1));

                WasteData newData(date, location, volume);
                historicalData.push_back(newData);
                insertLocation(location, &historicalData.back());
            }
            inFile.close();
        }
    }

public:
    WastePredictionSystem()
    {
        locationTrie = createNode();
        loadWasteDataFromFile();
    }

    ~WastePredictionSystem()
    {
        saveWasteDataToFile();
    }

    // Add new waste data
    void addWasteData(string date, string location, double volume)
    {
        WasteData newData(date, location, volume);
        historicalData.push_back(newData);
        insertLocation(location, &historicalData.back());
        saveWasteDataToFile();
    }

    // Generate prediction for a location
    vector<double> predictWasteVolume(string location)
    {
        vector<double> predictions;
        TrieNode *current = locationTrie;

        // Find location in Trie
        for (char c : location)
        {
            if (current->children.find(c) == current->children.end())
            {
                cout << "No data available for this location.\n";
                return predictions;
            }
            current = current->children[c];
        }

        if (!current->isEndOfWord)
        {
            cout << "Location not found.\n";
            return predictions;
        }

        // Access location history
        vector<WasteData *> &locationHistory = current->locationData;
        if (locationHistory.empty())
        {
            cout << "No historical data for the location.\n";
            return predictions;
        }

        // Sort historical data
        vector<WasteData> sortedData;
        for (auto *data : locationHistory)
        {
            if (data)
            {
                sortedData.push_back(*data);
            }
        }

        if (sortedData.empty())
        {
            cout << "Error: No valid historical data found.\n";
            return predictions;
        }

        bucketSort(sortedData);

        // Calculate predictions for the next 3 months
        double avgVolume = 0;
        for (const auto &data : sortedData)
        {
            avgVolume += data.volume;
        }
        avgVolume /= sortedData.size();

        // Linear trend prediction
        double trend = 0;
        if (sortedData.size() > 1)
        {
            trend = (sortedData.back().volume - sortedData.front().volume) / (sortedData.size() - 1);
        }

        for (int i = 1; i <= 3; ++i)
        {
            predictions.push_back(max(0.0, avgVolume + trend * i)); // Ensure non-negative predictions
        }

        return predictions;
    }

    // Generate forecast report
    void generateForecastReport(string location)
    {
        vector<double> predictions = predictWasteVolume(location);
        if (predictions.empty())
        {
            return;
        }

        cout << "\nWaste Volume Forecast Report for " << location << "\n";
        cout << "----------------------------------------\n";
        cout << "Predictions for the next 3 months:\n\n";

        for (std::vector<double>::size_type i = 0; i < predictions.size(); i++)
        {
            cout << "Month " << (i + 1) << ": " << fixed << setprecision(2)
                 << predictions[i] << " tons\n";
        }

        cout << "\nNote: Predictions are based on historical data and trends.\n";
    }
};
//-------------------------------------------------------------------------------------------------------

// Zero Waste Event Planner------------------------------------------------------------------------------
struct WasteFactors
{
    double baseRate;
    double scaleFactor;
    int attendeeImpact;
    double durationImpact;
};

struct Event
{
    string name;
    float duration;
    string type;
    int attendees;
    double wasteForecast;

    Event(string n, float d, string t, int a, double w)
        : name(n), duration(d), type(t), attendees(a), wasteForecast(w) {}
};

class Zero_Waste_Event_Planner
{
private:
    unordered_map<string, vector<string>> wasteCategories;
    unordered_map<string, WasteFactors> eventWasteFactors;
    vector<Event> events;

    double calculateWasteForecast(float duration, const string &type, int attendees)
    {
        WasteFactors factors = eventWasteFactors[type];

        double baseWaste = factors.baseRate * attendees * duration;
        double scaledWaste = baseWaste * factors.scaleFactor;
        double attendeeScaling = 1.0 + (log10(attendees) * factors.attendeeImpact / 1000.0);
        double durationFactor = 1.0 + (factors.durationImpact * log10(duration + 1));
        double totalWaste = scaledWaste * attendeeScaling * durationFactor;

        return round(totalWaste * 100) / 100;
    }

    void saveEventsToFile()
    {
        ofstream outFile("events.txt");
        if (outFile.is_open())
        {
            for (const auto &event : events)
            {
                outFile << event.name << "," << event.duration << "," << event.type << "," << event.attendees << "," << event.wasteForecast << "\n";
            }
            outFile.close();
        }
    }

    void loadEventsFromFile()
    {
        ifstream inFile("events.txt");
        if (inFile.is_open())
        {
            string line;
            while (getline(inFile, line))
            {
                size_t pos1 = line.find(',');
                size_t pos2 = line.find(',', pos1 + 1);
                size_t pos3 = line.find(',', pos2 + 1);
                size_t pos4 = line.find(',', pos3 + 1);

                string name = line.substr(0, pos1);
                float duration = stof(line.substr(pos1 + 1, pos2 - pos1 - 1));
                string type = line.substr(pos2 + 1, pos3 - pos2 - 1);
                int attendees = stoi(line.substr(pos3 + 1, pos4 - pos3 - 1));
                double wasteForecast = stod(line.substr(pos4 + 1));

                events.emplace_back(name, duration, type, attendees, wasteForecast);
            }
            inFile.close();
        }
    }

public:
    Zero_Waste_Event_Planner()
    {
        wasteCategories["Conference"] = {"Paper", "Plastic", "Electronic"};
        wasteCategories["Festival"] = {"Food", "Plastic", "Glass", "Decorations"};
        wasteCategories["Sports"] = {"Food", "Plastic", "Metal Cans"};
        wasteCategories["Concert"] = {"Paper", "Plastic", "Batteries"};
        wasteCategories["Exhibition"] = {"Paper", "Plastic", "Glass", "Electronics"};
        wasteCategories["Seminar"] = {"Paper", "Plastic", "Electronic"};

        eventWasteFactors["Conference"] = {0.15, 1.0, 5, 0.8};
        eventWasteFactors["Festival"] = {0.25, 1.2, 8, 0.9};
        eventWasteFactors["Sports"] = {0.18, 0.9, 6, 0.7};
        eventWasteFactors["Concert"] = {0.22, 1.1, 7, 0.85};
        eventWasteFactors["Exhibition"] = {0.20, 1.0, 6, 0.8};
        eventWasteFactors["Seminar"] = {0.12, 0.8, 4, 0.75};

        loadEventsFromFile();
    }

    ~Zero_Waste_Event_Planner()
    {
        saveEventsToFile();
    }

    void planEvent(const string &name, float duration, string type, int attendees)
    {
        transform(type.begin(), type.end(), type.begin(), ::tolower);
        if (!type.empty())
        {
            type[0] = toupper(type[0]);
        }

        if (wasteCategories.find(type) == wasteCategories.end())
        {
            cout << "Invalid event type. Please choose a valid type.\n";
            return;
        }

        double wasteForecast = calculateWasteForecast(duration, type, attendees);
        events.emplace_back(name, duration, type, attendees, wasteForecast);

        cout << "\nPlanned Event: " << name << "\n";
        cout << "  Type: " << type << "\n";
        cout << "  Duration: " << fixed << setprecision(1) << duration << " hours\n";
        cout << "  Attendees: " << attendees << "\n";
        cout << "  Waste Forecast: " << fixed << setprecision(2) << wasteForecast << " kg\n";
        cout << "  Waste Categories: ";
        for (const auto &category : wasteCategories[type])
        {
            cout << category << " ";
        }
        cout << "\n";
    }

    void displayEvents()
    {
        if (events.empty())
        {
            cout << "\nNo events planned yet.\n";
            return;
        }

        cout << "\nPlanned Events:\n";
        for (const auto &event : events)
        {
            cout << "  Name: " << event.name << "\n"
                 << "    Duration: " << fixed << setprecision(1) << event.duration << " hours\n"
                 << "    Type: " << event.type << "\n"
                 << "    Attendees: " << event.attendees << "\n"
                 << "    Waste Forecast: " << fixed << setprecision(2) << event.wasteForecast << " kg\n";
        }
    }

    void displayWasteBreakdown()
    {
        if (events.empty())
        {
            cout << "\nNo events to calculate waste breakdown.\n";
            return;
        }

        cout << "\nWaste Breakdown:\n";
        for (const auto &event : events)
        {
            cout << "Event: " << event.name << "\n";
            double categoryWaste = event.wasteForecast / wasteCategories[event.type].size();
            for (const auto &category : wasteCategories[event.type])
            {
                cout << "  " << category << " Waste: " << fixed << setprecision(2)
                     << categoryWaste << " kg\n";
            }
            cout << "\n";
        }
    }
};
//-------------------------------------------------------------------------------------------------------

// Hazardous Waste Management System----------------------------------------------------------------------
struct WasteCategory
{
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
    {"E-WASTE", "Electronic equipment, circuit boards", 4, {"Component separation and recycling", "Precious metal recovery and safe disposal"}}};

void logAction(const string &action)
{
    ofstream logFile("hazardous_waste_log.txt", ios::app);
    if (logFile.is_open())
    {
        time_t now = time(nullptr);
        logFile << ctime(&now) << " - " << action << "\n";
        logFile.close();
    }
}

struct HazardousItem
{
    string name;
    string category;
    string disposalMethod;
    int riskLevel;

    HazardousItem(string n, string c, string d, int r)
        : name(n), category(c), disposalMethod(d), riskLevel(r) {}
};

struct CompareRisk
{
    bool operator()(const HazardousItem &a, const HazardousItem &b)
    {
        return a.riskLevel < b.riskLevel;
    }
};

class Hazardous_System
{
private:
    unordered_map<string, string> disposalMethods;
    unordered_map<string, int> itemInventory;
    priority_queue<HazardousItem, vector<HazardousItem>, CompareRisk> disposalQueue;

    void saveDisposalMethodsToFile()
    {
        ofstream outFile("disposal_methods.txt");
        if (outFile.is_open())
        {
            for (const auto &pair : disposalMethods)
            {
                outFile << pair.first << "," << pair.second << "\n";
            }
            outFile.close();
        }
    }

    void loadDisposalMethodsFromFile()
    {
        ifstream inFile("disposal_methods.txt");
        if (inFile.is_open())
        {
            string line;
            while (getline(inFile, line))
            {
                size_t pos = line.find(',');
                string item = line.substr(0, pos);
                string method = line.substr(pos + 1);
                disposalMethods[item] = method;
            }
            inFile.close();
        }
    }

    void saveInventoryToFile()
    {
        ofstream outFile("inventory.txt");
        if (outFile.is_open())
        {
            for (const auto &pair : itemInventory)
            {
                outFile << pair.first << "," << pair.second << "\n";
            }
            outFile.close();
        }
    }

    void loadInventoryFromFile()
    {
        ifstream inFile("inventory.txt");
        if (inFile.is_open())
        {
            string line;
            while (getline(inFile, line))
            {
                size_t pos = line.find(',');
                string item = line.substr(0, pos);
                int count = stoi(line.substr(pos + 1));
                itemInventory[item] = count;
            }
            inFile.close();
        }
    }

    void saveDisposalQueueToFile()
    {
        ofstream outFile("disposal_queue.txt");
        if (outFile.is_open())
        {
            auto tempQueue = disposalQueue;
            while (!tempQueue.empty())
            {
                HazardousItem item = tempQueue.top();
                tempQueue.pop();
                outFile << item.name << "," << item.category << "," << item.disposalMethod << "," << item.riskLevel << "\n";
            }
            outFile.close();
        }
    }

    void loadDisposalQueueFromFile()
    {
        ifstream inFile("disposal_queue.txt");
        if (inFile.is_open())
        {
            string line;
            while (getline(inFile, line))
            {
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
        }
    }

public:
    Hazardous_System()
    {
        loadDisposalMethodsFromFile();
        loadInventoryFromFile();
        loadDisposalQueueFromFile();
    }

    ~Hazardous_System()
    {
        saveDisposalMethodsToFile();
        saveInventoryToFile();
        saveDisposalQueueToFile();
    }

    void displayWasteCategories()
    {
        cout << "\nPredefined Hazardous Waste Categories:\n";
        cout << string(100, '=') << "\n";
        cout << setw(15) << left << "Category"
             << setw(45) << left << "| Description"
             << setw(15) << left << "| Risk Level"
             << "| Recommended Disposal Methods\n";
        cout << string(100, '=') << "\n";

        for (const auto &category : WASTE_CATEGORIES)
        {
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

    vector<string> getRecommendedDisposalMethods(const string &category)
    {
        for (const auto &cat : WASTE_CATEGORIES)
        {
            if (cat.name == category)
            {
                return cat.disposalMethods;
            }
        }
        return vector<string>();
    }

    int getDefaultRiskLevel(const string &category)
    {
        for (const auto &cat : WASTE_CATEGORIES)
        {
            if (cat.name == category)
            {
                return cat.defaultRiskLevel;
            }
        }
        return 5;
    }

    void addDisposalMethod(const string &item, const string &method)
    {
        disposalMethods[item] = method;
        cout << "hazardous: Added disposal method for " << item << " -> " << method << "\n";
        logAction("Added disposal method for " + item + " -> " + method);
    }

    void updateInventory(const string &item, int count)
    {
        itemInventory[item] += count;
        cout << "hazardous: Updated inventory for " << item << " by " << count << " units. Total: " << itemInventory[item] << "\n";
        logAction("Updated inventory for " + item + " by " + to_string(count) + " units");
    }

    void displayDisposalMethods()
    {
        cout << "\nDisposal Methods:\n";
        for (const auto &pair : disposalMethods)
        {
            cout << "Item: " << pair.first << " -> Method: " << pair.second << "\n";
        }
    }

    void displayInventory()
    {
        cout << "\nItem Inventory:\n";
        for (const auto &pair : itemInventory)
        {
            cout << "Item: " << pair.first << " -> Quantity: " << pair.second << "\n";
        }
    }

    void enqueueHazardousItem(const string &name, const string &category, int riskLevel)
    {
        string disposalMethod = disposalMethods[name];
        if (disposalMethod.empty())
        {
            auto recommendedMethods = getRecommendedDisposalMethods(category);
            if (!recommendedMethods.empty())
            {
                cout << "\nRecommended disposal methods for " << category << ":\n";
                for (size_t i = 0; i < recommendedMethods.size(); ++i)
                {
                    cout << i + 1 << ". " << recommendedMethods[i] << "\n";
                }
            }
        }
        disposalQueue.emplace(name, category, disposalMethod, riskLevel);
        cout << "hazardous: Enqueued hazardous item: " << name << " with risk level: " << riskLevel << "\n";
        logAction("Enqueued hazardous item: " + name + " with risk level: " + to_string(riskLevel));
    }

    void viewDisposalQueue()
    {
        cout << "\nPending Disposal Queue:\n";
        auto tempQueue = disposalQueue;
        while (!tempQueue.empty())
        {
            HazardousItem item = tempQueue.top();
            tempQueue.pop();
            cout << "Item: " << item.name
                 << " | Category: " << item.category
                 << " | Risk Level: " << item.riskLevel << "\n";
        }
    }
};
//-------------------------------------------------------------------------------------------------------

// E-Waste Management System------------------------------------------------------------------------------
// E-Waste Collection Center Structure
class CollectionCenter
{
public:
    string id;
    string location;
    float eWasteQuantity; // Changed to float

    CollectionCenter(string id, string loc, float qty) // Changed parameter type
        : id(id), location(loc), eWasteQuantity(qty)
    {
    }
};

// AVL Node for Collection Center
class AVLNode
{
public:
    CollectionCenter center;
    AVLNode *left;
    AVLNode *right;
    int height;

    AVLNode(CollectionCenter c)
        : center(c), left(nullptr), right(nullptr), height(1) {}
};

// AVL Tree for Managing Collection Centers
class AVLTree
{
private:
    AVLNode *root;
    set<pair<string, string>> centerKeys; // To track (ID, Location) pairs

    int getHeight(AVLNode *node)
    {
        return node ? node->height : 0;
    }

    int getBalance(AVLNode *node)
    {
        return node ? getHeight(node->left) - getHeight(node->right) : 0;
    }

    AVLNode *rotateRight(AVLNode *y)
    {
        AVLNode *x = y->left;
        AVLNode *T2 = x->right;

        x->right = y;
        y->left = T2;

        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;

        return x;
    }

    AVLNode *rotateLeft(AVLNode *x)
    {
        AVLNode *y = x->right;
        AVLNode *T2 = y->left;

        y->left = x;
        x->right = T2;

        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;

        return y;
    }

    AVLNode *insert(AVLNode *node, CollectionCenter center)
    {
        if (!node)
        {
            return new AVLNode(center);
        }

        if (center.id < node->center.id)
        {
            node->left = insert(node->left, center);
        }
        else if (center.id > node->center.id || (center.id == node->center.id && center.location != node->center.location))
        {
            node->right = insert(node->right, center);
        }
        else
        {
            return node;
        }

        node->height = max(getHeight(node->left), getHeight(node->right)) + 1;

        int balance = getBalance(node);

        if (balance > 1 && center.id < node->left->center.id)
        {
            return rotateRight(node);
        }

        if (balance < -1 && center.id > node->right->center.id)
        {
            return rotateLeft(node);
        }

        if (balance > 1 && center.id > node->left->center.id)
        {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }

        if (balance < -1 && center.id < node->right->center.id)
        {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    AVLNode *getMinValueNode(AVLNode *node)
    {
        AVLNode *current = node;
        while (current && current->left)
        {
            current = current->left;
        }
        return current;
    }

    AVLNode *deleteNode(AVLNode *root, string id)
    {
        if (!root)
        {
            return root;
        }

        if (id < root->center.id)
        {
            root->left = deleteNode(root->left, id);
        }
        else if (id > root->center.id)
        {
            root->right = deleteNode(root->right, id);
        }
        else
        {
            centerKeys.erase({root->center.id, root->center.location});

            if (!root->left || !root->right)
            {
                AVLNode *temp = root->left ? root->left : root->right;
                if (!temp)
                {
                    temp = root;
                    root = nullptr;
                }
                else
                {
                    *root = *temp;
                }
                delete temp;
            }
            else
            {
                AVLNode *temp = getMinValueNode(root->right);
                root->center = temp->center;
                root->right = deleteNode(root->right, temp->center.id);
            }
        }

        if (!root)
        {
            return root;
        }

        root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
        int balance = getBalance(root);

        if (balance > 1 && getBalance(root->left) >= 0)
        {
            return rotateRight(root);
        }
        if (balance > 1 && getBalance(root->left) < 0)
        {
            root->left = rotateLeft(root->left);
            return rotateRight(root);
        }
        if (balance < -1 && getBalance(root->right) <= 0)
        {
            return rotateLeft(root);
        }
        if (balance < -1 && getBalance(root->right) > 0)
        {
            root->right = rotateRight(root->right);
            return rotateLeft(root);
        }

        return root;
    }

    void inOrderTraversal(AVLNode *node)
    {
        if (node)
        {
            inOrderTraversal(node->left);
            cout << "Center ID: " << node->center.id << ", Location: " << node->center.location
                 << ", E-Waste Quantity: " << node->center.eWasteQuantity << " kg\n";
            inOrderTraversal(node->right);
        }
    }

public:
    AVLTree() : root(nullptr)
    {
        loadCentersFromFile();
    }

    ~AVLTree()
    {
        saveCentersToFile();
    }

    bool addCenter(CollectionCenter center)
    {
        auto key = make_pair(center.id, center.location);
        if (centerKeys.find(key) != centerKeys.end())
        {
            AVLNode *current = root;
            while (current)
            {
                if (current->center.id == center.id && current->center.location == center.location)
                {
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

    void removeCenter(string id)
    {
        if (centerKeys.find({id, ""}) == centerKeys.end())
        {
            cout << "Error: Center ID does not exist.\n";
            return;
        }
        root = deleteNode(root, id);
    }

    void displayCenters()
    {
        if (!root)
        {
            cout << "No collection centers available.\n";
            return;
        }
        inOrderTraversal(root);
    }

    bool modifyCenter(string id, string newLocation, float newQuantity)
    {
        AVLNode *current = root;
        while (current)
        {
            if (current->center.id == id)
            {
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

    void saveCentersToFile()
    {
        ofstream outFile("centers.txt");
        if (outFile.is_open())
        {
            saveCenters(outFile, root);
            outFile.close();
        }
    }

    void loadCentersFromFile()
    {
        ifstream inFile("centers.txt");
        if (inFile.is_open())
        {
            string id, location;
            float quantity;
            while (inFile >> id >> location >> quantity)
            {
                CollectionCenter center(id, location, quantity);
                addCenter(center);
            }
            inFile.close();
        }
    }

private:
    void saveCenters(ofstream &outFile, AVLNode *node)
    {
        if (node)
        {
            saveCenters(outFile, node->left);
            outFile << node->center.id << " " << node->center.location << " " << node->center.eWasteQuantity << "\n";
            saveCenters(outFile, node->right);
        }
    }
};

// Graph for Routes between Centers
class Graph_E_Waste
{
private:
    map<string, vector<pair<string, double>>> adjList;
    map<string, string> locationMap; // Maps ID to Location

public:
    void addEdge(string u, string v, double weight)
    {
        if (u == v)
        {
            return; // Ignore routes where the two centers are the same
        }
        adjList[u].push_back({v, weight});
        adjList[v].push_back({u, weight});
        saveRoutesToFile();
    }

    void displayGraph_E_Waste()
    {
        for (const auto &node : adjList)
        {
            cout << "Location " << locationMap[node.first] << " is connected to:\n";
            for (const auto &neighbor : node.second)
            {
                cout << "  -> " << locationMap[neighbor.first] << " (Distance: " << neighbor.second << " km)\n";
            }
        }
    }

    void shortestPath(string start, string end)
    {
        map<string, double> dist;
        map<string, string> parent;
        map<string, bool> visited;

        for (auto &node : adjList)
        {
            dist[node.first] = numeric_limits<double>::max();
            visited[node.first] = false;
        }
        dist[start] = 0;

        for (size_t i = 0; i < adjList.size(); ++i)
        {
            string minNode;
            double minDist = numeric_limits<double>::max();

            for (auto &node : dist)
            {
                if (!visited[node.first] && node.second < minDist)
                {
                    minDist = node.second;
                    minNode = node.first;
                }
            }

            if (minNode.empty())
            {
                break;
            }
            visited[minNode] = true;

            for (auto &neighbor : adjList[minNode])
            {
                if (dist[minNode] + neighbor.second < dist[neighbor.first])
                {
                    dist[neighbor.first] = dist[minNode] + neighbor.second;
                    parent[neighbor.first] = minNode;
                }
            }
        }

        if (dist[end] == numeric_limits<double>::max())
        {
            cout << "No path exists from " << locationMap[start] << " to " << locationMap[end] << ".\n";
            return;
        }

        cout << "Shortest path from " << locationMap[start] << " to " << locationMap[end] << ": ";
        string current = end;
        vector<string> path;
        while (current != start)
        {
            path.push_back(current);
            current = parent[current];
        }
        path.push_back(start);
        reverse(path.begin(), path.end());
        for (const auto &node : path)
        {
            cout << locationMap[node] << " ";
        }
        cout << "\nTotal Distance: " << dist[end] << " km\n";
    }

    void modifyRoute(string u, string v, double newWeight)
    {
        bool routeFound = false;
        for (auto &neighbor : adjList[u])
        {
            if (neighbor.first == v)
            {
                neighbor.second = newWeight;
                routeFound = true;
                break;
            }
        }
        for (auto &neighbor : adjList[v])
        {
            if (neighbor.first == u)
            {
                neighbor.second = newWeight;
                break;
            }
        }
        if (routeFound)
        {
            cout << "Route between " << locationMap[u] << " and " << locationMap[v] << " updated successfully.\n";
            saveRoutesToFile();
        }
        else
        {
            cout << "Error: Route not found.\n";
        }
    }

    void addLocation(string id, string location)
    {
        locationMap[id] = location;
    }

    void saveRoutesToFile()
    {
        ofstream outFile("routes.txt");
        if (outFile.is_open())
        {
            for (const auto &node : adjList)
            {
                for (const auto &neighbor : node.second)
                {
                    outFile << node.first << " " << neighbor.first << " " << neighbor.second << "\n";
                }
            }
            outFile.close();
        }
    }

    void loadRoutesFromFile()
    {
        ifstream inFile("routes.txt");
        if (inFile.is_open())
        {
            string u, v;
            double weight;
            while (inFile >> u >> v >> weight)
            {
                addEdge(u, v, weight);
            }
            inFile.close();
        }
    }
};
//-------------------------------------------------------------------------------------------------------

// Citizen Complaint and Feedback System------------------------------------------------------------------
// Struct for Complaint Details
struct Complaint
{
    int priority;
    string citizenName;
    string area;
    string description;
    string status;          // Pending, In Progress, or Resolved
    string resolvedByAdmin; // Admin who resolved the complaint

    bool operator<(const Complaint &other) const
    {
        return priority < other.priority; // Higher priority complaints come first
    }
};

// Complaint Priority Class
class ComplaintPriority
{
private:
    map<string, int> priorityPatterns;

public:
    ComplaintPriority()
    {
        // Priority patterns with weights (higher number = higher priority)
        priorityPatterns = {
            // Critical - Priority 5
            {"(urgent|emergency|critical|immediate|asap|life.?threatening)", 5},
            {"(security breach|data leak|system.?down|outage)", 5},
            {"(fraud|theft|stolen|hack)", 5},

            // High - Priority 4
            {"(payment|billing|charge|refund|money)", 4},
            {"(error|bug|crash|failure|broken)", 4},
            {"(account.?locked|cannot.?access|locked.?out)", 4},

            // Medium - Priority 3
            {"(slow|performance|delay|waiting)", 3},
            {"(incorrect|wrong|invalid|mistake)", 3},
            {"(update|upgrade|change|modify)", 3},

            // Low - Priority 2
            {"(question|help|assistance|support|how.?to)", 2},
            {"(feature|request|suggestion|enhance)", 2},
            {"(document|instruction|guide|manual)", 2},

            // Very Low - Priority 1
            {"(feedback|comment|review)", 1},
            {"(thank|appreciate|kudos)", 1}};
    }

    int getPriority(const string &complaintText)
    {
        string text = complaintText;
        transform(text.begin(), text.end(), text.begin(), ::tolower);

        int maxPriority = 1; // Default priority is 1 (Very Low)
        for (const auto &pattern : priorityPatterns)
        {
            if (regex_search(text, regex(pattern.first)))
            {
                maxPriority = max(maxPriority, pattern.second);
            }
        }
        return maxPriority;
    }
};

// Complaint System Class
class ComplaintSystem
{
private:
    map<string, priority_queue<Complaint>> areaComplaints; // Categorized by area
    map<string, string> userComplaints;                    // Track user complaints
    vector<Complaint> resolvedComplaints;                  // Resolved complaints for stats
    map<string, int> adminResolvedCount;                   // Track number of complaints resolved by each admin
    int totalComplaintsSubmitted;
    int totalComplaintsResolved;

    bool validateArea(const string &area)
    {
        if (areaComplaints.find(area) == areaComplaints.end() || areaComplaints[area].empty())
        {
            cout << "\nInvalid area or no complaints in the area.\n";
            return false;
        }
        return true;
    }

    void displayComplaint(const Complaint &complaint)
    {
        cout << "\n----------------------------------------\n";
        cout << "Priority: " << complaint.priority << "\n"
             << "Citizen: " << complaint.citizenName << "\n"
             << "Area: " << complaint.area << "\n"
             << "Status: " << complaint.status << "\n"
             << "Description: " << complaint.description << "\n";
        if (complaint.status == "Resolved")
        {
            cout << "Resolved by Admin: " << complaint.resolvedByAdmin << "\n";
        }
        cout << "----------------------------------------\n";
    }

    void saveComplaintsToFile()
    {
        ofstream outFile("complaints.txt");
        if (outFile.is_open())
        {
            outFile << totalComplaintsSubmitted << " " << totalComplaintsResolved << "\n";
            for (const auto &entry : areaComplaints)
            {
                const auto &area = entry.first;
                const auto &complaints = entry.second;
                priority_queue<Complaint> tempQueue = complaints;

                while (!tempQueue.empty())
                {
                    const Complaint &complaint = tempQueue.top();
                    outFile << complaint.priority << " " << complaint.citizenName << " "
                            << complaint.area << " " << complaint.description << " "
                            << complaint.status << " " << complaint.resolvedByAdmin << "\n";
                    tempQueue.pop();
                }
            }
            outFile.close();
        }
        else
        {
            cerr << "Unable to open file for saving complaints.\n";
        }

        ofstream adminOutFile("admin_resolved.txt");
        if (adminOutFile.is_open())
        {
            for (const auto &entry : adminResolvedCount)
            {
                adminOutFile << entry.first << " " << entry.second << "\n";
            }
            adminOutFile.close();
        }
    }

    void loadComplaintsFromFile()
    {
        ifstream inFile("complaints.txt");
        if (inFile.is_open())
        {
            inFile >> totalComplaintsSubmitted >> totalComplaintsResolved;
            inFile.ignore();
            while (!inFile.eof())
            {
                Complaint complaint;
                inFile >> complaint.priority;
                inFile.ignore();
                getline(inFile, complaint.citizenName, ' ');
                getline(inFile, complaint.area, ' ');
                getline(inFile, complaint.description, ' ');
                getline(inFile, complaint.status, ' ');
                getline(inFile, complaint.resolvedByAdmin);
                if (!inFile.fail())
                {
                    if (complaint.status == "Resolved")
                    {
                        resolvedComplaints.push_back(complaint);
                    }
                    else
                    {
                        areaComplaints[complaint.area].push(complaint);
                        userComplaints[complaint.citizenName] = complaint.area;
                    }
                }
            }
            inFile.close();
        }

        ifstream adminInFile("admin_resolved.txt");
        if (adminInFile.is_open())
        {
            string adminName;
            int resolvedCount;
            while (adminInFile >> adminName >> resolvedCount)
            {
                adminResolvedCount[adminName] = resolvedCount;
            }
            adminInFile.close();
        }
    }

public:
    ComplaintSystem() : totalComplaintsSubmitted(0), totalComplaintsResolved(0)
    {
        loadComplaintsFromFile();
    }

    ~ComplaintSystem()
    {
        saveComplaintsToFile();
    }

    void viewResolvedComplaints()
    {
        cout << "\nLast 5 Resolved Complaints:\n";
        if (resolvedComplaints.empty())
        {
            cout << "No resolved complaints yet.\n";
            return;
        }

        int count = 0;
        for (auto it = resolvedComplaints.rbegin(); it != resolvedComplaints.rend() && count < 5; ++it, ++count)
        {
            displayComplaint(*it);
        }
    }

    void submitComplaint(const string &citizenName, const string &area, string description, ComplaintPriority &prioritySystem)
    {
        // Convert description to lowercase
        transform(description.begin(), description.end(), description.begin(), ::tolower);

        if (userComplaints.find(citizenName) != userComplaints.end())
        {
            cout << "\nYou have already registered a complaint. Only one complaint per user is allowed.\n";
            return;
        }

        int priority = prioritySystem.getPriority(description);
        Complaint newComplaint = {priority, citizenName, area, description, "Pending", ""};
        areaComplaints[area].push(newComplaint);
        userComplaints[citizenName] = area;
        totalComplaintsSubmitted++;
        saveComplaintsToFile();

        cout << "\nComplaint submitted successfully in area: " << area << " with priority: " << priority << "!\n";
    }

    void resolveComplaint(const string &area, const string &adminName)
    {
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
        saveComplaintsToFile();

        cout << "\nResolving complaint in area: " << area << ":\n";
        displayComplaint(topComplaint);
    }

    void updateComplaintStatus(const string &citizenName, const string &newStatus)
    {
        auto it = userComplaints.find(citizenName);
        if (it == userComplaints.end())
        {
            cout << "\nNo complaint found for citizen: " << citizenName << "\n";
            return;
        }

        string area = it->second;
        priority_queue<Complaint> tempQueue;
        bool updated = false;

        while (!areaComplaints[area].empty())
        {
            Complaint complaint = areaComplaints[area].top();
            areaComplaints[area].pop();

            if (complaint.citizenName == citizenName)
            {
                complaint.status = newStatus;
                updated = true;
            }

            tempQueue.push(complaint);
        }

        areaComplaints[area] = tempQueue;

        if (updated)
        {
            cout << "\nStatus updated successfully for citizen: " << citizenName << "\n";
            saveComplaintsToFile();
        }
        else
        {
            cout << "\nFailed to update status.\n";
        }
    }

    void viewComplaints(const string &area)
    {
        if (!validateArea(area))
            return;

        priority_queue<Complaint> tempQueue = areaComplaints[area];
        cout << "\nComplaints in area: " << area << ":\n";

        while (!tempQueue.empty())
        {
            Complaint complaint = tempQueue.top();
            tempQueue.pop();
            displayComplaint(complaint);
        }
    }

    void viewSummary()
    {
        cout << "\nSystem Summary:\n";
        cout << "----------------------------------------\n";
        cout << "Total Complaints Submitted: " << totalComplaintsSubmitted << "\n";
        cout << "Total Complaints Resolved: " << totalComplaintsResolved << "\n";
        cout << "Pending Complaints: " << totalComplaintsSubmitted - totalComplaintsResolved << "\n";
        cout << "----------------------------------------\n";
    }

    void viewMonthlyStats()
    {
        cout << "\nEnd-of-Month Statistics:\n";
        cout << "----------------------------------------\n";
        map<string, int> areaStats;
        for (const auto &complaint : resolvedComplaints)
        {
            areaStats[complaint.area]++;
        }

        cout << "Complaints Resolved by Area:\n";
        for (const auto &entry : areaStats)
        {
            cout << "Area: " << entry.first << ", Resolved: " << entry.second << "\n";
        }
        cout << "----------------------------------------\n";
    }

    void displayAllActiveComplaints()
    {
        cout << "\nActive Complaints:\n";
        cout << "----------------------------------------\n";
        bool hasActiveComplaints = false;

        for (const auto &entry : areaComplaints)
        {
            const string &area = entry.first;
            const auto &complaints = entry.second;

            if (!complaints.empty())
            {
                hasActiveComplaints = true;
                priority_queue<Complaint> tempQueue = complaints;
                while (!tempQueue.empty())
                {
                    const Complaint &complaint = tempQueue.top();
                    displayComplaint(complaint);
                    tempQueue.pop();
                }
            }
        }

        if (!hasActiveComplaints)
        {
            cout << "No active complaints\n";
        }
        cout << "----------------------------------------\n";
    }

    // Getter method to access adminResolvedCount
    const map<string, int> &getAdminResolvedCount() const
    {
        return adminResolvedCount;
    }
};
//-------------------------------------------------------------------------------------------------------

// Carbon Footprint Estimator System----------------------------------------------------------------------
const int MAX_CITIES = 100;
const double NO_PATH = -1;

struct Edge_Carbon
{
    string source;
    string target;
    double weight;
    double carbonEmission;
};

struct CityEmission
{
    string cityName;
    double totalEmission;
};

class Carbon_Footprint
{
private:
    int nodes;
    bool isInitialized;
    string cityNames[MAX_CITIES];
    int adjacencyList[MAX_CITIES][MAX_CITIES];
    vector<Edge_Carbon> edges;
    vector<CityEmission> cityEmissions;

    double calculateCarbonEmission(double distance, string wasteType)
    {
        double baseFactor = 2.6;
        double wasteMultiplier = 1.0;
        if (wasteType == "Plastic")
            wasteMultiplier = 1.2;
        else if (wasteType == "Metal")
            wasteMultiplier = 1.5;
        else if (wasteType == "Organic")
            wasteMultiplier = 0.8;
        else if (wasteType == "Electronic")
            wasteMultiplier = 1.7;
        return distance * baseFactor * wasteMultiplier;
    }

public:
    Carbon_Footprint() : nodes(0), isInitialized(false)
    {
        for (int i = 0; i < MAX_CITIES; i++)
        {
            for (int j = 0; j < MAX_CITIES; j++)
            {
                adjacencyList[i][j] = -1;
            }
        }
    }

    void initializeSystem()
    {
        if (!isInitialized)
        {
            cout << "Enter number of cities: ";
            cin >> nodes;

            cout << "Enter the names of the cities:\n";
            for (int i = 0; i < nodes; i++)
            {
                string cityName;
                cout << "City " << i + 1 << ": ";
                cin >> cityName;
                cityNames[i] = cityName;
                cityEmissions.push_back({cityName, 0.0});
            }
            isInitialized = true;
            saveSystemState();
        }
    }

    void addRoute(const string &source, const string &target, double distance, string wasteType)
    {
        int srcIndex = getCityIndex(source);
        int tgtIndex = getCityIndex(target);
        double emission = calculateCarbonEmission(distance, wasteType);

        edges.push_back({source, target, distance, emission});
        adjacencyList[srcIndex][tgtIndex] = 1;

        cityEmissions[srcIndex].totalEmission += emission;
        cityEmissions[tgtIndex].totalEmission += emission;

        saveSystemState();
    }

    void displayRoutes()
    {
        if (edges.empty())
        {
            cout << "\nNo routes available.\n";
            return;
        }

        cout << "\nRoute Network:\n";
        cout << "----------------------------------------\n";
        for (const Edge_Carbon &edge : edges)
        {
            cout << "Route: " << edge.source << " -> " << edge.target << "\n";
            cout << "Distance: " << edge.weight << " km\n";
            cout << "Carbon Emission: " << edge.carbonEmission << " kg CO2\n";
            cout << "----------------------------------------\n";
        }
    }

    void displayEmissionsByCity()
    {
        if (cityEmissions.empty())
        {
            cout << "\nNo emission data available.\n";
            return;
        }

        sort(cityEmissions.begin(), cityEmissions.end(),
             [](const CityEmission &a, const CityEmission &b)
             {
                 return a.totalEmission > b.totalEmission;
             });

        cout << "\nCarbon Emissions by City (Sorted):\n";
        cout << "----------------------------------------\n";
        for (const CityEmission &city : cityEmissions)
        {
            if (city.totalEmission > 0)
            {
                cout << "City: " << city.cityName << "\n";
                cout << "Total Emissions: " << city.totalEmission << " kg CO2\n";
                cout << "----------------------------------------\n";
            }
        }
    }

    vector<double> calculateShortestPaths(const string &startCity)
    {
        int startIndex = getCityIndex(startCity);
        vector<double> distances(nodes, NO_PATH);
        distances[startIndex] = 0;

        for (int i = 0; i < nodes - 1; i++)
        {
            for (const Edge_Carbon &edge : edges)
            {
                int srcIndex = getCityIndex(edge.source);
                int tgtIndex = getCityIndex(edge.target);
                if (distances[srcIndex] != NO_PATH &&
                    (distances[tgtIndex] == NO_PATH ||
                     distances[srcIndex] + edge.weight < distances[tgtIndex]))
                {
                    distances[tgtIndex] = distances[srcIndex] + edge.weight;
                }
            }
        }

        return distances;
    }

    int getCityIndex(const string &cityName)
    {
        for (int i = 0; i < nodes; i++)
        {
            if (cityNames[i] == cityName)
                return i;
        }
        return -1;
    }

    string getCityName(int index)
    {
        return cityNames[index];
    }

    int getNumCities()
    {
        return nodes;
    }

    bool isSystemInitialized()
    {
        return isInitialized;
    }

    void saveSystemState()
    {
        ofstream outFile("estimator_state.txt");
        if (outFile.is_open())
        {
            outFile << nodes << "\n";
            for (int i = 0; i < nodes; i++)
            {
                outFile << cityNames[i] << "\n";
            }
            for (const Edge_Carbon &edge : edges)
            {
                outFile << edge.source << " " << edge.target << " " << edge.weight << " " << edge.carbonEmission << "\n";
            }
            outFile.close();
        }
    }

    void loadSystemState()
    {
        ifstream inFile("estimator_state.txt");
        if (inFile.is_open())
        {
            inFile >> nodes;
            inFile.ignore();
            for (int i = 0; i < nodes; i++)
            {
                getline(inFile, cityNames[i]);
                cityEmissions.push_back({cityNames[i], 0.0});
            }
            Edge_Carbon edge;
            while (inFile >> edge.source >> edge.target >> edge.weight >> edge.carbonEmission)
            {
                edges.push_back(edge);
                int srcIndex = getCityIndex(edge.source);
                int tgtIndex = getCityIndex(edge.target);
                adjacencyList[srcIndex][tgtIndex] = 1;
                cityEmissions[srcIndex].totalEmission += edge.carbonEmission;
                cityEmissions[tgtIndex].totalEmission += edge.carbonEmission;
            }
            inFile.close();
            isInitialized = true;
        }
    }
};
//-------------------------------------------------------------------------------------------------------

// Plastic Waste Tracking System--------------------------------------------------------------------------
//  Structure to store waste records
struct WasteRecord
{
    string source;
    double quantity;
    string status;
};

class WasteTrackingSystem
{
private:
    vector<WasteRecord> records;

    // Function to save records to a file
    void saveRecordsToFile()
    {
        ofstream outFile("waste_records.txt");
        if (outFile.is_open())
        {
            for (const auto &record : records)
            {
                outFile << record.source << "," << record.quantity << "," << record.status << "\n";
            }
            outFile.close();
        }
    }

    // Function to load records from a file
    void loadRecordsFromFile()
    {
        ifstream inFile("waste_records.txt");
        if (inFile.is_open())
        {
            string line;
            while (getline(inFile, line))
            {
                size_t pos1 = line.find(',');
                size_t pos2 = line.find(',', pos1 + 1);
                string source = line.substr(0, pos1);
                double quantity = stod(line.substr(pos1 + 1, pos2 - pos1 - 1));
                string status = line.substr(pos2 + 1);
                records.push_back({source, quantity, status});
            }
            inFile.close();
        }
    }

public:
    // Function to add a waste record
    void addWasteRecord()
    {
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
    void displayRecords()
    {
        if (records.empty())
        {
            cout << "\nNo records to display.\n";
            return;
        }

        // Map to store aggregated records (source+status -> total quantity)
        map<pair<string, string>, double> aggregatedRecords;

        // Aggregate quantities for same source and status
        for (const auto &record : records)
        {
            pair<string, string> key = {record.source, record.status};
            aggregatedRecords[key] += record.quantity;
        }

        // Display records grouped by source and status
        cout << "\nDisplaying aggregated waste records:\n";
        cout << "--------------------------------\n";

        string currentSource = "";
        for (const auto &record : aggregatedRecords)
        {
            // Print source header if it's a new source
            if (currentSource != record.first.first)
            {
                currentSource = record.first.first;
                cout << "\nSource: " << currentSource << "\n";
                cout << "--------------------------------\n";
            }

            cout << "Status: " << record.first.second << "\n";
            cout << "Total Quantity: " << record.second << " kg\n";
            cout << "--------------------------------\n";
        }
    }

    WasteTrackingSystem()
    {
        loadRecordsFromFile();
    }
};
//-------------------------------------------------------------------------------------------------------

// Waste Management Quiz----------------------------------------------------------------------------------
//  Structure for quiz questions
struct Quiz
{
    string question;
    string answer;
};

class QuizSystem
{
private:
    // Quiz questions and answers related to Waste Management
    Quiz quizBank[50] = {
        {"What is the process of turning food waste into fertilizer called?", "composting"},
        {"Which bin color is used for recyclable waste?", "blue"},
        {"What material are beverage cans typically made of?", "aluminum"},
        {"Which waste type includes vegetable peels and fruit skins?", "organic"},
        {"What is the term for non-recyclable waste?", "garbage"},
        {"What type of waste includes old newspapers and magazines?", "paper"},
        {"What process separates waste into different categories?", "segregation"},
        {"What is the term for reusing materials to make new products?", "recycling"},
        {"Which bin color is used for general waste?", "green"},
        {"What material is a glass bottle made of?", "glass"},
        {"What is the term for reducing waste by using less packaging?", "source reduction"},
        {"Which type of waste includes batteries and electronics?", "e-waste"},
        {"What is the process of burning waste to generate energy called?", "incineration"},
        {"What type of waste includes paint and chemicals?", "hazardous"},
        {"Which bin color is used for compostable waste?", "brown"},
        {"What is the term for the decay of organic matter by microorganisms?", "decomposition"},
        {"Which material is commonly recycled to make new paper?", "cardboard"},
        {"What type of waste includes plastic bottles and containers?", "plastic"},
        {"What is the term for waste that is biodegradable?", "compostable"},
        {"Which waste management method involves burying waste in the ground?", "landfilling"},
        {"What is the term for waste that cannot be broken down naturally?", "non-biodegradable"},
        {"What is the process of breaking down hazardous waste into less harmful substances?", "neutralization"},
        {"Which type of waste includes construction debris?", "construction and demolition waste"},
        {"What is the method of turning organic waste into liquid fertilizer called?", "vermicomposting"},
        {"Which waste type includes broken furniture and large items?", "bulky waste"},
        {"What is the method of processing waste to extract valuable materials?", "resource recovery"},
        {"What is the term for waste generated by factories and manufacturing units?", "industrial waste"},
        {"Which type of waste includes leftover food from hotels and restaurants?", "food waste"},
        {"What is the term for recovering energy from waste materials?", "waste-to-energy"},
        {"Which waste management method uses worms to break down organic matter?", "vermiculture"},
        {"What type of waste includes garden clippings and leaves?", "green waste"},
        {"Which color bin is commonly used for hazardous waste?", "red"},
        {"What is the term for minimizing waste at the source?", "waste prevention"},
        {"What is the term for using a product for a purpose other than its original use?", "upcycling"},
        {"Which type of waste includes broken electronics and gadgets?", "e-waste"},
        {"What is the process of shredding waste materials for easier handling called?", "compaction"},
        {"What is the term for separating recyclable materials from non-recyclables?", "material recovery"},
        {"What type of waste includes items like syringes and bandages?", "medical waste"},
        {"Which waste management practice involves turning waste into compost?", "composting"},
        {"What is the term for waste that is harmful to humans and the environment?", "toxic waste"},
        {"Which waste type includes old tires and rubber materials?", "rubber waste"},
        {"What is the method of treating wastewater to make it reusable called?", "wastewater treatment"},
        {"What type of waste includes leftover building materials?", "construction waste"},
        {"What is the term for materials like sand, bricks, and concrete debris?", "inert waste"},
        {"Which waste management practice involves using heat to decompose organic matter?", "pyrolysis"},
        {"What type of waste includes contaminated soil and chemicals?", "contaminated waste"},
        {"What is the term for items that can be reused without processing?", "reusable waste"},
        {"Which color bin is often used for recyclable plastics?", "yellow"},
        {"What type of waste includes fertilizers and pesticides?", "agricultural waste"},
        {"What is the term for waste produced by households?", "domestic waste"}};

public:
    QuizSystem()
    {
        // Seed the random number generator
        srand(time(0));
    }

    // Function to start the quiz
    void startQuiz()
    {
        cout << "\n--- Quiz Time! ---\n";

        // Randomly select a question
        int randomIndex = rand() % 50; // Select an index from 0 to 49
        string userAnswer;

        // Display the question
        cout << "Question: " << quizBank[randomIndex].question << endl;
        cout << "Your Answer: ";
        cin.ignore(); // Clear newline from previous input
        getline(cin, userAnswer);

        // Convert user input to lowercase for case-insensitive comparison
        transform(userAnswer.begin(), userAnswer.end(), userAnswer.begin(), ::tolower);

        if (userAnswer == quizBank[randomIndex].answer)
        {
            cout << "Correct!\n";
        }
        else
        {
            cout << "Incorrect. The correct answer is: " << quizBank[randomIndex].answer << endl;
        }
    }
};
//-------------------------------------------------------------------------------------------------------

//Food Waste Management System---------------------------------------------------------------------------
// Function to convert a string to lowercase
string to_lowercasee(const string &input)
{
    string lowerStr = input;
    transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

// Structure to represent a food donation
struct FoodDonation
{
    string donorName;
    string location;
    string foodType;
    int quantity;
    string status;

    FoodDonation(const string &donorName, const string &location, const string &foodType, int quantity, const string &status)
        : donorName(donorName), location(location), foodType(foodType), quantity(quantity), status(status) {}
};

// Function to log actions
void log_Action(const string &action)
{
    ofstream logFile("food_waste_log.txt", ios::app);
    if (logFile.is_open())
    {
        time_t now = time(nullptr);
        logFile << ctime(&now) << " - " << action << "\n";
        logFile.close();
    }
}

// Class to manage food waste donations
class FoodWasteManagementSystem
{
private:
    vector<FoodDonation> donations;

    void saveDonationsToFile()
    {
        ofstream outFile("food_donations.txt");
        if (outFile.is_open())
        {
            for (const auto &donation : donations)
            {
                outFile << donation.donorName << "," << donation.location << "," << donation.foodType << "," << donation.quantity << "," << donation.status << "\n";
            }
            outFile.close();
        }
    }

    void loadDonationsFromFile()
    {
        ifstream inFile("food_donations.txt");
        if (inFile.is_open())
        {
            string line;
            while (getline(inFile, line))
            {
                istringstream iss(line);
                string donorName, location, foodType, status, quantityStr;
                int quantity;

                getline(iss, donorName, ',');
                getline(iss, location, ',');
                getline(iss, foodType, ',');
                getline(iss, quantityStr, ',');
                getline(iss, status, ',');

                quantity = stoi(quantityStr);
                donations.emplace_back(to_lowercasee(donorName), to_lowercasee(location), to_lowercasee(foodType), quantity, to_lowercasee(status));
            }
            inFile.close();
        }
    }

public:
    FoodWasteManagementSystem()
    {
        loadDonationsFromFile();
    }

    ~FoodWasteManagementSystem()
    {
        saveDonationsToFile();
    }

    void addFoodDonation(const string &donorName, const string &location, const string &foodType, int quantity, const string &status)
    {
        donations.emplace_back(to_lowercasee(donorName), to_lowercasee(location), to_lowercasee(foodType), quantity, to_lowercasee(status));
        log_Action("Added food donation from " + donorName + " located at " + location);
        cout << "Donation added successfully!\n";
    }

    void deleteFoodDonation(const string &donorName, const string &location)
    {
        auto it = remove_if(donations.begin(), donations.end(), [&](const FoodDonation &donation)
                            { return donation.donorName == to_lowercasee(donorName) && donation.location == to_lowercasee(location); });

        if (it != donations.end())
        {
            log_Action("Deleted food donation from " + donorName + " located at " + location);
            donations.erase(it, donations.end());
            cout << "Donation deleted successfully!\n";
        }
        else
        {
            cout << "No matching donation found to delete.\n";
        }
    }

    void searchFoodDonationByLocation(const string &location)
    {
        vector<FoodDonation> result;
        string searchLocation = to_lowercasee(location);
        for (const auto &donation : donations)
        {
            if (donation.location == searchLocation)
            {
                result.push_back(donation);
            }
        }

        if (result.empty())
        {
            cout << "No donations found in the specified location.\n";
        }
        else
        {
            cout << "Donations in location " << location << ":\n";
            for (const auto &donation : result)
            {
                cout << "Donor Name: " << donation.donorName
                     << ", Food Type: " << donation.foodType
                     << ", Quantity: " << donation.quantity
                     << ", Status: " << donation.status << "\n";
            }
        }
    }

    void generateReport()
    {
        if (donations.empty())
        {
            cout << "No donations to generate report.\n";
            return;
        }

        map<string, int> locationCount;
        map<string, int> foodTypeCount;
        int totalPotentialWaste = 0;

        for (const auto &donation : donations)
        {
            locationCount[donation.location] += donation.quantity;
            foodTypeCount[donation.foodType] += donation.quantity;
            if (donation.status == "near expiry")
            {
                totalPotentialWaste += donation.quantity;
            }
        }

        cout << "Food Donation Report:\n";
        cout << "By Location:\n";
        for (const auto &entry : locationCount)
        {
            cout << "Location: " << entry.first << ", Total Quantity: " << entry.second << "\n";
        }

        cout << "By Food Type:\n";
        for (const auto &entry : foodTypeCount)
        {
            cout << "Food Type: " << entry.first << ", Total Quantity: " << entry.second << "\n";
        }

        cout << "Potential Food Waste (Near Expiry): " << totalPotentialWaste << " plates\n";
    }
};
//-------------------------------------------------------------------------------------------------------

// Unified Login System----------------------------------------------------------------------------------
// Class to manage admin login
class LoginSystem
{
private:
    struct Admin
    {
        string name;
        int password;
    };

    Admin admins[4];
    int adminCount;
    string loggedInAdmin;

    // Function to hide password input
    string getPassword()
    {
        string password;
        char ch;
        while ((ch = _getch()) != '\r') // '\r' is the Enter key
        {
            if (ch == '\b') // Backspace key
            {
                if (!password.empty())
                {
                    cout << "\b \b";
                    password.pop_back();
                }
            }
            else
            {
                cout << '*';
                password += ch;
            }
        }
        cout << endl;
        return password;
    }

public:
    LoginSystem()
    {
        adminCount = 4;
        admins[0] = {"adarsh", 12345};
        admins[1] = {"bhargavi", 54321};
        admins[2] = {"shreyas", 67890};
        admins[3] = {"shubham", 67890};
    }

    bool adminLogin()
    {
        string adminName;
        int password;

        cout << "Enter Admin ID (Name): ";
        cin.ignore();
        getline(cin, adminName);

        cout << "Enter Password (5-digit positive integer): ";
        string passwordStr = getPassword();

        // Convert password string to integer
        try
        {
            password = stoi(passwordStr);
        }
        catch (invalid_argument &)
        {
            cout << "Invalid Password. Must be a 5-digit positive integer.\n";
            return false;
        }

        if (password < 10000 || password > 99999)
        {
            cout << "Invalid Password. Must be a 5-digit positive integer.\n";
            return false;
        }

        for (int i = 0; i < adminCount; ++i)
        {
            if (admins[i].name == adminName && admins[i].password == password)
            {
                cout << "Admin login successful!\n";
                loggedInAdmin = adminName;
                return true;
            }
        }

        cout << "Invalid Admin ID or Password.\n";
        return false;
    }

    void userLogin()
    {
        cout << "Welcome, User! You are logged in.\n";
    }

    string getLoggedInAdmin() const
    {
        return loggedInAdmin;
    }
};
//-------------------------------------------------------------------------------------------------------

// Module_01: Optimized Waste Collection Network
namespace RealTimeWasteTrackingSystem
{
    void adminMenu()
    {
        cout << "\nOptimized Waste Collection Network - Admin Menu:\n";
        cout << "01. Add collection points and their names\n";
        cout << "02. Modify collection point name\n";
        cout << "03. Delete collection point\n";
        cout << "04. Add routes (edges)\n";
        cout << "05. Modify route (edge)\n";
        cout << "06. Delete route (edge)\n";
        cout << "07. Display collection points and routes\n";
        cout << "08. Optimize routes to find their shortest path\n";
        cout << "09. Logout\n";
    }

    void userMenu()
    {
        cout << "\nOptimized Waste Collection Network - User Menu:\n";
        cout << "01. Display collection points and routes\n";
        cout << "02. Optimize routes to find their shortest path\n";
        cout << "03. Logout\n";
    }
}

// Module_02: Recycled Product Marketplace
namespace RecycledProductMarketplace
{
    void adminMenu()
    {
        cout << "\nRecycled Product Marketplace - Admin Menu:\n";
        cout << "01. Add Product\n";
        cout << "02. View Products\n";
        cout << "03. Logout\n";
    }

    void userMenu()
    {
        cout << "\nRecycled Product Marketplace - User Menu:\n";
        cout << "01. Search Products\n";
        cout << "02. Logout\n";
    }
}

// Module_03: Smart Bin Monitoring System
namespace SmartBinMonitoringSystem
{
    void adminMenu()
    {
        cout << "\nSmart Bin Monitoring System - Admin Menu:\n";
        cout << "01. Add Bin\n";
        cout << "02. Update Bin Status\n";
        cout << "03. Edit Bin Details\n";
        cout << "04. Delete Bin\n";
        cout << "05. View Bin Status\n";
        cout << "06. Logout\n";
    }

    void userMenu()
    {
        cout << "\nSmart Bin Monitoring System - User Menu:\n";
        cout << "01. View Bin Status\n";
        cout << "02. Logout\n";
    }
}

// Module_04: Smart Composting System
namespace SmartCompostingSystem
{
    void adminMenu()
    {
        cout << "\nSmart Composting System - Admin Menu:\n";
        cout << "01. Add Compost Data\n";
        cout << "02. View All Data\n";
        cout << "03. Check Current Status\n";
        cout << "04. Search by Stage\n";
        cout << "05. Logout\n";
    }

    void userMenu()
    {
        cout << "\nSmart Composting System - User Menu:\n";
        cout << "01. Check Current Status\n";
        cout << "02. Search by Stage\n";
        cout << "03. Logout\n";
    }
}

// Module_05: Smart Waste Segregation System
namespace SmartWasteSegregationSystem
{
    void adminMenu()
    {
        cout << "\nSmart Waste Segregation System - Admin Menu:\n";
        cout << "01. Add Waste\n";
        cout << "02. Modify Waste\n";
        cout << "03. Delete Waste\n";
        cout << "04. Display Waste Categories\n";
        cout << "05. Display Predefined Categories\n";
        cout << "06. Modify Waste Category\n";
        cout << "07. Delete Waste Category\n";
        cout << "08. Logout\n";
    }

    void userMenu()
    {
        cout << "\nSmart Waste Segregation System - User Menu:\n";
        cout << "01. Display Waste Categories\n";
        cout << "02. Display Predefined Categories\n";
        cout << "03. Logout\n";
    }
}

// Module_06: Waste Disposal and Recycling System
namespace WasteDisposalAndRecyclingSystem
{
    void adminMenu()
    {
        cout << "\nWaste Disposal and Recycling System - Admin Menu:\n";
        cout << "01. Add new recyclable item\n";
        cout << "02. Add new non-recyclable item\n";
        cout << "03. Delete recyclable item\n";
        cout << "04. Delete non-recyclable item\n";
        cout << "05. Modify recyclable item\n";
        cout << "06. Modify non-recyclable item\n";
        cout << "07. Display recyclable items\n";
        cout << "08. Display non-recyclable items\n";
        cout << "09. Logout\n";
    }

    void userMenu()
    {
        cout << "\nWaste Disposal and Recycling System - User Menu:\n";
        cout << "01. Display recyclable items\n";
        cout << "02. Display non-recyclable items\n";
        cout << "03. Check if an item is recyclable\n";
        cout << "04. Check if an item is non-recyclable\n";
        cout << "05. Logout\n";
    }
}

// Module_07: Waste to Energy Conversion System
namespace WasteToEnergyConversionSystem
{
    void adminMenu()
    {
        cout << "\nWaste to Energy Conversion System - Admin Menu:\n";
        cout << "01. Process Waste\n";
        cout << "02. Logout\n";
    }

    void userMenu()
    {
        cout << "\nWaste to Energy Conversion System - User Menu:\n";
        cout << "01. Process Waste\n";
        cout << "02. Logout\n";
    }
}

// Module_08: Waste Volume Prediction System
namespace WasteVolumePredictionSystem
{
    void adminMenu()
    {
        cout << "\nWaste Volume Prediction System - Admin Menu:\n";
        cout << "01. Add New Waste Data\n";
        cout << "02. Generate Forecast Report\n";
        cout << "03. Logout\n";
    }

    void userMenu()
    {
        cout << "\nWaste Volume Prediction System - User Menu:\n";
        cout << "01. View Forecast Report\n";
        cout << "02. Logout\n";
    }
}

// Module_09: Zero Waste Event Planner
namespace ZeroWasteEventPlanner
{
    void adminMenu()
    {
        cout << "\nZero Waste Event Planner - Admin Menu:\n";
        cout << "01. Plan Event\n";
        cout << "02. View Events\n";
        cout << "03. View Waste Breakdown\n";
        cout << "04. Logout\n";
    }

    void userMenu()
    {
        cout << "\nZero Waste Event Planner - User Menu:\n";
        cout << "01. View Events\n";
        cout << "02. Logout\n";
    }
}

// Module_10: Hazardous Waste Management System
namespace HazardousWasteManagementSystem
{
    void adminMenu()
    {
        cout << "\nHazardous Waste Management System - Admin Menu:\n";
        cout << "01. Add Disposal Method\n";
        cout << "02. Update Inventory\n";
        cout << "03. Display Disposal Methods\n";
        cout << "04. Display Inventory\n";
        cout << "05. Enqueue Hazardous Item\n";
        cout << "06. View Disposal Queue\n";
        cout << "07. View Waste Categories\n";
        cout << "08. Logout\n";
    }

    void userMenu()
    {
        cout << "\nHazardous Waste Management System - User Menu:\n";
        cout << "01. Display Disposal Methods\n";
        cout << "02. Display Inventory\n";
        cout << "03. View Disposal Queue\n";
        cout << "04. View Waste Categories\n";
        cout << "05. Logout\n";
    }
}

// Module_11: E-Waste Management System
namespace E_WasteManagementSystem
{
    void adminMenu()
    {
        cout << "\nE-Waste Management System - Admin Menu:\n";
        cout << "01. Add Collection Center\n";
        cout << "02. Remove Collection Center\n";
        cout << "03. Display Collection Centers\n";
        cout << "04. Modify Collection Center\n";
        cout << "05. Add Route between Centers\n";
        cout << "06. Display Routes\n";
        cout << "07. Find Shortest Path between Centers\n";
        cout << "08. Modify Route\n";
        cout << "09. Logout\n";
    }

    void userMenu()
    {
        cout << "\nE-Waste Management System - User Menu:\n";
        cout << "01. Display Collection Centers\n";
        cout << "02. Display Routes\n";
        cout << "03. Find Shortest Path between Centers\n";
        cout << "04. Logout\n";
    }
}

// Module_12: Complaint and Feedback System
namespace ComplaintAndFeedbackSystem
{
    void adminMenu()
    {
        cout << "\nComplaint and Feedback System - Admin Menu:\n";
        cout << "01. Resolve Complaint\n";
        cout << "02. Update Complaint Status\n";
        cout << "03. View Complaints by Area\n";
        cout << "04. View Monthly Statistics\n";
        cout << "05. View System Summary\n";
        cout << "06. Display All Active Complaints\n";
        cout << "07. Display Admin Resolution Counts\n";
        cout << "08. View Resolved Complaints History\n";
        cout << "09. Logout\n";
    }

    void userMenu()
    {
        cout << "\nComplaint and Feedback System - User Menu:\n";
        cout << "01. Submit Complaint\n";
        cout << "02. View Monthly Statistics\n";
        cout << "03. View System Summary\n";
        cout << "04. Logout\n";
    }
}

// Module_13: Carbon Footprint Estimator
namespace CarbonFootprintEstimator
{
    void adminMenu()
    {
        cout << "\nCarbon Footprint Estimator - Admin Menu:\n";
        cout << "01. Add New Route\n";
        cout << "02. Display All Routes\n";
        cout << "03. Calculate Shortest Paths\n";
        cout << "04. Display Emissions by City\n";
        cout << "05. Logout\n";
    }

    void userMenu()
    {
        cout << "\nCarbon Footprint Estimator - User Menu:\n";
        cout << "01. Display All Routes\n";
        cout << "02. Calculate Shortest Paths\n";
        cout << "03. Display Emissions by City\n";
        cout << "04. Logout\n";
    }
}

// Module_14: Plastic Waste Tracking System
namespace PlasticWasteTrackingSystem
{
    void adminMenu()
    {
        cout << "\nPlastic Waste Tracking System - Admin Menu:\n";
        cout << "01. Add Waste Record\n";
        cout << "02. Display Records\n";
        cout << "03. Logout\n";
    }

    void userMenu()
    {
        cout << "\nPlastic Waste Tracking System - User Menu:\n";
        cout << "01. Display Records\n";
        cout << "02. Logout\n";
    }
}

// Module_15: Waste Management Quiz System
namespace WasteQuizSystem
{
    void userMenu()
    {
        cout << "\nWaste Management Quiz - User Menu:\n";
        cout << "01. Start Quiz\n";
        cout << "02. Logout\n";
    }
}

// Module_16: Food Waste Management System
namespace FoodWasteSystem
{
    void adminMenu()
    {
        cout << "\nFood Waste Management System - Admin Menu:\n";
        cout << "1. Search Food Donation by Location\n";
        cout << "2. Delete Food Donation to be Redistributed\n";
        cout << "3. Generate Report\n";
        cout << "4. Logout\n";
    }

    void userMenu()
    {
        cout << "\nFood Waste Management System - User Menu:\n";
        cout << "1. Add Food Donation to be Redistributed\n";
        cout << "2. Delete Food Donation to be Redistributed\n";
        cout << "3. Search Food Donation by Location\n";
        cout << "4. Logout\n";
    }
}

// Unified Main Menu
void displayMainMenu()
{
    cout << "\nLogin Menu:\n";
    cout << "1. Admin Login\n";
    cout << "2. User Login\n";
    cout << "3. Exit\n";
}

// Admin Menu
void displayAdminModulesMenu()
{
    cout << "\n==========Admin Menu:==========\n";
    cout << "01. Optimized Waste Collection Network\n";
    cout << "02. Recycled Product Marketplace\n";
    cout << "03. Smart Bin Monitoring System\n";
    cout << "04. Smart Composting System\n";
    cout << "05. Smart Waste Segregation System\n";
    cout << "06. Waste Disposal and Recycling System\n";
    cout << "07. Waste to Energy Conversion System\n";
    cout << "08. Waste Volume Prediction System\n";
    cout << "09. Zero Waste Event Planner\n";
    cout << "10. Hazardous Waste Management System\n";
    cout << "11. E-Waste Management System\n";
    cout << "12. Complaint and Feedback System\n";
    cout << "13. Carbon Footprint Estimator\n";
    cout << "14. Plastic Tracking System\n";
    cout << "15. Food Waste Management System\n";
    cout << "16. Logout\n";
}

// User Menu
void displayUserModulesMenu()
{
    cout << "\n==========User Menu:==========\n";
    cout << "01. Optimized Waste Collection Network\n";
    cout << "02. Recycled Product Marketplace\n";
    cout << "03. Smart Bin Monitoring System\n";
    cout << "04. Smart Composting System\n";
    cout << "05. Smart Waste Segregation System\n";
    cout << "06. Waste Disposal and Recycling System\n";
    cout << "07. Waste to Energy Conversion System\n";
    cout << "08. Waste Volume Prediction System\n";
    cout << "09. Zero Waste Event Planner\n";
    cout << "10. Hazardous Waste Management System\n";
    cout << "11. E-Waste Management System\n";
    cout << "12. Complaint and Feedback System\n";
    cout << "13. Carbon Footprint Estimator\n";
    cout << "14. Plastic Tracking System\n";
    cout << "15. Food Waste Management System\n";
    cout << "16. Waste Management Quiz!\n";
    cout << "17. Logout\n";
}

void displayAdminResolutionCounts(const map<string, int> &adminResolvedCount)
{
    cout << "\nComplaints Resolved by Each Admin:\n";
    cout << "----------------------------------------\n";
    for (const auto &entry : adminResolvedCount)
    {
        cout << "Admin: " << entry.first << ", Resolved: " << entry.second << "\n";
    }
    cout << "----------------------------------------\n";
}

int main()
{
    LoginSystem loginSystem;
    Graph cityGraph;
    Recycled_Product_Marketplace marketplace;
    Bin_Monitoring_System system;
    Smart_Compost_System compostSystem;
    Smart_Waste_System wasteSystem;
    Recycling_System recyclingSystem;
    Waste_Processing_System waste_System;
    WastePredictionSystem predictionSystem;
    Zero_Waste_Event_Planner planner;
    Hazardous_System hazardous;
    Graph_E_Waste routeManager;
    AVLTree centerManager;
    ComplaintSystem complaintSystem;
    ComplaintPriority prioritySystem;
    Carbon_Footprint estimator;
    WasteTrackingSystem plastic_system;
    QuizSystem quizSystem;
    FoodWasteManagementSystem foodWasteSystem;

    string name, from, to;
    float weight;
    int choice;

    while (true)
    {
        displayMainMenu();
        cout << "Enter your choice: ";
        cin >> choice;

        if (cin.fail())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please try again.\n";
            continue;
        }

        switch (choice)
        {
        case 1:
            if (loginSystem.adminLogin())
            {
                int adminModuleChoice;
                do
                {
                    displayAdminModulesMenu();
                    cout << "Enter your choice: ";
                    cin >> adminModuleChoice;

                    if (cin.fail())
                    {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Invalid input. Please try again.\n";
                        continue;
                    }

                    switch (adminModuleChoice)
                    {
                    case 1:
                        int realTimeWasteTrackingChoice;
                        do
                        {
                            RealTimeWasteTrackingSystem::adminMenu();
                            cout << "Enter your choice: ";
                            cin >> realTimeWasteTrackingChoice;

                            if (cin.fail())
                            {
                                cin.clear();
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                cout << "Invalid input. Please try again.\n";
                                continue;
                            }

                            switch (realTimeWasteTrackingChoice)
                            {
                            case 1:
                            {
                                cout << "Enter Collection Point Name: ";
                                getline(cin, name);
                                cityGraph.addNode(name);
                                break;
                            }

                            case 2:
                            {
                                cout << "Enter old Collection Point Name: ";
                                getline(cin, from);
                                cout << "Enter new Collection Point Name: ";
                                getline(cin, to);
                                cityGraph.modifyNode(from, to);
                                break;
                            }

                            case 3:
                            {
                                cout << "Enter Collection Point Name to delete: ";
                                getline(cin, name);
                                cityGraph.deleteNode(name);
                                break;
                            }

                            case 4:
                            {
                                cout << "Enter from Collection Point Name: ";
                                getline(cin, from);
                                cout << "Enter to Collection Point Name: ";
                                getline(cin, to);
                                if (!getValidInput(weight, "Enter Route Distance: "))
                                {
                                    continue;
                                }
                                cityGraph.addEdge(from, to, weight);
                                break;
                            }

                            case 5:
                            {
                                cout << "Enter from Collection Point Name: ";
                                getline(cin, from);
                                cout << "Enter to Collection Point Name: ";
                                getline(cin, to);
                                if (!getValidInput(weight, "Enter new Route Distance: "))
                                {
                                    continue;
                                }
                                cityGraph.modifyEdge(from, to, weight);
                                break;
                            }

                            case 6:
                            {
                                cout << "Enter from Collection Point Name: ";
                                getline(cin, from);
                                cout << "Enter to Collection Point Name: ";
                                getline(cin, to);
                                cityGraph.deleteEdge(from, to);
                                break;
                            }

                            case 7:
                            {
                                cityGraph.displayGraph();
                                break;
                            }

                            case 8:
                            {
                                cout << "Enter starting Collection Point Name: ";
                                getline(cin, from);
                                dijkstra(cityGraph, from);
                                break;
                            }

                            case 9:
                            {
                                cout << "Logging out...\n";
                                break;
                            }

                            default:
                                cout << "Invalid choice. Please try again.\n";
                                break;
                            }
                        } while (realTimeWasteTrackingChoice != 9);
                        break;

                    case 2:
                        int recycledProductChoice;
                        do
                        {
                            RecycledProductMarketplace::adminMenu();
                            cout << "Enter your choice: ";
                            cin >> recycledProductChoice;

                            if (cin.fail())
                            {
                                cin.clear();
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                cout << "Invalid input. Please try again.\n";
                                continue;
                            }

                            switch (recycledProductChoice)
                            {
                            case 1:
                            {
                                string category, name;
                                double price;
                                cout << "Enter category: ";
                                getline(cin, category);
                                cout << "Enter product name: ";
                                getline(cin, name);
                                cout << "Enter price: ";
                                cin >> price;
                                marketplace.addProduct(category, Product(name, category, price));
                                break;
                            }

                            case 2:
                                marketplace.displayProducts();
                                break;

                            case 3:
                                cout << "Logging out...\n";
                                break;

                            default:
                                cout << "Invalid choice. Please try again.\n";
                            }
                        } while (recycledProductChoice != 3);
                        break;

                    case 3:
                        int smartBinMonitoringChoice;
                        do
                        {
                            SmartBinMonitoringSystem::adminMenu();
                            cout << "Enter your choice: ";
                            cin >> smartBinMonitoringChoice;

                            if (cin.fail())
                            {
                                cin.clear();
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                cout << "Invalid input. Please try again.\n";
                                continue;
                            }

                            switch (smartBinMonitoringChoice)
                            {
                            case 1:
                                system.addBin();
                                break;

                            case 2:
                                system.updateBinStatus();
                                break;

                            case 3:
                                system.editBinDetails();
                                break;

                            case 4:
                                system.deleteBin();
                                break;

                            case 5:
                                system.viewBinStatus();
                                break;

                            case 6:
                                cout << "Logging out...\n";
                                break;

                            default:
                                cout << "Invalid choice. Please try again.\n";
                            }
                        } while (smartBinMonitoringChoice != 6);
                        break;

                    case 4:
                        int smartCompostingChoice;
                        do
                        {
                            SmartCompostingSystem::adminMenu();
                            cout << "Enter your choice: ";
                            cin >> smartCompostingChoice;

                            if (cin.fail())
                            {
                                cin.clear();
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                cout << "Invalid input. Please try again.\n";
                                continue;
                            }

                            switch (smartCompostingChoice)
                            {
                            case 1:
                            {
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

                            case 4:
                            {
                                string stage;
                                cout << "Enter stage to search: ";
                                cin >> stage;
                                compostSystem.searchStage(stage);
                                break;
                            }

                            case 5:
                                cout << "Logging out...\n";
                                break;

                            default:
                                cout << "Invalid choice. Please try again.\n";
                            }
                        } while (smartCompostingChoice != 5);
                        break;

                    case 5:
                        int smartWasteSegregationChoice;
                        do
                        {
                            SmartWasteSegregationSystem::adminMenu();
                            cout << "Enter your choice: ";
                            cin >> smartWasteSegregationChoice;

                            if (cin.fail())
                            {
                                cin.clear();
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                cout << "Invalid input. Please try again.\n";
                                continue;
                            }

                            cin.ignore();

                            switch (smartWasteSegregationChoice)
                            {
                            case 1:
                            {
                                string category, item;
                                cout << "Enter waste category: ";
                                getline(cin, category);
                                cout << "Enter waste item: ";
                                getline(cin, item);
                                wasteSystem.addWaste(category, item);
                                break;
                            }

                            case 2:
                            {
                                string category, oldItem, newItem;
                                cout << "Enter waste category: ";
                                getline(cin, category);
                                cout << "Enter old waste item: ";
                                getline(cin, oldItem);
                                cout << "Enter new waste item: ";
                                getline(cin, newItem);
                                wasteSystem.modifyWaste(category, oldItem, newItem);
                                break;
                            }

                            case 3:
                            {
                                string category, item;
                                cout << "Enter waste category: ";
                                getline(cin, category);
                                cout << "Enter waste item to delete: ";
                                getline(cin, item);
                                wasteSystem.deleteWaste(category, item);
                                break;
                            }

                            case 4:
                                wasteSystem.displayWasteCategories();
                                break;

                            case 5:
                                wasteSystem.displayPredefinedCategories();
                                break;

                            case 6:
                            {
                                string oldCategory, newCategory;
                                cout << "Enter the category to modify: ";
                                getline(cin, oldCategory);
                                cout << "Enter the new category name: ";
                                getline(cin, newCategory);
                                wasteSystem.modifyWasteCategory(oldCategory, newCategory);
                                break;
                            }

                            case 7:
                            {
                                string category;
                                cout << "Enter the category to delete: ";
                                getline(cin, category);
                                wasteSystem.deleteWasteCategory(category);
                                break;
                            }

                            case 8:
                                cout << "Logging out...\n";
                                break;

                            default:
                                cout << "Invalid choice. Please try again.\n";
                            }
                        } while (smartWasteSegregationChoice != 8);
                        break;

                    case 6:
                        int wasteDisposalChoice;
                        do
                        {
                            WasteDisposalAndRecyclingSystem::adminMenu();
                            cout << "Enter your choice: ";
                            cin >> wasteDisposalChoice;

                            if (cin.fail())
                            {
                                cin.clear();
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                cout << "Invalid input. Please try again.\n";
                                continue;
                            }

                            switch (wasteDisposalChoice)
                            {
                            case 1:
                            {
                                string itemName;
                                cout << "Enter new recyclable item name: ";
                                cin.ignore();
                                getline(cin, itemName);
                                recyclingSystem.addRecyclableItem(itemName);
                                break;
                            }

                            case 2:
                            {
                                string itemName;
                                cout << "Enter new non-recyclable item name: ";
                                cin.ignore();
                                getline(cin, itemName);
                                recyclingSystem.addNonRecyclableItem(itemName);
                                break;
                            }

                            case 3:
                            {
                                string itemName;
                                cout << "Enter recyclable item name to delete: ";
                                cin.ignore();
                                getline(cin, itemName);
                                recyclingSystem.deleteRecyclableItem(itemName);
                                break;
                            }

                            case 4:
                            {
                                string itemName;
                                cout << "Enter non-recyclable item name to delete: ";
                                cin.ignore();
                                getline(cin, itemName);
                                recyclingSystem.deleteNonRecyclableItem(itemName);
                                break;
                            }

                            case 5:
                            {
                                string oldName, newName;
                                cout << "Enter recyclable item name to modify: ";
                                cin.ignore();
                                getline(cin, oldName);
                                cout << "Enter new name for the item: ";
                                cin.ignore();
                                getline(cin, newName);
                                recyclingSystem.modifyRecyclableItem(oldName, newName);
                                break;
                            }

                            case 6:
                            {
                                string oldName, newName;
                                cout << "Enter non-recyclable item name to modify: ";
                                cin.ignore();
                                getline(cin, oldName);
                                cout << "Enter new name for the item: ";
                                cin.ignore();
                                getline(cin, newName);
                                recyclingSystem.modifyNonRecyclableItem(oldName, newName);
                                break;
                            }

                            case 7:
                                recyclingSystem.displayRecyclableItems();
                                break;

                            case 8:
                                recyclingSystem.displayNonRecyclableItems();
                                break;

                            case 9:
                                cout << "Logging out...\n";
                                break;

                            default:
                                cout << "Invalid choice. Please try again.\n";
                            }
                        } while (wasteDisposalChoice != 9);
                        break;

                    case 7:
                        int wasteToEnergyChoice;
                        do
                        {
                            WasteToEnergyConversionSystem::adminMenu();
                            cout << "Enter your choice: ";
                            cin >> wasteToEnergyChoice;

                            if (cin.fail())
                            {
                                cin.clear();
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                cout << "Invalid input. Please try again.\n";
                                continue;
                            }

                            switch (wasteToEnergyChoice)
                            {
                            case 1:
                            {
                                string wasteType;
                                float quantity;
                                float energyGenerated;

                                // Energy generated per ton for each waste type (MWh/ton)
                                const float energyPerTonEwaste = 2.5;
                                const float energyPerTonAgricultural = 1.8;
                                const float energyPerTonBiodegradable = 1.5;
                                const float energyPerTonPlastic = 3.2;
                                const float energyPerTonMetal = 0.8;

                                cout << "\nSelect Waste Type:\n";
                                cout << "1. E-Waste" << endl;
                                cout << "2. Agricultural" << endl;
                                cout << "3. Biodegradable" << endl;
                                cout << "4. Plastic" << endl;
                                cout << "5. Metal" << endl;
                                cout << "Enter your choice: ";
                                int wasteChoice;
                                cin >> wasteChoice;

                                switch (wasteChoice)
                                {
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
                                waste_System.saveRecord(wasteType, quantity, energyGenerated, totalEnergy);
                                break;
                            }
                            break;

                            case 2:
                                cout << "Logging out...\n";
                                break;

                            default:
                                cout << "Invalid choice. Please try again.\n";
                            }
                        } while (wasteToEnergyChoice != 2);
                        break;

                    case 8:
                        int wasteVolumeChoice;
                        do
                        {
                            WasteVolumePredictionSystem::adminMenu();
                            cout << "Enter your choice: ";
                            cin >> wasteVolumeChoice;

                            if (cin.fail())
                            {
                                cin.clear();
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                cout << "Invalid input. Please try again.\n";
                                continue;
                            }

                            switch (wasteVolumeChoice)
                            {
                            case 1:
                            {
                                string date, location;
                                double volume;

                                cout << "Enter date (DD-MM-YYYY): ";
                                cin >> date;
                                cout << "Enter location: ";
                                cin.ignore();
                                getline(cin, location);
                                cout << "Enter waste volume (tons): ";
                                cin >> volume;

                                predictionSystem.addWasteData(date, location, volume);
                                cout << "Data added successfully!\n";
                                break;
                            }

                            case 2:
                            {
                                string location;
                                cout << "Enter location for forecast: ";
                                cin.ignore();
                                getline(cin, location);
                                predictionSystem.generateForecastReport(location);
                                break;
                            }

                            case 3:
                                cout << "Logging out...\n";
                                break;

                            default:
                                cout << "Invalid choice. Please try again.\n";
                            }
                        } while (wasteVolumeChoice != 3);
                        break;

                    case 9:
                        int zeroWasteChoice;
                        do
                        {
                            ZeroWasteEventPlanner::adminMenu();
                            cout << "Enter your choice: ";
                            cin >> zeroWasteChoice;

                            if (cin.fail())
                            {
                                cin.clear();
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                cout << "Invalid input. Please try again.\n";
                                continue;
                            }

                            switch (zeroWasteChoice)
                            {
                            case 1:
                            {
                                string name, type;
                                float duration;
                                int attendees;
                                cout << "Enter event name: ";
                                getline(cin, name);
                                cout << "Enter event duration (hours): ";
                                cin >> duration;
                                cout << "Enter number of attendees: ";
                                cin >> attendees;
                                cout << "Enter event type (Conference/Festival/Sports/Concert/Exhibition/Seminar): ";
                                getline(cin, type);

                                planner.planEvent(name, duration, type, attendees);
                                break;
                            }

                            case 2:
                                planner.displayEvents();
                                break;

                            case 3:
                                planner.displayWasteBreakdown();
                                break;

                            case 4:
                                cout << "Logging out...\n";
                                break;

                            default:
                                cout << "Invalid choice. Please try again.\n";
                            }
                        } while (zeroWasteChoice != 4);
                        break;

                    case 10:
                        int hazardousWasteChoice;
                        do
                        {
                            HazardousWasteManagementSystem::adminMenu();
                            cout << "Enter your choice: ";
                            cin >> hazardousWasteChoice;

                            if (cin.fail())
                            {
                                cin.clear();
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                cout << "Invalid input. Please try again.\n";
                                continue;
                            }

                            switch (hazardousWasteChoice)
                            {
                            case 1:
                            {
                                string item, method;
                                cout << "Enter item name: ";
                                getline(cin, item);

                                hazardous.displayWasteCategories();

                                cout << "Enter disposal method: ";
                                getline(cin, method);
                                hazardous.addDisposalMethod(item, method);
                                break;
                            }

                            case 2:
                            {
                                string item;
                                int count;
                                cout << "Enter item name: ";
                                getline(cin, item);
                                cout << "Enter count: ";
                                cin >> count;
                                hazardous.updateInventory(item, count);
                                break;
                            }

                            case 3:
                                hazardous.displayDisposalMethods();
                                break;

                            case 4:
                                hazardous.displayInventory();
                                break;

                            case 5:
                            {
                                string name, category;
                                int riskLevel;

                                hazardous.displayWasteCategories();

                                cout << "Enter item name: ";
                                getline(cin, name);
                                cout << "Enter category (from the list above): ";
                                getline(cin, category);

                                bool validCategory = false;
                                int defaultRisk = hazardous.getDefaultRiskLevel(category);

                                for (const auto &cat : WASTE_CATEGORIES)
                                {
                                    if (cat.name == category)
                                    {
                                        validCategory = true;
                                        break;
                                    }
                                }

                                if (!validCategory)
                                {
                                    cout << "Warning: Entered category doesn't match predefined categories.\n";
                                    cout << "Proceeding with default risk level of 5.\n";
                                }
                                else
                                {
                                    cout << "Default risk level for " << category << " is " << defaultRisk << "\n";
                                }

                                cout << "Enter risk level (1-10) [Press Enter to use default " << defaultRisk << "]: ";
                                string riskInput;
                                getline(cin, riskInput);

                                if (riskInput.empty())
                                {
                                    riskLevel = defaultRisk;
                                }
                                else
                                {
                                    riskLevel = stoi(riskInput);
                                    if (riskLevel < 1 || riskLevel > 10)
                                    {
                                        cout << "Invalid risk level. Using default risk level of " << defaultRisk << "\n";
                                        riskLevel = defaultRisk;
                                    }
                                }

                                hazardous.enqueueHazardousItem(name, category, riskLevel);
                                break;
                            }

                            case 6:
                                hazardous.viewDisposalQueue();
                                break;

                            case 7:
                                hazardous.displayWasteCategories();
                                break;

                            case 8:
                                cout << "Logging out...\n";
                                break;

                            default:
                                cout << "Invalid choice. Please try again.\n";
                            }
                        } while (hazardousWasteChoice != 8);
                        break;

                    case 11:
                        int e_WasteChoice;
                        do
                        {
                            E_WasteManagementSystem::adminMenu();
                            cout << "Enter your choice: ";
                            cin >> e_WasteChoice;

                            if (cin.fail())
                            {
                                cin.clear();
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                cout << "Invalid input. Please try again.\n";
                                continue;
                            }

                            switch (e_WasteChoice)
                            {
                            case 1:
                            {
                                string id, location;
                                float quantity;
                                cout << "Enter Center ID: ";
                                cin >> id;
                                cout << "Enter Location: ";
                                cin >> location;
                                cout << "Enter E-Waste Quantity (kg): ";
                                cin >> quantity;

                                CollectionCenter center(id, location, quantity);
                                if (centerManager.addCenter(center))
                                {
                                    routeManager.addLocation(id, location);
                                }
                                break;
                            }

                            case 2:
                            {
                                string id;
                                cout << "Enter Center ID to remove: ";
                                cin >> id;
                                centerManager.removeCenter(id);
                                break;
                            }

                            case 3:
                                centerManager.displayCenters();
                                break;

                            case 4:
                            {
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

                            case 5:
                            {
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
                                routeManager.displayGraph_E_Waste();
                                break;

                            case 7:
                            {
                                string start, end;
                                cout << "Enter Start Center ID: ";
                                cin >> start;
                                cout << "Enter End Center ID: ";
                                cin >> end;
                                routeManager.shortestPath(start, end);
                                break;
                            }

                            case 8:
                            {
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
                                break;

                            default:
                                cout << "Invalid choice. Please try again.\n";
                            }
                        } while (e_WasteChoice != 9);
                        break;

                    case 12:
                        int complaintChoice;
                        do
                        {
                            ComplaintAndFeedbackSystem::adminMenu();
                            cout << "Enter your choice: ";
                            cin >> complaintChoice;

                            if (cin.fail())
                            {
                                cin.clear();
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                cout << "Invalid input. Please try again.\n";
                                continue;
                            }

                            switch (complaintChoice)
                            {
                            case 1:
                            {
                                string area;
                                cout << "Enter the area to resolve a complaint: ";
                                cin.ignore();
                                getline(cin, area);
                                complaintSystem.resolveComplaint(area, loginSystem.getLoggedInAdmin());
                                break;
                            }

                            case 2:
                            {
                                string name, status;
                                cout << "Enter the citizen's name: ";
                                cin.ignore();
                                getline(cin, name);
                                cout << "Enter new status (Pending/In Progress/Resolved): ";
                                getline(cin, status);
                                complaintSystem.updateComplaintStatus(name, status);
                                break;
                            }

                            case 3:
                            {
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
                                displayAdminResolutionCounts(complaintSystem.getAdminResolvedCount());
                                break;

                            case 8:
                                complaintSystem.viewResolvedComplaints();
                                break;

                            case 9:
                                cout << "Logging out...\n";
                                break;

                            default:
                                cout << "Invalid choice. Please try again.\n";
                            }
                        } while (complaintChoice != 9);
                        break;

                    case 13:
                        int carbonFootprintChoice;
                        do
                        {
                            CarbonFootprintEstimator::adminMenu();
                            cout << "Enter your choice: ";
                            cin >> carbonFootprintChoice;

                            if (cin.fail())
                            {
                                cin.clear();
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                cout << "Invalid input. Please try again.\n";
                                continue;
                            }

                            switch (carbonFootprintChoice)
                            {
                            case 1:
                            {
                                string source, target, wasteType;
                                double distance;
                                cout << "Enter source city: ";
                                cin.ignore();
                                getline(cin, source);
                                cout << "Enter target city: ";
                                cin.ignore();
                                getline(cin, target);
                                cout << "Enter distance (km): ";
                                cin >> distance;
                                cin.ignore();
                                cout << "Enter waste type (Plastic/Metal/Organic/Electronic): ";
                                getline(cin, wasteType);

                                estimator.addRoute(source, target, distance, wasteType);
                                cout << "Route added successfully!\n";
                                break;
                            }

                            case 2:
                                estimator.displayRoutes();
                                break;

                            case 3:
                            {
                                string startCity;
                                cout << "Enter starting city: ";
                                cin >> startCity;
                                vector<double> distances = estimator.calculateShortestPaths(startCity);
                                cout << "\nShortest Paths from " << startCity << ":\n";
                                for (int i = 0; i < estimator.getNumCities(); i++)
                                {
                                    cout << "To " << estimator.getCityName(i) << ": ";
                                    if (distances[i] == NO_PATH)
                                    {
                                        cout << "No route available\n";
                                    }
                                    else
                                    {
                                        cout << distances[i] << " km\n";
                                    }
                                }
                                break;
                            }

                            case 4:
                                estimator.displayEmissionsByCity();
                                break;

                            case 5:
                                cout << "Logging out...\n";
                                break;

                            default:
                                cout << "Invalid choice. Please try again.\n";
                            }
                        } while (carbonFootprintChoice != 5);
                        break;

                    case 14:
                        int plasticWasteChoice;
                        do
                        {
                            PlasticWasteTrackingSystem::adminMenu();
                            cout << "Enter your choice: ";
                            cin >> plasticWasteChoice;

                            if (cin.fail())
                            {
                                cin.clear();
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                cout << "Invalid input. Please try again.\n";
                                continue;
                            }

                            switch (plasticWasteChoice)
                            {
                            case 1:
                                plastic_system.addWasteRecord();
                                break;

                            case 2:
                                plastic_system.displayRecords();
                                break;

                            case 3:
                                cout << "Logging out...\n";
                                break;

                            default:
                                cout << "Invalid choice. Please try again.\n";
                            }
                        } while (plasticWasteChoice != 3);
                        break;

                    case 15:
                        int foodWasteChoice;
                        do
                        {
                            FoodWasteSystem::adminMenu();
                            cout << "Enter your choice: ";
                            cin >> foodWasteChoice;

                            if (cin.fail())
                            {
                                cin.clear();
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                cout << "Invalid input. Please try again.\n";
                                continue;
                            }

                            switch (foodWasteChoice)
                            {
                            case 1:
                            {
                                string location;
                                cout << "Enter location to search: ";
                                cin.ignore();
                                getline(cin, location);
                                foodWasteSystem.searchFoodDonationByLocation(location);
                                break;
                            }

                            case 2:
                            {
                                string donorName, location;
                                cout << "Enter donor name: ";
                                cin.ignore();
                                getline(cin, donorName);
                                cout << "Enter location: ";
                                getline(cin, location);
                                foodWasteSystem.deleteFoodDonation(donorName, location);
                                break;
                            }

                            case 3:
                                foodWasteSystem.generateReport();
                                break;

                            case 4:
                                cout << "Logging out...\n";
                                break;

                            default:
                                cout << "Invalid choice. Please try again.\n";
                            }
                        } while (foodWasteChoice != 4);
                        break;

                    case 16:
                        cout << "Logging out of Admin Modules Menu...\n";
                        break;

                    default:
                        cout << "Invalid choice. Please try again.\n";
                    }
                } while (adminModuleChoice != 16);
            }
            break;

        case 2:
            loginSystem.userLogin();
            int userModuleChoice;
            do
            {
                displayUserModulesMenu();
                cout << "Enter your choice: ";
                cin >> userModuleChoice;

                if (cin.fail())
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid input. Please try again.\n";
                    continue;
                }

                switch (userModuleChoice)
                {
                case 1:
                    int realTimeWasteTrackingChoice;
                    do
                    {
                        RealTimeWasteTrackingSystem::userMenu();
                        cout << "Enter your choice: ";
                        cin >> realTimeWasteTrackingChoice;

                        if (cin.fail())
                        {
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            cout << "Invalid input. Please try again.\n";
                            continue;
                        }

                        switch (realTimeWasteTrackingChoice)
                        {
                        case 1:
                        {
                            cityGraph.displayGraph();
                            break;
                        }

                        case 2:
                        {
                            cout << "Enter starting Collection Point Name: ";
                            cin.ignore();
                            getline(cin, from);
                            dijkstra(cityGraph, from);
                            break;
                        }

                        case 3:
                            cout << "Logging out...\n";
                            break;

                        default:
                            cout << "Invalid choice. Please try again.\n";
                        }
                    } while (realTimeWasteTrackingChoice != 3);
                    break;

                case 2:
                    int recycledProductChoice;
                    do
                    {
                        RecycledProductMarketplace::userMenu();
                        cout << "Enter your choice: ";
                        cin >> recycledProductChoice;

                        if (cin.fail())
                        {
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            cout << "Invalid input. Please try again.\n";
                            continue;
                        }

                        switch (recycledProductChoice)
                        {
                        case 1:
                        {
                            string category;
                            cin.ignore();
                            cout << "Enter category to search: ";
                            getline(cin, category);
                            auto products = marketplace.searchProducts(category);
                            if (products.empty())
                            {
                                cout << "No products found in this category.\n";
                            }
                            else
                            {
                                marketplace.sortProductsByPrice(products);
                                cout << "Products in category: " << category << "\n";
                                for (const auto &product : products)
                                {
                                    cout << "  Name: " << product.name
                                         << ", Price: $" << product.price << "\n";
                                }
                            }
                            break;
                        }

                        case 2:
                            cout << "Logging out...\n";
                            break;

                        default:
                            cout << "Invalid choice. Please try again.\n";
                        }
                    } while (recycledProductChoice != 2);
                    break;

                case 3:
                    int smartBinMonitoringChoice;
                    do
                    {
                        SmartBinMonitoringSystem::userMenu();
                        cout << "Enter your choice: ";
                        cin >> smartBinMonitoringChoice;

                        if (cin.fail())
                        {
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            cout << "Invalid input. Please try again.\n";
                            continue;
                        }

                        switch (smartBinMonitoringChoice)
                        {
                        case 1:
                            system.viewBinStatus();
                            break;

                        case 2:
                            cout << "Logging out...\n";
                            break;

                        default:
                            cout << "Invalid choice. Please try again.\n";
                        }
                    } while (smartBinMonitoringChoice != 2);
                    break;

                case 4:
                    int smartCompostingChoice;
                    do
                    {
                        SmartCompostingSystem::userMenu();
                        cout << "Enter your choice: ";
                        cin >> smartCompostingChoice;

                        if (cin.fail())
                        {
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            cout << "Invalid input. Please try again.\n";
                            continue;
                        }

                        switch (smartCompostingChoice)
                        {
                        case 1:
                            compostSystem.checkCurrentStatus();
                            break;

                        case 2:
                        {
                            string stage;
                            cout << "Enter stage to search: ";
                            cin >> stage;
                            compostSystem.searchStage(stage);
                            break;
                        }

                        case 3:
                            cout << "Logging out...\n";
                            break;

                        default:
                            cout << "Invalid choice. Please try again.\n";
                        }
                    } while (smartCompostingChoice != 3);
                    break;

                case 5:
                    int smartWasteSegregationChoice;
                    do
                    {
                        SmartWasteSegregationSystem::userMenu();
                        cout << "Enter your choice: ";
                        cin >> smartWasteSegregationChoice;

                        if (cin.fail())
                        {
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            cout << "Invalid input. Please try again.\n";
                            continue;
                        }

                        switch (smartWasteSegregationChoice)
                        {
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
                    } while (smartWasteSegregationChoice != 3);
                    break;

                case 6:
                    int wasteDisposalChoice;
                    do
                    {
                        WasteDisposalAndRecyclingSystem::userMenu();
                        cout << "Enter your choice: ";
                        cin >> wasteDisposalChoice;

                        if (cin.fail())
                        {
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            cout << "Invalid input. Please try again.\n";
                            continue;
                        }

                        switch (wasteDisposalChoice)
                        {
                        case 1:
                            recyclingSystem.displayRecyclableItems();
                            break;

                        case 2:
                            recyclingSystem.displayNonRecyclableItems();
                            break;

                        case 3:
                        {
                            string itemName;
                            cout << "Enter item name to check if it's recyclable: ";
                            cin.ignore();
                            getline(cin, itemName);
                            if (recyclingSystem.isRecyclable(itemName) != "")
                            {
                                cout << itemName << " is recyclable.\n";
                            }
                            else
                            {
                                cout << itemName << " is not recyclable.\n";
                            }
                            break;
                        }

                        case 4:
                        {
                            string itemName;
                            cout << "Enter item name to check if it's non-recyclable: ";
                            cin.ignore();
                            getline(cin, itemName);
                            if (recyclingSystem.isNonRecyclable(itemName) != "")
                            {
                                cout << itemName << " is non-recyclable.\n";
                            }
                            else
                            {
                                cout << itemName << " is not non-recyclable.\n";
                            }
                            break;
                        }

                        case 5:
                            cout << "Logging out...\n";
                            break;

                        default:
                            cout << "Invalid choice. Please try again.\n";
                        }
                    } while (wasteDisposalChoice != 5);
                    break;

                case 7:
                    int wasteToEnergyChoice;
                    do
                    {
                        WasteToEnergyConversionSystem::userMenu();
                        cout << "Enter your choice: ";
                        cin >> wasteToEnergyChoice;

                        if (cin.fail())
                        {
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            cout << "Invalid input. Please try again.\n";
                            continue;
                        }

                        switch (wasteToEnergyChoice)
                        {
                        case 1:
                        {
                            string wasteType;
                            float quantity;
                            float energyGenerated;

                            // Energy generated per ton for each waste type (MWh/ton)
                            const float energyPerTonEwaste = 2.5;
                            const float energyPerTonAgricultural = 1.8;
                            const float energyPerTonBiodegradable = 1.5;
                            const float energyPerTonPlastic = 3.2;
                            const float energyPerTonMetal = 0.8;

                            cout << "\nSelect Waste Type:\n";
                            cout << "1. E-Waste\n";
                            cout << "2. Agricultural\n";
                            cout << "3. Biodegradable\n";
                            cout << "4. Plastic\n";
                            cout << "5. Metal\n";
                            cout << "Enter choice: ";
                            int wasteChoice;
                            cin >> wasteChoice;

                            switch (wasteChoice)
                            {
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
                            waste_System.saveRecord(wasteType, quantity, energyGenerated, totalEnergy);
                            break;
                        }
                        break;

                        case 2:
                            cout << "Logging out...\n";
                            break;

                        default:
                            cout << "Invalid choice. Please try again.\n";
                        }
                    } while (wasteToEnergyChoice != 2);
                    break;

                case 8:
                    int wasteVolumeChoice;
                    do
                    {
                        WasteVolumePredictionSystem::userMenu();
                        cout << "Enter your choice: ";
                        cin >> wasteVolumeChoice;

                        if (cin.fail())
                        {
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            cout << "Invalid input. Please try again.\n";
                            continue;
                        }

                        switch (wasteVolumeChoice)
                        {
                        case 1:
                        {
                            string location;
                            cout << "Enter location for forecast: ";
                            cin.ignore();
                            getline(cin, location);
                            predictionSystem.generateForecastReport(location);
                            break;
                        }

                        case 2:
                            cout << "Logging out...\n";
                            break;

                        default:
                            cout << "Invalid choice. Please try again.\n";
                        }
                    } while (wasteVolumeChoice != 2);
                    break;

                case 9:
                    int zeroWasteChoice;
                    do
                    {
                        ZeroWasteEventPlanner::userMenu();
                        cout << "Enter your choice: ";
                        cin >> zeroWasteChoice;

                        if (cin.fail())
                        {
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            cout << "Invalid input. Please try again.\n";
                            continue;
                        }

                        switch (zeroWasteChoice)
                        {
                        case 1:
                            planner.displayEvents();
                            break;

                        case 2:
                            cout << "Logging out...\n";
                            break;

                        default:
                            cout << "Invalid choice. Please try again.\n";
                        }
                    } while (zeroWasteChoice != 2);
                    break;

                case 10:
                    int hazardousWasteChoice;
                    do
                    {
                        HazardousWasteManagementSystem::userMenu();
                        cout << "Enter your choice: ";
                        cin >> hazardousWasteChoice;

                        if (cin.fail())
                        {
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            cout << "Invalid input. Please try again.\n";
                            continue;
                        }

                        switch (hazardousWasteChoice)
                        {
                        case 1:
                            hazardous.displayDisposalMethods();
                            break;

                        case 2:
                            hazardous.displayInventory();
                            break;

                        case 3:
                            hazardous.viewDisposalQueue();
                            break;

                        case 4:
                            hazardous.displayWasteCategories();
                            break;

                        case 5:
                            cout << "Logging out...\n";
                            break;

                        default:
                            cout << "Invalid choice. Please try again.\n";
                        }
                    } while (hazardousWasteChoice != 5);
                    break;

                case 11:
                    int e_WasteChoice;
                    do
                    {
                        E_WasteManagementSystem::userMenu();
                        cout << "Enter your choice: ";
                        cin >> e_WasteChoice;

                        if (cin.fail())
                        {
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            cout << "Invalid input. Please try again.\n";
                            continue;
                        }

                        switch (e_WasteChoice)
                        {
                        case 1:
                            centerManager.displayCenters();
                            break;

                        case 2:
                            routeManager.displayGraph_E_Waste();
                            break;

                        case 3:
                        {
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
                            break;

                        default:
                            cout << "Invalid choice. Please try again.\n";
                        }
                    } while (e_WasteChoice != 4);
                    break;

                case 12:
                    int complaintChoice;
                    do
                    {
                        ComplaintAndFeedbackSystem::userMenu();
                        cout << "Enter your choice: ";
                        cin >> complaintChoice;

                        if (cin.fail())
                        {
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            cout << "Invalid input. Please try again.\n";
                            continue;
                        }

                        switch (complaintChoice)
                        {
                        case 1:
                        {
                            string name, area, description;
                            cout << "Enter your name: ";
                            cin.ignore();
                            getline(cin, name);
                            cout << "Enter your area: ";
                            getline(cin, area);
                            cout << "Enter complaint description: ";
                            getline(cin, description);
                            complaintSystem.submitComplaint(name, area, description, prioritySystem);
                            break;
                        }

                        case 2:
                            complaintSystem.viewMonthlyStats();
                            break;

                        case 3:
                            complaintSystem.viewSummary();
                            break;

                        case 4:
                            cout << "Logging out...\n";
                            break;

                        default:
                            cout << "Invalid choice. Please try again.\n";
                        }
                    } while (complaintChoice != 4);
                    break;

                case 13:
                    int carbonFootprintChoice;
                    do
                    {
                        CarbonFootprintEstimator::userMenu();
                        cout << "Enter your choice: ";
                        cin >> carbonFootprintChoice;

                        if (cin.fail())
                        {
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            cout << "Invalid input. Please try again.\n";
                            continue;
                        }

                        switch (carbonFootprintChoice)
                        {
                        case 1:
                            estimator.displayRoutes();
                            break;

                        case 2:
                        {
                            string startCity;
                            cout << "Enter starting city: ";
                            cin >> startCity;
                            vector<double> distances = estimator.calculateShortestPaths(startCity);
                            cout << "\nShortest Paths from " << startCity << ":\n";
                            for (int i = 0; i < estimator.getNumCities(); i++)
                            {
                                cout << "To " << estimator.getCityName(i) << ": ";
                                if (distances[i] == NO_PATH)
                                {
                                    cout << "No route available\n";
                                }
                                else
                                {
                                    cout << distances[i] << " km\n";
                                }
                            }
                            break;
                        }

                        case 3:
                            estimator.displayEmissionsByCity();
                            break;

                        case 4:
                            cout << "Logging out...\n";
                            break;

                        default:
                            cout << "Invalid choice. Please try again.\n";
                        }
                    } while (carbonFootprintChoice != 4);
                    break;

                case 14:
                    int plasticWasteChoice;
                    do
                    {
                        PlasticWasteTrackingSystem::userMenu();
                        cout << "Enter your choice: ";
                        cin >> plasticWasteChoice;

                        if (cin.fail())
                        {
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            cout << "Invalid input. Please try again.\n";
                            continue;
                        }

                        switch (plasticWasteChoice)
                        {
                        case 1:
                            plastic_system.displayRecords();
                            break;

                        case 2:
                            cout << "Logging out...\n";
                            break;

                        default:
                            cout << "Invalid choice. Please try again.\n";
                        }
                    } while (plasticWasteChoice != 2);
                    break;

                case 15:
                    int foodWasteChoice;
                    do
                    {
                        FoodWasteSystem::userMenu();
                        cout << "Enter your choice: ";
                        cin >> foodWasteChoice;

                        if (cin.fail())
                        {
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            cout << "Invalid input. Please try again.\n";
                            continue;
                        }

                        switch (foodWasteChoice)
                        {
                            case 1:
                            {
                                string donorName, location, foodType, status;
                                int quantity;
                                cin.ignore();
                                cout << "Enter donor name: ";
                                getline(cin, donorName);
                                cout << "Enter location: ";
                                getline(cin, location);
                                cout << "Enter food type: ";
                                getline(cin, foodType);
                                cout << "Enter quantity(plates): ";
                                cin >> quantity;
                                cin.ignore();
                                cout << "Enter status (e.g., Fresh, Near Expiry): ";
                                getline(cin, status);
                                foodWasteSystem.addFoodDonation(donorName, location, foodType, quantity, status);
                                break;
                            }

                            case 2:
                            {
                                string donorName, location;
                                cout << "Enter donor name: ";
                                cin.ignore();
                                getline(cin, donorName);
                                cout << "Enter location: ";
                                getline(cin, location);
                                foodWasteSystem.deleteFoodDonation(donorName, location);
                                break;
                            }

                            case 3:
                            {
                                string location;
                                cout << "Enter location to search: ";
                                cin.ignore();
                                getline(cin, location);
                                foodWasteSystem.searchFoodDonationByLocation(location);
                                break;
                            }

                            case 4:
                                cout << "Logging out...\n";
                                break;

                        default:
                            cout << "Invalid choice. Please try again.\n";
                        }
                    } while (foodWasteChoice != 4);
                    break;

                case 16:
                    int QuizWasteChoice;
                    do
                    {
                        WasteQuizSystem::userMenu();
                        cout << "Enter your choice: ";
                        cin >> QuizWasteChoice;

                        if (cin.fail())
                        {
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            cout << "Invalid input. Please try again.\n";
                            continue;
                        }

                        switch (QuizWasteChoice)
                        {
                        case 1:
                            quizSystem.startQuiz();
                            break;

                        case 2:
                            cout << "Logging out...\n";
                            break;

                        default:
                            cout << "Invalid choice. Please try again.\n";
                        }
                    } while (QuizWasteChoice != 2);
                    break;

                case 17:
                    cout << "Logging out of User Modules Menu...\n";
                    break;

                default:
                    cout << "Invalid choice. Please try again.\n";
                }
            } while (userModuleChoice != 17);
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
