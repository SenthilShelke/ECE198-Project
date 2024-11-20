#ifndef __LCD_H
#define __LCD_H

#include "main.h"

// Define LCD control pins
#define RS GPIO_PIN_0
#define E GPIO_PIN_1
#define D4 GPIO_PIN_4
#define D5 GPIO_PIN_5
#define D6 GPIO_PIN_6
#define D7 GPIO_PIN_7
#define LCD_PORT GPIOA

// Function prototypes
void LCD_Init(void);
void LCD_Send_Cmd(uint8_t cmd);
void LCD_Send_Data(uint8_t data);
void LCD_Send_String(char *str);
void LCD_Set_Cursor(uint8_t row, uint8_t col);
void LCD_Write_4Bits(uint8_t data);
void LCD_Pulse_Enable(void);

#endif /* __LCD_H */
