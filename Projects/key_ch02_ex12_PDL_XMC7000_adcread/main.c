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
#include "cybsp.h"
#include <stdio.h>


int main(void)
{

    cy_rslt_t result;

    // UART context variable
	cy_stc_scb_uart_context_t UART_context;

    /* Initialize the device and board peripherals */
    result = cybsp_init() ;
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    __enable_irq();



    /* Configure and enable the UART peripheral */
	Cy_SCB_UART_Init(UART_HW, &UART_config, &UART_context);
	Cy_SCB_UART_Enable(UART_HW);

	/* Initialize the SAR2 module */
	Cy_SAR2_Init(ADC_HW, &ADC_config);
	/* Enable the SAR2 module */
	Cy_SAR2_Enable(ADC_HW);

	uint16_t resultAN0;
	int32_t uVolts;
	char stringBuffer[20];

    for (;;){
    //* Issue software start trigger */
    Cy_SAR2_Channel_SoftwareTrigger(ADC_HW, 0);
    /* Wait for conversion to finish */
    while (CY_SAR2_INT_GRP_DONE != Cy_SAR2_Channel_GetInterruptStatus(ADC_HW, 0));
    /* Get conversion results in counts, do not obtain or analyze status here */
    resultAN0 = Cy_SAR2_Channel_GetResult(ADC_HW, 0, NULL);
    /* Each channel results are stored in ADC counts at this point. */

    /* Clear interrupt source */
    Cy_SAR2_Channel_ClearInterrupt(PASS0_SAR0,0, CY_SAR2_INT_GRP_DONE);

    /*counts to voltage calculation */
    uVolts = (resultAN0 *3.3*1000000)/4095;

    sprintf(stringBuffer, "%ld\r\n", uVolts);
    Cy_SCB_UART_PutString(UART_HW,stringBuffer);
    Cy_SysLib_Delay(100);
    }

}


