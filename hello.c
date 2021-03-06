//*****************************************************************************
//
// Traffic Light Controller Lab
// Ajit Punj and Baban Malhi
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


#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif


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

int
main(void)
{
	int sw_input;
	int leds;
	
    // Set system clock to 50 MHz
	
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);		

	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);	// Enable the GPIO port F

		
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
				for(i=0;i<75;i++){	//TODO: fix timing in loop
						
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
				//car switch activated, 5 second intervals
				if(swPressed==1){
					//turn on green and turn off red
					GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
					GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x8);
					UARTprintf("Main: red     Side: green    Ped: don't walk \n");
					int greenPressed=0;
					//each iteration before was about .2 seconds, 25 should be 5 sec
					for(i=0;i<25;i++){
						sw_input = ROM_GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);
						if(greenPressed==0 && sw_input==1){
								greenPressed=1;
								UARTprintf("SW 1 detected\n");
						}
						SysCtlDelay((SysCtlClockGet()/3)/10);
					}
					if(greenPressed==1){
						greenPressed=0;
						for(i=0;i<25;i++){
							sw_input = ROM_GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);
							if(greenPressed==0 && sw_input==1){
								greenPressed=1;
								UARTprintf("SW 1 detected\n");
							}
							SysCtlDelay((SysCtlClockGet()/3)/10);
						}
					}
					if(greenPressed==1){
						greenPressed=0;
						for(i=0;i<25;i++){
							sw_input = ROM_GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);
							if(greenPressed==0 && sw_input==1){
								greenPressed=1;
								UARTprintf("SW 1 detected\n");
							}
							SysCtlDelay((SysCtlClockGet()/3)/10);
						}
					}
					//turn off green
					GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
				}
				//if the pedestrian buttton (SW2) was pressed, leave on for 15 sec
				else if(swPressed==2){
					UARTprintf("Main: red     Side: green    Ped: walk \n");
					//turn on green, turn off red
					GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
					GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x8);
					ROM_SysCtlDelay((SysCtlClockGet() / 3 )*10);
					UARTprintf("Main: red     Side: green    Ped: don't walk \n");
					ROM_SysCtlDelay((SysCtlClockGet() / 3 )*5);
					//turn off green
					GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
				}
				//yellow for 3 seconds, then off
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x8);
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x2);
				UARTprintf("Main: red     Side: yellow    Ped: don't walk \n");
				ROM_SysCtlDelay(SysCtlClockGet());
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
				//short period where both lights are red again
				UARTprintf("Main: red     Side: red    Ped: don't walk \n");
				ROM_SysCtlDelay(SysCtlClockGet() / 3 / 2);
				
			
    }
}
