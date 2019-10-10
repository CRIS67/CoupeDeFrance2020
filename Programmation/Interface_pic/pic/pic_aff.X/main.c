/*******************************************************************/
// Programme gestion d'un écran tactile nextion NX8040T050  
// Fichier : main.c
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


// <editor-fold defaultstate="collapsed" desc="Configuration Bits">
// PIC18F2520 Configuration Bit Settings

// 'C' source line config statements

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// CONFIG1H  INTIO67 or HSPLL
#pragma config OSC = INTIO67     // Oscillator Selection bits (Internal oscillator block, port function on RA6 and RA7)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 3         // Brown Out Reset Voltage bits (Minimum setting)

// CONFIG2H
#pragma config WDT = OFF         // Watchdog Timer Enable bit (WDT enabled)
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC   // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = ON      // PORTB A/D Enable bit (PORTB<4:0> pins are configured as analog input channels on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = OFF         // Single-Supply ICSP Enable bit (Single-Supply ICSP enabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-001FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (002000-003FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (004000-005FFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (006000-007FFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-001FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (002000-003FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (004000-005FFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (006000-007FFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-001FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (002000-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (004000-005FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (006000-007FFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot block (000000-0007FFh) not protected from table reads executed in other blocks)
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="include">
#include <xc.h>
#include <pic18f2520.h>
#include "main.h"
#include <math.h>
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="define">

#define _XTAL_FREQ			32000000  

#define blink()				LATCbits.LATC1=!(LATCbits.LATC1)
#define GpioInit()			TRISCbits.TRISC1=0

#define SCREEN_MAIN			0
#define SREEN_CONTROL		1
#define SCREEN_TERMINAL		2
#define SCREEN_PID			3
#define SCREEN_DEBUG		4

#define SPI_IDLE			0
#define SPI_NUM_VAR			1
#define SPI_MSG_UTILE		2
#define SPI_CHECKSUM		3

#define AUCUN				0
#define PRGM1				1
#define PRGM2				2
#define PRGM3				3
#define PRGM4				4
#define PRGM5				5
#define POS_X				6
#define POS_Y				7
#define TERM1				8
#define TERM2				9
#define TERM3				10
#define TERM4				11
#define DEBUG				12
#define SCORE               13
#define RESET               14
#define SENS                15
#define CHOIXPRGM           16
#define SHUTDOWN            17

#define SEND_VAR			1
#define SEND_TAILLE			4


#define NB_PRGM				5
#define TAILLE_PRGM_CHAINE	15
#define TAILLE_SPI_CHAINE	196
#define TAILLE_SEND_CHAINE	252
#define TAILLE_SEND         10

#define ON					1
#define OFF					0


// </editor-fold>

char SendMsgSreen[TAILLE_SEND_CHAINE];      //array uart vers l'ecran
char EtatScreen = SCREEN_MAIN;              //etat de la machine a etat
char EtatSpi = SPI_IDLE;					//etat de la machine a etat reception spi
char AutoLight = OFF;                         //lumiere automatique
char PosCursorPrgm = 0;                     //position du cusreur de selection de prgm
short int PosX = 400;                       //position sur l'ecran du robot en X (Xmax=750)
short int PosY = 240;                       //position en Y (Ymax=430)
unsigned char CptSpi = 1;                   //compteur des caractères recu en spi
unsigned char TailleMsgSpi = 0;             //stockage de la taille du message en spi
unsigned char TypeVarSpi = AUCUN;           //stockage du nom de la variable du message en spi
short int Checksum = 0;                     //calcul d'erreur d'envoi en spi
unsigned char FlagSpi = 1;					//nouveau message en spi
unsigned char PrgmChoisi = 0;
unsigned char CptSend = 1;                  //compteur d'envoi des octets
unsigned char FlagSleep = 0;                //drapeau de mise en veille de l'écran
int TailleTerm = 0;
int Score = 0;
unsigned char PrgmSens = 0;
unsigned char Stop = 0;
char TabPileSend[TAILLE_SEND];        //pile de récéption pour stocker les réponse à donner
unsigned char CptSpiSend = 0;
unsigned char SendNbSpi = 0;
unsigned char CptReadPile = 0;
unsigned char CptPile = 0;

char TextSpi[TAILLE_SPI_CHAINE];          	//chaine de text recu de la communication spi
char text_prgm[NB_PRGM][TAILLE_PRGM_CHAINE] = {"prgmDroite","prgmGauche","prgmBleu","prgmBlanc","prgmRouge"}; //nom des prgm à choisir


char cpt = 0;                 //test


void main(void) {
	ClockInit();
	ADCInit();
	SpiInit();
	UartInit();
	GpioInit();
	InitTmr0();
	//vars
	char CptTmr0 = 0;					//compteur timer 0 pour la led
	float adc_res = 0;					//valeur de l'adc
	unsigned char taille_text_prgm;		//taille de la chaine TextPrgm
	unsigned char taille_text_spi;		//taille de la chaine TextSpi
    __delay_ms(2000);                   //attente du démarrage de l'écran
	//boucle infini
	while(1){
		if(TMR0IF) {
			TMR0L = 0xB0; //preload value
			TMR0H = 0x3C;
			TMR0IF = 0;
			CptTmr0++;
			if(CptTmr0 > 10) {
				blink();
				CptTmr0 = 0;
			}
			if(AutoLight) {
				adc_res = Adc(1); //4.5V noir => 5, 0.4V ambient => 80 et 0.10 lum => 90
				//---------------affine------------------------//
				//adc_res = (int)(adc_res/10); //90-(adc_res/10); ou 90*exp(-adc_res/901);
				//if(adc_res < 0) {
				//	adc_res = 0;
				//}
				//if(adc_res > 85) {
				//	adc_res = 85;
				//}
				//adc_res = 90-adc_res;
				//---------------exponentielle------------------------//
				adc_res = 90*exp(-(adc_res/901));
				adc_res = (int)(adc_res);
				if(adc_res < 0) {
					adc_res = 0;
				}
				if(adc_res > 99) {
					adc_res = 99;
				}
				//64 69 6D 3D 30+Di 30+Un FF FF FF
				//printS("dim=50ÿÿÿ");
				SendMsgSreen[0] = 0x64;
				SendMsgSreen[1] = 0x69;
				SendMsgSreen[2] = 0x6D;
				SendMsgSreen[3] = 0x3D;
				SendMsgSreen[4] = 0x30 + (char)(adc_res/10);
				SendMsgSreen[5] = 0x30 + (char)(adc_res-(10*(char)(adc_res/10)));
				SendMsgSreen[6] = 0xFF;
				SendMsgSreen[7] = 0xFF;
				SendMsgSreen[8] = 0xFF;
				PrintS(SendMsgSreen);
				ClearArray();
				if(EtatScreen == 0) {
					//68 30 2E 76 61 6C 3D 30+Di 30+Un FF FF FF
					//printS("h0.val=50ÿÿÿ");
					SendMsgSreen[0] = 0x68;
					SendMsgSreen[1] = 0x30;
					SendMsgSreen[2] = 0x2E;
					SendMsgSreen[3] = 0x76;
					SendMsgSreen[4] = 0x61;
					SendMsgSreen[5] = 0x6C;
					SendMsgSreen[6] = 0x3D;
					SendMsgSreen[7] = 0x30 + (char)(adc_res/10);
					SendMsgSreen[8] = 0x30 + (char)(adc_res-(10*(char)(adc_res/10)));
					SendMsgSreen[9] = 0xFF;
					SendMsgSreen[10] = 0xFF;
					SendMsgSreen[11] = 0xFF;
					PrintS(SendMsgSreen);
					ClearArray();
				}
			}
			if(FlagSpi) {
				FlagSpi = 0;
				switch(EtatScreen) {
					case SCREEN_MAIN:
						//74 30 2E 74 78 74 3D 22 3E mon_txt 22 FF FF FF
						//printS("t0.txt=\">mont_text\"ÿÿÿ");
						SendMsgSreen[0] = 0x74;
						SendMsgSreen[1] = 0x30;
						SendMsgSreen[2] = 0x2E;
						SendMsgSreen[3] = 0x74;
						SendMsgSreen[4] = 0x78;
						SendMsgSreen[5] = 0x74;
						SendMsgSreen[6] = 0x3D;
						SendMsgSreen[7] = 0x22;
						SendMsgSreen[8] = 0x3E;
						taille_text_prgm = 0;
						while(taille_text_prgm < TAILLE_PRGM_CHAINE && text_prgm[PosCursorPrgm][taille_text_prgm] != 0) {
							SendMsgSreen[taille_text_prgm+9] = text_prgm[PosCursorPrgm][taille_text_prgm];
							taille_text_prgm++;
						}
						SendMsgSreen[taille_text_prgm+9] = 0x22;
						SendMsgSreen[taille_text_prgm+10] = 0xFF;
						SendMsgSreen[taille_text_prgm+11] = 0xFF;
						SendMsgSreen[taille_text_prgm+12] = 0xFF;
						PrintS(SendMsgSreen);
						ClearArray();
                        SendMsgSreen[0] = 0x6E;
						SendMsgSreen[1] = 0x30;
						SendMsgSreen[2] = 0x2E;
						SendMsgSreen[3] = 0x76;
						SendMsgSreen[4] = 0x61;
						SendMsgSreen[5] = 0x6C;
						SendMsgSreen[6] = 0x3D;
                        SendMsgSreen[7] = 0x30 + (char)(Score/100);
                        SendMsgSreen[8] = 0x30 + (char)((Score-((char)(Score/100)*100))/10);
                        SendMsgSreen[9] = 0x30 + (char)((Score-((char)(Score/10)*10)));
						SendMsgSreen[10] = 0xFF;
						SendMsgSreen[11] = 0xFF;
						SendMsgSreen[12] = 0xFF;
						PrintS(SendMsgSreen);
						ClearArray();
						break;
					case SREEN_CONTROL:
						//72 65 66 20 30 FF FF FF
						//printS("ref 0");
						FlagSpi = 1;
						SendMsgSreen[0] = 0x72;
						SendMsgSreen[1] = 0x65;
						SendMsgSreen[2] = 0x66;
						SendMsgSreen[3] = 0x20;
						SendMsgSreen[4] = 0x30;
						SendMsgSreen[5] = 0xFF;
						SendMsgSreen[6] = 0xFF;
						SendMsgSreen[7] = 0xFF;
						PrintS(SendMsgSreen);
						ClearArray();
						//70 69 63 20 31 30 30 2C 31 30 30 2C 32 FF FF FF
						//printS("pic 100,100,2");
						SendMsgSreen[0] = 0x70;
						SendMsgSreen[1] = 0x69;
						SendMsgSreen[2] = 0x63;
						SendMsgSreen[3] = 0x20;
						SendMsgSreen[4] = 0x30 + (char)(PosX/100);
						SendMsgSreen[5] = 0x30 + (char)((PosX-(100*(char)(PosX/100)))/10);
						SendMsgSreen[6] = 0x30 + (char)(PosX-(10*(char)(PosX/10)));
						SendMsgSreen[7] = 0x2C;
						SendMsgSreen[8] = 0x30 + (char)(PosY/100);
						SendMsgSreen[9] = 0x30 + (char)((PosY-(100*(char)(PosY/100)))/10);
						SendMsgSreen[10] = 0x30 + (char)(PosY-(10*(char)(PosY/10)));
						SendMsgSreen[11] = 0x2C;
						SendMsgSreen[12] = 0x32;
						SendMsgSreen[13] = 0xFF;
						SendMsgSreen[14] = 0xFF;
						SendMsgSreen[15] = 0xFF;
						PrintS(SendMsgSreen);
						ClearArray();
						break;
					case SCREEN_TERMINAL:
						//74 30+NbTxtBox 2E 74 78 74 3D 22 mon_txt 22 FF FF FF
						//printS("t0.txt=\"mont_text\"ÿÿÿ");
						if(TypeVarSpi == TERM1 || TypeVarSpi == TERM2 || TypeVarSpi == TERM3 || TypeVarSpi == TERM4) {
							TypeVarSpi -= TERM1;
							SendMsgSreen[0] = 0x74;
							SendMsgSreen[1] = 0x30 + TypeVarSpi;
							SendMsgSreen[2] = 0x2E;
							SendMsgSreen[3] = 0x74;
							SendMsgSreen[4] = 0x78;
							SendMsgSreen[5] = 0x74;
							SendMsgSreen[6] = 0x3D;
							SendMsgSreen[7] = 0x22;
							taille_text_spi = 0;
							/*while(taille_text_spi < TAILLE_SPI_CHAINE && TextSpi[taille_text_spi] != 0) {
								SendMsgSreen[taille_text_spi+8] = TextSpi[taille_text_spi];
								taille_text_spi++;
							}*/
                            while(taille_text_spi < TailleTerm) {
								SendMsgSreen[taille_text_spi+8] = TextSpi[taille_text_spi];
								taille_text_spi++;
							}
							SendMsgSreen[taille_text_spi+8] = 0x22;
							SendMsgSreen[taille_text_spi+9] = 0xFF;
							SendMsgSreen[taille_text_spi+10] = 0xFF;
							SendMsgSreen[taille_text_spi+11] = 0xFF;
							PrintS(SendMsgSreen);
							ClearArray();
						}
						break;
					case SCREEN_PID:
						//graph
						break;
					case SCREEN_DEBUG:
						//74 30+NbTxtBox 2E 74 78 74 3D 22 mon_txt 22 FF FF FF
						//printS("t0.txt=\"mont_text\"ÿÿÿ");
						FlagSpi = 1;
						SendMsgSreen[0] = 0x74;
						SendMsgSreen[1] = 0x30;
						SendMsgSreen[2] = 0x2E;
						SendMsgSreen[3] = 0x74;
						SendMsgSreen[4] = 0x78;
						SendMsgSreen[5] = 0x74;
						SendMsgSreen[6] = 0x3D;
						SendMsgSreen[7] = 0x22;
						SendMsgSreen[8] = 0x74;
						SendMsgSreen[9] = 0x65;
						SendMsgSreen[10] = 0x73;
						SendMsgSreen[11] = 0x74;
						SendMsgSreen[12] = 0x30 + cpt;
						cpt++;
						if(cpt > 9) {cpt = 0;}
						SendMsgSreen[13] = 0x22;
						SendMsgSreen[14] = 0xFF;
						SendMsgSreen[15] = 0xFF;
						SendMsgSreen[16] = 0xFF;
						PrintS(SendMsgSreen);
						ClearArray();
						break;
					default:
						EtatScreen = SCREEN_MAIN;
						break;
				}
			}
		}
	}
}

/**************************** INIT ***********************************/
void ClockInit(void) {
	OSCCONbits.IRCF = 0b111;	//8MHZ
	OSCTUNEbits.PLLEN = 1;	//enable PLL
	while(!OSCCONbits.IOFS);	//wait for INTOSC stabilization
}

void ADCInit(void) {
	TRISAbits.TRISA0 = 1;
	TRISAbits.TRISA1 = 1;
	TRISAbits.TRISA2 = 1;
	TRISAbits.TRISA3 = 1;

	//ADCON1bits.PCFG = 0b1101;   //AN0-1 are analog input 1011
    ADCON1bits.PCFG = 0b1011;
	ADCON1bits.VCFG = 0;        //Voltage references are Vdd and Vss
	ADCON2bits.ADFM = 1;        //Result is Right-justified
	ADCON2bits.ACQT = 0b010;    //TAQC = 4TAD
	ADCON2bits.ADCS = 0b001;    //TAD = Tosc*8  
}

void SpiInit(void) {
	TRISA5 = 1; //SS  Input
	TRISC3 = 1; //SCK Input
	TRISC4 = 1; //SDI Input
	TRISC5 = 0; //SDO Output
    LATAbits.LATA5 = 1;

	PIR1bits.SSPIF = 0; //Clear SPI interrupt flag
	PIE1bits.SSPIE = 1; //Enable SPI interrupt

	SSPSTATbits.SMP = 0; //SMP must be cleared when SPI is used in Slave mode.
	SSPSTATbits.CKE = 1; //SPI Clock Select bit : 1 = Transmit occurs on transition from active to Idle clock state
	SSPCON1bits.WCOL = 0; //Clear write collision flag
	SSPCON1bits.CKP = 0; //Idle state for clock : 0 = Idle state for clock is a low level
	SSPCON1bits.SSPM = 0b0100; //SPI Slave mode, clock = SCK pin; SS pin control enabled
	SSPCON1bits.SSPEN = 1; //Enable SPI
    
    SSPBUF = 0; //vide le buffer

	INTCONbits.PEIE = 1; //Enable peripheral interrupts
	INTCONbits.GIE = 1; //Enable global interrupts
}

void UartInit(void) {
	//Pin
	TRISC6 = 0; //TX output
	TRISC7 = 1; //RX input

	//cofig
	//TX
	TX9 = 0; //8 bits mode
	SYNC = 0; //asynchronous mode
	SENDB = 0; //no send sync break trans complet
	BRGH = 0; //correction baud rate
	//RX
	RX9 = 0; //receive 8 bits
	CREN = 1; //=> continus receive
	FERR = 0; //no farming error bit
	OERR = 0; //no overrun error bit
	RXDTP = 0; //polarity bit non invrting => 0
	TXCKP = 0; //transmit high level
	BRG16 = 1; //correction high  speed
	WUE = 0; //no wake up
	ABDEN = 0; //no auto baud rate
	SPBRGH = 0;
	SPBRG = 16; //207=>9600 et 16=>115200 ,15

	//interrupt
	RCIE = 1; //active interrupt
	RCIF =  0;

	TXEN = 1; //enable
	SPEN = 1; //serial port en
}

void InitTmr0(void) {
	T08BIT = 0; //16 bits mode
	T0CS = 0; //internal clock source
	T0SE = 0; //rising edge
	PSA = 0; //prescale on
	T0PS2 = 0; //000 => div 1/4, 011 => div 1/16
	T0PS1 = 1;
	T0PS0 = 1;
	TMR0L = 0xB0; //preload value
	TMR0H = 0x3C;
	TMR0IF = 0; //interrupt flag
	//TMR0IE = 1; //interrupt on

	TMR0ON = 1; //TMR0 on
}

/**************************** FONCTION ***********************************/

/*******************************************************************/
// Nom de la fonction: Adc
//                    
// Description: Fonction retournant le résultat d'un conversion analogique digitale
//
// Entrées: unsigned char channel : numero du channel selectionné
//
//
// Sorties: return (int) : résultat de la conversion A/D
//
//                              
/*******************************************************************/
unsigned int Adc(unsigned char channel) {
	ADCON0bits.ADON = 1;		//A/D Converter module is enabled
	ADCON0bits.CHS = channel;	//choose analog input
	ADCON0bits.GO = 1;			//Start A/D conversion
	while(ADCON0bits.GO);		//while conversion is in pogress
	ADCON0bits.ADON = 0;		//A/D Converter module is disabled
	return (ADRESH << 8) + ADRESL;   
}

/*******************************************************************/
// Nom de la fonction: PrintC
//                    
// Description: Fonction d'envoi d'un caractère en UART
//
// Entrées: char c_printC : caractère à envoyer
//
//                              
/*******************************************************************/
void PrintC(char c_printC) {
	while(TXSTA1bits.TRMT == 0) {};
	TXREG1 = c_printC;
	asm("nop");
}

/*******************************************************************/
// Nom de la fonction: PrintS
//                    
// Description: Fonction d'envoi d'une chaine de caractères en UART
//
// Entrées: const char * s_printS : chaine de caractère à envoyer
//
//                              
/*******************************************************************/
void PrintS(const char * s_printS) {
	while(*s_printS) {
		PrintC(*s_printS++);
	}
	PrintC(0xFF);
	PrintC(0xFF);
	PrintC(0xFF);
}

/*******************************************************************/
// Nom de la fonction: ClearArray
//                    
// Description: Fonction effacant la chaine de caractère SendMsgSreen
//
//                              
/*******************************************************************/
void ClearArray(void) {
	char cpt_vide = 0; //compteur pour vider la chaine SendMsgSreen[]
	for(cpt_vide=0;cpt_vide<TAILLE_SEND_CHAINE;cpt_vide++) {
		SendMsgSreen[cpt_vide] = '\0';
	}
}

/*******************************************************************/
// Nom de la fonction: Modulo
//                    
// Description: Fonction renvoi le modulo à 256 d'un entier
//
// Entrées: short int M_nombre : nombre à renvoyer entre 0 et 255
//
// Sorties: char M_nombre : nombre renvoyé
//
//                              
/*******************************************************************/
char Modulo(short int M_nombre) {
	while(M_nombre > 255) {
		M_nombre -= 256;
	}
	return M_nombre;
}

/**************************** ISR ***********************************/

void interrupt ISR(void) {
	//----------------SPI------------------------
	if(PIR1bits.SSPIF){
		char data_spi = SSPBUF;
		PIR1bits.SSPIF = 0;
		//TextSpi[CptSpi] = SSPBUF;
		//CptSpi++;
		switch(EtatSpi) {
			case SPI_IDLE:
				TailleMsgSpi = data_spi;
                if(TailleMsgSpi) {
                    EtatSpi = SPI_NUM_VAR;
                }
				Checksum = 0;
				CptSpi = 1;
				break;
			case SPI_NUM_VAR:
				TypeVarSpi = data_spi;
                if(TailleMsgSpi == 2) {
                    EtatSpi = SPI_CHECKSUM;
                } else {
                    EtatSpi = SPI_MSG_UTILE;
                }
				CptSpi++;
				break;
			case SPI_MSG_UTILE:
				TextSpi[CptSpi-2] = data_spi;
				CptSpi++;
				if(CptSpi == TailleMsgSpi) {
					EtatSpi = SPI_CHECKSUM;
				}
				break;
			case SPI_CHECKSUM:
				if(Modulo(Checksum) == data_spi) {
					FlagSpi = 1;
					CptSpi = 0;
					switch(TypeVarSpi) {
						case PRGM1:
                            for(CptSpi=0;CptSpi<TAILLE_PRGM_CHAINE;CptSpi++) {
                                if(CptSpi < TailleMsgSpi-2) {
                                    text_prgm[0][CptSpi] = TextSpi[CptSpi];
                                } else {
                                    text_prgm[0][CptSpi] = '\0';
                                }
                            }
							break;
						case PRGM2:
                            for(CptSpi=0;CptSpi<TAILLE_PRGM_CHAINE;CptSpi++) {
                                if(CptSpi < TailleMsgSpi-2) {
                                    text_prgm[1][CptSpi] = TextSpi[CptSpi];
                                } else {
                                    text_prgm[1][CptSpi] = '\0';
                                }
                            }
							break;
						case PRGM3:
                            for(CptSpi=0;CptSpi<TAILLE_PRGM_CHAINE;CptSpi++) {
                                if(CptSpi < TailleMsgSpi-2) {
                                    text_prgm[2][CptSpi] = TextSpi[CptSpi];
                                } else {
                                    text_prgm[2][CptSpi] = '\0';
                                }
                            }
							break;
						case PRGM4:
                            for(CptSpi=0;CptSpi<TAILLE_PRGM_CHAINE;CptSpi++) {
                                if(CptSpi < TailleMsgSpi-2) {
                                    text_prgm[3][CptSpi] = TextSpi[CptSpi];
                                } else {
                                    text_prgm[3][CptSpi] = '\0';
                                }
                            }
							break;
						case PRGM5:
                            for(CptSpi=0;CptSpi<TAILLE_PRGM_CHAINE;CptSpi++) {
                                if(CptSpi < TailleMsgSpi-2) {
                                    text_prgm[4][CptSpi] = TextSpi[CptSpi];
                                } else {
                                    text_prgm[4][CptSpi] = '\0';
                                }
                            }
							break;
						case POS_X:
							PosX = TextSpi[0];
							PosX = PosX << 8;
							PosX += TextSpi[1];
							if(PosX > 750) {
								PosX = 750;
							}
							break;
						case POS_Y:
							PosY = TextSpi[0];
							PosY = PosY << 8;
							PosY += TextSpi[1];
							if(PosY > 430) {
								PosY = 430;
							}
							break;
						case DEBUG:
							//a faire
							break;
                        case TERM1:
                        case TERM2:
                        case TERM3:
                        case TERM4:
                            TailleTerm = TailleMsgSpi-2;
                            break;
                        case SCORE:
                            Score = TextSpi[0];
							Score = Score << 8;
							Score += TextSpi[1];
							if(Score > 999) {
								Score = 999;
							}
                            break;
                        case RESET:
                            asm("RESET");
                            break;
						default:
							//do nothing
							break;
					}
				}
				EtatSpi = SPI_IDLE;
				break;
			default:
				EtatSpi = SPI_IDLE;
				break;
		}
		Checksum += data_spi;
        if(TabPileSend[CptReadPile]) {
            CptSpiSend++;
            switch(CptSpiSend) {
                case 1:
                    SSPBUF = 0x03;
                    break;
                case 2:
                    SSPBUF = TabPileSend[CptReadPile];
                    break;
                case 3:
                    switch(TabPileSend[CptReadPile]) {
                        case SENS:
                            SendNbSpi = PrgmSens;
                            PrgmSens = 0;
                            break;
                        case SHUTDOWN:
                            SendNbSpi = Stop;
                            Stop = 0;
                            break;
                        case CHOIXPRGM:
                            SendNbSpi = PosCursorPrgm;
                            break;
                    }
                    SSPBUF = SendNbSpi;
                    break;
                case 4:
                    SSPBUF = (SendNbSpi+TabPileSend[CptReadPile]+3)%256;
                    TabPileSend[CptReadPile] = AUCUN;
                    CptReadPile++;
                    CptReadPile %= TAILLE_SEND;
                    CptSpiSend = 0;
                    SendNbSpi = 0;
                    break;
            }
        } else {
            SSPBUF = 0;
        }
	}
	//----------------UART-----------------------
	if(RCIF) {
		char Receive;			//reception de l'ecran
		RCIF = 0;
		Receive = RCREG;
        if(FlagSleep) {
            if(Receive == 0x68) {
                FlagSleep = 0; //a tester
                FlagSpi = 1;
            }
        } else {
            switch(Receive) {
                case 0x31:
                case 0x32:
                case 0x33:
                case 0x34:
                    EtatScreen = Receive-0x30;
                    FlagSpi = 1;
                    break;
                case 0x41:
                case 0x42:
                case 0x43:
                case 0x44:
                    EtatScreen = SCREEN_MAIN;
                    FlagSpi = 1;
                    break;
                case 0x35:
                    AutoLight = 1;
                    break;
                case 0x36:
                    AutoLight = 0;
                    break;
                case 0x37: //down
                    if(PosCursorPrgm > 0) {
                        PosCursorPrgm--;
                        FlagSpi = 1;
                    }
                    break;
                case 0x38: //up
                    if(PosCursorPrgm < NB_PRGM-1) {
                        PosCursorPrgm++;
                        FlagSpi = 1;
                    }
                    break;
                case 0x39:
                    //send spi pgrm choisi -> PosCursorPrgm
                    PrgmChoisi = 1;
                    TabPileSend[CptPile] = CHOIXPRGM;
                    CptPile++;
                    CptPile %= TAILLE_SEND;
                    break;
                case 0x40:
                    FlagSleep = 1;
                    break;
                case 0x51:
                    PrgmSens = 1;
                    TabPileSend[CptPile] = SENS;
                    CptPile++;
                    CptPile %= TAILLE_SEND;
                    break;
                case 0x52:
                    PrgmSens = 2;
                    TabPileSend[CptPile] = SENS;
                    CptPile++;
                    CptPile %= TAILLE_SEND;
                    break;
                case 0x53:
                    Stop = 1;
                    TabPileSend[CptPile] = SHUTDOWN;
                    CptPile++;
                    CptPile %= TAILLE_SEND;
                    break;
                default:
                    //do nothing
                    break;
            }
		}
	}
}