#include "Arduino.h"
#define __STDINT__
#include "UsartHardware.h"
#include "EventQueue.h"
#include "Event.h"
#include "List.h"
#include "irq.h"
#include "UsartDriver.h"
#include "TimerEventQueue.h"


#define isLastByte(info) (info.lastByte)

volatile UsartInfo usartHardwareInfo;
STATIC void initUsartHardwareInfo(UsartPort port){
    usartHardwareInfo.rxCallBack = (RxCallback)usartReceiveHandler;
    usartHardwareInfo.txCallBack = (TxCallback)usartTransmissionHandler;
    usartHardwareInfo.hwTxState = HW_TX_IDLE;
    usartHardwareInfo.hwRxState = HW_RX_IDLE;
    usartHardwareInfo.txTurn =0;
    usartHardwareInfo.lastByte =0;
}

void usartHardwareInit(void){
		disableIRQ();
    initUsartHardwareInfo(MAIN_CONTROLLER);
    enableIRQ();
}

void hardwareUsartTransmit(UsartPort port){
    disableIRQ();
    usartHardwareInfo.txTurn = 1;
    enableIRQ();
}

void hardwareUsartReceive(UsartPort port){
    disableIRQ();
    usartHardwareInfo.txTurn = 0;
    enableIRQ();
}
/*
void usartIrqHandler(UsartPort port){
    char rxByte;
    char txByte;

    if(usartHardwareInfo.txTurn){
        txByte = usartTransmitHardwareHandler(port);
				//UART1_ClearITPendingBit(UART1_IT_TC);
				//UART1_ClearFlag(UART1_IT_TC);
        Serial1.write(txByte);
    }
    else{
        rxByte = Serial1.read();
				//UART1_ClearITPendingBit(UART1_IT_RXNE);
				//UART1_ClearFlag(UART1_IT_RXNE);
        usartReceiveHardwareHandler(port,rxByte);
    }
}
*/
uint8_t usartTransmitHardwareHandler(UsartPort port){
    uint8_t transmitByte;
    switch(usartHardwareInfo.hwTxState){
        case HW_TX_IDLE :
            usartHardwareInfo.lastByte = 0;
            usartHardwareInfo.hwTxState = HW_TX_SEND_DELIMITER;
            transmitByte = 0x7E;
            break;
        case HW_TX_SEND_DELIMITER :
            usartHardwareInfo.hwTxState = HW_TX_SEND_BYTE;
            transmitByte = 0x81;
            break;
        case HW_TX_SEND_BYTE :
            transmitByte = usartHardwareInfo.txCallBack(port);
            //instead of passing port better usartHardwareInfo.usartDriverInfo
            if(transmitByte == 0x7E){
                usartHardwareInfo.hwTxState = HW_TX_SEND_7E_BYTE;
            }
            else if(usartHardwareInfo.lastByte){
                endOfUsartTxHandler(port);
								hardwareUsartReceive(port);
                usartHardwareInfo.hwTxState = HW_TX_IDLE;
                //usartHardwareInfo.lastByte = 0;
            }
            break;
        case HW_TX_SEND_7E_BYTE :
            if(usartHardwareInfo.lastByte){
                endOfUsartTxHandler(port);
								hardwareUsartReceive(port);
                usartHardwareInfo.hwTxState = HW_TX_IDLE;
                //usartHardwareInfo.lastByte = 0;
            }
            else{
                usartHardwareInfo.hwTxState = HW_TX_SEND_BYTE;
            }
            transmitByte = 0xE7;
            break;
    }
    return transmitByte;
}

void usartReceiveHardwareHandler(UsartPort port,uint8_t rxByte){
    switch(usartHardwareInfo.hwRxState){
        case HW_RX_IDLE :
            if(rxByte == 0x7E){
                usartHardwareInfo.hwRxState = HW_RX_RECEIVED_DELIMITER;
            }
            break;
        case HW_RX_RECEIVED_DELIMITER :
            if(rxByte == 0x81){
                usartHardwareInfo.rxCallBack(port,RX_PACKET_START<<8);
                usartHardwareInfo.hwRxState = HW_RX_RECEIVE_BYTE;
            }
            else{
                usartHardwareInfo.hwRxState = HW_RX_IDLE;
            }
            break;
        case HW_RX_RECEIVE_BYTE :
            if(rxByte == 0x7E){
                usartHardwareInfo.hwRxState = HW_RX_RECEIVE_7E_BYTE;
            }
            else{
                usartHardwareInfo.rxCallBack(port,rxByte);
            }
            break;
        case HW_RX_RECEIVE_7E_BYTE :
            if(rxByte == 0x81){
                usartHardwareInfo.rxCallBack(port,(RX_PACKET_START<<8));
                usartHardwareInfo.hwRxState = HW_RX_RECEIVE_BYTE;
            }
            else if (rxByte == 0xE7){
                usartHardwareInfo.rxCallBack(port,0x7E);
                usartHardwareInfo.hwRxState = HW_RX_RECEIVE_BYTE;
            }
            else{
                usartHardwareInfo.hwRxState = HW_RX_IDLE;
            }
            break;
    }
}

void setHardwareTxLastByte(UsartPort port){
    usartHardwareInfo.lastByte = 1;
}

int isTransmitLastByte(){
    return usartHardwareInfo.lastByte ;
}
void resetIsTransmitLastByte(){
     usartHardwareInfo.lastByte = 0 ;
}

void endOfUsartTxHandler(UsartPort port){
    usartHardwareInfo.txTurn = 0;
}
