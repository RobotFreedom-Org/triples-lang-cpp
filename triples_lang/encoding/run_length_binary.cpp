/* 
Compress binary signal using run length encoding 

 Copyright (c) 2026 RobotFreedom.org 
 Author: RobotFreedom.org  
 License: MIT License


*/

#include <iostream>
#include <vector>
#include <stdexcept>
#include <string>
 
std::vector<std::pair<char, int>> compressBinarySignal(std::vector<double> signals) {
    /*
      Compresses binary signals
    */

    if (signals.empty()) return {};

    std::vector<std::pair<char, int>> compressed;
    double current = signals[0];

    if (current != 0 && current != 1) {
        throw std::invalid_argument("Signals must be binary");
    }

    int count = 1;  
    for (auto &v  :signals) {  
        if (v != 0 && v!= 1) {
            throw std::invalid_argument("Signals must be binary");
        }

        if (v == current) {
            ++count;
        } else {
            compressed.push_back({current, count});
            current = v;
            count = 1;
        }
    }
    compressed.push_back({current, count});
    return compressed;
}

std::string decompressBinarySignal(const std::vector<std::pair<char, int>> &compressed) {
    /* 
       Decompress RLE binary signal
    */
    std::string result;
    for (auto &p : compressed) {
        if (p.first != 0 && p.first != 1) {
            throw std::invalid_argument("Invalid data.");
        } 
        result.append(p.second, p.first);
    }
    return result;
}
