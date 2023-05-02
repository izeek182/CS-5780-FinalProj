#ifndef __CHAR_BUFFER_1
#define __CHAR_BUFFER_1

#ifdef __cplusplus
extern "C" {
#endif
#define __CHAR_NODE_SIZE 16

#include "main.h"

struct CharNodeStruct{
    char chars[__CHAR_NODE_SIZE];
    struct CharNodeStruct* nextNode;
};

typedef struct CharNodeStruct charNode;

typedef struct
{
    int size;
    int place;
    int endIndex;
    charNode* CurrentNode;
    charNode* EndNode;   
    /* data */
} charBuffer;

charBuffer* NewCharBuffer(void);

void AppendCharBuffer(charBuffer* buffer, char* characters,int length);

int PeakCharBuffer(charBuffer* buffer, char* characters,int maxLength);

int PopCharBuffer(charBuffer* buffer, char* characters,int maxLength);

int WriteCharBufferOverUsart(charBuffer* buffer, UART_HandleTypeDef* Usart,int length);

#ifdef __cplusplus
}
#endif

#endif /* CHAR_BUFFER_1 */
