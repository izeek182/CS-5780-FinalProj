#include "charBuffer.h"
#include "stm32f0xx_hal.h"
#include "usart.h"
#include <stdlib.h>

charBuffer *NewCharBuffer()
{
    charBuffer* buf = malloc(sizeof(charBuffer));
    buf->place =0;
    buf->endIndex =0;
    buf->CurrentNode = malloc(sizeof(charNode));
    for(int i =0;i<__CHAR_NODE_SIZE;i++){
    buf->CurrentNode->chars[i] = '\0';
    }
    buf->CurrentNode->nextNode =0;
    buf->EndNode=buf->CurrentNode;
    buf->size = 0;
    return buf;
}

void AppendCharBuffer(charBuffer *b, char *characters, int length)
{
    __disable_irq();
    for(int i =0;i<length;i++){
        if(b->endIndex >= __CHAR_NODE_SIZE){
            charNode* next = malloc(sizeof(charNode));
            for(int i =0;i<__CHAR_NODE_SIZE;i++){
                next->chars[i] = '\0';
            }
            next->nextNode =0;
            b->EndNode->nextNode = next;
            b->EndNode = next;
            b->endIndex = 0;
        }
        b->EndNode->chars[b->endIndex] = characters[i];
        b->endIndex++;
    }
    b->size += length;
    __enable_irq();
}

int PeakCharBuffer(charBuffer *b, char *characters, int maxLength)
{
    __disable_irq();
    charNode* cur = b->CurrentNode;
    int place = b->place;
    for(int i=0;i<maxLength;i++){
        if(place >= __CHAR_NODE_SIZE){
            place =0;
            cur = cur->nextNode;
        }
        if(cur == b->EndNode){
            if(place >= b->endIndex){
                return i;
                b->size -= i;
            }
        }
        if(cur == 0){
            return i;
            b->size -= i;
        }
        characters[i] = cur->chars[place];            
        place++;
    }
    __enable_irq();
    return maxLength;
}

int PopCharBuffer(charBuffer *b, char *characters, int maxLength)
{
    __disable_irq();
    for(int i=0;i<maxLength;i++){
        if(b->place >= __CHAR_NODE_SIZE){
            b->place =0;
            charNode* last = b->CurrentNode;
            b->CurrentNode = last->nextNode;
            free(last);
        }
        if(b->CurrentNode == b->EndNode){
            if(b->place >= b->endIndex){
                b->size -= i;
                return i;
            }
        }
        if(b->CurrentNode == 0){
            b->size -= i;
            return i;
        }
        characters[i] = b->CurrentNode->chars[b->place];            
        b->place++;
    }
    b->size -= maxLength;
    __enable_irq();
    return maxLength;
}

int WriteCharBufferOverUsart(charBuffer* queue, UART_HandleTypeDef *Usart,int length){
  char buffer[length];
    int size = PopCharBuffer(queue, (char *)&buffer, length);
    if(size < length){
        length = size;
    }
    HAL_UART_Transmit(Usart, (uint8_t *)(&buffer), length, 100);
    return 1;
}
