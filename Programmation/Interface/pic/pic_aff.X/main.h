/*******************************************************************/
// Programme gestion d'un écran tactile nextion NX8040T050  
// Fichier : main.h
//
// Microcontrôleur PIC18F2520 MICROCHIP
// Interface IDE MPLAB X version 415
//
// Jean SOUDIER
// Club Robot INSA
// 17/02/2018
//
// Programme version 1 
/*******************************************************************/

#ifndef MAIN_H
#define	MAIN_H
//INIT
void ClockInit(void);
void ADCInit(void);
void SpiInit(void);
void UartInit(void);
void InitTmr0(void);
//FUNCTION
void PrintC(char C_printC);
void PrintS(const char * s_printS);
unsigned int Adc(unsigned char channel);
void ClearArray(void);
char Modulo(short int M_nombre);
#endif /* XC_HEADER_TEMPLATE_H */