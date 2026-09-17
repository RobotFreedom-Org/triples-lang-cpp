#include <iostream>
#include <list>
#include <string>
#include <stdexcept> 
#include <vector> 
#include <sstream> 
#include <type_traits> 
#include <map>
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
 
class Archives {

private: 
     map<string, vector<int> > corpus;
     //
     map<string, int> vocab;
     map<int, string> tokens;
     map<int, int> metrics; 

     //Todo create 3d embeddings based on UMap
     map<int, vector<int>> embeddings; 
     map<int, vector<int>> links;


public:

   vector<int> similar(vector<int> doc)
   {
    vector<int> results;
    int matchs ;
    int in_size= doc.size(); 
 

        float cutoff = in_size*.8;
    for (auto &kv : embeddings) {
       matchs = 0; 

       for (int tok  : kv.second ) 
        {  
 
             auto it = std::find(doc.begin(), doc.end(), tok);
             if (it != doc.end())
                 matchs = matchs + 1;
        } 
  
        if (matchs > cutoff)
        {
 
            results.push_back(kv.first);
 
        }

     } 
     return results;
   }

   int exists(string word )
   {

         auto it = vocab.find(word);  

         if (it != vocab.end()) 
         {
            return it->second ; 
         } else {
            return -1;
         }
   }

  vector<string> parse(const std::string& text) {
    std::vector<std::string> words;
    std::string word;
    for (char c : text) {
        if (isalnum(c)) {
            word += c;
        } else {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        }
    }
    if (!word.empty())   words.push_back(word);
    return words;
   }


   vector<int> tokenizer(vector<string> doc, bool add =false)
   {
     vector<int> results ; 
     int id;
     for (const auto& f : doc) { 
        
      if (f != " ")
        {
         auto it = vocab.find(f);  

         if (it != vocab.end()) 
         {
           id  = it->second ;
           if (add)
           { 
            metrics[id] = metrics[id] +1;
           } 
         } else { 
           if (add)
           {
               id =  vocab.size()  ;
               vocab[f]    = id;
               tokens[id]  = f;
               metrics[id] = 1;
           }
         }  
           results.push_back(id);
        }
    } 


     return results;

   }
  
   vector<string> decode(vector<int> doc )
   {
     vector<string> results ; 
     string val;
 
     for (const auto& f : doc) {  
 
         auto it = tokens.find(f);  

          std::cout << f  << std::endl;
         if (it != tokens.end()) 
         {
           val  = it->second ;  
           results.push_back(val);  
         } 
         else
         {
            val = "";
         }
      }   
     return results;

   }
 

   int add(  vector<string> doc, string label="general")
   {

     vector<int> toks =  tokenizer(doc,true);

     int id   = embeddings.size() ;
     embeddings[id] = toks;
     corpus[label].push_back(id);

     return id;
   }

   void link(int from, int to, string label="link")
   {
 
     links[from].push_back(to); 

   }

   vector<int> linked( int from )
   { 
     return links[from];  
   } 


   vector<string>  retrieve(int archID)
   {
       vector<int> res = embeddings[archID]; 
       return decode(res);
   }

   vector<string>  match(vector<string> mess, string corpusLabel="all")
   {
       vector<int> input = tokenizer(mess);  

       vector<int> res = similar(input); 
       // vector<string> fin; 
       //  fin = decode(res); 
       return decode(res);
   }

    vector<string>  definition(vector<string> mess)
   {
        
       vector<int> input = tokenizer(mess);    

       int archID;
       int doc_id;

       vector<int> res1 = similar(input);
       doc_id = res1.front();  

       vector<int> res2 = links[doc_id];
       archID = res2.front(); 
       vector<int> res3 = embeddings[archID]; 
 
       return decode(res3);
   }   

}; 
 
class Memory {


private:

   map<string, int> ids; 

   //signal
   map<int, vector<float>> signals;
   map<int,float> signal;

   //record
   map<int, vector<string>> records;
   map<int,string> record;

   int get(string key)
   { 
    auto it =  ids.find(key);
    int v = it->second; 
  
    if (it != ids.end()) 
        {
           return it->second ;
   } else {
            return  -1;
         } 
   }

 public:

    int exists(string key )
    {
       return get(key); 
    } 

    void add_record(string key, string value)
    {
        int id = get(key);
        if(id == -1)
        { 
          id = ids.size()  ;
          ids[key] = id; 
        }
        record[id] = value;
    } 

    string get_record(string key )
    {
        int id = get(key);
        return record[id]  ;
    }  

    void add_records(string key, string value)
    {
        int id = get(key);
        if(id == -1)
        { 
          id = ids.size() ;
          ids[key] = id; 
        }
        records[id].push_back(value);
    } 

    vector<string> get_records(string key )
    {
        int id = get(key);
        return records[id] ;
    } 

    void add_signal(string key, float value)
    {
        int id = get(key);
        if(id == -1)
        { 
          id = ids.size() ;
          ids[key] = id; 
        }
        signal[id] = value;
    }

    float get_signal(string key )
    {
        int id = get(key);
        return signal[id]  ;
    } 


    void add_signals(string key, float value)
    {
        int id = get(key);
        if(id == -1)
        { 
          id = ids.size()  ;
          ids[key] = id; 
        }
        signals[id].push_back(value);
    } 

    vector<float> get_signals(string key )
    {
        int id = get(key);
        return signals[id] ;
    } 

};

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
 

class Graph {

private:
  
    std::vector<std::map<int, int>> adj;   

    std::vector<std::map<string, string>> vert; 
    std::map<string, int> rev_vert; 
    std::vector<std::map<string, int>> verti; 

    std::vector<std::map<string, int>> epropi;  
    std::vector<std::map<string, string>> eprops;  
     

public:
    /*
     Constructor: initialize with given number of vertices
     */
    Graph(size_t vertices) : adj(vertices) {
        if (vertices == 0) {
            throw std::invalid_argument("Graph must have at least one vertex.");
        }
    }


    /* 
    Add an vert  
    */
    int addVertex( string label, string vclass ="vert" ) { 

        int vert_id = vert.size() ;
 
        map<string,string> prop;
        prop["label"] = label;   
        prop["class"] = vclass;   
        vert.push_back(prop);
        
        rev_vert[label] = vert_id;
 
        return vert_id;
    }

    string property(int vert_id, string prop_label)
    {

        auto it =  vert[vert_id].find(prop_label);

        if (it != vert[vert_id].end()) 
        {
           return it->second ;
         } else {
            return "";
         } 
    }

    // Search for a value by key
    int exists(string label) const { 
       // for (const auto &kv : vert) { 
        auto it = rev_vert.find(label);

        if (it != rev_vert.end()) 
        {
           return it->second ;
         } else {
            return -1;
         }
    }

    /* 
    Add an edge  
    */
    void addEdge(int u, int v, int w = 1, string eclass ="edge", bool undirected = false) {
        if (u < 0 || v < 0 || u >= (int)adj.size() || v >= (int)adj.size()) {
            throw std::out_of_range("Vertex index out of range.");
        } 

        int edge_id = eprops.size() ;

        map<string,string> prop;
 
        prop["weight"] = w;
        prop["class"]  = eclass;
        eprops.push_back(prop);  

        adj[u][v] = edge_id;  

        if (undirected) {
            adj[v][u] = edge_id;
        }
    }

    /*
     Remove an edge
     */
    void removeEdge(int u, int v, bool undirected = false) {
        if (u < 0 || v < 0 || u >= (int)adj.size() || v >= (int)adj.size()) {
            throw std::out_of_range("Vertex index out of range.");
        }
        adj[u].erase(v);
        if (undirected) {
            adj[v].erase(u);
        }
    }

    /* 
    Print adjacency table
    */
    void print() const {
        for (size_t u = 0; u < adj.size(); ++u) {
            std::cout << u << ": ";
            for (const auto &p : adj[u]) {
                std::cout << "(" << p.first << ", w=" << p.second << ") ";
            }
            std::cout << "\n";
        }
    }

    //  const  vector<int> neighbors(int u, n_degree =2)


    const std::map<int, int>& edges(int u) const {
        if (u < 0 || u >= (int)adj.size()) {
            throw std::out_of_range("Vertex index out of range.");
        }
        return adj[u];
    }
 
    // Search Ego net 
    const  vector<int> egoNet(int u, string vclass, int weight=-1) const {

        if (u < 0 || u >= (int)adj.size()) {
            throw std::out_of_range("Vertex index out of range.");
        }

         vector<int> results;
         std::map<string, string> _p ;

       // for (const auto& [to, eid] : adj[u]) { 

         for (const auto &p : adj[u]) {
 
            _p = eprops[p.second];
 
            if (_p["class"] == vclass)
            { 
                results.push_back(p.first);
            }

        }
             
        return  results ;


    }

  //vector<float> overlap

  //vector<float> infer

  //save

  //load

  //import

  void bfs(const std::vector<std::map<int, int>>& graph, int start) {
    if (start < 0 || start >= static_cast<int>(graph.size())) {
        throw std::out_of_range("Start node index is out of range.");
    }

    std::vector<bool> visited(graph.size(), false);
    std::queue<int> q;

    visited[start] = true;
    q.push(start);

    std::cout << "BFS Traversal starting from node " << start << ": ";

    while (!q.empty()) {
        int node = q.front();
        q.pop();
        std::cout << node << " ";

        // Iterate over neighbors in the map
        // for (const auto& [neighbor, weight] : graph[node]) {
       for (const auto &p : adj[node]) {
            (void)p.second; // Weight is unused in BFS traversal
            if (!visited[p.first]) {
                visited[p.first] = true;
                q.push(p.first);
            }
        }
    }
    std::cout << "\n";
}

};

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
