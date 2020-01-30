#ifndef DSTARTLITE_H_INCLUDED
#define DSTARTLITE_H_INCLUDED

#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>
#include <utility>
#include <algorithm>
#include <cmath>
#include <bits/stdc++.h>
#include <limits>
#include "mapGeneration.hpp"
#include "dStarLite.hpp"
#include <unordered_map>
#include <boost/functional/hash.hpp>
#include <cstdlib>
#include <climits>

//const double infinity = std::numeric_limits<double>::infinity(); // infinity value
const int infinity = std::numeric_limits<int>::max(); // infinity value


const int nodeCost = 3;// Cost to move from node to node
const int linearCost = 1;
const int diagonalCost=2;

struct Node
{
	int costG; // cost of the path as in A*
	int costRHS; // lookahead value
	int costH; // cost from current to the start (because we start from the goal in D* Lite)
	std::pair<int,int> coord; // coordinates of the node in the map
	bool isObstacle = false; // if the node is an obstacle is set to true
	std::pair<int, int> key {-1,-1}; // default node key
	int rank; 
	int id; 
	int actionCoeff; 

};

//
//typedef std::map< std::pair<int, int>, Node> priorityList; // Each node will be indentified by their key
//typedef std::map< std::pair<int, int>, Node> mappedNodes; // all the nodes in the map

typedef std::unordered_map<std::pair<int,int>,Node, boost::hash<std::pair<int,int> >> priorityList;
typedef std::unordered_map<std::pair<int,int>,Node, boost::hash<std::pair<int,int> >> mappedNodes;

// FUNCTION PROTOTYPES
// DStarLite
int distance(int x1, int y1, int x2, int y2);
bool compareKeys(std::pair<int, int> k1, std::pair<int,int> k2);
std::pair<int,int> topKey(priorityList& uList);
Node top(priorityList& uList);
Node pop(priorityList& uList);
void update( std::pair<int,int> oldKey, std::pair<int,int> newKey, priorityList& uList);
int distance2(Node node1, Node node2);

std::pair<int, int> calculateKey(Node node, Node startNode);
void initialize(std::vector<std::vector<int>>& randomMap, mappedNodes& knownNodes, priorityList& uList, Node startNode, Node goalNode);
void updateNode( Node node, priorityList& uList, mappedNodes& knownNodes,std::pair<int,int> startCoord, Node goal);
void computeShortestPath(priorityList& uList, mappedNodes& knownNodes, std::pair<int,int> startCoord, Node goalNode);
int minSuccessor(Node node, mappedNodes& knownNodes);
void updateAdjacents(Node currentNode, priorityList& uList, mappedNodes& knownNodes, std::pair<int,int> startCoord, Node goalNode);
void findPath(std::vector<std::vector<int>>& randomMap, mappedNodes& knownNodes, Node currentNode, Node goalNode);
Node bestNode(Node currentNode ,mappedNodes& knownNodes, Node goalNode);
void updateMap(mappedNodes& knownNodes, std::vector<std::vector<int>>& randomMap, priorityList& priorList, std::pair<int,int> startCoord, Node goalNode);
std::vector<Node> getPath(std::vector<std::vector<int>>& randomMap, mappedNodes& knownNodes, Node currentNode, Node goalNode);

// DEBUG
void printKnownNode(mappedNodes& knownNodes);
void printNodesAndKeys(mappedNodes& knownNodes);
void printKey(std::pair<int,int> key);
#endif // DSTARTLITE_H_INCLUDED
