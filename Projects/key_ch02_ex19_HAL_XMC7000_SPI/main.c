/******************************************************************************
* File Name:   main.c
*
* Description: This example project demonstrates the basic operation of SPI
* resource as Master using HAL. The SPI master sends command packets
* to the SPI slave to control an user LED.
*
* Related Document: See README.md
*
*
*******************************************************************************
* Copyright 2019-2022, Cypress Semiconductor Corporation (an Infineon company) or
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

#include "cyhal.h"
#include "cybsp.h"

/*******************************************************************************
* Macros
*******************************************************************************/
/* This code example requires two SPI ports, one as master and the other one 
 * as slave.
 */


#define SPI_FREQ_HZ                (1000000UL)
#define CMD_TO_CMD_DELAY_MS        (1000UL)

/*******************************************************************************
* Function Name: handle_error
********************************************************************************
* Summary:
* User defined error handling function
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void handle_error(void)
{
     /* Disable all interrupts. */
    __disable_irq();

    CY_ASSERT(0);
}

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* This is the main function.
*   1. SPI Master sends command  to the slave
*   2. Slave reads the command and executes the instructions
*
* Parameters:
*  void
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init();
    if (result != CY_RSLT_SUCCESS)
    {
        handle_error();
    }
/*Initialize the GPIO */
    result = cyhal_gpio_init( CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT,
                              CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
    if (result != CY_RSLT_SUCCESS)
    {
        handle_error();
    }


    /*Configure Master*/
    uint32_t cmd_send = CYBSP_LED_STATE_OFF;
    cyhal_spi_t mSPI;
    result = cyhal_spi_init( &mSPI, P14_1, P14_0, P14_2,
                             P14_3, NULL, 8, CYHAL_SPI_MODE_00_MSB, false);
    if (result != CY_RSLT_SUCCESS)
    {
        handle_error();
    }

    result = cyhal_spi_set_frequency( &mSPI, SPI_FREQ_HZ);
    if (result != CY_RSLT_SUCCESS)
    {
        handle_error();
    }


    /*Configure Slave*/

    cyhal_spi_t sSPI;
    uint32_t cmd_recv = CYBSP_LED_STATE_OFF;

    result = cyhal_spi_init( &sSPI, P18_1, P18_0, P18_2,
    		P18_3, NULL, 8, CYHAL_SPI_MODE_00_MSB, true);
    if (result != CY_RSLT_SUCCESS)
    {
        handle_error();
    }
    /*Set required frequency*/
    result = cyhal_spi_set_frequency( &sSPI, SPI_FREQ_HZ);
    if (result != CY_RSLT_SUCCESS)
    {
        handle_error();
    }

    /* Enable interrupts */
    __enable_irq();

    for (;;)
    {

        /* Toggle the current LED state */
        cmd_send = (cmd_send == CYBSP_LED_STATE_OFF) ? CYBSP_LED_STATE_ON : CYBSP_LED_STATE_OFF;

        /* Master sends the command byte to the slave. */
        result = cyhal_spi_send(&mSPI, cmd_send);

        if (CY_RSLT_SUCCESS != result)
        {
            handle_error();
        }

        /* Give delay between commands. */
        cyhal_system_delay_ms(CMD_TO_CMD_DELAY_MS);


        /* Read the command byte by slave from master*/
        if (CY_RSLT_SUCCESS == cyhal_spi_recv(&sSPI, &cmd_recv))
        {
            /* Execute command */
            cyhal_gpio_write( CYBSP_USER_LED, cmd_recv);
        }
        else
        {
            handle_error();
        }
    }
}

/* [] END OF FILE */
