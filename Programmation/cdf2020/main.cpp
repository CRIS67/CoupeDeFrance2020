#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <vector>
#include <queue>
#include "web.hpp"
#include "dspic.hpp"
#include "SPI.hpp"
#include "robot.hpp"
#include "actuator.hpp"
#include "lidar.hpp"
#include "hmi.hpp"

#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <ncurses.h>
#include <opencv/highgui.h>

// DStarIncludes
#include <utility>
#include <algorithm>
#include <cmath>
#include <bits/stdc++.h>
#include <limits>
#include <map>
#include "mapGeneration.hpp"
#include "trajectoryHandle.hpp"
#include "dStarLite.hpp"



#include <fstream>
#include <sstream>

#define DEBUG_ENABLE_PRINT			1

#if(DEBUG_ENABLE_PRINT)
#define DEBUG_PRINT(x) 				std::cout << "DEBUG_PRINT>> " << x << std::endl;
#else
#define DEBUG_PRINT(x)
#endif

#define MAP_MM_PER_ARRAY_ELEMENT 	10


#define MM_ROWS						2000
#define MM_COLUMNS					3000

#define MM_START_X					1000
#define MM_START_Y					320

#define MM_SIZE_ENNEMY 				200
#define MM_SIZE_ENNEMY_MARGIN		220

#define SIZE_ENNEMY					MM_SIZE_ENNEMY / MAP_MM_PER_ARRAY_ELEMENT	//size of ennemy /2 4 element in array map -> (here 40cm)WARNING depends of map resolution
#define SIZE_ENNEMY_MARGIN			MM_SIZE_ENNEMY_MARGIN / MAP_MM_PER_ARRAY_ELEMENT

#define ARRAY_START_X				MM_START_X / MAP_MM_PER_ARRAY_ELEMENT
#define ARRAY_START_Y				MM_START_Y / MAP_MM_PER_ARRAY_ELEMENT
//DStarGlobal
int mapRows {MM_ROWS / MAP_MM_PER_ARRAY_ELEMENT};
int mapColumns {MM_COLUMNS / MAP_MM_PER_ARRAY_ELEMENT};
int km {0}; // variable for the D*

std::vector<std::vector<int>> mapVector; // the robot's map

bool obstacleDetection {false};
bool pointReached {false};

Node startNode = {std::pair<int,int>(ARRAY_START_X,ARRAY_START_Y)};
Node goalNode;
Node fromNode;
Node lastNode;

priorityList uList; // priority List
mappedNodes knownNodes; // node the robot can see


const int enemyWidth = 500; // The distance the enemy takes place on the map, it is represented as a square

std::vector<Node> simplifiedPath;
std::vector<Node> completePath;

// Strategy include
#include "LK.h"

// Debug
std::string command = "rm ./logMaps/*.txt";
const char *commandPointer = command.c_str();

int main()
{
    std::system(commandPointer);
	std::cout << std::endl << "     Coupe de France 2020 by CRIS" << std::endl << std::endl;
    wiringPiSetup();

    std::cout << "Initialisation..." << std::endl;

    DsPIC dspic;
    dspic.reset();
    delay(250); //wait for reset
    dspic.async_read(); //flush rx buffer



    SPI spi(SPI_CHANNEL,SPI_SPEED); //initialise SPI
	Actuator actBack("Actuator Back",&spi,SPI_ID_ACT_BACK,6,0,0,0,2,0,6,0,0);
    //Actuator actBack("Actuator Back 2019",&spi,SPI_ID_ACT_BACK,3,0,3,3,0,0,0,0,0);
    Actuator actScara("Actuator Scara",&spi,SPI_ID_ACT_SCARA,0,2,2,2,0,0,5,3,1);
	/*actFront.ResetAtmega();
    actBack.ResetAtmega();*/
    Web web(&dspic, &actScara, &actBack);

    Hmi hmi("HMI",&spi,SPI_ID_HMI);
    Actuator xbee("Xbee",&spi,SPI_ID_XBEE,4,2,0,0,0,1,3,0,0);
    Lidar lidar("Lidar",&spi,SPI_ID_LIDAR,&web);

    lidar.stop();

    puts("Hello human ! I, your fervent robot, am initialised. Press <ENTER> to continue.");
	getchar();

	lidar.start();
    dspic.start();
    dspic.startThreadReception();
    web.startThread();
    lidar.startThreadDetection();
    hmi.startThreadDetection();
    actScara.startThreadDetection();
    actBack.startThreadDetection();
    xbee.startThreadDetection();

    dspic.setVar8(CODE_VAR_VERBOSE,1);
    puts("verbose set to 1");
    dspic.getVar(CODE_VAR_BAT);
    dspic.loadVarDspicFromFile("config.txt");
    //dspic.initPos(1000,1500,3.14159);
    //dspic.initPos(1200,2550,-3.14159/2);
    dspic.initPos(startNode.coord.first*MAP_MM_PER_ARRAY_ELEMENT,startNode.coord.second*MAP_MM_PER_ARRAY_ELEMENT, -3.14159 -3.14159/2);
    //dspic.initPos(1000,1500,-3.14159/2);
    //dspic.initPos(0,0,0);
    //dspic.initPos(1000,3000,-3.14159/2);

    actBack.MoveServo(2, 980);
    actBack.MoveServo(0, 1600);
    actBack.MoveServo(1, 1800);

    std::cout << "Press enter to dspic.start() " << std::endl;
    getchar();

    //dspic.reset();
    //getchar();
    dspic.start();
    std::cout << "Press enter to start the D*" << std::endl;
    getchar();

    /*=============Strategy  START===================*/
    //while(!hmi.isStopMain()){}

    std::vector<Node> strategyTour; // Contains the nodes we are going to visit
    LK strategy(100); //faire la liste des taches jusqu'a 80 secondes
    strategy.readNodes(strategyTour); // reads the nodes from the entree.txt file
    for(unsigned int i = 0; i < strategyTour.size(); i++ ){
    	strategyTour.at(i).coord.first /= MAP_MM_PER_ARRAY_ELEMENT;
    	strategyTour.at(i).coord.second /= MAP_MM_PER_ARRAY_ELEMENT;
    }
    /*strategy.optimize(strategyTour); // optimize the tour
    strategy.printSolution(); // debug
    std::cout << "Initial distance " <<  strategy.calculateDistance(strategyTour) << std::endl;
    std::cout << "Final distance " <<  strategy.getDistance() << std::endl;
    strategyTour = strategy.getSolution();  // We update the optimized tour*/
	//Rajouter les taches à faire lors des fin de matchs
    /*std::vector<Node> Dernieres_taches_a_faire;
    strategyTour.insert( strategyTour.end(), Dernieres_taches_a_faire.begin(), Dernieres_taches_a_faire.end() );*/
    // Map Generation
    generateMap(mapVector,mapRows,mapColumns); // generates empty map
    createRectangle(0,0,mapColumns,300/MAP_MM_PER_ARRAY_ELEMENT, mapVector); // creates a 400x2000 obstacle rectangle  at (1600,0)
    createRectangle(0,1700/MAP_MM_PER_ARRAY_ELEMENT,mapColumns,250 / MAP_MM_PER_ARRAY_ELEMENT, mapVector); // creates a 400x2000 obstacle rectangle  at (1600,0)
    //createRectangle(90,140,20,20, mapVector); // creates a 400x2000 obstacle rectangle  at (1600,0)
    std::vector<std::vector<int>> debugMap(mapVector);

    std::cout << "MAP GENERATED" << std::endl;
    //printMap(mapRows, mapColumns, mapVector);


	/*save map in a file*/
	/*std::ofstream file;
	file.open("logMaps/out_map.txt");
	for(int y = 0; y < mapRows;y++){
		for(int x = 0; x < mapColumns; x++){
			file << mapVector.at(y).at(x) << " ";
		}
		file << std::endl;
	}
	file.close();*/

	unsigned int nbStrat = 0;
	unsigned int nbPath = 0;

    for(uint i = 0; i< strategyTour.size(); i++)
    {
    	std::ofstream file;
		std::ostringstream ss;
		ss << "VECTOR_map.txt";


    	file.open(ss.str());
					for(int y = 0; y < mapRows;y++){
						for(int x = 0; x < mapColumns; x++){
							file << mapVector.at(y).at(x) << " ";
						}
						file << std::endl;
					}
					file.close();


    	DEBUG_PRINT("strategyTour iteration nb " << i);
    	nbStrat = i;
		/*=============DStarImplementation START===================*/

        //reset lists when going to next point
        uList.clear();
        knownNodes.clear();


        //change start and goal node
        lastNode = startNode;
        goalNode.coord = strategyTour.at(i).coord;

        //initialize lists
        initialize(mapVector, knownNodes, uList, startNode, goalNode);
        //printKnownNode(knownNodes);
        goalNode = knownNodes.at(goalNode.coord);

        //computing best path to goal
        std::cout << "Start computing..." << std::endl;
        computeShortestPath(uList, knownNodes, startNode.coord, goalNode);
        std::cout << "End  computing..." << std::endl;
        //printKnownNode(knownNodes);
        startNode = knownNodes.at(startNode.coord); // we update the start node
        goalNode = knownNodes.at(goalNode.coord); // we update the goal node
		std::cout << "Start Node coord " << startNode.coord.first << " " << startNode.coord.second << "   "<<startNode.costG<<std::endl;
		std::cout << " Goal Node coord " << goalNode.coord.first << " " << goalNode.coord.second << "   "<<goalNode.costG<<std::endl;


		if(startNode.costG == INT_MAX){

			std::cout << "AOUCH (but continue to next action)" << std::endl;
      std::cout << startNode.coord.first << " " << startNode.coord.second << " " << startNode.costG << std::endl;

			continue;
		}

		std::vector<Node> completePath = getPath(mapVector, knownNodes, startNode, goalNode); // get the whole path

	  	DEBUG_PRINT("getPath ended");
		//std::cout << "debug1" << std::endl;
		/* <modifié*/
		DEBUG_PRINT("size of path = " << completePath.size());
		std::vector<Node> tempSimplifiedPath = pathTreatment(completePath);
		DEBUG_PRINT("path1");
		tempSimplifiedPath.push_back(goalNode); // we need to add the last node manually :(
		DEBUG_PRINT("path2");
		std::vector<Node> simplifiedPath = optimizePath(mapVector,tempSimplifiedPath, startNode);
		completePath.push_back(goalNode);
		DEBUG_PRINT("path1");
		//std::vector<Node> simplifiedPath = optimizePath(mapVector,completePath, startNode);


		DEBUG_PRINT("path optimized");
		/*std::cout << "tempSimplified path : " << std::endl;
		for(unsigned int j = 0 ; j < tempSimplifiedPath.size(); j++){
			std::cout << j << " : (" << tempSimplifiedPath.at(j).coord.first << " ; "  << tempSimplifiedPath.at(j).coord.second << ")" << std::endl;
		}

		std::cout << "optimized path" << std::endl;
		for(unsigned int j = 0 ; j < simplifiedPath.size(); j++){
			std::cout << j << " : (" << simplifiedPath.at(j).coord.first << " ; "  << tempSimplifiedPath.at(j).coord.second << ")" << std::endl;
		}*/
		/*modifié>*/
		/*<original*/

		/*std::vector<Node> simplifiedPath = pathTreatment(completePath);

		//std::cout << "debug2" << std::endl;
		simplifiedPath.push_back(goalNode); // we need to add the last node manually :(
		//std::cout << "debug3" << std::endl;
		*/
		/*original>*/


		int counter=0;
		/* Dstar Loop*/
		while(startNode.coord != goalNode.coord){
			DEBUG_PRINT("while start != goal iteration nb " << counter);
			if(startNode.costG == INT_MAX){
				std::cerr << "NOT KNOWN PATH" << std::endl;
				break;
			}

			//startNode = bestNode(startNode, knownNodes); // we "move" the robot
            fromNode = startNode;
			startNode = simplifiedPath.at(counter);
			counter++;
			std::cout << "PRINTING PATH" << std::endl;
			//findPath(mapVector,knownNodes,startNode,goalNode); // prints the path in the terminal

			int xSetpoint = startNode.coord.first * MAP_MM_PER_ARRAY_ELEMENT;
			int ySetpoint = startNode.coord.second * MAP_MM_PER_ARRAY_ELEMENT;
			std::cout << "send go " << xSetpoint << " ; " << ySetpoint << std::endl;
			dspic.go(xSetpoint, ySetpoint,0,0); // we move the robot to the next point
			delay(50);  //wait before asking so the dspic can start the movement /  and don't SPAM the UART channel
			//std::cout << dspic.getX() << ";" << dspic.getY()  << std::endl;

			// Wait until the robot reaches the point

			//while(!dspic.getArrived()){
			/*while( (dspic.getX() - xSetpoint)*(dspic.getX() - xSetpoint) + (dspic.getY() - ySetpoint)*(dspic.getY() - ySetpoint) > 400){
               delay(50);  //wait before asking so the dspic can start the movement /  and don't SPAM the UART channel
               dspic.getVar(CODE_VAR_ARRIVED); //send a request to update the arrived variable
               delay(50);
			}*/
			/*while((dspic.getX() - xSetpoint)*(dspic.getX() - xSetpoint) + (dspic.getY() - ySetpoint)*(dspic.getY() - ySetpoint) > 400){
				delay(50);
			}*/

			lidar.setFillBuffer(true);	//keep track of the points detected by lidar in a member buffer
			while((dspic.getX() - xSetpoint)*(dspic.getX() - xSetpoint) + (dspic.getY() - ySetpoint)*(dspic.getY() - ySetpoint) > 400){
				//std::cout << dspic.getX() << ";" << dspic.getY()  << std::endl;
				//get lidar points
				std::queue<pointFloat2d> points = lidar.getAndClearDetectedPoints();	//get detected points and clear internal buffer
				std::queue<pointFloat2d> savePoints = points; //TODO : verifier que c'est une copie et pas une copie de reference
				unsigned int nbPoints = points.size();
				std::cout << "nbPoints" << nbPoints << std::endl;
				//add on binary map
				std::vector<std::vector<int>> newMap(mapVector); //used to detect colision
				for(unsigned int j = 0; j < nbPoints;j++){
					pointFloat2d p = points.front();
					points.pop();
					int xObstacle = (int)round(p.x / MAP_MM_PER_ARRAY_ELEMENT);
					int yObstacle = (int)round(p.y / MAP_MM_PER_ARRAY_ELEMENT);
					//std::cout << "(" << x << " ; " << y << ") ";
					for(int y = yObstacle - SIZE_ENNEMY; y < yObstacle + SIZE_ENNEMY;y++){ //TODO : verifier pk pas <=
					    for(int x = xObstacle - SIZE_ENNEMY; x < xObstacle + SIZE_ENNEMY;x++){
							if(y >= 0 && y < mapRows && x >= 0 && x < mapColumns){
								newMap.at(y).at(x) = 1;
								//std::cout << "(" << j << " ; " << k << ") ";
							}
						}
					}
					//std::cout << std::endl;
				}
				//detect Collision
				Node nRobot;
				nRobot.coord.first = (int)(dspic.getX()/MAP_MM_PER_ARRAY_ELEMENT);
				nRobot.coord.second = (int)(dspic.getY()/MAP_MM_PER_ARRAY_ELEMENT);
				DEBUG_PRINT("detect collision begin");
				obstacleDetection = detectCollision(newMap,simplifiedPath, nRobot, startNode);	//ATTENTION à changer plus tard pour ne pas prendre en compte la partie du trajet déjà fait
				std::cout<<obstacleDetection<<std::endl;
				DEBUG_PRINT("detect collision ended");
				//bool obstacleDetection = detectCollisionLine(100,50,100,250,newMap);	//ATTENTION à changer plus tard pour ne pas prendre en compte la partie du trajet déjà fait
				if(obstacleDetection){
					DEBUG_PRINT("collision detected !");
					lidar.setFillBuffer(false);
					std::vector<std::vector<int>> augmentedMap(mapVector); //used to compute modified path
					DEBUG_PRINT("Filling augmentedMap ...");
					for(unsigned int j = 0; j < nbPoints;j++){
						pointFloat2d p = savePoints.front();
						savePoints.pop();
                        int xObstacle = (int)round(p.x / MAP_MM_PER_ARRAY_ELEMENT);
                        int yObstacle = (int)round(p.y / MAP_MM_PER_ARRAY_ELEMENT);
                        //std::cout << "(" << x << " ; " << y << ") ";
                        for(int y = yObstacle - SIZE_ENNEMY_MARGIN; y < yObstacle + SIZE_ENNEMY_MARGIN;y++){
                            for(int x = xObstacle - SIZE_ENNEMY_MARGIN; x < xObstacle + SIZE_ENNEMY_MARGIN;x++){
                                if(y >= 0 && y < mapRows && x >= 0 && x < mapColumns){
                                    augmentedMap.at(y).at(x) = 1;
                                    if(newMap.at(y).at(x) == 0){
                                        newMap.at(y).at(x) = 10;
                                    }
                                    //std::cout << "(" << x << " ; " << y << ") ";
                                }
                            }
                        }
						//std::cout << std::endl;
					}
					DEBUG_PRINT("AugmentedMap filled ");

                    if(augmentedMap.at(nRobot.coord.second).at(nRobot.coord.first) == 1){ // If the nRobot is an obstacle
                        std::cout << "Start Node is an obstacle ! Finding new startNode..." << std::endl;
                        std::cout << "fromNode " << fromNode.coord.first << " " << fromNode.coord.second << std::endl;
                        std::cout << "nRobot Node " << nRobot.coord.first << " " << nRobot.coord.second << std::endl;
                        int dx =  nRobot.coord.first - fromNode.coord.first ;
                        int dy = nRobot.coord.second - fromNode.coord.second ;
                        startNode = searchNewStartNode(nRobot.coord.first, nRobot.coord.second, fromNode.coord.first, fromNode.coord.second, augmentedMap);
                        std::cout << "nRobot Node " << nRobot.coord.first << " " << nRobot.coord.second << std::endl;
                    }else{
                        startNode.coord.first = nRobot.coord.first;
                        startNode.coord.second = nRobot.coord.second;
                    }
					startNode = knownNodes.at(startNode.coord); // we update the start node
                    std::cout << "New start Node " << startNode.coord.first << " " << startNode.coord.second << " Is obstacle : " << startNode.isObstacle << std::endl;
                    std::cout << "Augmented map New start Node : " << augmentedMap.at(startNode.coord.second).at(startNode.coord.first) << std::endl;

                    DEBUG_PRINT("printing MAP");
					std::ofstream file;
					std::ostringstream ss;
					//ss << "logMaps/out_map" << nbStrat << "_" << nbPath++ << ".txt";
					ss << "out_map.txt";
					file.open(ss.str());
                    std::vector<std::vector<int>> debugMap(augmentedMap);
					debugMap.at(startNode.coord.second).at(startNode.coord.first) = 3;
					for(int y = 0; y < mapRows;y++){
						for(int x = 0; x < mapColumns; x++){
							file << debugMap.at(y).at(x) << " ";
						}
						file << std::endl;
					}
					file.close();
					DEBUG_PRINT("printing MAP ended");

					if(augmentedMap.at(goalNode.coord.second).at(goalNode.coord.first) == 1){ // If the goalNode is an obstacle
                        std::cout << "AOUCH goalNode is an obstcale" << std::endl;
                        std::cout << goalNode.coord.first << ";" << goalNode.coord.second << std::endl;
                        dspic.stop();
                        dspic.setVar8(CODE_VAR_VERBOSE,0);
                        dspic.stopThreadReception();
                        puts("verbose set to 0");
                        puts("exiting ...");
                        lidar.stop();
                        lidar.stopThreadDetection();
                        web.stopThread();

                        puts("exiting...");

                        delay(200);
					    //return -1;
					}

                    std::cout << "Goal Node" << goalNode.coord.first << " " << goalNode.coord.second << std::endl;
					std::cout << " WARNING : collision !" << std::endl;
					km = km + distance2(lastNode, startNode);
					lastNode = startNode;
					updateMap(knownNodes, augmentedMap, uList, startNode.coord, goalNode); // we update all the changed nodes
					std::cout << "computing new path" << std::endl;
					computeShortestPath(uList, knownNodes, startNode.coord, goalNode);
					std::cout << "new path found ! =)" << std::endl;
					startNode = knownNodes.at(startNode.coord); // we update the start node
					goalNode = knownNodes.at(goalNode.coord);
					DEBUG_PRINT("Getting path ...");
					if(startNode.costG == INT_MAX){
						std::cout << "not new recalculated path found :( " << std::endl;

						dspic.stop();
						dspic.setVar8(CODE_VAR_VERBOSE,0);
					    dspic.stopThreadReception();
						puts("verbose set to 0");
					    puts("exiting ...");
						lidar.stop();
					    lidar.stopThreadDetection();
					    web.stopThread();

					    puts("exiting...");

						delay(200);

					    return -1;

					}
					completePath = getPath(augmentedMap, knownNodes, startNode, goalNode); // get the whole path
					DEBUG_PRINT("get Path ended");
					tempSimplifiedPath = pathTreatment(completePath);
					std::cout << "debug2.1" << std::endl;
					tempSimplifiedPath.push_back(goalNode); // we need to add the last node manually :(
					std::cout << "debug2.2" << std::endl;
					simplifiedPath = optimizePath(augmentedMap,tempSimplifiedPath, startNode);
					//simplifiedPath = optimizePath(augmentedMap,completePath, startNode);
					std::cout << "debug2.3" << std::endl;
					counter=0;
					std::cout << "debug3" << std::endl;

					DEBUG_PRINT("printing MAP");
					//ss << "logMaps/out_map" << nbStrat << "_" << nbPath++ << ".txt";
					ss << "logMaps/out_map" << nbPath++ << ".txt";
					file.open(ss.str());
					for(unsigned int i = 0; i < simplifiedPath.size();i++){
						newMap.at(simplifiedPath.at(i).coord.second).at(simplifiedPath.at(i).coord.first) = 4;
					}
					newMap.at(nRobot.coord.second).at(nRobot.coord.first) = 3;
					for(int y = 0; y < mapRows;y++){
						for(int x = 0; x < mapColumns; x++){
							file << newMap.at(y).at(x) << " ";
						}
						file << std::endl;
					}
					file.close();
					DEBUG_PRINT("printing MAP ended");

					break;
				}
				//std::cout << "collision = " << obstacleDetection << std::endl;
				//std::cout << "collision = " << detectCollisionLine(100,100,100,250,newMap) << std::endl;
				DEBUG_PRINT("printing MAP");
				std::ofstream file;
				std::ostringstream ss;
				//ss << "logMaps/out_map" << nbStrat << "_" << nbPath++ << ".txt";
				ss << "logMaps/out_map" << nbPath++ << ".txt";
				file.open(ss.str());
				for(unsigned int j = 0; j < simplifiedPath.size();j++){
					newMap.at(simplifiedPath.at(j).coord.second).at(simplifiedPath.at(j).coord.first) = 4;
				}
				newMap.at(nRobot.coord.second).at(nRobot.coord.first) = 3;
				for(int y = 0; y < mapRows;y++){
					for(int x = 0; x < mapColumns; x++){
						file << newMap.at(y).at(x) << " ";
					}
					file << std::endl;
				}
				file.close();
				DEBUG_PRINT("printing MAP ended");
				//printMap(mapRows,mapColumns,newMap);
				delay(50);
			}
			lidar.setFillBuffer(false);	//stop keeping lidar points to avoid running out of memory

          /*
          while(!pointReached)
          {

              TO BE IMPLEMENTED
              - getPointReached() from the dsPic
              - readSensorValues()
              - obstcaleDetection = sensorTreatment() -> determine if we have to update the map

            if(obstacleDetection)
              {
                  km = km + distance2(lastNode, startNode);
                  lastNode = startNode;
                  updateMap(knownNodes, mapVector, uList, startNode.coord, goalNode); // we update all the changed nodes
                  computeShortestPath(uList, knownNodes, startNode.coord, goalNode);
                  startNode = knownNodes.at(startNode.coord); // we update the start node
                  goalNode = knownNodes.at(goalNode.coord);


                  TO BE IMPLEMENTED
                  - simplifiedPath = pathTreatment(getPath()) set critical points to go to
                  - create a vector with those points and update them if changes in the map


              }
          }*/

          // Debug
          std::cout << "Press nothing to continue to the next point" << std::endl;
          //getchar();
      }

      /*=============DStarImplementation END===================*/
      startNode = goalNode;
      std::cout << "Press nothing for next action"  << std::endl;
      //getchar();
      //strategyTour.erase(strategyTour.begin()+i); // We remove the action
    }


    /*=============Strategy  END ===================*/

    dspic.stop();
	dspic.setVar8(CODE_VAR_VERBOSE,0);
    dspic.stopThreadReception();
	puts("verbose set to 0");
    puts("exiting ...");
	lidar.stop();
	actScara.stopThreadDetection();
    actBack.stopThreadDetection();
    xbee.stopThreadDetection();
    hmi.stopThreadDetection();
    lidar.stopThreadDetection();
    web.stopThread();

    puts("exiting...");

	delay(200);
	if(hmi.isStopPrgm()) {
        system("sudo shutdown now");
    }

    return 0;
}
