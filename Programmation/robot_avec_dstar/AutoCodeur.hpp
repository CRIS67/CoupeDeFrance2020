#ifndef AUTOCODEUR_H
#define AUTOCODEUR_H

#include <iostream>
#include <sstream>
#include <fstream>
//#include <conio.h>

#define PUMP		'P'
#define SERVO		'S'
#define MOT_LIN		'M'
#define MOVE		'G'
#define NEXT        'N'
#define END 		'E'

#define KEY_END		'f'
#define KEY_MOVE	'g'
#define KEY_FRONT	'z'
#define KEY_BACK	's'
#define KEY_RIGHT	'd'
#define KEY_LEFT	'q'
#define KEY_MOTLIN	'0'
#define KEY_SERVO0	'1'
#define KEY_SERVO1	'2'
#define KEY_SERVO2	'3'
#define KEY_SERVO3	'4'
#define KEY_SERVO4	'5'
#define KEY_SERVO5	'6'
#define KEY_PUMP0	'&'
#define KEY_PUMP1	'_'
#define KEY_PUMP2	'"'
#define KEY_PUMP3	'\''
#define KEY_PUMP4	'('
#define KEY_PUMP5	'-'
#define KEY_NEXT	'n'

void Codeur(int nb_code);
void Record(void);

#endif // AUTOCODEUR_H
