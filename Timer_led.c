#include "platform.h"
#include "xstatus.h"
#include "xtmrctr_l.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xgpio_l.h"
#include "xintc_l.h"


#ifndef SDT
#define TMRCTR_BASEADDR		XPAR_TMRCTR_0_BASEADDR
#else
#define TMRCTR_BASEADDR		XPAR_XTMRCTR_0_BASEADDR
#endif

#define TIMER_COUNTER_0	 0
volatile int flag;
void timer(void);
void ISR ()__attribute__((interrupt_handler));

int main(void)
{

	init_platform();
	flag=1;
Xil_Out32(XPAR_AXI_SWITHES_GPIO_BASEADDR + XGPIO_GIE_OFFSET, 0x80000000); //abilito global interrupt enable
Xil_Out32(XPAR_AXI_SWITHES_GPIO_BASEADDR + XGPIO_IER_OFFSET, 0x1); //Abilito i due canali nell IER
Xil_Out32(XPAR_INTC_SINGLE_BASEADDR + XIN_IER_OFFSET, 0b10);//enable perferica switch
Xil_Out32(XPAR_INTC_SINGLE_BASEADDR + XIN_IER_OFFSET, 0b100);//enable perferica timer
Xil_Out32(XPAR_INTC_SINGLE_BASEADDR+ XIN_MER_OFFSET, 0b11); //Attivo entrambi i canali master enable register
    microblaze_enable_interrupts();
   timer();
}

   	  void ISR ()
   	  {
   		int *switches;
   		switches=(int*)(XPAR_AXI_SWITHES_GPIO_BASEADDR);
   		  if (Xil_In32(XPAR_INTC_SINGLE_BASEADDR)== 0b100) //Se c'è stato un interrupt, allora viene eseguito l'if
   		  	  {
   			  //print("Conta\r\n");
   			  Xil_Out32(XPAR_AXI_16LEDS_GPIO_BASEADDR, ~Xil_In32(XPAR_AXI_16LEDS_GPIO_BASEADDR)); //Vado a scrivere all'interno del registro dei led il contenuto dello stesso negato così da poter alternare le configurazioni dei led (accesi/spenti)
		      XTmrCtr_SetControlStatusReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0, ( XTmrCtr_GetControlStatusReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0) ) | (XTC_CSR_INT_OCCURED_MASK));
		      //acknowledge del timer, setto a 1 il bit 8 dello status register, che mi dice che l'interrupt è stato servito
		 	  Xil_Out32(XPAR_INTC_SINGLE_BASEADDR + XIN_IAR_OFFSET, 0b100); //Acknowledge Interrupt controller
		 	  //XTmrCtr_Enable(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0);

   		  	  }

   		  if (Xil_In32(XPAR_INTC_SINGLE_BASEADDR+XIN_IPR_OFFSET)== 0b100) //Se c'è stato un interrupt da switch, allora viene eseguito l'if
   		   		  	  {
   			  if(Xil_In32(switches)==0) flag=1;   //se gli switch sono spenti flag (divisiore del valore max del timer per aumentare la velocità) è 1, altrimenti carico in flag il valore sugli switch
   			  else flag=Xil_In32(switches);
   			  XTmrCtr_SetLoadReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0, 100000000/flag);
   			  Xil_Out32(XPAR_AXI_SWITHES_GPIO_BASEADDR + XGPIO_ISR_OFFSET, 0b11);//Acknowledge gpio
   			  Xil_Out32(XPAR_INTC_SINGLE_BASEADDR + XIN_IAR_OFFSET, 0b10);//Acknowledge Interrupt controller

   		   		  	  }
}

   	void timer(){
   		 /******** IMPOSTAZIONE TIMER *********/
   		    // Inizializzazione e impostazioni del timer

   		    u32 ControlStatus;

   		    XTmrCtr_SetControlStatusReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0, 0x52); //Setto i bit dello status register

   		    XTmrCtr_SetLoadReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0, 100000000); //setto il valore di partenza del timer
   		    XTmrCtr_LoadTimerCounterReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0);  //carico il valore
   		    ControlStatus = XTmrCtr_GetControlStatusReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0);
   		    XTmrCtr_SetControlStatusReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0, ControlStatus & (~XTC_CSR_LOAD_MASK));

   		    XTmrCtr_Enable(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0); //abilito il timer

   		    XTmrCtr_WriteReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0, XTC_TCSR_OFFSET,
   		                     (XTmrCtr_ReadReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0, XTC_TCSR_OFFSET) | XTC_CSR_ENABLE_TMR_MASK));
   		    /*************************************/

   	}

