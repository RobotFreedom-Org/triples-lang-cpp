/*
Not UMAP

 Copyright (c) 2026 RobotFreedom.org 
 Author: RobotFreedom.org  
 License: MIT License
 */


#include <vector>
#include <cmath>
#include <random>
#include <algorithm>
#include <iostream>
#include "knn_fuzzy.cpp"  
#include <string> 
#include <fstream>  


double clamp(double v, double lo, double hi) {
    return std::max(lo, std::min(v, hi));
}


// Structure representing a single training point's existing data
struct ReferencePoint {
    std::vector<double> high_dim; // Original space features
    std::vector<double> low_dim;  // Embedded space coordinates
};

 

class NUMAP {

private:   

// Computes Euclidean distance squared between two vectors
double compute_distance_sq(const std::vector<double>& a, const std::vector<double>& b) {
    double dist = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        double diff = a[i] - b[i];
        dist += diff * diff;
    }
    return dist;
}

// Transforms a new data point into the existing embedding space
std::vector<double> transform(
    const std::vector<double>& new_data,
    const std::vector<ReferencePoint>& reference_set,
    int k_neighbors, 
    int epochs, 
    double learning_rate,
    double min_dist = 0.1) 
{
    size_t num_refs = reference_set.size();
     

    // 1. Find the k-nearest neighbors in high-dimensional space
    // Stores pairs of (distance_squared, reference_index)
    std::vector<std::pair<double, size_t>> neighbors;
    for (size_t i = 0; i < num_refs; ++i) {
        double d_sq = compute_distance_sq(new_data, reference_set[i].high_dim);
        neighbors.push_back({d_sq, i});
    } 

    std::sort(neighbors.begin(), neighbors.end());
    neighbors.resize(k_neighbors);
 
    // 2. Convert raw metric distances into a fuzzy simplicial set local neighborhood
    double rho = neighbors[0].first; // Distance to the nearest neighbor
    double sigma = 0.0;             // Scale parameter for the local fuzzy metric
    
    // Binary search/approximation for sigma to satisfy the local connectivity constraint
    double target = std::log2(k_neighbors);
    double low = 0.0, high = 1e6;
    for (int iter = 0; iter < 64; ++iter) {
        double mid = (low + high) / 2.0;
        double sum = 0.0;
        for (const auto& n : neighbors) {
            double val = (n.first - rho) / mid;
            sum += std::exp(-std::max(0.0, val));
        }
        if (sum > target) high = mid;
        else low = mid;
    }
    sigma = (low + high) / 2.0;
 

    // Compute the high-dimensional fuzzy membership weights (fuzzy simplicial 1-skeleton)
    std::vector<double> fuzzy_weights(k_neighbors);
    for (int i = 0; i < k_neighbors; ++i) {
        double val = (neighbors[i].first - rho) / sigma;
        fuzzy_weights[i] = std::exp(-std::max(0.0, val));
    }
 
    // 3. Initialize new point's low-dimensional coordinates 

    std::vector<double> embedded_coord(reference_set[0].low_dim.size(), 0.0);
 

    for (const auto& n : neighbors) {
        for (size_t d = 0; d < embedded_coord.size(); ++d) { 
            embedded_coord[d] += reference_set[n.second].low_dim[d];
        }
    }
 
    for (double& val : embedded_coord) val /= k_neighbors;
 
    // Hyperparameters matching standard t-distribution curves for low-dimensional layout
    double a = 1.0; 
    double b = 1.0; 

    // 4. Optimize coordinates using Fuzzy Cross-Entropy Gradient Descent
    for (int epoch = 0; epoch < epochs; ++epoch) {
        // Decay the learning rate linearly over training
        double current_lr = learning_rate * (1.0 - static_cast<double>(epoch) / epochs);
 

        for (int i = 0; i < k_neighbors; ++i) {
            size_t ref_idx = neighbors[i].second;
            double p = fuzzy_weights[i]; // High-dim fuzzy membership strength

            // Get distance in the low-dimensional embedding space
            double q_dist_sq = compute_distance_sq(embedded_coord, reference_set[ref_idx].low_dim);
            
            // Low-dimensional fuzzy membership function (Student-t style)
            double q = 1.0 / (1.0 + a * std::pow(q_dist_sq, b));

            // Calculate gradient weight from fuzzy cross-entropy loss
            // Mix of attraction (if p is large) and repulsion forces
            double grad_coeff = 0.0;
            if (p > 0.0) {
                grad_coeff += p * 2.0 * b / (q_dist_sq + 1e-4);
            }
            grad_coeff -= (1.0 - p) * 2.0 * b / ((1.0 - q + 1e-4) * (q_dist_sq + 1e-4));

            // Apply the gradient updates to the new point's coordinate layout
            for (size_t d = 0; d < embedded_coord.size(); ++d) {
                double grad = grad_coeff * (embedded_coord[d] - reference_set[ref_idx].low_dim[d]);
                // Clip gradient to prevent explosion
                grad =  clamp(grad, -4.0, 4.0); 
                embedded_coord[d] -= current_lr * grad;
            }
        }
    }

    return embedded_coord;
}


// Transforms a new data point into the existing embedding space
std::vector<double> generate(
    const std::vector<double>& new_data,
    const std::vector<ReferencePoint>& reference_set,
    int k_neighbors, 
    int epochs, 
    double learning_rate,
    double min_dist = 0.1) 
{
    size_t num_refs = reference_set.size();
     

    // 1. Find the k-nearest neighbors in high-dimensional space
    // Stores pairs of (distance_squared, reference_index)
    std::vector<std::pair<double, size_t>> neighbors;
    for (size_t i = 0; i < num_refs; ++i) {
        double d_sq = compute_distance_sq(new_data, reference_set[i].low_dim);
        neighbors.push_back({d_sq, i});
    } 

    std::sort(neighbors.begin(), neighbors.end());
    neighbors.resize(k_neighbors);
 
    // 2. Convert raw metric distances into a fuzzy simplicial set local neighborhood
    double rho = neighbors[0].first; // Distance to the nearest neighbor
    double sigma = 0.0;             // Scale parameter for the local fuzzy metric
    
    // Binary search/approximation for sigma to satisfy the local connectivity constraint
    double target = std::log2(k_neighbors);
    double low = 0.0, high = 1e6;
    for (int iter = 0; iter < 64; ++iter) {
        double mid = (low + high) / 2.0;
        double sum = 0.0;
        for (const auto& n : neighbors) {
            double val = (n.first - rho) / mid;
            sum += std::exp(-std::max(0.0, val));
        }
        if (sum > target) high = mid;
        else low = mid;
    }
    sigma = (low + high) / 2.0;
 

    // Compute the high-dimensional fuzzy membership weights (fuzzy simplicial 1-skeleton)
    std::vector<double> fuzzy_weights(k_neighbors);
    for (int i = 0; i < k_neighbors; ++i) {
        double val = (neighbors[i].first - rho) / sigma;
        fuzzy_weights[i] = std::exp(-std::max(0.0, val));
    }
 
    // 3. Initialize new point's low-dimensional coordinates
    // Commonly initialized as the average low-dim coordinates of its nearest neighbors

    std::vector<double> embedded_coord(reference_set[0].high_dim.size(), 0.0);
 

    for (const auto& n : neighbors) {
        for (size_t d = 0; d < embedded_coord.size(); ++d) { 
            embedded_coord[d] += reference_set[n.second].high_dim[d];
        }
    }
 
    for (double& val : embedded_coord) val /= k_neighbors;
 
    // Hyperparameters matching standard t-distribution curves for low-dimensional layout
    double a = 1.0; 
    double b = 1.0; 

    // 4. Optimize coordinates using Fuzzy Cross-Entropy Gradient Descent
    for (int epoch = 0; epoch < epochs; ++epoch) {
        // Decay the learning rate linearly over training
        double current_lr = learning_rate * (1.0 - static_cast<double>(epoch) / epochs);
 

        for (int i = 0; i < k_neighbors; ++i) {
            size_t ref_idx = neighbors[i].second;
            double p = fuzzy_weights[i]; // High-dim fuzzy membership strength

            // Get distance in the low-dimensional embedding space
            double q_dist_sq = compute_distance_sq(embedded_coord, reference_set[ref_idx].high_dim);
            
            // Low-dimensional fuzzy membership function (Student-t style)
            double q = 1.0 / (1.0 + a * std::pow(q_dist_sq, b));

            // Calculate gradient weight from fuzzy cross-entropy loss
            // Mix of attraction (if p is large) and repulsion forces
            double grad_coeff = 0.0;
            if (p > 0.0) {
                grad_coeff += p * 2.0 * b / (q_dist_sq + 1e-4);
            }
            grad_coeff -= (1.0 - p) * 2.0 * b / ((1.0 - q + 1e-4) * (q_dist_sq + 1e-4));

            // Apply the gradient updates to the new point's coordinate layout
            for (size_t d = 0; d < embedded_coord.size(); ++d) {
                double grad = grad_coeff * (embedded_coord[d] - reference_set[ref_idx].high_dim[d]);
                // Clip gradient to prevent explosion
                grad =  clamp(grad, -4.0, 4.0); 
                embedded_coord[d] -= current_lr * grad;
            }
        }
    }

    return embedded_coord;
} 
    std::vector<ReferencePoint>  model;  

public: 

   using DataPoint = std::vector<double>;
   using Dataset = std::vector<DataPoint>;  

   void  save(const std::string& FileName  )
   {
 
    std::ofstream out(FileName, std::ios::binary);
    if (!out) throw std::runtime_error("Cannot open file for writing");
 
    for (auto& refpt : model) {  
        for (auto& col : refpt.high_dim) {  
              out << col << "|";
        }
         out << ">";
        for (auto& col : refpt.low_dim) {  
              out << col << "|";
        }
         out << "\n";

    }
   }

  void load(const std::string& FileName  )   
  {
   std::vector<ReferencePoint> reference_set;
  
      
   FILE *filePtr;  
   filePtr = fopen(FileName.c_str(), "r"); 
   std::cout << "Loading Umap model  " << std::endl;

   char currentline[1000];  
   char currentval[1000];  
   int i;
   while (fgets(currentline , 1000, filePtr))
    {        
     char* org   = std::strtok(currentline  , ">");  
     char* embed = std::strtok(NULL,">");   

     ReferencePoint refpt; 
     i =0;
     char* currentval   = std::strtok(org  , "|");  
     while (currentval != NULL)
        {
          refpt.high_dim.push_back(atof(currentval));
          currentval   = std::strtok(NULL  , "|");  
        }

     currentval   = std::strtok(embed  , "|");  
     while (currentval != NULL)
        {
          refpt.low_dim.push_back(atof(currentval));
          currentval   = std::strtok(NULL  , "|");   
        }

      reference_set.push_back(refpt);

    }


  model = reference_set;
}
 
// Compute squared Euclidean distance between two points in the embedding
double squared_dist(const std::vector<double>& y1, const std::vector<double>& y2) {
    double dist = 0.0;
    for (size_t d = 0; d < y1.size(); ++d) {
        double diff = y1[d] - y2[d];
        dist += diff * diff;
    }
    return dist;
}

Dataset compute_high_dimensional_graph(const Dataset& data, int n_neighbors) { 

        auto knn_graph = compute_knn(data, n_neighbors);
        /*  
        for (auto& row : knn_graph) {
            for (auto& col: row) { 
               std::cout <<   col.index   << " " << col.distance  << "\n ";
           }
               std::cout <<  "\n";
         }
        */
        auto fuzzy_set = fuzzy_simplicial_set(knn_graph);
        return fuzzy_set;
}

struct FuzzyEdge {
    int source;
    int target;
    double weight; // μ_ij: membership strength from high-dimensional space
};


void optimize_fuzzy_simplicial_set( const std::vector<FuzzyEdge>& fuzzy_graph,
    std::vector<std::vector<double>>& embedding,
    int num_epochs,
    double learning_rate,
    int num_negative_samples,
    double a = 1.0,  
    double b = 1.0  ){

    int num_nodes = embedding.size();
    int num_dims = embedding[0].size();

    std::mt19937 rng(42);
    std::uniform_int_distribution<int> rand_node(0, num_nodes - 1);

    // Main optimization loop over epochs
    for (int epoch = 0; epoch < num_epochs; ++epoch) {
        // Slowly decay the learning rate across epochs
        double current_lr = learning_rate * (1.0 - static_cast<double>(epoch) / num_epochs);

        for (const auto& edge : fuzzy_graph) {
            int i = edge.source;
            int j = edge.target;
            double mu = edge.weight;

            // 1. Attractive Force (Applied to actual edges based on membership weight)
            double dist_sq = squared_dist(embedding[i], embedding[j]);
            
            // Calculate gradient scaling factor for attractive force
            // Derivative of fuzzy cross-entropy with respect to the low-dim distance curve
            double phi_attr = (mu * b) / (1.0 + a * std::pow(dist_sq, b));
            
            std::vector<double> grad_attr(num_dims, 0.0);
            for (int d = 0; d < num_dims; ++d) {
                double delta = embedding[i][d] - embedding[j][d];
                grad_attr[d] = current_lr * phi_attr * delta;
                embedding[i][d] -= grad_attr[d];
                embedding[j][d] += grad_attr[d];
            }

            // 2. Repulsive Force (Applied via negative sampling to simulate non-edges)
            for (int sample = 0; sample < num_negative_samples; ++sample) {
                int k = rand_node(rng);
                if (i == k) continue;

                double neg_dist_sq = squared_dist(embedding[i], embedding[k]);
                if (neg_dist_sq == 0.0) neg_dist_sq = 1e-6; // Avoid division by zero

                // Calculate gradient scaling factor for repulsive force
                double phi_rep = ((1.0 - mu) * b) / ((1e-4 + neg_dist_sq) * (1.0 + a * std::pow(neg_dist_sq, b)));

                for (int d = 0; d < num_dims; ++d) {
                    double delta = embedding[i][d] - embedding[k][d];
                    double grad_rep = current_lr * phi_rep * delta;
                    embedding[i][d] += grad_rep; // Repel i from k
                }
            }
        }
    }
}


Dataset train(NUMAP::Dataset data ) {

    std::vector<double>  results;
    NUMAP::Dataset fuzzy_data = compute_high_dimensional_graph(data, 3); 
 
    const size_t n = data.size();   
    
    double lower = -5.5, upper = 5.5; 
    std::uniform_real_distribution<double> dist(lower, upper);
    std::default_random_engine engine;  
    Dataset embeddings  ; 
    embeddings.resize(data.size());     
    int i = 0;
    for (auto& row : data) { 
        double emd1  =  dist(engine);
        double emd2  =  dist(engine);
        double emd3  =  dist(engine); 
        embeddings[i].push_back(emd1);
        embeddings[i].push_back(emd2);
        embeddings[i].push_back(emd3);   
        i ++;
    }; 
 
    std::vector<ReferencePoint>  reference_set; 

    for (auto& row : data) { 
          ReferencePoint refpt; 
          for (auto& w: row) { 
             refpt.high_dim.push_back(w);
          }
          reference_set.push_back(refpt);
        }

    std::vector<FuzzyEdge> fuzzy_graph ; 
     int u = 0;
     int v = 0;
     for (auto& row : fuzzy_data) {
          v ++;
          u = 0; 
          for (auto& w: row) {

             if (u != v)
              { 
               fuzzy_graph.push_back({u, v, w});
                 }
             u++; 
        } 
      }
 
    std::cout << "optimize_fuzzy_simplicial_set " << "\n"; 

    optimize_fuzzy_simplicial_set(fuzzy_graph, embeddings, 500, 1.0, 3);

    std::cout << "\nOptimized Coordinates:\n";

    for (size_t i = 0; i < embeddings.size(); ++i) { 
 
        reference_set[i].low_dim.push_back(embeddings[i][0]);
        reference_set[i].low_dim.push_back(embeddings[i][1]);
        reference_set[i].low_dim.push_back(embeddings[i][2]); 
    }
   
    model = reference_set;
    

    return embeddings;
   }


   std::vector<double> fit( std::vector<double> data) {
     std::vector<double>  results;
     results =  transform(  data, model, 5, 60, 0.1) ;
     return results;
   }
   
   std::vector<double> decode(std::vector<double> data) { 
     std::vector<double> embedded = generate( data, model, 1, 60, 0.1) ;
     return embedded;
   }
 
} ;
// note decode if fit in reverse
/* 
 go from low to high
struct ReferencePoint {
    std::vector<double> high_dim; // Original space features
    std::vector<double> low_dim;  // Embedded space coordinates
};

*/


int test_numap() {

     NUMAP numap;

     NUMAP::Dataset data = {{1.0, 2.0, 5.0,4.5, 9}, {1.5, 1.8, 4.8,3.8, 7}, {8.0, 9.0, 1.0,2.1, 4} , 
{.9, 1.4, 4.0, 4.9, 7}, {1.1, 2.1, 5.1, 3.2, 5}, {6.0, 7.0, .1, .61, 1.2} };

  
    NUMAP::Dataset  embeddings;
  
    embeddings = numap.train(data);

    for (size_t i = 0; i < embeddings.size(); ++i) {
        std::cout << "Node " << i << ": (" << embeddings[i][0] << ", " << embeddings[i][1] << ", " << embeddings[i][2] << ")\n";
    }

     std::vector<double> new_data = {1.1, 1.9, 4.8,4.4, 8.7} ; 

     std::vector<double> embedded = numap.fit( new_data) ;

    for (auto& col : embedded) { 
        std::cout <<  col << " "; 
    }
        std::cout <<  "\n"; 

     numap.save("umap.refpt");

     numap.load("umap.refpt");

     std::vector<double> sim_data = embedded; //{8.34868, 5.58223, -12.4776 } ; 
     std::vector<double> gen      = numap.decode( sim_data) ;

    for (auto& col : gen) { 
        std::cout <<  col << " "; 
    }

    std::cout <<  "\n";    

    return 0;
  
}
