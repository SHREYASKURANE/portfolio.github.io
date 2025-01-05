#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <fstream>

using namespace std;

// Structure to store admin credentials
struct Admin {
    string name;
    int password; // 5-digit positive integer password
};

// Structure to represent a product
struct Product {
    string name;
    string category;
    double price;

    Product(string n, string c, double p) : name(n), category(c), price(p) {}
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

    void userLogin() {
        cout << "Welcome, User! You are logged in.\n";
    }
};

class RecycledProductMarketplace {
private:
    unordered_map<string, vector<Product>> productCatalog; // Maps categories to products

    void saveProductsToFile() {
        ofstream outFile("products.txt");
        if (outFile.is_open()) {
            for (const auto &pair : productCatalog) {
                for (const auto &product : pair.second) {
                    outFile << product.category << "," << product.name << "," << product.price << "\n";
                }
            }
            outFile.close();
        } else {
            cerr << "Unable to open file for saving products.\n";
        }
    }

    void loadProductsFromFile() {
        ifstream inFile("products.txt");
        if (inFile.is_open()) {
            string line;
            while (getline(inFile, line)) {
                size_t pos1 = line.find(',');
                size_t pos2 = line.find(',', pos1 + 1);
                string category = line.substr(0, pos1);
                string name = line.substr(pos1 + 1, pos2 - pos1 - 1);
                double price = stod(line.substr(pos2 + 1));
                productCatalog[category].emplace_back(name, category, price);
            }
            inFile.close();
        } else {
            cerr << "Unable to open file for loading products.\n";
        }
    }

public:
    RecycledProductMarketplace() {
        loadProductsFromFile();
    }

    ~RecycledProductMarketplace() {
        saveProductsToFile();
    }

    void addProduct(const string &category, const Product &product) {
        productCatalog[category].push_back(product);
        cout << "Added product: " << product.name << " in category: " << category << "\n";
    }

    void displayProducts() {
        for (const auto &pair : productCatalog) {
            cout << "Category: " << pair.first << "\n";
            for (const auto &product : pair.second) {
                cout << "  Name: " << product.name
                     << ", Price: $" << product.price << "\n";
            }
        }
    }

    vector<Product> searchProducts(const string &category) {
        vector<Product> result;
        if (productCatalog.find(category) != productCatalog.end()) {
            result = productCatalog[category];
        }
        return result;
    }

    void sortProductsByPrice(vector<Product> &products) {
        // Sort by price in ascending order
        sort(products.begin(), products.end(), [](const Product &a, const Product &b) {
            return a.price < b.price;
        });
    }
};

int main() {
    LoginSystem loginSystem;
    RecycledProductMarketplace marketplace;

    while (true) {
        int choice;
        cout << "\nLogin Page\n";
        cout << "1. Admin Login\n";
        cout << "2. User Login\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            if (loginSystem.adminLogin()) {
                int adminChoice;
                do {
                    cout << "\nAdmin Dashboard\n";
                    cout << "1. Add Product\n";
                    cout << "2. View Products\n";
                    cout << "3. Logout\n";
                    cout << "Enter your choice: ";
                    cin >> adminChoice;

                    switch (adminChoice) {
                    case 1: {
                        string category, name;
                        double price;
                        cin.ignore();
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
                        cout << "Invalid choice. Try again.\n";
                    }
                } while (adminChoice != 3);
            }
            break;
        case 2: {
            loginSystem.userLogin();
            int userChoice;
            do {
                cout << "\nUser Dashboard\n";
                cout << "1. Search Products\n";
                cout << "2. Logout\n";
                cout << "Enter your choice: ";
                cin >> userChoice;

                switch (userChoice) {
                case 1: {
                    string category;
                    cin.ignore();
                    cout << "Enter category to search: ";
                    getline(cin, category);
                    auto products = marketplace.searchProducts(category);
                    if (products.empty()) {
                        cout << "No products found in this category.\n";
                    } else {
                        marketplace.sortProductsByPrice(products);
                        cout << "Products in category: " << category << "\n";
                        for (const auto &product : products) {
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
                    cout << "Invalid choice. Try again.\n";
                }
            } while (userChoice != 2);
            break;
        }
        case 3:
            cout << "Exiting the system. Goodbye!\n";
            return 0;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}
