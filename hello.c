//*****************************************************************************
//
// hello.c - Simple hello world example.
//
// Copyright (c) 2012-2013 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 1.1 of the EK-TM4C123GXL Firmware Package.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "inc/hw_gpio.h"

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Hello World (hello)</h1>
//!
//! A very simple ``hello world'' example.  It simply displays ``Hello World!''
//! on the UART and is a starting point for more complicated applications.
//!
//! UART0, connected to the Virtual Serial Port and running at
//! 115,200, 8-N-1, is used to display messages from this application.
//
//*****************************************************************************


//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************
void
ConfigureUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable UART0
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure GPIO Pins for UART mode.
    //
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, 16000000);
}

//*****************************************************************************
//
// Print "Hello World!" to the UART on the evaluation board.
//
//*****************************************************************************
int
main(void)
{
	int sw_input;
	int leds;
	
		// Set system clock to 50 MHz
	
	  ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);		

		ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);	// Enable the GPIO port F

		//
    // Unlock PF0 
	
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;

		// Configure PF0 and PF4 as inputs to read switch values
		// Configure as inputs before configuring pad with pull-up resistors
	
		ROM_GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);	// PF0, PF4 = switches

		// Need pull-up resistors on switch inputs
		// Call this after pins configured as inputs
	
		ROM_GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);

		// Configure PF1 (Red LED), PF2 (Blue LED) and PF3 (Green LED) as outputs

		ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

		ConfigureUART();
		
		// Turn on Blue LED only
			
		//ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_PIN_2);
	
    while(1)
    {

			// Poll PF0 and PF4. Wait until either pressed. Switches are active low.
			
			/*do {
				sw_input = ROM_GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);
			} while ((sw_input & 0x11) == 0x11);
			*/
				int i=0;
				int pressedFlag=0;
				int swPressed=0;
			UARTprintf("Main: Green     Side: Red    Ped: don't walk \n");
				for(i=0;i<15;i++){	//fix timing in loop
						
						GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x2);
					
					
						sw_input = ROM_GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);
					//UARTprintf("%d",pressedFlag);
						if(pressedFlag==0 && sw_input!=17){
								pressedFlag=1;
								if(sw_input==1){
									UARTprintf("SW 1 detected\n");
									swPressed=1;
								}
								else{
									UARTprintf("SW 2 detected\n");
									swPressed=2;
								}
						}
						SysCtlDelay((SysCtlClockGet()/3)/10);
						
				}
				//GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
				
				//if switch was not pressed wait
				while(pressedFlag==0){
						do {
							sw_input = ROM_GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);
							if(pressedFlag==0 && sw_input!=17){
								pressedFlag=1;
								if(sw_input==1){
									UARTprintf("SW 1 detected\n");
									swPressed=1;
								}
								else{
									UARTprintf("SW 2 detected\n");
									swPressed=2;
								}
							}
					} while ((sw_input & 0x11) == 0x11);
				}
				
				//once switch was pressed, continue with yellow for 3 sec
				
				//GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x4);
				//GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x8);
				UARTprintf("Main: yellow     Side: red    Ped: don't walk \n");
				ROM_SysCtlDelay(SysCtlClockGet());
				//GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
				
				//after yellow, both red for .5 sec
				UARTprintf("Main: red     Side: red    Ped: don't walk \n");
				ROM_SysCtlDelay(SysCtlClockGet() / 3 / 2);
				
				//red on main green on side depending on switch pressed
				if(swPressed==1){
					UARTprintf("Main: red     Side: green    Ped: walk \n");
					ROM_SysCtlDelay((SysCtlClockGet() / 3 )*10);
				}
				else if(swPressed==2){
					UARTprintf("Main: red     Side: green    Ped: walk \n");
					ROM_SysCtlDelay((SysCtlClockGet() / 3 )*10);
					UARTprintf("Main: red     Side: green    Ped: don't walk \n");
					ROM_SysCtlDelay((SysCtlClockGet() / 3 )*5);
				}
				
				
			
    }
}
