#include "lcd.h"

// Function to initialize the LCD
void LCD_Init(void) {
    HAL_Delay(20); // Wait for LCD to power up
    LCD_Write_4Bits(0x03);
    HAL_Delay(5);
    LCD_Write_4Bits(0x03);
    HAL_Delay(1);
    LCD_Write_4Bits(0x03);
    LCD_Write_4Bits(0x02); // Set to 4-bit mode

    LCD_Send_Cmd(0x28); // 4-bit mode, 2-line, 5x8 font
    LCD_Send_Cmd(0x08); // Display off
    LCD_Send_Cmd(0x01); // Clear display
    HAL_Delay(2);
    LCD_Send_Cmd(0x06); // Cursor move direction
    LCD_Send_Cmd(0x0C); // Turn on display (no cursor)
}

// Function to send a command to the LCD
void LCD_Send_Cmd(uint8_t cmd) {
    HAL_GPIO_WritePin(LCD_PORT, RS, GPIO_PIN_RESET); // RS = 0 for command
    LCD_Write_4Bits(cmd >> 4); // Send higher nibble
    LCD_Write_4Bits(cmd & 0x0F); // Send lower nibble
}

// Function to send data (characters) to the LCD
void LCD_Send_Data(uint8_t data) {
    HAL_GPIO_WritePin(LCD_PORT, RS, GPIO_PIN_SET); // RS = 1 for data
    LCD_Write_4Bits(data >> 4); // Send higher nibble
    LCD_Write_4Bits(data & 0x0F); // Send lower nibble
}

// Function to send a string to the LCD
void LCD_Send_String(char *str) {
    while (*str) {
        LCD_Send_Data(*str++);
    }
}

// Function to set the cursor position on the LCD
void LCD_Set_Cursor(uint8_t row, uint8_t col) {
    uint8_t address;

    if (row == 0) {
        address = 0x00 + col; // First row
    } else if (row == 1) {
        address = 0x40 + col; // Second row
    } else {
        return; // Invalid row
    }

    LCD_Send_Cmd(0x80 | address); // Send DDRAM address command
}

// Function to write 4 bits of data to the LCD
void LCD_Write_4Bits(uint8_t data) {
    HAL_GPIO_WritePin(LCD_PORT, D4, (data & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_PORT, D5, (data & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_PORT, D6, (data & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_PORT, D7, (data & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    LCD_Pulse_Enable();
}

// Function to generate a pulse on the Enable pin
void LCD_Pulse_Enable(void) {
    HAL_GPIO_WritePin(LCD_PORT, E, GPIO_PIN_SET);
    HAL_Delay(1); // Short delay
    HAL_GPIO_WritePin(LCD_PORT, E, GPIO_PIN_RESET);
    HAL_Delay(1); // Short delay
}
