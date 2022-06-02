/**
 * Proyecto - Parqueo
 *
 * José Santizo (20185)
 *
 * Electrónica digital 2
 *
 */

//--------------------------------- Incluir librerías ----------------------------------
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/systick.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"


//------------------------------ Definición de constantes ------------------------------
#define XTAL 16000000

//-------------------------------- Varibles de programa --------------------------------
int i = 0;
int Num_parqueos = 0b0000;
int P1 = 0;
int P2 = 0;
int P3 = 0;
int P4 = 0;

//------------------------------- Prototipo de funciones -------------------------------
void delay(uint32_t msec);
void setup(void);
void UART3IntHandler(void);
void setup_UART3(void);


//--------------------------------------- Main -----------------------------------------
int main(void)
 {
    // ---------------------------------------------------------------------------------
    // Setup
    // ---------------------------------------------------------------------------------
    setup();

    // ---------------------------------------------------------------------------------
    // Loop principal
    // ---------------------------------------------------------------------------------

	while(1){

	    // -----------------------------------------------------------------------------
        // Manejo de número de parqueos según detección de sensor
        // -----------------------------------------------------------------------------

	    // Parqueo 1:
	    if(GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_2) > 0){
	        Num_parqueos = (Num_parqueos | 0b0001);

	    }
	    else if(GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_2) == 0 && (Num_parqueos & 0b0001) == 0b0001){
	        Num_parqueos = (Num_parqueos ^ 0b0001);
	    }

	    // Parqueo 2:
	    if(GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_3) > 0){
	        Num_parqueos = (Num_parqueos | 0b0010);

        }
        else if(GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_3) == 0 && (Num_parqueos & 0b0010) == 0b0010){
            Num_parqueos = (Num_parqueos ^ 0b0010);

        }

	    // Parqueo 3:
        if(GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_2) > 0){
            Num_parqueos = (Num_parqueos | 0b0100);
        }
        else if(GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_2) == 0 && (Num_parqueos & 0b0100) == 0b0100){
            Num_parqueos = (Num_parqueos ^ 0b0100);                                                  // Se realiza un XOR con 0b1000 para que por medio de ese bit, se indique que ese es el parqueo prendido

        }

        // Parqueo 4:
        if(GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_2) > 0){
            Num_parqueos = (Num_parqueos | 0b1000);

        }
        else if(GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_2) == 0 && (Num_parqueos & 0b1000) == 0b1000){
            Num_parqueos = (Num_parqueos ^ 0b1000);                                                  // Se realiza un XOR con 0b1000 para que por medio de ese bit, se indique que ese es el parqueo prendido

        }


        // -----------------------------------------------------------------------------
        // Manejo de las luces que se prenden en el parqueo
        // -----------------------------------------------------------------------------
        // Parqueo 1
        if(GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_2) > 0){
            GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0b001);
        }
        else{
            GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0b010);
        }

        // Parqueo 2
        if(GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_3) > 0){
            GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1 | GPIO_PIN_2, 0b010);
        }
        else{
            GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1 | GPIO_PIN_2, 0b100);
        }

        // Parqueo 3
        if(GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_2) > 0){
            GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0b001);
        }
        else{
            GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0b010);
        }

        // Parqueo 4
        if(GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_2) > 0){
            GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3 | GPIO_PIN_4, 0b10000);
        }
        else{
            GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3 | GPIO_PIN_4, 0b1000);
        }
	}
}

//-------------------------------------- Subrutinas ------------------------------------
//**************************************************************************************************************
// Función de delay en milisegundos
//**************************************************************************************************************
void delay(uint32_t msec)
{
    for (i = 0; i < msec; i++)
    {
        SysCtlDelay(10000);                                                                         // 1 ms de delay
    }

}

//**************************************************************************************************************
// Función de setup para todo el sistema
//**************************************************************************************************************
void setup(void)
{
    // Asignación del reloj del sistema
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);               // 40 MHz

    // Habilitación del reloj para puertos
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);                                                    // Puerto D
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);                                                    // Puerto E
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);                                                    // Puerto B
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);                                                    // Puerto A


    IntMasterEnable();                                                                              // Se habilitan las interrupciones Globales

    // Configuración de UART 3
    setup_UART3();

    // Pines como entradas y salidas
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1);                                // Pin 0 y 1 del puerto D como salida
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_1 | GPIO_PIN_2);                                // Pin 1 y 2 del puerto E como salida
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);                                // Pin 0 y 1 del puerto B como salida
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_3 | GPIO_PIN_4);                                // Pin 3 y 4 del puerto A como salida

    GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_2);                                              // Pin 2 del puerto D como entrada
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_3);                                              // Pin 3 del puerto E como entrada
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_2);                                              // Pin 2 del puerto B como entrada
    GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_2);                                              // Pin 2 del puerto A como entrada

    // Configuración de los pines como input
    GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);        // Pin 2 del puerto D como entrada con weak pull up
    GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_3, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);        // Pin 3 del puerto E como entrada con weak pull up
    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);        // Pin 2 del puerto B como entrada con weak pull up
    GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);        // Pin 2 del puerto A como entrada con weak pull up


}

//**************************************************************************************************************
// Función para setear UART
//**************************************************************************************************************
void setup_UART3(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);                                                    // Se habilita el puerto A, donde se encuentra RX y TX de UART3
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);                                                    // Habilitar el reloj para el UART3

    GPIOPinConfigure(GPIO_PC6_U3RX);                                                                // Habilitar pines 6 y 7 del puerto C para poder utilizarlos como RX y TX
    GPIOPinConfigure(GPIO_PC7_U3TX);
    GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);                                      // Pines de UART

    /* Setear configuración de UART. */
    UARTConfigSetExpClk(
            UART3_BASE, SysCtlClockGet(), 115200,
            UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);                      // Configurar UART 3 con baud rate de 115200, datos de 8 bits, 1 bit de stop y sin paridad

    IntEnable(INT_UART3);                                                                           // Habilitar la interrupción de UART3
    UARTIntEnable(UART3_BASE, UART_INT_RX | UART_INT_RT);
    UARTIntRegister(UART3_BASE,UART3IntHandler);                                                    // Asigna interrupción al handler de UART3
    UARTEnable(UART3_BASE);
}


//**************************************************************************************************************
// Handler de interrupción de UART 0
//**************************************************************************************************************
void UART3IntHandler(void){

    UARTIntClear(UART3_BASE, UART_INT_RX| UART_INT_RT);                                             // Reiniciar la bandera de interrupción de comunicación UART

    UARTCharPutNonBlocking(UART3_BASE, Num_parqueos);                                               // Enviar los datos de parqueos hacia ESP32



}

