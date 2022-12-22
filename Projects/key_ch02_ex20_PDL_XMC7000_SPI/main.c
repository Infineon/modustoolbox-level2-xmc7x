/***************************************************************************//**
* \file main.c
* \version 1.0
*
* \brief
* Tests the functionality of SPI.
********************************************************************************
* \copyright
* Copyright 2019-2020, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#include "cybsp.h"
#include "cy_pdl.h"
#include <stdio.h>


/***************************** Macros ****************************************/

/* LED States. LEDs in the supported kits are in active low connection. */
#define LED_ON 				(0)
#define LED_OFF				(1)


/************************ Function Prototypes ********************************/

/******************************************************************************/


/******************************************************************************
* Function Name: main
*******************************************************************************
*
* Summary: 	1. 	Sets up two SPI blocks - one as master and another as slave
*   		2. 	SPI master sends commands to the slave to turn LED ON or OFF,
*   			every one second using high level APIs.
*   		3.  When the required number of	bytes is transferred, data received
*   			by the slave is checked and LED is turned ON or OFF based on the
*   			received command.
*
* Parameters:	None
*
* Return:		Int
*
******************************************************************************/


int main(void)
{


	uint32_t Data =0;
	uint32_t cmd  =0;
	uint32_t value =0;
	cy_rslt_t result;
    result = cybsp_init() ;
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

	/* UART context variable */
		cy_stc_scb_uart_context_t UART_context;

	/* Allocate context for SPI master operation */
	cy_stc_scb_spi_context_t mSPI_context;
	/* Allocate context for SPI slave operation */
	cy_stc_scb_spi_context_t sSPI_context;


	Cy_SCB_UART_Init(UART_HW, &UART_config, &UART_context);
    Cy_SCB_UART_Enable(UART_HW);
    //printf("Initialization of UART complete \n");



        /* Initialize the SPI Master */
        Cy_SCB_SPI_Init(mSPI_HW, &mSPI_config, &mSPI_context);
        /* Initialize the SPI slave */
        Cy_SCB_SPI_Init(sSPI_HW, &sSPI_config, &sSPI_context);
    	/* Set active slave select to line 0 */
    	Cy_SCB_SPI_SetActiveSlaveSelect(mSPI_HW, CY_SCB_SPI_SLAVE_SELECT0);

    	/* Enable SPI master block. */
    	Cy_SCB_SPI_Enable(mSPI_HW);
    	/* Enable SPI slave block. */
    	Cy_SCB_SPI_Enable(sSPI_HW);


	/* Enable global interrupt */
	__enable_irq();
	for(;;)
	    {
	    	/* Toggle the current LED state */
			cmd = (cmd == LED_ON) ? LED_OFF : LED_ON;
			/* Form the command packet */
			Data = cmd;
			/* Pass the command packet to the master.*/
			Cy_SCB_SPI_Write (mSPI_HW,Data);
			/* Wait till transfer is complete.*/
			while (!Cy_SCB_SPI_IsTxComplete(mSPI_HW))
			{
			}
			/* Wait till data is received in RXFIFO .*/
		    while(Cy_SCB_SPI_GetNumInRxFifo(sSPI_HW)==0)
			{
				
			}
			/* Read Cmd from the slave .*/
			value =Cy_SCB_SPI_Read(sSPI_HW);
		    Cy_GPIO_Write(CYBSP_USER_LED_PORT, CYBSP_USER_LED_NUM,value);
            Cy_SysLib_Delay(500);

	    }
	}




