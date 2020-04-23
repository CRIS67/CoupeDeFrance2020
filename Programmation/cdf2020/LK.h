#ifndef LK_H
#define LK_H

#include <utility>
#include <list>
#include <vector>
#include <cmath>
#include <fstream>       // for ifstream, ofstream

#include "dStarLite.hpp" // to have the Node structure 
/*
struct Node
{
	float costG; // cost of the path as in A*
	float costRHS; // lookahead value
	float costH; // cost from current to the start (because we start from the goal in D* Lite)
	std::pair<int,int> coord; // coordinates of the Node in the map
	bool isObstacle = false; // if the Node is an obstacle is set to true
	std::pair<float, float> key {-1,-1}; // default Node key
        int rank;
        int id;

};
*/
// an edge (u,v)
struct edge {
    Node u;
    Node v;
};


class LK {
    private:
        int m_count; /*< number of k-opt moves to do */
        std::vector<Node> m_solution; 
        std::vector<Node> m_initialTour; 
    public:
        LK(int c):m_count(c){};
        
        // optimize
        void optimize( std::vector<Node> intialTour);
        // choose x1 
        bool choose_x1(Node *tour, int n, bool *visited, Node *t1,
                edge *x1);
        // choose y1
        bool choose_y1( Node *tour, int n, bool *visited, 
                edge x1, Node *t2, Node *t3, edge *y1, int *G1);

        // choose xi
        bool choose_xi(Node *tour, int n, bool *visited, Node *t3, 
                edge *x2);
       
        // choose yi
        bool choose_yi( Node *tour,int n, bool *visited,
                Node *t4, edge x2, edge *y2,int *G2);

        // flip construct resulting tour
        void flip(Node *tour, int n, std::list<edge> X);
       
        // store tour in tour_
        void store(Node *tour_, Node *tour, int n);

        // Returns distance between two Nodes 
        double distance(Node n1, Node n2); 
        
        // Returns the solution 
        std::vector<Node> getSolution(); 

        // Prints solution
        void printSolution(); 

        // Gets solution distance 
        double getDistance(); 

        double calculateDistance(std::vector<Node> tour); 

        void readNodes(std::vector<Node>& initialTour);

        void writeSolution(std::vector<Node> solution); 
};


#endif
