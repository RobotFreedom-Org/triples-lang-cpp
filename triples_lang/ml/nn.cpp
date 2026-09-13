/*
 A very simple Neural Network

 Copyright (c) 2026 RobotFreedom.org 
 Author: RobotFreedom.org  
 License: MIT License


*/

#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <fstream> 

using namespace std;

// Sigmoid activation function
double sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x));
}

// Derivative of sigmoid
double sigmoidDerivative(double x) {
    return x * (1.0 - x);
}

// Neural Network class
class NeuralNetwork {
private:
    // Network structure: 2 inputs, 2 hidden neurons, 1 output
    vector<double> inputLayer;
    vector<double> hiddenLayer;
    vector<double> outputLayer;

    // Weights
    vector<vector<double>> weightsInputHidden;
    vector<vector<double>> weightsHiddenOutput;

    // Biases
    vector<double> biasHidden;
    vector<double> biasOutput;

    // Learning rate
    double learningRate;

public:

    
    NeuralNetwork(double lr = 0.5) {
        learningRate = lr;
        inputLayer.resize(2);
        hiddenLayer.resize(2);
        outputLayer.resize(1);

        // Initialize weights and biases with small random values
        srand((unsigned)time(0));
        weightsInputHidden = vector<vector<double>>(2, vector<double>(2));
        weightsHiddenOutput = vector<vector<double>>(2, vector<double>(1));
        biasHidden.resize(2);
        biasOutput.resize(1);

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++)
                weightsInputHidden[i][j] = ((double)rand() / RAND_MAX) - 0.5;
        }
        for (int i = 0; i < 2; i++) {
            weightsHiddenOutput[i][0] = ((double)rand() / RAND_MAX) - 0.5;
        }
        for (int i = 0; i < 2; i++)
            biasHidden[i] = ((double)rand() / RAND_MAX) - 0.5;
        biasOutput[0] = ((double)rand() / RAND_MAX) - 0.5;
    }

    void save( const std::string& FileName) {

    std::ofstream out(FileName, std::ios::binary);
    if (!out) throw std::runtime_error("Cannot open file for writing");

    // Write size first
    size_t size = inputLayer.size(); 

    out << "<inputLayer>" << "\n";
    out << inputLayer.size() << "\n";
    for (const auto& n : inputLayer) { 
       out << n  <<"\n";   
       }

    out << "<hiddenLayer>" << "\n";
    out << hiddenLayer.size() << "\n";
    for (const auto& n :hiddenLayer) { 
       out << n  <<"\n";   
       }

    out << "<outputLayer>" << "\n";
    out << outputLayer.size() << "\n";
    for (const auto& n :outputLayer) { 
       out << n  <<"\n";   
       }

    out << "<weightsInputHidden>" << "\n";
    out << weightsInputHidden.size() << "\n";
    for (const auto& sub :weightsInputHidden) { 

         out << "<Hidden>" << "\n";
         out << sub.size() << "\n";
         for (const auto& n :sub) { 
             out << n  <<"\n";   
           }
       }


    out << "<weightsHiddenOutput>" << "\n";
    out << weightsHiddenOutput.size() << "\n";
    for (const auto& sub :weightsHiddenOutput) { 

         out << "<Hidden>" << "\n";
         out << sub.size() << "\n";
         for (const auto& n :sub) { 
             out << n  <<"\n";   
           }
       }

    out << "<biasHidden>" << "\n";
    out << biasHidden.size() << "\n";
    for (const auto& n :biasHidden) { 
       out << n  <<"\n";   
       }

    out << "<biasOutput>" << "\n";
    out << biasOutput.size() << "\n";
    for (const auto& n :biasOutput) { 
       out << n  <<"\n";   
       }


    }

 
    bool load(const std::string& FileName) {
        /*
 
        */

        std::string line; 
        std::ifstream in(FileName, std::ios::binary); 
        if (!in) throw std::runtime_error("Cannot open file for reading");
     
        std::getline(in, line); 
        std::getline(in, line); 
        int size = std::stoi(line);
        std::vector<double> vec(size);  
        for (auto& n : vec) {  
            std::getline(in, line);  
            n = std::stoi(line);  
        } 
        inputLayer = vec;

        std::getline(in, line); 
        std::getline(in, line);  
        size = std::stoi(line);
        std::vector<double> vec0(size);  
        for (auto& n : vec0) {  
            std::getline(in, line); 
            n = std::stof(line);  
        } 
        hiddenLayer = vec0;

        std::getline(in, line); 
        std::getline(in, line);  
        size = std::stoi(line); 
        std::vector<double> vec1(size);  
        for (auto& n : vec1) {  
            std::getline(in, line); 
            n = std::stof(line);  
        } 
        outputLayer = vec1; 

        std::getline(in, line); 
        std::getline(in, line);  
        size = std::stoi(line); 
        std::vector<vector<double>> vec2(size);  
        for (auto& n : vec2) {  
            std::getline(in, line); 
            std::getline(in, line);  
            int size = std::stoi(line);
            n.resize(size);
            for (auto& s : n) {  
               std::getline(in, line); 
               s = std::stof(line);  
             }  
        } 
        weightsInputHidden = vec2;


        std::getline(in, line); 
        std::getline(in, line);  
        size = std::stoi(line);
        std::vector<vector<double>> vec3(size);  
        for (auto& n : vec3) { 

            std::getline(in, line); 
            std::getline(in, line);  
            int size = std::stoi(line);
            n.resize(size);
            for (auto& s : n) {  
               std::getline(in, line); 
               s = std::stof(line);  
             }  
        } 
        weightsHiddenOutput = vec3;
 
        std::getline(in, line); 
        std::getline(in, line);  
        size = std::stoi(line);
        std::vector<double> vec4(size);  
        for (auto& n : vec4) {  
            std::getline(in, line); 
            n = std::stof(line);  
        } 
        biasHidden = vec4;

        std::getline(in, line); 
        std::getline(in, line); 
        size = std::stoi(line);
        std::vector<double> vec5(size);  
        for (auto& n : vec5) {  
            std::getline(in, line); 
            n = std::stof(line);  
        } 
        biasOutput = vec5;

        return true;
    }


     
    // Forward pass
    double forward(const vector<double> &inputs) {
        inputLayer = inputs;

        // Hidden layer
        for (int i = 0; i < 2; i++) {
            double sum = biasHidden[i];
            for (int j = 0; j < 2; j++)
                sum += inputLayer[j] * weightsInputHidden[j][i];
            hiddenLayer[i] = sigmoid(sum);
        }

        // Output layer
        for (int i = 0; i < 1; i++) {
            double sum = biasOutput[i];
            for (int j = 0; j < 2; j++)
                sum += hiddenLayer[j] * weightsHiddenOutput[j][i];
            outputLayer[i] = sigmoid(sum);
        }

        return outputLayer[0];
    }

    // Backpropagation
    void train(const vector<double> &inputs, const vector<double> &targets) {
        double output = forward(inputs);

        // Output layer error
        vector<double> outputErrors(1);
        outputErrors[0] = targets[0] - output;

        // Hidden layer error
        vector<double> hiddenErrors(2);
        for (int i = 0; i < 2; i++) {
            hiddenErrors[i] = outputErrors[0] * weightsHiddenOutput[i][0];
        }

        // Update weights Hidden->Output
        for (int i = 0; i < 2; i++) {
            double delta = outputErrors[0] * sigmoidDerivative(outputLayer[0]);
            weightsHiddenOutput[i][0] += learningRate * delta * hiddenLayer[i];
        }
        biasOutput[0] += learningRate * outputErrors[0] * sigmoidDerivative(outputLayer[0]);

        // Update weights Input->Hidden
        for (int i = 0; i < 2; i++) {
            double delta = hiddenErrors[i] * sigmoidDerivative(hiddenLayer[i]);
            for (int j = 0; j < 2; j++) {
                weightsInputHidden[j][i] += learningRate * delta * inputLayer[j];
            }
            biasHidden[i] += learningRate * delta;
        }
    }
};

int test_nn() {
    NeuralNetwork nn(0.5);

    // XOR dataset
    vector<vector<double>> inputs = {
        {0, 0},
        {0, 1},
        {1, 0},
        {1, 1}
    };
    vector<vector<double>> targets = {
        {0},
        {1},
        {1},
        {0}
    };

    // Train the network
    for (int epoch = 0; epoch < 5000; epoch++) {
        for (size_t i = 0; i < inputs.size(); i++) {
            nn.train(inputs[i], targets[i]);
        }
    }
    nn.save("nn.out");
    nn.load("nn.out");
    // Test the network
    cout << fixed << setprecision(4);
    for (size_t i = 0; i < inputs.size(); i++) {
        double output = nn.forward(inputs[i]);
        cout << inputs[i][0] << " XOR " << inputs[i][1] << " = " << output << endl;
    }

    return 0;
}
