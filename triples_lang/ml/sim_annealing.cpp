#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>

// Example objective function: Sphere function sum(x[i]^2)
// The global minimum is at x = [0, 0, ..., 0] with cost 0.
double costFunction(const std::vector<double>& x) {
    double sum = 0.0;
    for (double val : x) {
        sum += val * val;
    }
    return sum;
}

// Generate a neighboring solution by adding small Gaussian noise to the vector
std::vector<double> getNeighbor(const std::vector<double>& current, double stepSize, 
                                std::mt19937& gen, std::normal_distribution<double>& dist) {
    std::vector<double> neighbor = current;
    for (size_t i = 0; i < neighbor.size(); ++i) {
        neighbor[i] += dist(gen) * stepSize;
    }
    return neighbor;
}

int main() {
    // Random number generation setup
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<double> normalDist(0.0, 1.0);
    std::uniform_real_distribution<double> uniformDist(0.0, 1.0);

    // Initial configuration vector x (e.g., dimension 5, initialized arbitrarily)
    std::vector<double> currentX = {5.0, -4.0, 3.0, -2.0, 5.0};
    std::vector<double> bestX = currentX;

    double currentCost = costFunction(currentX);
    double bestCost = currentCost;

    // Simulated Annealing parameters
    double temp = 100.0;          // Initial temperature
    double coolingRate = 0.995;   // Multiplicative cooling factor
    double finalTemp = 1e-4;      // Stopping temperature
    double stepSize = 0.5;        // Magnitude of neighborhood perturbation
    int maxIterationsPerTemp = 100;

    // Main cooling loop
    while (temp > finalTemp) {
        for (int i = 0; i < maxIterationsPerTemp; ++i) {
            // Propose a neighbor
            std::vector<double> nextX = getNeighbor(currentX, stepSize, gen, normalDist);
            double nextCost = costFunction(nextX);

            // Calculate change in energy/cost
            double deltaCost = nextCost - currentCost;

            // Acceptance criteria: accept if better, or with probability exp(-deltaCost / temp) if worse
            if (deltaCost < 0 || uniformDist(gen) < std::exp(-deltaCost / temp)) {
                currentX = nextX;
                currentCost = nextCost;

                // Update global best if applicable
                if (currentCost < bestCost) {
                    bestX = currentX;
                    bestCost = currentCost;
                }
            }
        }
        // Cool down the system
        temp *= coolingRate;
    }

    // Output results
    std::cout << "Optimization finished.\nBest Cost: " << bestCost << "\nBest Vector x: [ ";
    for (double val : bestX) {
        std::cout << val << " ";
    }
    std::cout << "]\n";

    return 0;
}