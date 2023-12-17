#include "platform.h"
#include "xstatus.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xgpio_l.h"
#include "xtmrctr_l.h"
#include "xuartlite_l.h"
#include "xintc_l.h"

#ifndef SDT
#define TMRCTR_BASEADDR    XPAR_TMRCTR_0_BASEADDR
#else
#define TMRCTR_BASEADDR    XPAR_XTMRCTR_0_BASEADDR
#endif
#define TIMER_COUNTER_0   0

int duty_cycle = 0; // Inizializzazione duty_cycle

void ISR() __attribute__((interrupt_handler));
void timer(void);
void selectcolor(int Red_LED1, int Green_LED1, int Blue_LED1, int Red_LED2, int Green_LED2, int Blue_LED2);
void selectcolor(int Red_LED1, int Green_LED1, int Blue_LED1, int Red_LED2, int Green_LED2, int Blue_LED2)
{
    //

    if (duty_cycle <= 255)
    {
        // Impostazione del COLORE per il PRIMO LED
        if (duty_cycle <= Red_LED1)   //definisco la logica per simulare il pwm, accendo il led per una frazione del duty cycle, il tempo in cui rimane acceso lo definisco nella funzione selectcolor, mi serve per modificare l'intensità del led
        {
            Xil_Out32(XPAR_AXI_RGBLEDS_GPIO_BASEADDR, (Xil_In32(XPAR_AXI_RGBLEDS_GPIO_BASEADDR) & ~0b001) | (0b001)); //accendo il led
        }
        else if (duty_cycle > Red_LED1)
        {
            Xil_Out32(XPAR_AXI_RGBLEDS_GPIO_BASEADDR, (Xil_In32(XPAR_AXI_RGBLEDS_GPIO_BASEADDR) & 0b110)); //spengo il led
        }


        if (duty_cycle <= Red_LED2)
        {

            Xil_Out32(XPAR_AXI_RGBLEDS_GPIO_BASEADDR, (Xil_In32(XPAR_AXI_RGBLEDS_GPIO_BASEADDR) & ~0b001000) | (0b001000));
        }
        else if (duty_cycle > Red_LED2)
        {

            Xil_Out32(XPAR_AXI_RGBLEDS_GPIO_BASEADDR, (Xil_In32(XPAR_AXI_RGBLEDS_GPIO_BASEADDR) & 0b110111));
        }
        if (duty_cycle <= Green_LED1)
             {

                 Xil_Out32(XPAR_AXI_RGBLEDS_GPIO_BASEADDR, (Xil_In32(XPAR_AXI_RGBLEDS_GPIO_BASEADDR) & ~0b10) | (0b10));
             }
             else if (duty_cycle > Green_LED1)
             {

                 Xil_Out32(XPAR_AXI_RGBLEDS_GPIO_BASEADDR, (Xil_In32(XPAR_AXI_RGBLEDS_GPIO_BASEADDR) & 0b101));
             }
        if (duty_cycle <= Green_LED2)
                     {

                         Xil_Out32(XPAR_AXI_RGBLEDS_GPIO_BASEADDR, (Xil_In32(XPAR_AXI_RGBLEDS_GPIO_BASEADDR) & ~0b10000) | (0b10000));
                     }
              else if (duty_cycle > Green_LED2)
                     {

                         Xil_Out32(XPAR_AXI_RGBLEDS_GPIO_BASEADDR, (Xil_In32(XPAR_AXI_RGBLEDS_GPIO_BASEADDR) & 0b101111));
                     }
        if(duty_cycle<=Blue_LED1)
               {

                   Xil_Out32(XPAR_AXI_RGBLEDS_GPIO_BASEADDR,  (Xil_In32(XPAR_AXI_RGBLEDS_GPIO_BASEADDR) & (~0b100 ) ) | (0b100) );
               }
               else if(duty_cycle>Blue_LED1)
               {
                   Xil_Out32(XPAR_AXI_RGBLEDS_GPIO_BASEADDR,  Xil_In32(XPAR_AXI_RGBLEDS_GPIO_BASEADDR)& 0b011 );
               }
        if(duty_cycle<=Blue_LED2)
               {

                   Xil_Out32(XPAR_AXI_RGBLEDS_GPIO_BASEADDR,  (Xil_In32(XPAR_AXI_RGBLEDS_GPIO_BASEADDR) & (~0b100000 ) ) | (0b100000) );
               }
               else if(duty_cycle>Blue_LED2)
               {

                   Xil_Out32(XPAR_AXI_RGBLEDS_GPIO_BASEADDR,  Xil_In32(XPAR_AXI_RGBLEDS_GPIO_BASEADDR)& 0b011111 );
               }

        duty_cycle = duty_cycle + 1; // Incremento del ciclo
    }
    else
    {
        duty_cycle = 0; // Reset del ciclo
    }
}

int main()
{
    // Inizializzazione dell'interrupt controller e del LED RGB

    XIntc_Out32((XPAR_AXI_INTC_0_BASEADDR) + XIN_MER_OFFSET, (0b11)); // Abilita interrupt Master e Hardware
    XIntc_Out32((XPAR_AXI_INTC_0_BASEADDR) + XIN_IER_OFFSET, (0b100)); // Abilita interrupt del timer
    microblaze_enable_interrupts();

    Xil_Out32(XPAR_AXI_RGBLEDS_GPIO_BASEADDR, 0x0); // Inizializzazione dei LED RGB
    timer();

    return 0;
}

void timer(){
	 /******** IMPOSTAZIONE TIMER *********/
	    // Inizializzazione e impostazioni del timer

	    u32 ControlStatus;

	    XTmrCtr_SetControlStatusReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0, 0x52); //Setto i bit dello status register

	    XTmrCtr_SetLoadReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0, 1000); //setto il valore di partenza del timer
	    XTmrCtr_LoadTimerCounterReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0);  //carico il valore
	    ControlStatus = XTmrCtr_GetControlStatusReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0);
	    XTmrCtr_SetControlStatusReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0, ControlStatus & (~XTC_CSR_LOAD_MASK));

	    XTmrCtr_Enable(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0); //abilito il timer

	    XTmrCtr_WriteReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0, XTC_TCSR_OFFSET,
	                     (XTmrCtr_ReadReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0, XTC_TCSR_OFFSET) | XTC_CSR_ENABLE_TMR_MASK));
	    /*************************************/

}
//INTERRUPT SERVICE ROUTINE
void ISR()
{
    // Routine di interruzione del timer

    if (Xil_In32(0x41200000) == 0b100) //VERIFICO CHE L'INTERRUPT SIA ARRIVATO DAL TIMER
    {
        // Chiamata alla funzione per impostare i colori dei due LED, settare il valore di R G e B per ognuno dei due led
        selectcolor(255, 0, 0, 0, 255, 0);

        // ACKNOWLEDGE TIMER, SETTO IL BIT APPOSITO DELLO STATUS REGISTER
        XTmrCtr_SetControlStatusReg(XPAR_TMRCTR_0_BASEADDR, TIMER_COUNTER_0,
                                    (XTmrCtr_GetControlStatusReg(XPAR_TMRCTR_0_BASEADDR, TIMER_COUNTER_0)) |
                                    (XTC_CSR_INT_OCCURED_MASK));
        XIntc_Out32((XPAR_AXI_INTC_0_BASEADDR) + XIN_IAR_OFFSET, (0b100)); //ACKNOWLEDGE INTERRUPT CONTROLLER
    }
}
