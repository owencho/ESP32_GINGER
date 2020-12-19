#ifndef COMMON_H
#define COMMON_H

void generateReturnMessage(int statusCode , char * message);

typedef union
{
 float number;
 uint8_t bytes[4];
} FLOATUNION_t;

#endif // COMMON_H
