/*
 Very Simple C Decsion Tree

 Orgional Copyright (c) 2001 AnalysticalWay.com 
 Author: RobotFreedom.org  
 License: MIT License
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE. 
*/
 
#include <stdexcept>
#include <string> 
#include <iostream>
#include <vector>

#include <map>
#include <cmath>
#include <limits>
#include <set>

using namespace std;

/*
 decision tree nodes
*/
struct Node {

    bool isLeaf;   
    string label;  
    string feature; 
    map<string, Node*> children; 

    int levelid ;    
    Node* Parent;

    Node() : isLeaf(false) {} 
};


void tokenize(char *line) {
    char *cmd = strtok(line, "=");
    while (cmd != NULL) {
        printf("%s\n", cmd);
        cmd = strtok(NULL, "=");
    }
}

// Function to classify a new instance
Node* load_tree(string FileName) {

    Node* tree;

    Node* node = new Node();
    Node* parent = NULL;
    string output;

    char *buffer ;
    FILE *filePtr;  
  
    char currentline[1000];  
    int i = 0;
    size_t len;
    int level_id  = 0;


    filePtr = fopen(FileName.c_str(), "r"); 
    cout << "Loading Tree  " << endl;

    while (fgets(currentline , 1000, filePtr))
    {       
       i ++; 
 
       len = strlen(currentline); 
       Node* node = new Node();

       buffer = strtok(currentline, "=");      
       buffer = strtok(NULL, "=");  
       level_id = atoi(buffer);
 
       if (parent == NULL) 
       {
          tree   = node;
          parent = node;  
 
          parent->levelid = 1;
          node->levelid = 1;

         // cout << "level id used " << endl;
       }
       else
       { 

        node->levelid = level_id; 
 
        int par_ent = parent->levelid  +1;

        while (level_id != par_ent)
        {  
           Node*  _parent  = parent->Parent  ;   
           parent = _parent;
           par_ent = parent->levelid +1 ;   
        }  
        node->Parent = parent; 
       }  

       fgets(currentline , 1000, filePtr); 
       len = strlen(currentline); 
       buffer = strtok(currentline, "=");      
       buffer = strtok(NULL, "=");    
       string feature = buffer;
       feature.erase(std::remove(feature.begin(), feature.end(), '\n'), feature.end()); 
       node->feature = feature; 

       fgets(currentline , 1000, filePtr); 
       len = strlen(currentline); 
       buffer = strtok(currentline, "=");     
       buffer = strtok(NULL, "=");    
       string label = buffer;
       label.erase(std::remove(label.begin(), label.end(), '\n'), label.end());
       node->label = label;   

       fgets(currentline , 1000, filePtr); 
       len = strlen(currentline);  
       buffer = strtok(currentline, "=");      
       buffer = strtok(NULL, "=");  
       string isleaf = buffer;  
       isleaf.erase(std::remove(isleaf.begin(), isleaf.end(), '\n'), isleaf.end()); 
 
       if (strcmp(isleaf.c_str(), "yes")  == 0) 
       {
           node->isLeaf = true;
       }
       else
       {
           node->isLeaf = false;
       } 


       fgets(currentline , 1000, filePtr); 
       len = strlen(currentline); 
       buffer = strtok(currentline, "=");      
       buffer = strtok(NULL, "=");  
       string split = buffer;  
       split.erase(std::remove(split.begin(), split.end(), '\n'), split.end()); 

       if   (split != "") 
       { 
           parent->children[split] = node;
           if (node->isLeaf == false)
           {
            parent = node;
           } 
       }
        
    }
 
 return tree;  
}
 
/* 
  //  output.append("level_id="); 
 //   output.append("feature=");  
  //  output.append("value=");  
  //  output.append("label="); 
    output.append("is_leaf=");  

    if (leaf )
       {output.append("yes\n");}
    else
       {output.append("no\n");     
        }     
        

    output.append("split=");  
*/

// Function to classify a new instance
string dectree_repr(Node* tree , int inodeid = 0, string split="") {

    string output;  
    inodeid ++;

    string sid = to_string(inodeid);  
    string feat;

    output.append("level_id=");
    output.append(to_string(inodeid)); 
    output.append("\n"); 

    output.append("feature=");
    feat = tree->feature;  
    output.append(feat); 
    output.append("\n");
   
    output.append("label=");
    string label  = tree->label; 
    output.append(label);
    output.append("\n") ;
    output.append("is_leaf="); 
    bool leaf  = tree->isLeaf;

    if (leaf )
       {output.append("yes\n");}
    else
       {output.append("no\n");     
        }     
        

    output.append("split="); 
    output.append(split);
    output.append("\n") ;

    for (auto &kv : tree->children) {  
       string children =  dectree_repr(kv.second, inodeid, kv.first); 
       output.append(children );   
     } 
  
    return output;  
}

 


/*
 entropy
*/ 
double entropy(const vector<string>& labels) {

    double ent = 0.0;
    map<string, int> freq;

    for (const auto& label : labels) {
        freq[label]++;
    }

    for (const auto& p : freq) {

        double prob = (double)p.second / labels.size();
        ent -= prob * log2(prob);
    }
    return ent;
}

// Function to split dataset by a feature value
void splitDataset( const vector<map<string, string>>& data,
                   const vector<string>& labels,
                   const string& feature,
                   map<string, vector<map<string, string>>>& subsets,
                    map<string, vector<string>>& subsetLabels ) {
    for (size_t i = 0; i < data.size(); ++i) {
        string value = data[i].at(feature);
        subsets[value].push_back(data[i]);
        subsetLabels[value].push_back(labels[i]);
    }
}

// Function to choose the best feature using Information Gain
string chooseBestFeature(
    const vector<map<string, string>>& data,
    const vector<string>& labels,
    const set<string>& features
) {
    double baseEntropy = entropy(labels);
    double bestGain = -numeric_limits<double>::infinity();
    string bestFeature;

    for (const auto& feature : features) {
        map<string, vector<map<string, string>>> subsets;
        map<string, vector<string>> subsetLabels;
        splitDataset(data, labels, feature, subsets, subsetLabels);

        double newEntropy = 0.0;
        for (const auto& kv : subsetLabels) {
            double prob = (double)kv.second.size() / labels.size();
            newEntropy += prob * entropy(kv.second);
        }

        double infoGain = baseEntropy - newEntropy;
        if (infoGain > bestGain) {
            bestGain = infoGain;
            bestFeature = feature;
        }
    }
    return bestFeature;
}

// Function to check if all labels are the same
bool allSame(const vector<string>& labels) {
    for (size_t i = 1; i < labels.size(); ++i) {
        if (labels[i] != labels[0]) return false;
    }
    return true;
}

// Recursive function to build the decision tree
Node* buildTree(
    const vector<map<string, string>>& data,
    const vector<string>& labels,
    set<string> features
) {
 
    Node* node = new Node();

    // If all labels are the same, make a leaf
    if (allSame(labels)) {
        node->isLeaf = true;
        node->label = labels[0];
        return node;
    }

    // If no features left, return majority label
    if (features.empty()) {
        map<string, int> freq;
        for (const auto& label : labels) freq[label]++;
        string majority;
        int maxCount = -1;
        for (const auto& p : freq) {
            if (p.second > maxCount) {
                maxCount = p.second;
                majority = p.first;
            }
        }
        node->isLeaf = true;
        node->label = majority;
        return node;
    }

    // Choose best feature
    string bestFeature = chooseBestFeature(data, labels, features);
    node->feature = bestFeature;
    features.erase(bestFeature);

    // Split dataset
    map<string, vector<map<string, string>>> subsets;
    map<string, vector<string>> subsetLabels;
    splitDataset(data, labels, bestFeature, subsets, subsetLabels);

    // Recursively build children
    for (const auto& kv : subsets) {
        node->children[kv.first] = buildTree(kv.second, subsetLabels[kv.first], features);
    }

    return node;
}

// Function to classify a new instance
string classify(Node* tree, const map<string, string>& instance) {
    if (tree->isLeaf) return tree->label;
    string value = instance.at(tree->feature);
    if (tree->children.count(value)) {
        return classify(tree->children[value], instance);
    }
    return "-1";  
}

// Function to free memory
void releaseMem(Node* tree) {

    for (auto& kv : tree->children) {
        releaseMem(kv.second);

    }
    delete tree;
}
 

int dectree_save(Node*, string FileName) {
    
   ofstream outFile(FileName +".DecTree"); 
   outFile << "sub|verb|obj\n"; 
   outFile.close();

   return -1;

}
 
Node* dectree_fit(vector<map<string, string>> data , vector<string> labels  , set<string> features ) {
   /*
   
   */
   Node* tree;
   try {  

        tree = buildTree(data, labels, features);
       // releaseMem(tree);
     }
    catch (const exception& e) {
      cerr << "Error: " << e.what() << endl;
    }

    return tree;
}

string dectree_inference(Node* tree ,  map<string, string> data   ) {
   /*
  

   */
   string output;
   try {    
         output =  classify(tree, data) ;
        //  releaseMem(tree);
     }
    catch (const exception& e) {
      cerr << "Error: " << e.what() << endl;
    } 
    return output;
}

