/*
  Not HDBScan

 Copyright (c) 2026 RobotFreedom.org 
 Author: RobotFreedom.org  
 License: MIT License
 */
 
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <fstream>  

// Structure to represent a 2D Point
struct HDBSPoint {
    double x, y, z;
};

class NHDBSCAN 
{

 private:



 public:
    
    // Simple Edge structure for Kruskal's MST
    struct Edge {
        int u, v;
        double weight;
        bool operator<(const Edge& other) const {
            return weight < other.weight;
        }
    };

   
   std::vector<Edge> Edges;
   std::vector<HDBSPoint> Points;
   std::vector<int> Labels;
   int Dim;

   void  save(const std::string& FileName  )
   {
 
    std::ofstream out(FileName, std::ios::binary);
    if (!out) throw std::runtime_error("Cannot open file for writing");
  
     out << "<" << Dim <<">\n";

     for (auto& col : Edges) {   
         out << col.u << "|" << col.v << "|" << col.weight << "\n";
     }
     out << "<Points>\n";
     for (auto& col : Points) {  
          out << col.x << "|" << col.y << "|" << col.z << "\n";
      }
    //  out << "\n"; 

     out << "<Labels>\n";
     for (int col : Labels) {  
          out << col << "\n";
      }
    // out << "\n"; 
   }

  void load(const std::string& FileName  )   
  {
   std::vector<Edge> edges;
   std::vector<HDBSPoint> points;
   std::vector<int> labels;
  
      
   FILE *filePtr;  
   filePtr = fopen(FileName.c_str(), "r"); 
   std::cout << "Loading Umap model  " << std::endl;

   char currentline[1000];  
   char currentval[1000];  
   int i;
   int dim;
   int itype = 0;
   while (fgets(currentline , 1000, filePtr))
    {         

      size_t len = strlen(currentline);
      if (len > 0 && currentline[len - 1] == '\n') {
            currentline[len - 1] = '\0';
       }

      if ((strcmp(currentline, "<Points>")  ==0))
     {
       itype = 1; 
     }
     else if  (strcmp(currentline, "<Labels>")  ==0) 
     {  
       itype =2 ; 
     }
     else if ( currentline[0] == '<') 
     {
 
       char* p1    = std::strtok(currentline  , "<");    
       char* p2    = std::strtok(p1, ">");      
       dim = atoi(p2);
     }
     else if (itype ==0)
     {
       char* u   = std::strtok(currentline  , "|");  
       char* v   = std::strtok(NULL,"|");   
       char* w   = std::strtok(NULL,"|");   
      
        Edge edge; 
        edge.u  = atoi(u); 
        edge.u  = atoi(u); 
        edge.u  = atof(u); 
        edges.push_back(edge);
     }
     else if (itype ==1)
     {

        HDBSPoint pt;
 
        char* x   = std::strtok(currentline  , "|");  
        char* y   = std::strtok(NULL,"|");   
        char* z   = std::strtok(NULL,"|");   

        pt.x = atoi(x);
        pt.y = atoi(y);
        pt.z = atoi(z);
        points.push_back(pt);

      }
     else
     {
    
         labels.push_back(atoi(currentline));
   
      }
   
    }
   
      Dim    = dim; 
      Edges  = edges;
      Points = points;
      Labels = labels;
   }
  
    // Calculate Euclidean distance between two points
    double euclideanDistance(const HDBSPoint& p1, const HDBSPoint& p2) {
      //  return std::sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
        return std::sqrt(  std::pow(p1.x - p2.x, 2) +  std::pow(p1.y - p2.y, 2)    );
    }
    
    double euclideanDistance3(const HDBSPoint& p1, const HDBSPoint& p2 )   {
        return std::sqrt(  std::pow(p1.x - p2.x, 2) +  std::pow(p1.y - p2.y, 2) +  std::pow(p1.z - p2.z, 2)   );
    }
     

    struct  DisjointSet {
 
        std::vector<int> parent;
        std::vector<int> rank;
 
         DisjointSet(int n) {
              parent.resize(n);
              rank.resize(n, 0);
              for (int i = 0; i < n; i++) parent[i] = i;
         }
         
         int find(int x) {
              if (parent[x] != x)
              parent[x] = find(parent[x]); // Path compression
              return parent[x];
         }
     
         void unite(int x, int y) {
             int rootX = find(x);
             int rootY = find(y);
             if (rootX == rootY) return;
             
             // Union by rank
             if (rank[rootX] < rank[rootY]) {
                 parent[rootX] = rootY;
                 } else if (rank[rootX] > rank[rootY]) {
                 parent[rootY] = rootX;
                 } else {
                 parent[rootY] = rootX;
                 rank[rootX]++;
                 }
             }
        };

    static bool compareWeights(const Edge &a, const Edge &b)
    {
         return  a.weight < b.weight;
    }

    std::vector<int> transform(std::vector<HDBSPoint> new_points  ) { 


        int n = new_points.size(); 
        int p = Points.size();
        std::vector<int> labels(n, -1);

       // std::cout  <<  n << std::endl;

        for (int i = 0; i < n; ++i) {

           //std::cout  <<  i << std::endl;
 
           // std::cout  <<  "gen dist" << std::endl;
            std::vector<Edge> edges; 
            for (int j = 0; j < p; ++j) {

               //  std::cout  <<  i  << " " << j << std::endl;
                if (Dim == 2)
                {
                  edges.push_back({i, j, euclideanDistance(new_points[i], Points[j] )})  ;
                }
                else
                {
                  edges.push_back({i, j, euclideanDistance3(new_points[i], Points[j] )})  ; 
                }
             }

            //  std::cout  <<  "sorting" << std::endl;
             std::sort(edges.begin(), edges.end(), compareWeights); 

            // for  (const auto& edge :   edges) {
            //   std::cout  <<  edge.u << " " <<  edge.v << " " << edge.weight << std::endl;
           // }

             labels[i] = Labels[edges[0].v];
            //if match not close enough return -1 
        } 
 
       return labels;
   }

 
    std::vector<int> fit(std::vector<HDBSPoint> points, int k=2, int dim =2 ) { 


        Dim    = dim;
        int n  = points.size(); 
        Points = points;

        std::vector<double> core_distances(n);
        for (int i = 0; i < n; ++i) {
            std::vector<double> dists;
            for (int j = 0; j < n; ++j) {
                if (dim == 2)
                {
                   dists.push_back(euclideanDistance(points[i], points[j]));
                }
                else
                {
                   dists.push_back(euclideanDistance3(points[i], points[j])); 
                }
            }
            std::sort(dists.begin(), dists.end());
            core_distances[i] = dists[k - 1]; // 0-indexed k-th neighbor
        }

        // Compute Mutual Reachability Distance & build graph edges
        std::vector<Edge> edges;
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                 double raw_dist;
                if (dim ==2)
                {
                       raw_dist = euclideanDistance(points[i], points[j]);
                }
                else
                {
                       raw_dist = euclideanDistance3(points[i], points[j]);
                }
    
                // Mutual Reachability Distance formula: max(core_dist(u), core_dist(v), dist(u, v))
                double mrd = std::max({core_distances[i], core_distances[j], raw_dist});
                edges.push_back({i, j, mrd});
            }
        }

        // 4. Build Minimum Spanning Tree (MST) using Kruskal's Algorithm
        std::sort(edges.begin(), edges.end());
        DisjointSet ds_mst(n);
        std::vector<Edge> mst_edges;
    
        for (const auto& edge : edges) {
            if (ds_mst.find(edge.u) != ds_mst.find(edge.v)) {
                ds_mst.unite(edge.u, edge.v);
                mst_edges.push_back(edge);
            }
        }
    
        // 5.å Evaluate the Single Linkage Tree / Cluster Extraction 
        //for (int i = 0; i < n; ++i) {
        //    std::cout << "Point " << i << " Core Distance: " << core_distances[i] << std::endl;
        //} 
    
       // for (const auto& edge : mst_edges) {
       //     std::cout << "Linkage (" << edge.u << " <-> " << edge.v << ") distance: " << edge.weight << std::endl;
       // }

        // Extracting stable flat clusters (For n=3, the strongest link joins, the outlier isolates)
        std::vector<int> labels(n, -1); // Default all to noise (-1)
        // need to look through all linked edges
        int cluster_id = 0;
        float wgt = 0;
        if (!mst_edges.empty()) {

            for (const auto& edge : mst_edges) {
                if (labels[edge.u]  ==-1)
                {
                   labels[edge.u] = cluster_id; // Cluster 0
                   labels[edge.v] = cluster_id; // Cluster 0
                   wgt = edge.weight;
                   for (const auto& edge2 : mst_edges) {
                       wgt = edge2.weight +  edge.weight; 
                       if ((edge.u == edge2.u) && (wgt  < 3)) labels[edge2.v] = cluster_id;
                       if ((edge.u == edge2.v) && (wgt  < 3)) labels[edge2.u] = cluster_id;
                       if ((edge.v == edge2.u) && (wgt  < 3)) labels[edge2.v] = cluster_id;
                       if ((edge.v == edge2.v) && (wgt  < 3)) labels[edge2.u] = cluster_id; 
                   }
                 cluster_id ++;
                 wgt = 0;
                }
            }
        }
    
        Edges = mst_edges;
        Labels = labels;
    
        return labels;
 }
    
};

int test_nhdbscan() { 

    std::vector<HDBSPoint> points = {
        {0.0, 0.0},    
        {1.0, 1.0},    
        {5.0, 5.0},    
        {6.0, 6.0},    
        {5.0, 6.0},    
        {7.0, 5.0},    
        {10.0, 10.0},  
        {11.0, 11.0},  
        {10.0, 11.0},  
        {11.0, 10.0},  
        {15.0, 16.0},  
        {15.0, 15.0},  
        {15.0, 16.0},  
        {15.0, 15.0},  
        {25.0, 25.0}   
    };

    NHDBSCAN nhdbscn ;
    std::vector<int> labels0 =  nhdbscn.fit(points);


    std::cout << "\n--- Results ---" << std::endl; 
    int n = points.size();
    for (int i = 0; i < n; ++i) {
        std::cout << "Point " << i << " (" << points[i].x << ", " << points[i].y 
                  << ") -> Label: " << labels0[i]   << std::endl;
    }

    nhdbscn.save("nhdbscn.txt");

    NHDBSCAN nhdbscn2 ;

    nhdbscn2.load("nhdbscn.txt");

    std::vector<HDBSPoint> newPoints = {
        {0.9, 0.3},   
        {10.0, 11.5},   
        {100, 101},    
    };

    std::vector<int> labels1 =  nhdbscn2.transform(newPoints);

    std::cout << "\n--- Results ---" << std::endl; 
    int n2 = newPoints.size();
    for (int i = 0; i < n2; ++i) {
        std::cout << "Point " << i << " (" << newPoints[i].x << ", " << newPoints[i].y 
                  << ") -> Label: " << labels1[i]   << std::endl;
    }


    /* 
    std::vector<HDBSPoint> points3 = {
        {0.0, 0.0, 0.0},    // Point 0
        {1.0, 1.0, 1.0},    // Point 1
        {15.0, 15.0, 15.0}   // Point 2
    };
    std::vector<int> out2 =  nhdbscn.fit(points3,2,3);

    */
  return 1;

}