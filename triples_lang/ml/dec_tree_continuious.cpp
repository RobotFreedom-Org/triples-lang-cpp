/*


*/
 
#include <vector>
#include <iostream>
#include <fstream> 
#include <string> 
using namespace std;

struct NodeDT  {
    bool is_leaf = false;
    int prediction = -1;       // Used only if is_leaf is true
     
    // Splitting criteria (used if is_leaf is false)
    int feature_index = -1;    
    double threshold = 0.0;    
    
    // Indices pointing to child nodes inside the flat std::vector<NodeDT>
    int left_child_idx = -1;   
    int right_child_idx = -1;  
};

 // Helper struct for holding dataset samples
struct Sample {
        std::vector<double> features;
        int label;
    };
  
class DecisionTreeContinuous{

private:
 
    int root_idx = -1; 

    // PTODO (add Information Gain or Gini)
    bool find_best_split(const std::vector<Sample>& data, int& out_feature, double& out_threshold) {
        
        // In a full implementation, you compute Gini/Entropy here. 
        if (data.empty()) return false;
        
        // Simple mock check: if all labels match, don't split
        bool all_same = true;
        for (const auto& s : data) {
            if (s.label != data[0].label) { all_same = false; break; }
        }
        if (all_same) return false;

        out_feature = 0;
        out_threshold = 0.5; 
        return true;
    }

    // Recursive helper to fit the tree using vector indices
    int build_tree_recursive(const std::vector<Sample>& data) {
        if (data.empty()) return -1;
 
        int current_idx = nodes.size();
        nodes.push_back(NodeDT());

        int feature;
        double threshold;
         
        if (!find_best_split(data, feature, threshold)) {
            // Make it a leaf node
            nodes[current_idx].is_leaf = true;
            nodes[current_idx].prediction = data[0].label; // Majority vote or first label
            return current_idx;
        }

        // 3. Populate splitting metadata
        nodes[current_idx].is_leaf = false;
        nodes[current_idx].feature_index = feature;
        nodes[current_idx].threshold = threshold;

        // 4. Partition the data
        std::vector<Sample> left_data;
        std::vector<Sample> right_data;
        for (const auto& sample : data) {
            if (sample.features[feature] <= threshold) {
                left_data.push_back(sample);
            } else {
                right_data.push_back(sample);
            }
        }

        // 5. Recursively build children and hook up indices
        // CRITICAL NOTE: Do not pass references like `nodes[current_idx].left_child_idx` 
        // directly as parameters because vector reallocation can invalidate memory addresses!
        int left_idx = build_tree_recursive(left_data);
        int right_idx = build_tree_recursive(right_data);

        nodes[current_idx].left_child_idx = left_idx;
        nodes[current_idx].right_child_idx = right_idx;

        return current_idx;
    }


public:

    std::vector<NodeDT> nodes;  

    void fit(const std::vector<Sample>& data) {
        nodes.clear(); 
        nodes.reserve(data.size() * 2); 
        root_idx = build_tree_recursive(data);
    }
  
   
    int predict(const std::vector<double>& features) const {

        if (root_idx == -1) return -1;

        int current_idx = root_idx;

        while (!nodes[current_idx].is_leaf) {
            const NodeDT& node = nodes[current_idx];
            if (features[node.feature_index] <= node.threshold) {
                current_idx = node.left_child_idx;
            } else {
                current_idx = node.right_child_idx;
            }
        }
        return nodes[current_idx].prediction;
    }
 
   void save( const std::string& filename) {
    std::ofstream out(filename, std::ios::binary);
    if (!out) throw std::runtime_error("Cannot open file for writing");

    // Write size first
    size_t size = nodes.size(); 

    out << nodes.size() << "\n";
    for (const auto& n : nodes) { 
       out << n.is_leaf << "\n" << n.prediction << "\n" << n.feature_index << "\n" << n.threshold  << "\n" << n.left_child_idx   << "\n" << n.right_child_idx  <<"\n";  

       }
    }
 
    bool stringToBool(const std::string& str) {
        return str == "1";
    }
    //std::vector<NodeDT> load(const std::string& filename) {
    bool load(const std::string& filename) {

        std::ifstream in(filename, std::ios::binary);
    
        if (!in) throw std::runtime_error("Cannot open file for reading");
     
        std::string line;

        in >> line;
        int size = std::stoi(line);
        std::vector<NodeDT> vec(size);//3); //(size); 

        for (auto& n : vec) {

            in >> line;  

           // std::getline(in, line);   
            n.is_leaf = stringToBool(line); 
            in.ignore();
            
            std::getline(in, line);    
            n.prediction = std::stoi(line);  

            std::getline(in, line);   
            n.feature_index = std::stoi(line);  

            std::getline(in, line); 
            n.threshold = std::stof(line); 

            std::getline(in, line);   
            n.left_child_idx = std::stoi(line); 
    
            std::getline(in, line);    
            n.right_child_idx = std::stoi(line);
        }
        nodes = vec;
        root_idx = 0;
        return true;
    }

};

int main() { 

    // Construct dummy data: binary classification based on Feature 0
    std::vector<Sample> dataset = {
        {{0.1, 1.2}, 0},
        {{0.2, 3.4}, 0},
        {{0.7, 0.5}, 1},
        {{0.9, 2.1}, 1}
    }; 
   
    DecisionTree tree;
    tree.fit(dataset);
    tree.save("test");

    DecisionTree tree2;
    bool loaded = tree2.load("test"); 


    // Predict values
    std::vector<double> test_sample_1 = {0.15, 2.0}; // Should be 0
    std::vector<double> test_sample_2 = {0.85, 1.1}; // Should be 1

    std::cout << "Prediction 1: " << tree2.predict(test_sample_1) << std::endl;
    std::cout << "Prediction 2: " << tree2.predict(test_sample_2) << std::endl;

    return 0;
}




