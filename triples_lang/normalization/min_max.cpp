/*
 CPP Min Max Encoding libray  
 Function to perform min-max normalization on a vector

 Copyright (c) 2026 RobotFreedom.org 
 Author: RobotFreedom.org  
 License: MIT License

*/

#include <iostream>
#include <vector>
#include <string>
#include <iostream>
#include <fstream> 
#include <algorithm> 
#include <stdexcept> 
 

struct MinMaxEnc { 
  double max; 
  double min;   
}; 
 
 
bool minMaxSave(string FileName, struct MinMaxEnc mdl) {
   /*
    
   */

    std::ofstream out(FileName );

    if (!out) throw std::runtime_error("Cannot open file for writing");

    out << mdl.min <<"\n";
    out << mdl.max <<"\n";

    return true;
}

struct MinMaxEnc minMaxLoad(string FileName ) {
   /*
    
   */
   struct MinMaxEnc mdl;

   std::ifstream in(FileName );

   string line;
   std::getline(in, line);     
   mdl.min = std::stof(line); 

   std::getline(in, line);     
   mdl.max = std::stof(line); 
 
   return mdl;

}


 
struct MinMaxEnc minMaxFit(const std::vector<double>& data) {
   /*
    
   */
    auto minIt = std::min_element(data.begin(), data.end());    
    auto maxIt = std::max_element(data.begin(), data.end());   
    double minVal = *minIt;
    double maxVal = *maxIt; 
    MinMaxEnc results;
    results.min  = minVal;
    results.max  = maxVal;

    return results;
}

std::vector<double> minMaxEncode(const std::vector<double>& data ,  MinMaxEnc mdl) {
   /*
    
   */ 
    if (mdl.min == mdl.max) { 
        return std::vector<double>(data.size(), 0.0);
    }

    /* 
    (x - Min) / (Max - Min)
    */
    std::vector<double> normalized;
    normalized.reserve(data.size());
    for (double val : data) {
        normalized.push_back((val - mdl.min) / (mdl.max - mdl.min));
    }

    return normalized;
}
 
 