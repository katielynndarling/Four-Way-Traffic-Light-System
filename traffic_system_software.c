HELPERS.H FUNCTION:

/*
 * helpers.h
 *
 *  Created on: Mar 15, 2018
 *      Author: Chance Tarver
 */

#ifndef HELPERS_H_
#define HELPERS_H_


// Base addresses of each port
#define PortA       0x40004000
#define PortB       0x40058000
#define PortC       0x40006000
#define PortD       0x40007000
#define PortE       0x40024000
#define PortF       0x40025000

//  General-Purpose Input/Output Run Mode Clock Gating Control. Each bit corresponds to a Port.
#define RCGCGPIO    0x400FE608
#define ClocksA     0x01
#define ClocksB     0x02
#define ClocksC     0x04
#define ClocksD     0x08
#define ClocksE     0x10
#define ClocksF     0x20

// Define the bitmask for each pin
#define pin0     0x01
#define pin1     0x02
#define pin2     0x04
#define pin3     0x08
#define pin4     0x10
#define pin5     0x20
#define pin6     0x40
#define pin7     0x80

//Other defines for code readability
#define OUTPUT  1
#define INPUT   0
#define INTERRUPT    1
#define NoINTERRUPT  0

// Offsets corresponding to registers we may need to configure
#define GPIODATA    0x3FC   // pg662 : GPIO Data
#define GPIODIR     0x400   // pg663 : GPIO Direction
#define GPIOIS      0x404   // pg664 : GPIO Interrupt Sense
#define GPIOIBE     0x408   // pg665 : GPIO Interrupt Both Edges
#define GPIOIEV     0x40C   // pg666 : GPIO Interrupt Event     -   0: falling edge or Low level is trigger.    1: rising edge or High level is trigger
#define GPIOIM      0x410    // pg667 : GPIO Interrupt Mask      -   0: the pin is masked.                       1: the interrupt is sent to the controller
#define GPIOICR     0x41C    // pg670 : GPIO Interrupt Clear     -   0: no affect                                1: the corresponding interrupt is cleared
#define GPIOAFSEL   0x420   // pg671 : GPIO Alternative Function Select - 0: pin functions as GPIO              1: Function as something else depending on port
#define GPIOPUR     0x510   // pg677 : GPIO Pull-Up Select      -   0: turn off pull up resistor                1: turn on pull up for corresponding pin
#define GPIOPDR     0x514   // pg679 : GPIO Pull-Down Select    -   0: turn off pull down resistor              1: turn on pull down for corresponding pin
#define GPIODEN     0x51C   // pg682 : GPIO Digital Enable      -   0: disable digital functions                1: enable pin's digital functions
#define GPIOLOCK    0x520   // pg684 : GPIO Lock. A write of the value 0x4C4F.434B unlocks the GPIO Commit (GPIOCR) register for write access.
#define GPIOKEY     0x4C4F434B // pg684. Special key for the GPIOLOCK register
#define GPIOCR      0x524   // pg685 : GPIO Commit              -   0: Corresponding GPIOAFSEL, GPIOPUR, GPIOPDR, or GPIODEN bits cannot be written. 1: They can

#define NVIC_EN0_R              (*((volatile unsigned long *)0xE000E100))  // pg142 IRQ 0 to 31 Set Enable Register

#define NVIC_PRI0_R             (*((volatile unsigned long *)0xE000E41C))  // pg152 IRQ 0 to 3 Priority Register
#define NVIC_PRI7_R             (*((volatile unsigned long *)0xE000E41C))  // pg152 IRQ 28 to 31 Priority Register  pg 104 has interrupt assignments. GPIO Port F is Interrupt #30. Bits 23:21

void WaitForInterrupt(void);  // low power mode

void SetupDigitalGPIO(unsigned int port, unsigned int pinMask, int direction,
                        int useInterrupts)
{
    /*Function for setting up a digital GPIO Pin

     This function will do all the register configs for a give port and pin.

     Examples
     -------
     SetupOnDigitalGPIO(PortF, pin3, INPUT, INTERRUPT) // Set up an input with interrupts on PF3.
     SetupOnDigitalGPIO(PortE, pin4, OUTPUT, NoINTERRUPT) // Set up an output with no interrupts on PE4.
     Notes
     -----
     We assume a pullup resistor.
     We also assume an edge based trigger.
     It is best the header with the #defines for each port and pin are included.

     Inputs
     ------
     (*((volatile unsigned long *) port  - Base address of the port being used.
     (*((volatile unsigned long *) pin   - Bit mask of the pin being used
     int direction - Boolean flag for direction of the pin. 1 = Output. 0 = input
     int UseInterrupts - Boolean flag to use interrupts. 1 = use them. 0 = don't
     */

    //Define the generic pointers
    unsigned int volatile *pRCGCGPIO = (unsigned int *) RCGCGPIO;
    unsigned int volatile *pGPIOLOCK = (unsigned int *) (port + GPIOLOCK);
    unsigned int volatile *pGPIOCR = (unsigned int *) (port + GPIOCR);
    unsigned int volatile *pGPIODIR = (unsigned int *) (port + GPIODIR);
    unsigned int volatile *pGPIOAFSEL = (unsigned int *) (port + GPIOAFSEL);
    unsigned int volatile *pGPIOPUR = (unsigned int *) (port + GPIOPUR);
    unsigned int volatile *pGPIODEN = (unsigned int *) (port + GPIODEN);
    unsigned int volatile *pGPIODATA = (unsigned int *) (port + GPIODATA);

    // Define the pointers for interrupt configuration
    unsigned int volatile *pGPIOIS = (unsigned int *) (port + GPIOIS);
    unsigned int volatile *pGPIOIBE = (unsigned int *) (port + GPIOIBE);
    unsigned int volatile *pGPIOIEV = (unsigned int *) (port + GPIOIEV);
    unsigned int volatile *pGPIOIM = (unsigned int *) (port + GPIOIM);
    unsigned int volatile *pGPIOICR = (unsigned int *) (port + GPIOICR);

    // activate the clocks for the port
    int clocks;
    switch ((int) port)
    {
    case PortA:
        clocks = ClocksA;
        break;
    case PortB:
        clocks = ClocksB;
        break;
    case PortC:
        clocks = ClocksC;
        break;
    case PortD:
        clocks = ClocksD;
        break;
    case PortE:
        clocks = ClocksE;
        break;
    case PortF:
        clocks = ClocksF;
        break;
    default:
        clocks = ClocksF;
        break;//ERROR. TODO: Add an exception to handle this. Send to Error ISR or something
    }

    *pRCGCGPIO |= clocks;
    while ((*pRCGCGPIO & clocks) == 0)
        ;

    *pGPIOLOCK = GPIOKEY;
    *pGPIOCR |= pinMask;
    if (direction == 0)
    {
        *pGPIODIR &= ~pinMask;
        *pGPIOPUR |= pinMask;
    }
    else if (direction == 1)
    {
        *pGPIODIR |= pinMask;
    }
    else
    {
        *pGPIODIR |= pinMask; // TODO. Addd an exception to handle this.
    }

    *pGPIOAFSEL &= ~pinMask;
    *pGPIODEN |= pinMask;

    if (useInterrupts)
    {
        *pGPIOIS &= ~pinMask;  // Edge-sensitive (default setting)
        *pGPIOIBE &= ~pinMask;  // Not both edges (default setting)
        *pGPIOIEV &= ~pinMask;  // Falling edge event (default setting)
        *pGPIOICR |= pinMask;   // clear flag
        *pGPIOIM |= pinMask;   // enable interrupt. Unmask it
    }
}


void WaitForInterrupt(void)
{
    __asm ("    WFI\n"
            "    BX     LR\n");
}


#endif /* HELPERS_H_ */


#include "helpers.h"

int main(void)
{
    //Switch
    SetupDigitalGPIO(PortD, pin7, INPUT, NoINTERRUPT);
    //North pins
    //Red
    SetupDigitalGPIO(PortC, pin7, OUTPUT, NoINTERRUPT);
    //Yellow
    SetupDigitalGPIO(PortC, pin6, OUTPUT, NoINTERRUPT);
    //Green
    SetupDigitalGPIO(PortC, pin5, OUTPUT, NoINTERRUPT);

    //South pins
    //Red
    SetupDigitalGPIO(PortA, pin5, OUTPUT, NoINTERRUPT);
    //Yellow
    SetupDigitalGPIO(PortA, pin6, OUTPUT, NoINTERRUPT);
    //Green
    SetupDigitalGPIO(PortA, pin7, OUTPUT, NoINTERRUPT);

    //West pins
    //Red
    SetupDigitalGPIO(PortA, pin4, OUTPUT, NoINTERRUPT);
    //Yellow
    SetupDigitalGPIO(PortA, pin3, OUTPUT, NoINTERRUPT);
    //Green
    SetupDigitalGPIO(PortA, pin2, OUTPUT, NoINTERRUPT);

    //East pins
    //Red
    SetupDigitalGPIO(PortD, pin1, OUTPUT, NoINTERRUPT);
    //Yellow
    SetupDigitalGPIO(PortD, pin2, OUTPUT, NoINTERRUPT);
    //Green
    SetupDigitalGPIO(PortD, pin3, OUTPUT, NoINTERRUPT);


    unsigned int volatile *pGPIODATA_A = (unsigned int *) (PortA + GPIODATA);
    unsigned int volatile *pGPIODATA_C = (unsigned int *) (PortC + GPIODATA);
    unsigned int volatile *pGPIODATA_D = (unsigned int *) (PortD + GPIODATA);
    int switch_position = *pGPIODATA_D & 0x80;
    int count = 0;
    while (1)
    {
        switch_position = *pGPIODATA_D & 0x80;
        if (switch_position != 128)
        {
            //If switch position is turned on turn all red lights on and all other lights off
            *pGPIODATA_C = *pGPIODATA_C | 0x80;
            *pGPIODATA_C = *pGPIODATA_C & ~0x40;
            *pGPIODATA_C = *pGPIODATA_C & ~0x20;

            *pGPIODATA_A = *pGPIODATA_A | 0x20;
            *pGPIODATA_A = *pGPIODATA_A & ~0x40;
            *pGPIODATA_A = *pGPIODATA_A & ~0x80;

            *pGPIODATA_A = *pGPIODATA_A | 0x10;
            *pGPIODATA_A = *pGPIODATA_A & ~0x08;
            *pGPIODATA_A = *pGPIODATA_A & ~0x04;

            *pGPIODATA_D = *pGPIODATA_D | 0x02;
            *pGPIODATA_D = *pGPIODATA_D & ~0x04;
            *pGPIODATA_D = *pGPIODATA_D & ~0x08;
            break;
        }
    else
    {
        count = count + 1;
//Based on count turn different colored lights on and off
        if (count < 300000)
        {
            *pGPIODATA_C = *pGPIODATA_C | 0x80;
            *pGPIODATA_A = *pGPIODATA_A | 0x20;
            *pGPIODATA_A = *pGPIODATA_A | 0x10;
            *pGPIODATA_D = *pGPIODATA_D | 0x02;
        }
        else if (300000 < count && count < 900000)
        {
            *pGPIODATA_C = *pGPIODATA_C & ~0x80;
            *pGPIODATA_A = *pGPIODATA_A & ~0x20;
            *pGPIODATA_C = *pGPIODATA_C | 0x20;
            *pGPIODATA_A = *pGPIODATA_A | 0x80;

        }
        else if (count > 900000 && count < 1200000)
        {
            *pGPIODATA_C = *pGPIODATA_C & ~0x20;
            *pGPIODATA_A = *pGPIODATA_A & ~0x80;
            *pGPIODATA_C = *pGPIODATA_C | 0x40;
            *pGPIODATA_A = *pGPIODATA_A | 0x40;

        }
        else if (count > 1200000 && count < 1500000)
        {
            *pGPIODATA_C = *pGPIODATA_C & ~0x40;
            *pGPIODATA_A = *pGPIODATA_A & ~0x40;
            *pGPIODATA_C = *pGPIODATA_C | 0x80;
            *pGPIODATA_A = *pGPIODATA_A | 0x20;

        }
        else if (count > 1500000 && count < 2100000)
        {
            *pGPIODATA_A = *pGPIODATA_A & ~0x10;
            *pGPIODATA_D = *pGPIODATA_D & ~0x02;
            *pGPIODATA_A = *pGPIODATA_A | 0x04;
            *pGPIODATA_D = *pGPIODATA_D | 0x08;

        }
        else if (count > 2100000 && count < 2400000)
        {
            *pGPIODATA_A = *pGPIODATA_A & ~0x04;
            *pGPIODATA_D = *pGPIODATA_D & ~0x08;
            *pGPIODATA_A = *pGPIODATA_A | 0x08;
            *pGPIODATA_D = *pGPIODATA_D | 0x04;

        }
        else if (count > 2400000)
        {
            count = 0;
            *pGPIODATA_A = *pGPIODATA_A & ~0x08;
            *pGPIODATA_D = *pGPIODATA_D & ~0x04;

        }
    }
    }

}