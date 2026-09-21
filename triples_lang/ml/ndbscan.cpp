/*
  Not DBScan

 Copyright (c) 2026 RobotFreedom.org 
 Author: RobotFreedom.org  
 License: MIT License
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <queue>


class NDBSCAN{


private:   


public: 
 
    struct Point {
        double x, y;
        int cluster_id = -1; // -1 means UNCLASSIFIED, -2 means NOISE
    };
    
 

   // Calculate Euclidean distance between two points
   double calculate_distance(const Point& p1, const Point& p2) {
       return std::sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y) );
   }

   // Find all neighbor indices within epsilon distance
   std::vector<int> get_neighbors(const std::vector<Point>& points, int target_idx, double eps) {
       std::vector<int> neighbors;
       for (int i = 0; i < points.size(); ++i) {
           if (calculate_distance(points[target_idx], points[i]) <= eps) {
               neighbors.push_back(i);
           }
       }
       return neighbors;
   }
    
    // Core DBSCAN algorithm implementation
    void train(std::vector<Point>& points, double eps, int min_pts) {
        int cluster_id = 0;
    
        for (int i = 0; i < points.size(); ++i) {
            // Skip if the point is already processed
            if (points[i].cluster_id != -1) continue;
    
            // Find neighbors
            std::vector<int> neighbors = get_neighbors(points, i, eps);
    
            // Density check: if it has fewer neighbors than min_pts, mark as noise for now
            if (neighbors.size() < min_pts) {
                points[i].cluster_id = -2; // Mark as NOISE
                continue;
            }
    
            // Expand cluster
            points[i].cluster_id = cluster_id;
            std::queue<int> seeds;
            for (int neighbor_idx : neighbors) {
                if (neighbor_idx != i) seeds.push(neighbor_idx);
            }
    
            while (!seeds.empty()) {
                int current_idx = seeds.front();
                seeds.pop();
    
                // If previously marked as noise, change it to border point of this cluster
                if (points[current_idx].cluster_id == -2) {
                    points[current_idx].cluster_id = cluster_id;
                }
    
                // Skip if already processed in another cluster
                if (points[current_idx].cluster_id != -1) continue;
    
                // Mark point as part of current cluster
                points[current_idx].cluster_id = cluster_id;
    
                // Find neighbors of the current seed point
                std::vector<int> current_neighbors = get_neighbors(points, current_idx, eps);
    
                // If seed point is a core point, add its neighbors to the seed queue
                if (current_neighbors.size() >= min_pts) {
                    for (int next_neighbor_idx : current_neighbors) {
                        seeds.push(next_neighbor_idx);
                    }
                }
            }
            cluster_id++; // Move to next cluster ID
        }
    }
};

int test_nhbdscan() {

    NDBSCAN ndbsan;
    // Sample dataset with two distinct groups and one outlier (noise)
    std::vector<NDBSCAN::Point> points = {
        {1.0, 1.0}, {1.1, 1.2}, {1.2, 1.0}, {0.9, 0.8}, // Cluster 0
        {10.0, 10.0}, {10.2, 10.1}, {9.8, 10.2},       // Cluster 1
        {5.0, 5.0}                                      // Noise (Outlier)
    };

    double eps = 1.5;
    int min_pts = 3;

    ndbsan.train(points, eps, min_pts);

    // Print results
    std::cout << "Point Clustered Results:\n";
    for (size_t i = 0; i < points.size(); ++i) {
        std::cout << "Point (" << points[i].x << ", " << points[i].y << ") -> ";
        if (points[i].cluster_id == -2) {
            std::cout << "Outlier\n";
        } else {
            std::cout << "Cluster " << points[i].cluster_id << "\n";
        }
    }

    return 0;
}
