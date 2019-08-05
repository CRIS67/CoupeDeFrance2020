/*
 * File:   AX12.c
 * Author: Johan
 *
 * Created on 24 avril 2018, 14:37
 */


#include "AX12.h"

char TxLoopBufferAX12[AX12_TX_SIZE];
char RxBufferAX12[AX12_RX_SIZE];
unsigned char iDAX12,iFAX12;                //index of first data and of first free element
unsigned char TxOnAX12;
unsigned char iRxAX12;

void initAX12()
{
    IEC5bits.U3TXIE = 0;            //Disable UART1 Tx interrupt
    IEC5bits.U3RXIE = 0;            //Disable UART1 Rx interrupt
    
    RPOR12bits.RP127R = 0b011011; 
    
    U3MODEbits.UARTEN = 0;
    U3MODEbits.USIDL = 0;           // Bit13 Continue in Idle
    U3MODEbits.IREN = 0;            // Bit12 No IR translation
    U3MODEbits.RTSMD = 0;           // Bit11 Simplex Mode
    U3MODEbits.UEN = 0b00;
    U3MODEbits.LPBACK = 0;
    U3MODEbits.ABAUD = 0;
    U3MODEbits.URXINV = 0;
    U3MODEbits.BRGH = 1;
    U3MODEbits.PDSEL = 0b00;
    U3MODEbits.STSEL = 0;

    U3STAbits.UTXBRK = 0;           //Bit11 Disabled
    U3STAbits.UTXISEL0 = 0;
    U3STAbits.UTXISEL1 = 0;         //Interrupt is generated when any character is transferred to the Transmit Shift Register and the transmit buffer is empty (which implies at least one location is empty in the transmit buffer)
    U3STAbits.UTXINV = 0;
    U3STAbits.ADDEN = 0;
    U3STAbits.URXISEL = 0;
    
    U3BRG = AX12_BRGVAL;
    
    IFS5bits.U3TXIF = 0;            // Clear the Transmit Interrupt Flag
    IEC5bits.U3TXIE = 1;            // Enable Transmit Interrupts
    IFS5bits.U3RXIF = 0;            // Clear the Recieve Interrupt Flag
    IEC5bits.U3RXIE = 1;            // Enable Recieve Interrupts
    
    U3MODEbits.UARTEN = 1;          //Enable the module
    U3STAbits.UTXEN = 1;
    
    iDAX12 = 0;
    iFAX12 = 0;
    TxOnAX12 = 0;
    iRxAX12 = 0;
}

void pushAX12(char c)
{
    TxLoopBufferAX12[iFAX12] = c;
    iFAX12++;
    if(iFAX12 == AX12_TX_SIZE)
        iFAX12 = 0;
}

char popAX12()
{
    char r = TxLoopBufferAX12[iDAX12];
    iDAX12++;
    if(iDAX12 == AX12_TX_SIZE)
        iDAX12 = 0;
    return r;
}

void turnLedAX12(char id, char state)
{
    char data[] = {AX12_COMMAND_LED, state};
    writeDataAX12(id, 2, data);
}

void blockAnswerAX12(char id)
{
    char data[] = {AX12_COMMAND_STATUS_RETURN_LEVEL, 0};
    writeDataAX12(id, 2, data);
}

void rotateToAX12(char id, char posL, char posH)
{
    char data[] = {AX12_COMMAND_GOAL_POSITION_LOW, posL, posH};
    writeDataAX12(id, 3, data);
}

void writeDataAX12(char id, char length, char* datas)
{
    unsigned char i = 0;
    unsigned char checksum = 0;
    if(TxOnAX12 != 0){
        pushAX12(0xFF);
    }
    pushAX12(0xFF);
    pushAX12(id);
    checksum += id;
    pushAX12(length + 2);
    checksum += length + 2;
    pushAX12(AX12_INSTRUCTION_WRITE_DATA);
    checksum += AX12_INSTRUCTION_WRITE_DATA;
    for(i = 0; i < length; i++)
    {
        pushAX12(datas[i]);
        checksum += datas[i];
    }
    pushAX12(~checksum);
    if(TxOnAX12 == 0){
        TxOnAX12 = 1;
        U3TXREG = 0xFF;
    }
}

/*void LedOnAllAX12()
{
    pushAX12(0xFF);
    pushAX12(0xFE);
    pushAX12(0x04);
    pushAX12(0x03);
    pushAX12(0x19);
    pushAX12(0x01);
    pushAX12(~(0xFD+0x05+0x03+0x19+0x01));
    if(TxOnAX12 == 0){
        TxOnAX12 = 1;
        U3TXREG = 0xFF;
    }
    else
        pushAX12(0xFF);
}*/

/*void LedOffAllAX12()
{
    pushAX12(0xFF);
    pushAX12(0x03);
    pushAX12(0x04);
    pushAX12(0x03);
    pushAX12(0x19);
    pushAX12(0x00);
    pushAX12(~(0xFD+0x05+0x03+0x19+0x00));
    if(TxOnAX12 == 0){
        TxOnAX12 = 1;
        U3TXREG = 0xFF;
    }
    else
        pushAX12(0xFF);
}*/

void __attribute__((interrupt,no_auto_psv)) _U3TXInterrupt(void)
{
    IFS5bits.U3TXIF = 0; // Clear TX Interrupt flag
    if(iDAX12 != iFAX12){
        U3TXREG = popAX12();
    }
    else
        TxOnAX12 = 0;
}

void __attribute__((interrupt,no_auto_psv)) _U3RXInterrupt(void)
{
    IFS5bits.U3RXIF = 0; // Clear RX Interrupt flag
}
