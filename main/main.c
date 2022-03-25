/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : xQueue_Test_Speed
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 Espressif.
 * All rights reserved.
 *
 * Vo Duc Toan / B1907202
 * Can Tho University.
 * March - 2022
 * Built with ESP-IDF Version: 4.4.
 * Target device: ESP32-WROOM.
 *
 ******************************************************************************
 */
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include "esp_log.h"

#define SIZE_TEST sizeof(uint64_t)

QueueHandle_t xQueue1;
SemaphoreHandle_t xSemaphore1;
int64_t timeStartSend, timeEndSend;
int64_t timeStartReceive, timeEndReceive;

const char *TAG = "[xQueueTest]";

static void addQueueData(uint16_t *pointer, uint16_t data)
{
    *pointer++ = data;
}

static void taskReceiveData()
{
    uint16_t *sampleData = malloc(SIZE_TEST * 1000);
    memset(sampleData, 0x0000, SIZE_TEST * 1000);
    xSemaphoreTake(xSemaphore1, portMAX_DELAY);
    timeStartReceive = esp_timer_get_time();
    for (int i = 0; i < 1000; i++)
    {
        xQueueReceive(xQueue1, sampleData++, portMAX_DELAY);
    }
    sampleData = sampleData - 1000;
    timeEndReceive = esp_timer_get_time();
    ESP_LOGI(TAG, "Sending: %lld us", timeEndSend - timeStartSend);
    ESP_LOGI(TAG, "Receive: %lld us", timeEndReceive - timeStartReceive);
    ESP_LOGI(TAG, "Delay: %lld us", timeStartReceive - timeEndSend);
    ESP_LOGI(TAG, "%d %d %d", *sampleData, *(sampleData + 1), *(sampleData + 2));
    vTaskDelay(portMAX_DELAY);
    vTaskDelete(NULL);
}

static void taskSendData()
{
    uint16_t *sampleData = malloc(SIZE_TEST * 1000);
    uint16_t test = 0xAAAA;
    memset(sampleData, 0xFFFF, SIZE_TEST * 1000);
    timeStartSend = esp_timer_get_time();
    for (int i = 0; i < 1000; i++)
    {
        if (i == 0)
        {
            xQueueSend(xQueue1, &test, portMAX_DELAY);
        }
        else
        {
            xQueueSend(xQueue1, sampleData, portMAX_DELAY);
        }

        // addQueueData(sampleData, 0xFFFF);
    }
    timeEndSend = esp_timer_get_time();
    xSemaphoreGive(xSemaphore1);
    vTaskDelete(NULL);
}

void app_main(void)
{
    xSemaphore1 = xSemaphoreCreateBinary();
    xQueue1 = xQueueCreate(1000, SIZE_TEST);
    xTaskCreate(taskSendData, "[taskSendData]", 1024 * 3, NULL, 2, NULL);
    xTaskCreate(taskReceiveData, "[taskPrepareData]", 1024 * 3, NULL, 2, NULL);
    vTaskDelete(NULL);
}
