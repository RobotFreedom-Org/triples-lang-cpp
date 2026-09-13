/*

 A very simple Gradient Descent  Algorithm

 Copyright (c) 2026 RobotFreedom.org 
 Author: RobotFreedom.org  
 License: MIT License


*/
#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <string>
#include <fstream> 
 
using Vector = std::vector<double>;


void saveGradientDescent(const std::string& FileName,std::vector<double> mdl) {
    std::ofstream out(FileName );

    if (!out) throw std::runtime_error("Cannot open file for writing");

    // Write size first
    size_t size = mdl.size(); 

    out << mdl.size() << "\n";
    for (const auto& n : mdl) { 
       out << n <<"\n";
       }
    }
  

std::vector<double> loadGradientDescent(const std::string& FileName )
{
    std::ifstream in(FileName );
   
    if (!in) throw std::runtime_error("Cannot open file for reading");
   
    std::string line;

    in >> line;
    int size = std::stoi(line);
    std::vector<double> vec(size); 

    for (auto& n : vec) { 
        std::getline(in, line);     
        n = std::stof(line);  
    }

    return vec;

}
  
/*
    Gradient Descent 
*/
std::vector<double> gradientDescent(std::vector<double> x0, 
                                   std::vector<double> grad, 
                                   double learningRate, 
                                   int maxIters, 
                                   double tolerance) {
    std::vector<double> x = x0;
    for (int iter = 0; iter < maxIters; ++iter) { 
    
        double gradNorm = 0.0;
        for (double g : grad) gradNorm += g * g;
        gradNorm = std::sqrt(gradNorm);

        if (gradNorm < tolerance) {
            std::cout << "Converged in " << iter << " iterations.\n";
            break;
        }

        /* 
        x: x = x - a * grad
        */
        for (size_t i = 0; i < x.size(); ++i) {
            x[i] -= learningRate * grad[i];
        }
    }
    return x;
}

int test_gd() {
    // Initial guess
    Vector x0 = {0.0, 0.0};
    Vector y0 = {0.0, 0.0};

    // Parameters
    double learningRate = 0.1;
    int maxIters = 1000;
    double tolerance = 1e-6;

    // Run gradient descent
    Vector result = gradientDescent(x0, y0, learningRate, maxIters, tolerance);

    // Output results
    std::cout << "Minimum found at: (";
    for (size_t i = 0; i < result.size(); ++i) {
        std::cout << result[i] << (i + 1 < result.size() ? ", " : "");
    }
    std::cout << ")\n"; 

    return 0;
}
