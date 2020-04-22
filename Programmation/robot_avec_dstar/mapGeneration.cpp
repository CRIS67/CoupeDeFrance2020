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

	if( ( (startY + height) >  mapVector.size()  ) || ( (startX + width)  > mapVector[0].size() ) )
	{
		std::cerr << "Rectangle creation fail : Too big of rectangle :( " << std::endl;
		std::cerr << "X : " << startX << " Y " << startY << " W " << width << " H " << height << std::endl;
		return;
	}

	for(int i = 0; i<width; i++)
	{
		for(int j = 0; j<height; j++)
		{
			//std::cout<<i<<"  "<<j<<std::endl;
			mapVector[startY+j][startX+i] = 1;
		}
	}
}

void printMap(int mapRows, int mapColumns, std::vector<std::vector<int>>& mapVector) {

	std::cout << "PRINTING  MAP" << std::endl;
	for(int i = 0; i< mapRows; i++){

		for (int j= 0; j<mapColumns; j++){
			std::cout << mapVector[i][j] << " ";
		}
		std::cout << std::endl;
	}
}

void clearMap(std::vector<std::vector<int>>& mapVector){

    for(uint i = 0 ; i< mapVector.size(); i++){

        for(uint j = 0; j< mapVector[0].size(); j++){
            mapVector[i][j] = 0;
        }
    }
}
