/*******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the Empty Application Example
*              for ModusToolbox.
*
* Related Document: See README.md
*
*
********************************************************************************
* Copyright 2021-2022, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

volatile int uxTopUsedPriority;

//TODO create variable for mutex
static SemaphoreHandle_t LED_lock;

/* LED task 1 - blink at 1 Hz */
void led_task1()
{
    while (1)
    {
        //TODO lock the mutex
    	xSemaphoreTake(LED_lock, portMAX_DELAY);

        cyhal_gpio_toggle(CYBSP_USER_LED);

        //TODO unlock the mutex
    	xSemaphoreGive(LED_lock);

        vTaskDelay (1000); /* 1 Hz delay and give the other task a turn */
    }
}

/* LED task 2 - blink at 2 Hz when button is pressed */
void led_task2()
{
    while (1)
    {

        //TODO lock the mutex
    	xSemaphoreTake(LED_lock, portMAX_DELAY);

        while (CYBSP_BTN_PRESSED == cyhal_gpio_read(CYBSP_USER_BTN)) /* Button is pressed */
        {
        	cyhal_gpio_toggle(CYBSP_USER_LED);
        	vTaskDelay (500);
        }

        //TODO unlock the mutex
        xSemaphoreGive(LED_lock);

        vTaskDelay (1);	/* Give the other task a turn when button is not pressed */
    }
}

int main(void)
{
    uxTopUsedPriority = configMAX_PRIORITIES - 1;

    /* Initialize the device and board peripherals */
	cybsp_init() ;
    __enable_irq();

    /* Initialize the User LED */
    cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT,
                    CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);

    /* Initialize User Button */
    cyhal_gpio_init(CYBSP_USER_BTN, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_PULLUP, 1);

	//TODO Create Mutex
    LED_lock = xSemaphoreCreateMutex();

    /* Create tasks with a stack of 1024, no parameters, and a priority of 1 */
    xTaskCreate(led_task1, (char *)"led_task1", 1024, 0, 1, NULL);
    xTaskCreate(led_task2, (char *)"led_task2", 1024, 0, 1, NULL);

    /* Start the RTOS scheduler */
    vTaskStartScheduler();
    CY_ASSERT(0);
}

/* [] END OF FILE */
