/*
 * File:   DMA.c
 * Author: Quentin Boyer
 *
 * Created on 1 juillet 2018, 14:32
 */


#include "DMA.h"

#define DMA_BUFFER_A_SIZE   26
#define DMA_BUFFER_SEND_SIZE   100
#define DMA_BUFFER_RECEIVE_SIZE   100
uint8_t BufferA[8];
unsigned int BufferB[8];
uint8_t testBuffer[8];
//unsigned char BufferA[DMA_BUFFER_A_SIZE];
//unsigned int BufferB[8];
__eds__ int BufferC[8] __attribute__((eds,aligned(256)));
char BufferSend[DMA_BUFFER_SEND_SIZE];
unsigned char BufferRXDMA[DMA_BUFFER_RECEIVE_SIZE];

extern volatile uint8_t RxDMABuffer[RX_DMA_SIZE];


int testInt;

void initDMA() {
    initDMA0();
    initDMA1();
}
void initDMA0(){    //TX2 Channel
    //DMA0CON = 0x2003; // One-Shot, Post-Increment, RAM-to-Peripheral, ping-pong, word
    //DMA0CON = 0x2001; // One-Shot, Post-Increment, RAM-to-Peripheral , word
    /*DMA0CON = 0x6001;  // One-Shot, Post-Increment, RAM-to-Peripheral, byte
    
    DMA0CNT = DMA_BUFFER_A_SIZE - 1; // Eight DMA requests
    DMA0REQ = 0x001F; // Select UART2 transmitter
    DMA0PAD = (volatile unsigned int) &U2TXREG;
    
    //DMA0STAL = __builtin_dmaoffset(BufferA);
    DMA0STAL = (int)&BufferA;
    DMA0STAH = (int)0x0000;
    
    DMA0STBL = (int)&BufferB;
    DMA0STBH = 0x0000;
    IFS0bits.DMA0IF = 0; // Clear DMA Interrupt Flag
    IEC0bits.DMA0IE = 1; // Enable DMA interrupt*/
}
void initDMA1(){    //RX2 Channel
    DMA1CON = 0x0000; // Continuous, Post-Inc., Periph-RAM
    DMA1CONbits.SIZE = 1;   //Byte
    DMA1CNT = RX_DMA_SIZE - 1; // RX_DMA_SIZE DMA requests
    DMA1REQ = 0x001E; // Select UART2 receiver
    DMA1PAD = 0x0236;//(volatile unsigned int) &U2RXREG;
    DMA1STAL = (int)&RxDMABuffer;
    DMA1STAH = 0x0000;
    IFS0bits.DMA1IF = 0; // Clear DMA interrupt
    IEC0bits.DMA1IE = 1; // Enable DMA interrupt
    DMA1CONbits.CHEN = 1; // Enable DMA channel
}


void __attribute__((__interrupt__,no_auto_psv)) _DMA0Interrupt(void){
    IFS0bits.DMA0IF = 0; // Clear the DMA0 Interrupt Flag
}
void __attribute__((__interrupt__,no_auto_psv)) _DMA1Interrupt(void){
    /*static unsigned int BufferCount = 0; // Keep record of which buffer
    // contains RX Data
    if(BufferCount == 0){
        DMA0STAL = __builtin_dmaoffset(BufferA);
        DMA0STAH = 0x0000;
    }
    else{
        DMA0STBL = __builtin_dmaoffset(BufferB);
        DMA0STBH = 0x0000;
    }
    DMA0CONbits.CHEN = 1; // Enable DMA0 channel
    DMA0REQbits.FORCE = 1; // Manual mode: Kick-start the 1st transfer
    BufferCount ^= 1;*/
    IFS0bits.DMA1IF = 0; // Clear the DMA1 Interrupt Flag
}
void sendDMA(char* buffer, unsigned int size){
    DMA0STAL = (int)&BufferSend;
    DMA0STAH = (int)0x0000;
    //BufferSend[0] = '\0';
    if(DMA0CONbits.CHEN){
        int s = DMA0CNT;
        int s2 = DSADRL;
        DMA0CONbits.CHEN = 0;
        BufferSend[12] = '\0';
        strncat(BufferSend,buffer,size);
        DMA0CNT = size + s;
        DMA0STAL = s2 + 1;
        DMA0CONbits.CHEN = 1;
        DMA0REQbits.FORCE = 1;
    }
    else{
        memcpy(BufferSend,buffer,size);
        DMA0CNT = size - 1;
        //strncat(BufferSend,buffer,size);
        DMA0CONbits.CHEN = 1; // Enable DMA channel
        DMA0REQbits.FORCE = 1;
    }
}
