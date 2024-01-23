#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xtmrctr_l.h"
#include "xgpio_l.h"
#include "xstatus.h"
#include "xuartlite_l.h"
#include "xparameters.h"
#include "xintc_l.h"

#ifndef SDT

#define TMRCTR_BASEADDR		XPAR_TMRCTR_0_BASEADDR
#else
#define TMRCTR_BASEADDR		XPAR_XTMRCTR_0_BASEADDR
#endif
#define TIMER_COUNTER_0	 0

int buttons;
int volatile led;
int volatile counter;
typedef enum {pressed, idle} debounce_state_t;
typedef enum {left, center, right} led_state_t;
void timer(void);
void ISR ()__attribute__((interrupt_handler));

int FSM_debounce(int buttons);
int FSM_led(int buttons);
int volatile sliding(int volatile led, int volatile cont);
int volatile flashing_lights  (int volatile led, int volatile cont);

int FSM_debounce(int buttons){
static int debounced_buttons;
static debounce_state_t currentState = idle;
switch (currentState) {  //definisco i due stati del bottone 'pressed' e 'idle'
	case idle:
		debounced_buttons=buttons;
		if (buttons!=0)
			currentState=pressed;
		break;
	case pressed:
		debounced_buttons=0;
		if (buttons==0)
			currentState=idle;
		break;
	}
return debounced_buttons;
}


	int FSM_led(int buttons){
		static int led;
		static led_state_t currentState = center;
		switch (currentState) {  //in questo switch definisco i case per la gestione dell'accensione dei led
		case center:
			led=0x0;
			if (buttons==0b10) {
				currentState=left;
				counter=0;
			}
			else if (buttons==0b1000) {
				currentState=right;
				counter=0;
			}
			break;
		case left:
			led=0xFF00;

			if (buttons==0b10) currentState=center;
			else if (buttons==0b1000) {
				currentState=right;
				counter=0;
			}
			break;
		case right:
			led=0x00FF;
			if (buttons==0b1000) currentState=center;
			else if (buttons==0b10){
				currentState=left;
				counter=0;
			}
			break;
		}

		return led;
}

int main(void)
{
	init_platform();
	counter=0;
		Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IER_OFFSET, 0b100); // enable timer
	Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_MER_OFFSET, 0b11);  // Attivo entrambi i canali master enable register

	microblaze_enable_interrupts();
	timer();

	//Setta il valore di mode a 0 o a 1 per scegliere tra le due modalità di accensione dei led
	int mode=0;



	while(1){
		 	buttons=Xil_In32 (XPAR_AXI_BUTTONS_GPIO_BASEADDR);
	   	    	buttons=FSM_debounce(buttons);
	   	    	led=FSM_led(buttons);
				
//Definisco con l'if quale delle due funzioni vada richiamata
	   	    	if (mode==0){
	   	    				 led=flashing_lights (led, counter);
	   	    	}

	   	    	if (mode==1) //Se c'è stato un interrupt, allora viene eseguito l'if
	   	    					   		  	  {
	   	    				led=sliding(led, counter);

	   	    					   		  	  }

	   	    	Xil_Out32(XPAR_AXI_16LEDS_GPIO_BASEADDR, led);
	   	    	cleanup_platform();
	   		}
	}

void ISR ()
   	  {


   		  if (Xil_In32(XPAR_AXI_INTC_0_BASEADDR)== 0b100) //Se c'è stato un interrupt, allora viene eseguito l'if
   		  	  {
   			  if (counter==8) counter=1;
   			  else counter=counter+1;
   			  XTmrCtr_SetControlStatusReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0, ( XTmrCtr_GetControlStatusReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0) ) | (XTC_CSR_INT_OCCURED_MASK));
		      Xil_Out32((XPAR_AXI_INTC_0_BASEADDR) + XIN_IAR_OFFSET, 0b100); //Acknowledge Interrupt controller
		 	  }
   	  	  }

int volatile flashing_lights  (int volatile led, int volatile counter){
	 if(led==0xFF00){
		 if (counter%2==0) led=0xFF00;
		 else led=0x0000;
	 }
	 if(led==0x00FF){
		 if (counter%2==1) led=0x00FF;
		 else led=0x0000;
	 }
	 return led;
}

int volatile sliding(int volatile led, int volatile counter)
{
	if(led==0xFF00){
		led = 0x0100 <<  (counter-1);
	}
	else if(led==0x00FF){
		led = 0x0080 >>  (counter-1);
	}
	return led;
}
void timer(){
		 /******** IMPOSTAZIONE TIMER *********/
		    // Inizializzazione e impostazioni del timer

		    u32 ControlStatus;

		    XTmrCtr_SetControlStatusReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0, 0x52); //Setto i bit dello status register

		    XTmrCtr_SetLoadReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0, 10000000); //setto il valore di partenza del timer
		    XTmrCtr_LoadTimerCounterReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0);  //carico il valore
		    ControlStatus = XTmrCtr_GetControlStatusReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0);
		    XTmrCtr_SetControlStatusReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0, ControlStatus & (~XTC_CSR_LOAD_MASK));

		    XTmrCtr_Enable(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0); //abilito il timer

		    XTmrCtr_WriteReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0, XTC_TCSR_OFFSET,
		                     (XTmrCtr_ReadReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0, XTC_TCSR_OFFSET) | XTC_CSR_ENABLE_TMR_MASK));
		    /*************************************/

	}
