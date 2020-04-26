#include "mapGeneration.hpp"

void generateMap(std::vector<std::vector<int>>& mapVector, int mapRows, int mapColumns) {

/*	mapVector.resize(mapRows);

	for(int y =0; y<mapColumns; y++){
		mapVector[y].resize(mapColumns);
	}*/
        for( int y = 0; y< mapRows; y++)
        {
          std::vector<int> tmp;
          mapVector.push_back(tmp);
          for(int x =0; x< mapColumns; x++)
          {
            mapVector.at(y).push_back(0);
          }
        }
}

/*
Creates an obstacle rectangle at (startX, startY)
*/
void createRectangle(uint startX, uint startY, int width, int height, std::vector<std::vector<int>>& mapVector){

	if( ( (startX + width) >  mapVector[0].size()  ) || ((startY + height)  > mapVector.size()) || startX<0 || startY<0 || width <=0 || height<=0)
	{
		std::cerr << "Rectangle creation fail : Too big of rectangle :( " << std::endl;
		std::cerr << "X : " << startX << " Y " << startY << " W " << width << " H " << height << std::endl;
		return;
	}

	for(int y = startY; y<startY+width; y++)
	{
		for(int x = startX; x<startX+height; x++)
		{
			mapVector[y][x] = 1;
		}
	}
}

void printMap(int mapRows, int mapColumns, std::vector<std::vector<int>>& mapVector) {

	std::cout << "PRINTING  MAP" << std::endl;
	for(int y = 0; y< mapRows; y++){

		for (int x= 0; x<mapColumns; x++){
			std::cout << mapVector[y][x] << "\t";
		}
		std::cout << std::endl;
	}
}

void clearMap(std::vector<std::vector<int>>& mapVector){

    for(uint y = 0 ; y< mapVector.size(); y++){

        for(uint x = 0; x< mapVector[0].size(); x++){
            mapVector[y][x] = 0;
        }
    }
}
