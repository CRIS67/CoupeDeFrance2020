CDF: web.o dspic.o SPI.o mapGeneration.o GameElements.o robot.o vision.o lidar.o actuator.o hmi.o dStarLite.o trajectoryHandle.o LK.o main.o
	g++ -O3 -o cdf web.o dspic.o SPI.o robot.o vision.o lidar.o actuator.o hmi.o mapGeneration.o GameElements.o dStarLite.o trajectoryHandle.o LK.o main.o -Wall -lwiringPi -pthread -I/usr/local/include/ -lraspicam -lraspicam_cv `pkg-config --cflags  opencv` `pkg-config --cflags --libs opencv`

web.o: web.cpp
	g++ -O3 -o web.o -c web.cpp -Wall -lwiringPi -pthread 

dspic.o: dspic.cpp 
	g++ -O3 -o dspic.o -c dspic.cpp -Wall -lwiringPi -pthread

SPI.o: SPI.cpp 
	g++ -O3 -o SPI.o -c SPI.cpp -Wall -lwiringPi -pthread 

vision.o: vision.cpp
	g++ -o vision.o -c vision.cpp -Wall -lwiringPi -g -pthread -I/usr/local/include/ -lraspicam -lraspicam_cv -lmmal -lmmal_core -lmmal_util -lopencv_contrib `pkg-config --cflags  opencv` `pkg-config --cflags --libs opencv`

robot.o: robot.cpp 
	g++ -O3 -o robot.o -c robot.cpp -Wall -lwiringPi -pthread

lidar.o: lidar.cpp 
	g++ -O3 -o lidar.o -c lidar.cpp -Wall -lwiringPi -pthread

actuator.o: actuator.cpp 
	g++ -O3 -o actuator.o -c actuator.cpp -Wall -lwiringPi -pthread

hmi.o: hmi.cpp 
	g++ -O3 -o hmi.o -c hmi.cpp -Wall -lwiringPi -pthread

mapGeneration.o : mapGeneration.cpp 
	g++ -O3 -o mapGeneration.o -c mapGeneration.cpp -Wall -lwiringPi -pthread

GameElements.o : GameElements.cpp
	g++ -O3 -o GameElements.o -c GameElements.cpp -Wall -lwiringPi -pthread

dStarLite.o : dStarLite.cpp 
	g++ -I /home/pi/boost_1_68_0/ -O3 -o dStarLite.o -c dStarLite.cpp -Wall -lwiringPi -pthread

trajectoryHandle.o : trajectoryHandle.cpp 
	g++ -I /home/pi/boost_1_68_0/ -O3 -o trajectoryHandle.o -c trajectoryHandle.cpp -Wall -lwiringPi -pthread

LK.o: LK.cpp
	g++ -I /home/pi/boost_1_68_0/ -O3 -o LK.o -c LK.cpp -Wall -lwiringPi -pthread

main.o: main.cpp web.hpp dspic.hpp SPI.hpp robot.hpp actuator.hpp hmi.hpp lidar.hpp vision.hpp mapGeneration.hpp GameElements.hpp dStarLite.hpp trajectoryHandle.hpp LK.h
	g++ -I /home/pi/boost_1_68_0/ -O3 -o main.o -c main.cpp -Wall -lwiringPi -pthread `pkg-config --cflags  opencv` `pkg-config --cflags --libs opencv`
