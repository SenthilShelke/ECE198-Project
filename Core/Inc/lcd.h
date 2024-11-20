#ifndef LCD_H
#define LCD_H

#include "stm32f4xx_hal.h"

// Define custom types for the LCD
typedef struct {
    GPIO_TypeDef *RS_Port;    // Port for RS pin
    uint16_t RS_Pin;          // Pin for RS
    GPIO_TypeDef *EN_Port;    // Port for EN pin
    uint16_t EN_Pin;          // Pin for EN
    GPIO_TypeDef *D4_Port;    // Port for D4 pin
    uint16_t D4_Pin;          // Pin for D4
    GPIO_TypeDef *D5_Port;    // Port for D5 pin
    uint16_t D5_Pin;          // Pin for D5
    GPIO_TypeDef *D6_Port;    // Port for D6 pin
    uint16_t D6_Pin;          // Pin for D6
    GPIO_TypeDef *D7_Port;    // Port for D7 pin
    uint16_t D7_Pin;          // Pin for D7
} LCD_HandleTypeDef;

// Function declarations
void LCD_Init(LCD_HandleTypeDef *lcd);
void LCD_Clear(LCD_HandleTypeDef *lcd);
void LCD_WriteString(LCD_HandleTypeDef *lcd, const char *str);
void LCD_SetCursor(LCD_HandleTypeDef *lcd, uint8_t row, uint8_t col);
void LCD_WriteChar(LCD_HandleTypeDef *lcd, char character);

#endif // LCD_H
