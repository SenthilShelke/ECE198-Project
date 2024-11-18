#ifndef __DHT11_H
#define __DHT11_H

#include "main.h"

// Function prototypes
void DHT11_Init(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
uint8_t DHT11_Read(float *temperature, float *humidity);

#endif /* __DHT11_H */
