/*
KNN and Fuzzy Simplistic Sets

 Copyright (c) 2026 RobotFreedom.org 
 Author: RobotFreedom.org  
 License: MIT License
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <stdexcept>
 
struct Neighbor {
    int index;
    double distance;
};
 
using Matrix = std::vector<std::vector<double>>;
  

/* 
euclidean distance  
*/
double euclideanDistance(const std::vector<double>& a, const std::vector<double>& b) {
    if (a.size() != b.size()) {
        throw std::invalid_argument("Points must have the same dimension.");
    }
    double sum = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        double diff = a[i] - b[i];
        sum += diff * diff;
    }
    return std::sqrt(sum);
}
 
/*
 K nearest neighbors
 */
std::vector<std::vector<Neighbor>> compute_knn(const Matrix& data, int k) {

    if (k <= 0 || k >= (int)data.size()) {
        throw std::invalid_argument("k must be between 1 and N-1.");
    }

    std::vector<std::vector<Neighbor>> knn_graph(data.size());

    for (size_t i = 0; i < data.size(); ++i) {
        std::vector<Neighbor> distances;
        distances.reserve(data.size() - 1);

        for (size_t j = 0; j < data.size(); ++j) {
            if (i == j) continue;
            double dist = euclideanDistance(data[i], data[j]);
            distances.push_back({(int)j, dist});
        }

        /*
        Sort by distance and keep top k
        */
        std::partial_sort(distances.begin(), distances.begin() + k, distances.end(),
                          [](const Neighbor& a, const Neighbor& b) {
                              return a.distance < b.distance;
                          });

        distances.resize(k);
        knn_graph[i] = distances;
    }
    return knn_graph;
}

/* 
fuzzy simplicial set weights
*/
Matrix fuzzy_simplicial_set(const std::vector<std::vector<Neighbor>>& knn_graph, double local_connectivity = 1.0) {
    size_t n = knn_graph.size();
    Matrix weights(n, std::vector<double>(n, 0.0));

    for (size_t i = 0; i < n; ++i) {
        double rho = knn_graph[i][0].distance; // local connectivity offset
        double sigma = 0.0;

        // Binary search for sigma to achieve desired perplexity-like behavior
        double target = std::log2(knn_graph[i].size());
        double low = 1e-5, high = 1000.0;

        for (int iter = 0; iter < 50; ++iter) {
            sigma = (low + high) / 2.0;
            double sum = 0.0;
            for (const auto& nb : knn_graph[i]) {
                double val = std::exp(-(std::max(0.0, nb.distance - rho)) / sigma);
                sum += val;
            }
            if (std::abs(sum - target) < 1e-5) break;
            if (sum > target) high = sigma;
            else low = sigma;
        }

        // Assign weights
        for (const auto& nb : knn_graph[i]) {
            double weight = std::exp(-(std::max(0.0, nb.distance - rho)) / sigma);
            weights[i][nb.index] = weight;
        }
    }

    /*
     Symmetrize using fuzzy set union
     */
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            if (i != j) {
                double wij = weights[i][j];
                double wji = weights[j][i];
                weights[i][j] = wij + wji - wij * wji; // fuzzy union
            }
        }
    }

    return weights;
}


// Comparator to sort by distance
bool compareNeighbors(const Neighbor& a, const Neighbor& b) {
    return a.distance < b.distance;
}

 std::vector<Neighbor> findNeighbors(std::vector<std::vector<double>> trainingData, std::vector<double> queryPoint ) {
    // Example training data: each row is a point
    //std::vector<std::vector<double>> trainingData = {
    //    {1.0, 2.0}, {3.0, 4.0}, {6.0, 5.0}, {7.0, 8.0}, {9.0, 1.0}
   // };

    // Query point
    //std::vector<double> queryPoint = {5.0, 6.0};
    int k = 3; // number of nearest neighbors

    // Compute distances from query point to all training points
    std::vector<Neighbor> neighbors;
    for (size_t i = 0; i < trainingData.size(); ++i) {
        double dist = euclideanDistance(queryPoint, trainingData[i]);
        neighbors.push_back({static_cast<int>(i), dist});
    }

    // Sort by distance
    std::sort(neighbors.begin(), neighbors.end(), compareNeighbors);

    // Output top k neighbors
    std::cout << "Top " << k << " nearest neighbors:\n";
    for (int i = 0; i < k; ++i) {
        std::cout << "Index: " << neighbors[i].index
                  << ", Distance: " << neighbors[i].distance << "\n";
    }

    return neighbors;
}

/*
 Transform new data points using prior embeddings
*/
std::vector<std::vector<double>> transform(
    const std::vector<std::vector<double>>& train_data,
    const std::vector<std::vector<double>>& train_embedding,
    const std::vector<std::vector<double>>& new_points,
    size_t k = 5
) { 
    const size_t dim = train_data.size();
    const size_t embed_dim = train_embedding[0].size();
 
 
    std::vector<std::vector<double>> transformed;
    transformed.reserve(new_points.size());

    // For each new point
    //resultSet =  findNeighbors

    for (const auto& p : new_points) { 

        std::vector<size_t> ret_indexes(k);
        std::vector<double> out_dists_sqr(k); 

        // Weighted average in embedding space
        std::vector<double> embed(embed_dim, 0.0);
        double weight_sum = 0.0;

        for (size_t i = 0; i < k; ++i) {
            double dist = std::sqrt(out_dists_sqr[i]);
            double w = std::exp(-dist); // Gaussian-like weight
            weight_sum += w;
            for (size_t d = 0; d < embed_dim; ++d) {
                embed[d] += w * train_embedding[ret_indexes[i]][d];
            }
        }

        for (size_t d = 0; d < embed_dim; ++d) {
            embed[d] /= weight_sum;
        }

        transformed.push_back(embed);
    }

    return transformed;
}
 