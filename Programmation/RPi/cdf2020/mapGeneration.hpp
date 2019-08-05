#ifndef MAPGENERATION_H_INCLUDED
#define MAPGENERATION_H_INCLUDED

#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>
#include <utility>
#include <algorithm>
#include <cmath>
#include <bits/stdc++.h>
#include <limits>

void generateMap(std::vector<std::vector<int>>& mapVector, int mapRows, int mapColumns);
void createRectangle(uint startX, uint startY, int width, int length, std::vector<std::vector<int>>& mapVector); 
void printMap(int mapRows, int mapColumns, std::vector<std::vector<int>>& randomMap);
void clearMap(std::vector<std::vector<int>>& randomMap);

#endif // MAPGENERATION_H_INCLUDED
