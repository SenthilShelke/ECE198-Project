#include "dht11.h"

// GPIO pin and port for the DHT11
static GPIO_TypeDef *DHT11_GPIO_Port;
static uint16_t DHT11_GPIO_Pin;

// Timing functions
static void DelayMicroseconds(uint32_t us);
static uint8_t DHT11_ReadData(uint8_t *data);

void DHT11_Init(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
    DHT11_GPIO_Port = GPIOx;
    DHT11_GPIO_Pin = GPIO_Pin;
}

uint8_t DHT11_Read(float *temperature, float *humidity) {
    uint8_t data[5] = {0};

    // Start signal
    HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_GPIO_Pin, GPIO_PIN_RESET);
    HAL_Delay(18); // Pull low for at least 18 ms
    HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_GPIO_Pin, GPIO_PIN_SET);
    DelayMicroseconds(30); // Pull high for 20-40 µs

    // Switch to input mode
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DHT11_GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);

    // Wait for response
    if (!DHT11_ReadData(data)) {
        return 0; // Read error
    }

    // Calculate temperature and humidity
    *humidity = data[0];
    *temperature = data[2];
    return 1; // Success
}

static uint8_t DHT11_ReadData(uint8_t *data) {
    uint32_t timeout = 0;

    // Wait for the response signal
    while (HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_GPIO_Pin) == GPIO_PIN_SET) {
        if (timeout++ > 1000) return 0;
        DelayMicroseconds(1);
    }
    timeout = 0;
    while (HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_GPIO_Pin) == GPIO_PIN_RESET) {
        if (timeout++ > 1000) return 0;
        DelayMicroseconds(1);
    }
    timeout = 0;
    while (HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_GPIO_Pin) == GPIO_PIN_SET) {
        if (timeout++ > 1000) return 0;
        DelayMicroseconds(1);
    }

    // Read 40 bits of data
    for (uint8_t i = 0; i < 40; i++) {
        timeout = 0;

        // Wait for the start of the bit
        while (HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_GPIO_Pin) == GPIO_PIN_RESET) {
            if (timeout++ > 1000) return 0;
            DelayMicroseconds(1);
        }

        // Measure the duration of the high signal
        uint32_t length = 0;
        while (HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_GPIO_Pin) == GPIO_PIN_SET) {
            if (length++ > 1000) return 0;
            DelayMicroseconds(1);
        }

        // Determine if it's a 0 or 1
        data[i / 8] <<= 1;
        if (length > 50) data[i / 8] |= 1;
    }

    return 1;
}

static void DelayMicroseconds(uint32_t us) {
    uint32_t start = __HAL_TIM_GET_COUNTER(&htim1);
    while ((__HAL_TIM_GET_COUNTER(&htim1) - start) < us);
}
