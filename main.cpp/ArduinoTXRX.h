#ifndef ARDUINOTXRX_H
#define ARDUINOTXRX_H
uint8_t * transmitAndReceivePacket(int size,uint8_t *data,int address);
void transmitPacket(int size,uint8_t *data,int address);
void reTransmitPacket();
void setTxRS485();
void setRxRS485();
#endif // ARDUINOTXRX_H
