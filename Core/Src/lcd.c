#include "lcd.h"
#include "stm32f4xx_hal.h"
#include <string.h>

// Helper function declarations
static void LCD_SendCommand(LCD_HandleTypeDef *lcd, uint8_t cmd);
static void LCD_SendData(LCD_HandleTypeDef *lcd, uint8_t data);
static void LCD_EnablePulse(LCD_HandleTypeDef *lcd);
static void LCD_Send4Bits(LCD_HandleTypeDef *lcd, uint8_t data);

void LCD_Init(LCD_HandleTypeDef *lcd) {
    // Initial delay after power-on
    HAL_Delay(20);

    // Set to 4-bit mode
    LCD_Send4Bits(lcd, 0x03);
    HAL_Delay(5);
    LCD_Send4Bits(lcd, 0x03);
    HAL_Delay(5);
    LCD_Send4Bits(lcd, 0x03);
    HAL_Delay(5);
    LCD_Send4Bits(lcd, 0x02);

    // Function set: 4-bit, 2-line, 5x8 font
    LCD_SendCommand(lcd, 0x28);
    // Display control: Display on, no cursor, no blink
    LCD_SendCommand(lcd, 0x0C);
    // Clear display
    LCD_SendCommand(lcd, 0x01);
    HAL_Delay(2);
    // Entry mode set: Increment, no shift
    LCD_SendCommand(lcd, 0x06);
}

void LCD_Clear(LCD_HandleTypeDef *lcd) {
    LCD_SendCommand(lcd, 0x01);
    HAL_Delay(2);
}

void LCD_SetCursor(LCD_HandleTypeDef *lcd, uint8_t row, uint8_t col) {
    uint8_t address = col + (row == 0 ? 0x00 : 0x40);
    LCD_SendCommand(lcd, 0x80 | address);
}

void LCD_WriteString(LCD_HandleTypeDef *lcd, const char *str) {
    while (*str) {
        LCD_WriteChar(lcd, *str++);
    }
}

void LCD_WriteChar(LCD_HandleTypeDef *lcd, char character) {
    LCD_SendData(lcd, character);
}

// Static helper functions
static void LCD_SendCommand(LCD_HandleTypeDef *lcd, uint8_t cmd) {
    HAL_GPIO_WritePin(lcd->RS_Port, lcd->RS_Pin, GPIO_PIN_RESET);
    LCD_Send4Bits(lcd, cmd >> 4);
    LCD_Send4Bits(lcd, cmd & 0x0F);
}

static void LCD_SendData(LCD_HandleTypeDef *lcd, uint8_t data) {
    HAL_GPIO_WritePin(lcd->RS_Port, lcd->RS_Pin, GPIO_PIN_SET);
    LCD_Send4Bits(lcd, data >> 4);
    LCD_Send4Bits(lcd, data & 0x0F);
}

static void LCD_Send4Bits(LCD_HandleTypeDef *lcd, uint8_t data) {
    HAL_GPIO_WritePin(lcd->D4_Port, lcd->D4_Pin, (data >> 0) & 0x01);
    HAL_GPIO_WritePin(lcd->D5_Port, lcd->D5_Pin, (data >> 1) & 0x01);
    HAL_GPIO_WritePin(lcd->D6_Port, lcd->D6_Pin, (data >> 2) & 0x01);
    HAL_GPIO_WritePin(lcd->D7_Port, lcd->D7_Pin, (data >> 3) & 0x01);
    LCD_EnablePulse(lcd);
}

static void LCD_EnablePulse(LCD_HandleTypeDef *lcd) {
    HAL_GPIO_WritePin(lcd->EN_Port, lcd->EN_Pin, GPIO_PIN_SET);
    HAL_Delay(1); // Pulse width > 450ns
    HAL_GPIO_WritePin(lcd->EN_Port, lcd->EN_Pin, GPIO_PIN_RESET);
    HAL_Delay(1); // Wait for command to execute
}
