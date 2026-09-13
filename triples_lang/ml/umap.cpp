#include <iostream>
#include <vector>
#include <random>
#include "umap/umap.hpp" // Include from umappp library

int main() {
    try {
        // Step 1: Generate synthetic dataset (100 points, 5 dimensions)
        size_t n_obs = 100;
        size_t n_dim = 5;
        std::vector<double> data(n_obs * n_dim);

        std::mt19937 rng(42); // Fixed seed for reproducibility
        std::normal_distribution<double> dist(0.0, 1.0);

        for (auto &val : data) {
            val = dist(rng);
        }

        // Step 2: Configure UMAP
        umappp::UMAP<double> umap;
        umap.set_num_neighbors(15);   // Typical default
        umap.set_min_dist(0.1);       // Controls tightness of clusters
        umap.set_num_epochs(200);     // Training iterations

        // Step 3: Run UMAP
        std::vector<double> output(n_obs * 2); // 2D embedding
        umap.run(
            n_obs,
            n_dim,
            data.data(),
            output.data()
        );

        // Step 4: Print first 5 results
        std::cout << "First 5 embedded points:\n";
        for (size_t i = 0; i < 5; ++i) {
            std::cout << output[i * 2] << ", " << output[i * 2 + 1] << "\n";
        }

    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
