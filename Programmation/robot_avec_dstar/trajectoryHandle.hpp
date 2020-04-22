#ifndef TRAJECTORY_H
#define TRAJECTORY_H


#include <vector>
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

std::vector<Node> pathTreatment(std::vector<Node> path);
bool sensorTreatment(int enemyX, int enemyY, int enemyWidth,
	std::vector<std::vector<int>>& mapVector, std::vector<Node> path);

bool detectCollision(std::vector<std::vector<int> > &map, std::vector<Node> path, Node start, Node nextDestination);
std::vector<Node> optimizePath(std::vector<std::vector<int> > &map, std::vector<Node> path, Node &start);
bool detectCollisionLine( float x1, float y1, float x2, float y2 , std::vector<std::vector<int> >& mapVector);
//Debug

void printPath(std::vector<Node> path, std::vector<std::vector<int>>& mapVector);
#endif // DSTARTLITE_H_INCLUDED