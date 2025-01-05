#include <iostream>
#include <string>
#include <fstream>

using namespace std;

// Function to convert string to lowercase
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

// Structure to store admin credentials
struct Admin
{
    string name;
    int password; // 5-digit positive integer password
};

// Node structure for linked list
struct ItemNode
{
    string name;
    ItemNode *next;
    ItemNode(const string &itemName) : name(itemName), next(nullptr) {}
};

// Recycling System
class RecyclingSystem
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
        else
        {
            cerr << "Unable to open file for saving items.\n";
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
        else
        {
            cerr << "Unable to open file for loading items.\n";
        }
    }

public:
    RecyclingSystem() : recyclableHead(nullptr), nonRecyclableHead(nullptr)
    {
        loadItemsFromFile("recyclable_items.txt", recyclableHead);
        loadItemsFromFile("non_recyclable_items.txt", nonRecyclableHead);
    }

    ~RecyclingSystem()
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

// Login System
class LoginSystem
{
private:
    Admin admins[4];
    int adminCount;

public:
    LoginSystem()
    {
        adminCount = 4;
        admins[0] = {"adarsh", 12345};
        admins[1] = {"bhargavi", 54321};
        admins[2] = {"shreyas", 67890};
        admins[3] = {"shubham", 67890};
    }

    // Function to authenticate an admin
    bool adminLogin()
    {
        string adminName;
        int password;

        cout << "Enter Admin ID (Name): ";
        cin.ignore(); // Clear newline from previous input
        getline(cin, adminName);
        toLowercase(adminName); // Convert admin name to lowercase

        cout << "Enter Password (5-digit positive integer): ";
        cin >> password;

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
                return true;
            }
        }

        cout << "Invalid Admin ID or Password.\n";
        return false;
    }
};

// Main Program
int main()
{
    LoginSystem loginSystem;
    RecyclingSystem recyclingSystem;

    while (true)
    {
        int role;
        cout << "\nWelcome to the Waste Disposal and Recycling Recommendation System\n";
        cout << "Are you an:\n";
        cout << "1. Admin\n";
        cout << "2. User\n";
        cout << "3. Exit\n";
        cout << "Enter your role: ";
        cin >> role;

        if (role == 3)
        {
            cout << "Exiting..\n";
            break;
        }

        if (role == 1)
        {
            if (loginSystem.adminLogin())
            {
                int choice;
                do
                {
                    cout << "\nAdmin Menu\n";
                    cout << "1. Add new recyclable item\n";
                    cout << "2. Add new non-recyclable item\n";
                    cout << "3. Delete recyclable item\n";
                    cout << "4. Delete non-recyclable item\n";
                    cout << "5. Modify recyclable item\n";
                    cout << "6. Modify non-recyclable item\n";
                    cout << "7. Display recyclable items\n";
                    cout << "8. Display non-recyclable items\n";
                    cout << "9. Back to Main Menu\n";
                    cout << "Enter your choice: ";
                    cin >> choice;

                    cin.ignore();
                    switch (choice)
                    {
                    case 1:
                    {
                        string itemName;
                        cout << "Enter new recyclable item name: ";
                        getline(cin, itemName);
                        recyclingSystem.addRecyclableItem(itemName);
                        break;
                    }
                    case 2:
                    {
                        string itemName;
                        cout << "Enter new non-recyclable item name: ";
                        getline(cin, itemName);
                        recyclingSystem.addNonRecyclableItem(itemName);
                        break;
                    }
                    case 3:
                    {
                        string itemName;
                        cout << "Enter recyclable item name to delete: ";
                        getline(cin, itemName);
                        recyclingSystem.deleteRecyclableItem(itemName);
                        break;
                    }
                    case 4:
                    {
                        string itemName;
                        cout << "Enter non-recyclable item name to delete: ";
                        getline(cin, itemName);
                        recyclingSystem.deleteNonRecyclableItem(itemName);
                        break;
                    }
                    case 5:
                    {
                        string oldName, newName;
                        cout << "Enter recyclable item name to modify: ";
                        getline(cin, oldName);
                        cout << "Enter new name for the item: ";
                        getline(cin, newName);
                        recyclingSystem.modifyRecyclableItem(oldName, newName);
                        break;
                    }
                    case 6:
                    {
                        string oldName, newName;
                        cout << "Enter non-recyclable item name to modify: ";
                        getline(cin, oldName);
                        cout << "Enter new name for the item: ";
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
                        cout << "Returning to main menu.\n";
                        break;
                    default:
                        cout << "Invalid choice!\n";
                    }
                } while (choice != 9); // Breaks out to main menu after choice 9
            }
        }
        else if (role == 2)
        {
            int choice;
            do
            {
                cout << "\nUser Menu\n";
                cout << "1. Display recyclable items\n";
                cout << "2. Display non-recyclable items\n";
                cout << "3. Check if an item is recyclable\n";
                cout << "4. Check if an item is non-recyclable\n";
                cout << "5. Exit\n";
                cout << "Enter your choice: ";
                cin >> choice;

                cin.ignore();
                switch (choice)
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
                    getline(cin, itemName);
                    if (recyclingSystem.isRecyclable(itemName) != "")
                        cout << itemName << " is recyclable.\n";
                    else
                        cout << itemName << " is not recyclable.\n";
                    break;
                }
                case 4:
                {
                    string itemName;
                    cout << "Enter item name to check if it's non-recyclable: ";
                    getline(cin, itemName);
                    if (recyclingSystem.isNonRecyclable(itemName) != "")
                        cout << itemName << " is non-recyclable.\n";
                    else
                        cout << itemName << " is not non-recyclable.\n";
                    break;
                }
                case 5:
                    cout << "Exiting user menu.\n";
                    break;
                default:
                    cout << "Invalid choice!\n";
                }
            } while (choice != 5); // Breaks out to main menu after choice 5
        }
        else
        {
            cout << "Invalid choice! Please enter 1, 2, or 3.\n";
        }
    }

    return 0;
}
