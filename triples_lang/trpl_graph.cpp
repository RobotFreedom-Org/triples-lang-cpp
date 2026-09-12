#include <iostream>
#include <list>
#include <string>
#include <stdexcept> 
#include <vector> 
#include <sstream> 
#include <type_traits> 
using namespace std;
 

double stringSimilarity(std::string first, std::string second) {
    int m = first.length();
    int n = second.length();
    int T[m+1][n+1];
    for (int i = 0; i <= m; i++) T[i][0] = i;
    for (int j = 0; j <= n; j++) T[0][j] = j;
    for (int i = 1; i <= m; i++)
        for (int j = 1; j <= n; j++)
            T[i][j] = first[i-1] == second[j-1] ? 0 : 1 +
                      std::min(T[i-1][j] + 1, std::min(T[i][j-1] + 1, T[i-1][j-1] + 1));
    return (m + n - T[m][n]) / (m + n);
}
 

class HashTableProp {
private:
    static const int TABLE_SIZE = 100; // Fixed size for simplicity
    list<pair<int, string>> table[TABLE_SIZE]; // Array of lists for chaining
    int CURRENTID = -1;

    // Simple hash function
    int hashFunction(int key) const {
        return key % TABLE_SIZE;
    }
 
public:
    // Insert key-value pair
    int nextID( ) {
       CURRENTID ++;
       return CURRENTID  ;
    }
    // Insert key-value pair
    void insert(int key, const string &value) {
        int index = hashFunction(key);

        // Check if key already exists, update value if found
        for (auto &kv : table[index]) {
            if (kv.first == key) {
                kv.second = value;
            //    cout << "Updated key " << key << " with new value.\n";
                return;
            }
        }

        // Otherwise, insert new key-value pair
        table[index].emplace_back(key, value);
        cout << "Inserted (" << key << ", " << value << ") into hash table.\n";
    }

    // Remove a key-value pair
    void remove(int key) {
        int index = hashFunction(key);
        for (auto it = table[index].begin(); it != table[index].end(); ++it) {
            if (it->first == key) {
                table[index].erase(it);
                cout << "Removed key " << key << " from hash table.\n";
                return;
            }
        }
        cout << "Key " << key << " not found.\n";
    }

    // Search for a value by key
    string retrieve(int key) const {
        int index = hashFunction(key);
        for (const auto &kv : table[index]) {
            if (kv.first == key) {
                return kv.second;
            }
        }
        throw runtime_error("Key not found.");
    }
  
    //  returns id for match
    int exists(string value) const {  
        for (int i = 0; i <  10; i++) {  
           for (const auto &kv : table[i]) {  
              if (value == kv.second) {
                  return kv.first;
              }
            } 
        }
      return -1;
    }

    // Search for a value by value similarity
    int similar(string value) const { 

        for (int i = 0; i <  10; i++) {  
           for (const auto &kv : table[i]) {
              //int simcnt =0;// editDist(value, kv.second);

              double sim = stringSimilarity(value, kv.second);
 
              if (sim > .6){
                  return kv.first;
              }
            } 
        }
      return -1;
    }


    // Display the hash table
    void display() const {
        for (int i = 0; i < TABLE_SIZE; ++i) {
            cout << i << ": ";
            for (const auto &kv : table[i]) {
                cout << "(" << kv.first << ", " << kv.second << ") -> ";
            }
            cout << "NULL\n";
        }
    }
};

class HashTableEdge {
private:
    static const int TABLE_SIZE = 10; // Fixed size for simplicity
    int CURRENTID = -1;
    list<pair<int, int>> table[TABLE_SIZE]; // Array of lists for chaining

    // Simple hash function
    int hashFunction(int key) const {
        return key % TABLE_SIZE;
    }

public:
    // Insert key-value pair
    int nextID( ) {
       return CURRENTID + 1;
    }

    void insert(int key, int value) {
        int index = hashFunction(key);

        // Check if key already exists, update value if found
        for (auto &kv : table[index]) {
            if (kv.first == key) {
                kv.second = value;
            //    cout << "Updated key " << key << " with new value.\n";
                return;
            }
        }

        // Otherwise, insert new key-value pair
        table[index].emplace_back(key, value);
        cout << "Inserted (" << key << ", " << value << ") into hash table.\n";
    }

    // Search for a value by key
    int retrieve(int key) const {
        int index = hashFunction(key);
        for (const auto &kv : table[index]) {
            if (kv.first == key) {
                return kv.second;
            }
        }
        throw runtime_error("Key not found.");
    }

    // Remove a key-value pair
    void remove(int key) {
        int index = hashFunction(key);
        for (auto it = table[index].begin(); it != table[index].end(); ++it) {
            if (it->first == key) {
                table[index].erase(it);
                cout << "Removed key " << key << " from hash table.\n";
                return;
            }
        }
        cout << "Key " << key << " not found.\n";
    }

    // Display the hash table
    void display() const {
        for (int i = 0; i < TABLE_SIZE; ++i) {
            cout << i << ": ";
            for (const auto &kv : table[i]) {
                cout << "(" << kv.first << ", " << kv.second << ") -> ";
            }
            cout << "NULL\n";
        }
    }
};


class HashTableAdjacency{ 

private:
    static const int TABLE_SIZE = 10; // Fixed size for simplicity
    static const int VECT_SIZE  = 1000; // Fixed size for simplicity
    int CURRENTID = -1;

    using PairType = std::pair<int, std::vector<int>>;
    std::list<PairType> table[TABLE_SIZE]; 
 
   // list<pair<int, vector<float> >> table[TABLE_SIZE]; // Array of lists for chaining 

    // Simple hash function
    int hashFunction(int key) const {
        return key % TABLE_SIZE;
    }

public:
    // Insert key-value pair
    int nextID( ) {
       return CURRENTID + 1;
    }

    void insert(int key, int value) {

        int index = hashFunction(key);

        // Check if key already exists, update value if found
        for (auto &kv : table[index]) {
            if (kv.first  == key) { 
                if (kv.second.size() >0) {
                    kv.second.push_back( value);
              //      cout << "Updated key " << key << " with new value.\n";
                    return;
                }
            } 
        }

        PairType p;
        p.first = key;
        //p.second.resize(1000, 0.0f); // Fill with 0.5
        p.second.push_back( value);
        table[index].push_back(p); 
        cout << "Inserted (" << key << ", " << value << ") into hash table.\n";
    }


    // Remove a key-value pair
    void remove(int key) {
        int index = hashFunction(key);
        for (auto it = table[index].begin(); it != table[index].end(); ++it) {
            if (it->first == key) {
                table[index].erase(it);
                cout << "Removed key " << key << " from hash table.\n";
                return;
            }
        }
        cout << "Key " << key << " not found.\n";
    }

    // Search for a value by key
    int exists(int key) const {
        int index = hashFunction(key);
        for (const auto &kv : table[index]) {
           // if (kv.second == key) {
                return kv.first;
        //    }
        }
        throw runtime_error("Key not found.");
    }

    // Search for a value by key
    vector<int> retrieve(int key) const {

        int index = hashFunction(key);
        for (const auto &kv : table[index]) {
            if (kv.first == key) {
                return kv.second;
            }
        }
        throw runtime_error("Key not found.");
    } 

};


class HashTableSig{

private:
    static const int TABLE_SIZE = 10; // Fixed size for simplicity
    static const int VECT_SIZE  = 1000; // Fixed size for simplicity
    int CURRENTID = -1;

    using PairType = std::pair<int, std::vector<float>>;
    std::list<PairType> table[TABLE_SIZE];

 
   // list<pair<int, vector<float> >> table[TABLE_SIZE]; // Array of lists for chaining 

    // Simple hash function
    int hashFunction(int key) const {
        return key % TABLE_SIZE;
    }

public:
    // Insert key-value pair
    int nextID( ) {
       return CURRENTID + 1;
    }

    void insert(int key, float value) {

        int index = hashFunction(key);

        // Check if key already exists, update value if found
        for (auto &kv : table[index]) {
            if (kv.first  == key) { 
                if (kv.second.size() >0) {
                    kv.second.push_back( value);
              //      cout << "Updated key " << key << " with new value.\n";
                    return;
                }
            } 
        }

        PairType p;
        p.first = key;
        //p.second.resize(1000, 0.0f); // Fill with 0.5
        p.second.push_back( value);
        table[index].push_back(p); 
        cout << "Inserted (" << key << ", " << value << ") into hash table.\n";
    }


    // Remove a key-value pair
    void remove(int key) {
        int index = hashFunction(key);
        for (auto it = table[index].begin(); it != table[index].end(); ++it) {
            if (it->first == key) {
                table[index].erase(it);
                cout << "Removed key " << key << " from hash table.\n";
                return;
            }
        }
        cout << "Key " << key << " not found.\n";
    }

    // Search for a value by key
    float exists(float key) const {
        int index = hashFunction(key);
        for (const auto &kv : table[index]) {
           // if (kv.second == key) {
                return kv.first;
        //    }
        }
        throw runtime_error("Key not found.");
    }

    // Search for a value by key
    vector<float> retrieve(int key) const {
        int index = hashFunction(key);
        for (const auto &kv : table[index]) {
            if (kv.first == key) {
                return kv.second;
            }
        }
        throw runtime_error("Key not found.");
    } 

};

class HashTableSigStr{

private:
    static const int TABLE_SIZE = 10; // Fixed size for simplicity
    static const int VECT_SIZE  = 1000; // Fixed size for simplicity
    int CURRENTID = -1;

    using PairType = std::pair<int, std::vector<string>>;
    std::list<PairType> table[TABLE_SIZE];

 
   // list<pair<int, vector<float> >> table[TABLE_SIZE]; // Array of lists for chaining 

    // Simple hash function
    int hashFunction(int key) const {
        return key % TABLE_SIZE;
    }

public:
    // Insert key-value pair
    int nextID( ) {
       return CURRENTID + 1;
    }

    void insert(int key, string value) {

        int index = hashFunction(key);

        // Check if key already exists, update value if found
        for (auto &kv : table[index]) {
            if (kv.first  == key) { 
                if (kv.second.size() >0) {
                    kv.second.push_back( value);
               //     cout << "Updated key " << key << " with new value.\n";
                    return;
                }
            } 
        }

        PairType p;
        p.first = key;
        //p.second.resize(1000, 0.0f); // Fill with 0.5
        p.second.push_back( value);
        table[index].push_back(p); 
        cout << "Inserted (" << key << ", " << value << ") into hash table.\n";
    }


    // Remove a key-value pair
    void remove(int key) {
        int index = hashFunction(key);
        for (auto it = table[index].begin(); it != table[index].end(); ++it) {
            if (it->first == key) {
                table[index].erase(it);
                cout << "Removed key " << key << " from hash table.\n";
                return;
            }
        }
        cout << "Key " << key << " not found.\n";
    }

    // Search for a value by key
    float exists(float key) const {
        int index = hashFunction(key);
        for (const auto &kv : table[index]) {
           // if (kv.second == key) {
                return kv.first;
        //    }
        }
        throw runtime_error("Key not found.");
    }

    // Search for a value by key
    vector<string> retrieve(int key) const {
        int index = hashFunction(key);
        for (const auto &kv : table[index]) {
            if (kv.first == key) {
                return kv.second;
            }
        }
        throw runtime_error("Key not found.");
    } 

};

/* 

class TriplesGraph {
private: 

    HashTable s_v;
    HashTable v_s;
    HashTable v_o;
    HashTable o_v;
    HashTable s_o;
    HashTable o_s;
    HashTable ht;
public: 
    void add(int s, int v, int o)
    {

        
    }


};

*/
/*
name mouse
purpose movement
emotion happy

chat name;
chat purpose;
char cat;

kg add cat->like;
chat cat;
*/
int maasasain() {


    int choice, keyId ,  valueID ;
    string key , value;
    HashTableProp resps  ;
    HashTableProp props  ;
    HashTableEdge edges  ;

    while (true) {
        cout << "\n--- Hash Table Menu ---\n";
        cout << "1. Insert\n2. Search\n3. Remove\n4. Display\n5. Similar\n6. Chat\n7. Exit\n";
        cout << "Enter choice: ";

        if (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        try {
            switch (choice) {
                case 1:

                    cout << "Enter key (string): "; 
                    cin >> key;
                    cout << "Enter value (string): ";
                    cin >> value; 
                    
                    keyId   = edges.nextID();
                    valueID = keyId + 1;
                    edges.insert(keyId, valueID);
                    edges.insert(valueID, keyId);
                    props.insert(keyId, key);
                    props.insert(valueID, value); 

                    break;

                case 2:
                    cout << "Enter key to search: ";
                    if (!(cin >> keyId)) {
                        throw invalid_argument("Key must be an integer.");
                    }
                    cout << "Value: " << edges.retrieve(keyId) << "\n";
                    break;

                case 3:
                    cout << "Enter key to remove: ";
                    if (!(cin >> keyId)) {
                        throw invalid_argument("Key must be an integer.");
                    }
                    edges.remove(keyId);
                    break;

                case 4:
                    edges.display();
                    props.display();
                    break;

                case 5:
                    cout << "Enter key to search: ";
                    cin >> key;
                    keyId = props.similar(key)  ; 
                    cout << "Closest: " << props.retrieve(keyId) << "\n";
                    break;

                case 6:
                    cout << "Enter key to search: ";
                    cin >> key;
                    keyId = props.similar(key)  ;
                    valueID = edges.retrieve(keyId) ;
                    cout << "Resp: " << props.retrieve(valueID) << "\n";

                    break;


                case 7:
                    cout << "Exiting...\n";
                    return 0;


                default:
                    cout << "Invalid choice. Try again.\n";
            }
        } catch (const exception &e) {
            cout << "Error: " << e.what() << "\n";
            cin.clear();
            cin.ignore(10000, '\n');
        }
    }
}
