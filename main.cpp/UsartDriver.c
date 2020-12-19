#include "UsartDriver.h"
#include "UsartHardware.h"
#include "EventQueue.h"
#include "Event.h"
#include "List.h"
#include "Irq.h"
#include "TimerEventQueue.h"
#include "CommEventQueue.h"
#include "Crc.h"

#include <stdio.h>
#include <string.h>

uint8_t txDataForFlags[2];
volatile UsartDriverInfo usartDriverInfo;

#define hasRequestedTxPacket(info) ((info).requestTxPacket)
#define hasRequestedRxPacket(info) ((info).requestRxPacket)
#define isLastTxByte(info) ((info.txLen) < (info.txCounter)+2)
#define isLastRxByte(info) ((info.rxLen) <= (info.rxCounter)-PAYLOAD_OFFSET)
#define getCommandByte(info) (info.rxMallocBuffer[CMD_OFFSET])
#define getSenderAddress(info) (info.rxMallocBuffer[SENDER_ADDRESS_OFFSET])

STATIC int findPacketLength(uint8_t* data){
    int size = *(&data + 1) - data;
    return (sizeof(data)/sizeof(data[0]));
}

STATIC int getPacketLength(uint8_t * txData){
    int packetLength = txData[LENGTH_OFFSET];
    return packetLength;
}

STATIC int isCorrectAddress(UsartDriverInfo info){
    char * packet = info.rxStaticBuffer;
    char usartAddress = *(packet + RECEIVER_ADDRESS_OFFSET);

    if((int)usartAddress == USART_ADDRESS)
        return 1;
    else
        return 0;
}

STATIC void usartDriverInit(void){
    usartDriverInfo.txState = TX_IDLE;
    usartDriverInfo.txBuffer = NULL;
    usartDriverInfo.requestTxPacket = 0;
    usartDriverInfo.txCounter = 0;
    usartDriverInfo.txLen = 0;
    usartDriverInfo.txFlag = 0;

    usartDriverInfo.rxState = RX_IDLE;
    usartDriverInfo.receivedBuffer = NULL;
    usartDriverInfo.rxCounter = 0;
    usartDriverInfo.rxLen = 0;
}

void usartInit(void){
    usartDriverInit();
    usartHardwareInit();
}

void usartDriverTransmit(UsartPort port,uint8_t rxAddress,int length,uint8_t * txData,UsartEvent * event){
		disableIRQ();

    if(!hasRequestedTxPacket(usartDriverInfo)){
        usartDriverInfo.txLen =length+1;
        usartDriverInfo.receiverAddress = rxAddress;
        usartDriverInfo.txBuffer = txData;
        generateCRC16forTxPacket(port);
        usartDriverInfo.requestTxPacket = 1;
        hardwareUsartTransmit(port);
    }
    enableIRQ();
}

uint8_t usartTransmissionHandler(UsartPort port){
    uint8_t * txBuffer = usartDriverInfo.txBuffer;
    uint8_t * txCRC16 = usartDriverInfo.txCRC16;
    uint8_t transmitByte;

    switch(usartDriverInfo.txState){
        case TX_IDLE :
            transmitByte = usartDriverInfo.receiverAddress;
            usartDriverInfo.txState = TX_SEND_RECEIVER_ADDRESS;
            break;
        case TX_SEND_RECEIVER_ADDRESS:
            transmitByte = USART_ADDRESS;
            usartDriverInfo.txState = TX_SEND_TRANSMITTER_ADDRESS;
            break;
        case TX_SEND_TRANSMITTER_ADDRESS:
            transmitByte = usartDriverInfo.txLen;
            usartDriverInfo.txState = TX_SEND_LENGTH;
            break;
        case TX_SEND_LENGTH:
            transmitByte = usartDriverInfo.txFlag;
            usartDriverInfo.txState = TX_SEND_FLAG;
            break;
        case TX_SEND_FLAG:
            transmitByte = txBuffer[usartDriverInfo.txCounter];
            usartDriverInfo.txCounter ++;
            usartDriverInfo.txState = TX_SEND_BYTE;
            if(isLastTxByte(usartDriverInfo)){
                usartDriverInfo.txCounter = 0;
                usartDriverInfo.txState = TX_SEND_CRC16;
            }
            break;
        case TX_SEND_BYTE:
            transmitByte = txBuffer[usartDriverInfo.txCounter];
            usartDriverInfo.txCounter ++;
            if(isLastTxByte(usartDriverInfo)){
                usartDriverInfo.txCounter = 0;
                usartDriverInfo.txState = TX_SEND_CRC16;
            }
            break;
        case TX_SEND_CRC16:
            transmitByte = txCRC16[usartDriverInfo.txCounter];
            usartDriverInfo.txCounter ++;
            if(usartDriverInfo.txCounter > 1){
								setHardwareTxLastByte(port);
								usartDriverInfo.txCounter = 0;
                usartDriverInfo.requestTxPacket = 0;
                usartDriverInfo.txState = TX_IDLE;
								setHardwareTxLastByte(port);
            }
            break;
    }
    return transmitByte;
}

void usartReceiveHandler(UsartPort port,uint16_t rxByte){
    uint8_t eventByte = rxByte >> 8;
    uint8_t dataByte = rxByte & 0xFF;
    uint8_t * staticBuffer = usartDriverInfo.rxStaticBuffer;
    uint8_t * rxCRC16 = usartDriverInfo.rxCRC16;

    switch(usartDriverInfo.rxState){
        case RX_IDLE :
            if(eventByte == RX_PACKET_START){
							usartDriverInfo.rxState = RX_ADDRESS_LENGTH;
            }
            break;
        case RX_ADDRESS_LENGTH :
            handleRxAddressAndLength(port,rxByte);
            break;
        case RX_RECEIVE_PAYLOAD_STATIC_BUFFER :
            handleRxStaticBufferPayload(port,rxByte);
            break;
        case RX_WAIT_CRC16_STATIC_BUFFER :
            handleCRC16WithStaticBuffer(port,rxByte);
            break;
    }
}

STATIC void handleRxAddressAndLength(UsartPort port,uint16_t rxByte){
    uint8_t eventByte = rxByte >> 8;
    uint8_t dataByte = rxByte & 0xFF;
    uint8_t * staticBuffer = usartDriverInfo.rxStaticBuffer;

    if(eventByte == RX_PACKET_START){
        resetUsartDriverReceive(port);
        return;
    }
    else if(usartDriverInfo.rxCounter < 3){
        staticBuffer[usartDriverInfo.rxCounter] = dataByte;
        usartDriverInfo.rxCounter++;
    }

    if (usartDriverInfo.rxCounter >= 3 ){
        if(isCorrectAddress(usartDriverInfo)){
            usartDriverInfo.rxLen = staticBuffer[LENGTH_OFFSET];
            usartDriverInfo.rxState = RX_RECEIVE_PAYLOAD_STATIC_BUFFER;
        }
        else{
            usartDriverInfo.rxCounter = 0;
            usartDriverInfo.rxState = RX_IDLE;
        }
    }
}

STATIC void handleRxStaticBufferPayload(UsartPort port,uint16_t rxByte){
    uint8_t eventByte = rxByte >> 8;
    uint8_t dataByte = rxByte & 0xFF;
    uint8_t * staticBuffer = usartDriverInfo.rxStaticBuffer;
    uint8_t * rxCRC16 = usartDriverInfo.rxCRC16;

    if(eventByte == RX_PACKET_START){
        resetUsartDriverReceive(port);
        return;
    }
    else if (isLastRxByte(usartDriverInfo)){
        rxCRC16[0] = dataByte;
        usartDriverInfo.rxState = RX_WAIT_CRC16_STATIC_BUFFER;
    }
    else{
        staticBuffer[usartDriverInfo.rxCounter] = dataByte;
        usartDriverInfo.rxCounter++;
    }
}

STATIC void handleCRC16WithStaticBuffer(UsartPort port,uint16_t rxByte){
    uint8_t eventByte = rxByte >> 8;
    uint8_t dataByte = rxByte & 0xFF;
    UsartDriverInfo * info =&usartDriverInfo;
    uint8_t * staticBuffer = usartDriverInfo.rxStaticBuffer;
    uint8_t * rxCRC16 = usartDriverInfo.rxCRC16;

    if(eventByte == RX_PACKET_START){
        resetUsartDriverReceive(port);
        return;
    }
    else{
        rxCRC16[1] = dataByte;
        usartDriverInfo.rxState = RX_IDLE;
				generateEventForReceiveComplete(port);
    }
}
uint16_t generatedCrc16;
uint16_t crcRxValue;
STATIC int checkRxPacketCRC(UsartPort port){
    int rxLength = usartDriverInfo.rxLen;
    uint8_t * rxCRC16ptr = usartDriverInfo.rxCRC16;
    uint8_t * rxBuffer = usartDriverInfo.rxStaticBuffer;
    crcRxValue = (rxCRC16ptr[0]<<8) + rxCRC16ptr[1];
    

    generatedCrc16=generateCrc16(&rxBuffer[PAYLOAD_OFFSET+1], rxLength-1);

    if(crcRxValue == generatedCrc16){
        return 1;
    }
    return 0;
}
STATIC void generateEventForReceiveComplete(UsartPort port){
    if(checkRxPacketCRC(port)){
        findSMInfoAndGenerateEvent(port);
    }
    else{
        usartDriverInfo.rxState = RX_IDLE;
        usartDriverInfo.rxCounter = 0;
        usartDriverInfo.rxLen = 0;
        //findSMInfoAndGenerateEvent(port);
    }
}


STATIC void findSMInfoAndGenerateEvent(UsartPort port){
    uint8_t * rxBuffer = usartDriverInfo.rxStaticBuffer;
		GenericStateMachine *infoSM;
		char command = rxBuffer[4];
    usartDriverInfo.receivedBuffer = usartDriverInfo.rxStaticBuffer;
    usartDriverInfo.rxState = RX_IDLE;
    usartDriverInfo.rxCounter = 0;
    usartDriverInfo.rxLen = 0;
}

STATIC void resetUsartDriverReceive(UsartPort port){
		usartDriverInfo.rxState = RX_ADDRESS_LENGTH;
    usartDriverInfo.rxCounter = 0;
    usartDriverInfo.rxLen = 0;
}

STATIC void generateCRC16forTxPacket(UsartPort port){
    uint8_t * txBuffer = usartDriverInfo.txBuffer;
    uint8_t * txCRC16 = usartDriverInfo.txCRC16;
    int length = usartDriverInfo.txLen;
    uint16_t crc16 ;
    crc16 = generateCrc16(txBuffer, length-1);
	*(uint16_t*)&txCRC16[0] = crc16;
}

uint8_t * getPacketFromSlaves(){
  return usartDriverInfo.receivedBuffer;
}

int getCRCValue(){
    return crcRxValue;
}

void resetUsartRxBuffer(){
  usartDriverInfo.receivedBuffer = NULL;
}
