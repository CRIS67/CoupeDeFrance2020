#include "mapGeneration.hpp"

void generateMap(std::vector<std::vector<int>>& mapVector, int mapRows, int mapColumns) {

/*	mapVector.resize(mapRows);

	for(int i =0; i<mapColumns; i++){
		mapVector[i].resize(mapColumns);
	}*/
        for( int i = 0; i< mapRows; i++)
        {
          std::vector<int> tmp;
          mapVector.push_back(tmp);
          for(int j =0; j< mapColumns; j++)
          {
            mapVector.at(i).push_back(0);
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
		std::cerr << "X : " << startX << " Y " << startY << " W " << width << " L " << height << std::endl;
		return;
	}

	for(int y = startY; i<startY+width; i++)
	{
		for(int x = startX; j<startX+height; j++)
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
