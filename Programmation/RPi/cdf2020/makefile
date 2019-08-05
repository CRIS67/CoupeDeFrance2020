CDF: web.o dspic.o SPI.o lidar.o actuators.o hmi.o mapGeneration.o dStarLite.o trajectoryHandle.o LK.o main.o  
	g++ -O3 -o cdf web.o dspic.o SPI.o lidar.o actuators.o hmi.o mapGeneration.o dStarLite.o trajectoryHandle.o LK.o main.o -Wall -lwiringPi -pthread 

web.o: web.cpp
	g++ -O3 -o web.o -c web.cpp -Wall -lwiringPi -pthread 

dspic.o: dspic.cpp 
	g++ -O3 -o dspic.o -c dspic.cpp -Wall -lwiringPi -pthread

SPI.o: SPI.cpp 
	g++ -O3 -o SPI.o -c SPI.cpp -Wall -lwiringPi -pthread 
	
lidar.o: lidar.cpp 
	g++ -O3 -o lidar.o -c lidar.cpp -Wall -lwiringPi -pthread 
	
actuators.o: actuators.cpp 
	g++ -O3 -o actuators.o -c actuators.cpp -Wall -lwiringPi -pthread

mapGeneration.o : mapGeneration.cpp 
	g++ -O3 -o mapGeneration.o -c mapGeneration.cpp -Wall -lwiringPi -pthread

dStarLite.o : dStarLite.cpp 
	g++ -I /home/pi/boost_1_68_0/ -O3 -o dStarLite.o -c dStarLite.cpp -Wall -lwiringPi -pthread

trajectoryHandle.o : trajectoryHandle.cpp 
	g++ -I /home/pi/boost_1_68_0/ -O3 -o trajectoryHandle.o -c trajectoryHandle.cpp -Wall -lwiringPi -pthread

LK.o: LK.cpp
	g++ -I /home/pi/boost_1_68_0/ -O3 -o LK.o -c LK.cpp -Wall -lwiringPi -pthread
	
hmi.o: hmi.cpp 
	g++ -O3 -o hmi.o -c hmi.cpp -Wall -lwiringPi -pthread 

main.o: main.cpp web.hpp dspic.hpp SPI.hpp lidar.hpp actuators.hpp hmi.hpp mapGeneration.hpp dStarLite.hpp trajectoryHandle.hpp LK.h
	g++ -I /home/pi/boost_1_68_0/ -O3 -o main.o -c main.cpp -Wall -lwiringPi -pthread 
