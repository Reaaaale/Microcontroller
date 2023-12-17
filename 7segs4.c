#include "platform.h"
#include "xstatus.h"
#include "xtmrctr_l.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xgpio_l.h"
#include "xintc_l.h"

#ifndef SDT
#define TMRCTR_BASEADDR XPAR_TMRCTR_0_BASEADDR
#else
#define TMRCTR_BASEADDR XPAR_XTMRCTR_0_BASEADDR
#endif

#define TIMER_COUNTER_0 0

volatile int cont;
char  numbers[] = {'0', '1', '2', '3', '4', '5', '6', '7','8'}; //definisco l'array da scansionare

void ISR() __attribute__((interrupt_handler));
void timer(void);
void print_number(char c);

int main(void) {

    init_platform();


    cont = 0;

    Xil_Out32(XPAR_AXI_SWITHES_GPIO_BASEADDR + XGPIO_GIE_OFFSET, 0x80000000); //abilito global interrupt enable
    Xil_Out32(XPAR_AXI_SWITHES_GPIO_BASEADDR + XGPIO_IER_OFFSET, 0x1); //Abilito i due canali nell IER
    Xil_Out32(XPAR_INTC_SINGLE_BASEADDR + XIN_IER_OFFSET, 0b100);//enable perferica timer
    Xil_Out32(XPAR_INTC_SINGLE_BASEADDR+ XIN_MER_OFFSET, 0b11); //Attivo entrambi i canali master enable register

    microblaze_enable_interrupts();
    timer();
    cleanup_platform();

    return 0;
}

void ISR() {
    if (Xil_In32(XPAR_INTC_SINGLE_BASEADDR) == 0b100) { // Se c'è stato un interrupt, allora viene eseguito l'if
        print("Conta\r\n");
        cont = (cont + 1) % 9; // Incremento cont e mi assicuro che rimanga nell'intervallo 0-8
        print_number(numbers[cont]);

        // Acknowledge del timer, setto a 1 il bit 8 dello status register, che mi dice che l'interrupt è stato servito
        XTmrCtr_SetControlStatusReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0, (XTmrCtr_GetControlStatusReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0)) | (XTC_CSR_INT_OCCURED_MASK));

        // Acknowledge nell'Interrupt controller, indicando che l'interrupt è stato servito
        Xil_Out32(XPAR_INTC_SINGLE_BASEADDR + XIN_IAR_OFFSET, 0b100);
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

void print_number(char c) {
    switch (c) {
    	case '0':
    		Xil_Out32(XPAR_AXI_7SEGSAN_GPIO_BASEADDR, 0x00); //accendi tutti 0 alla fine del conteggio, scrivendo in questo registro definisco in quale posizione sto scrivendo
    			Xil_Out32 (XPAR_AXI_7SEGS_GPIO_BASEADDR, 0xC0); //scrivendo in questo registro definisco quali segmenti si accendono, quindi definisco la cifra
    		break;
    	case '1':
    		Xil_Out32(XPAR_AXI_7SEGSAN_GPIO_BASEADDR, 0xFE);
    		Xil_Out32 (XPAR_AXI_7SEGS_GPIO_BASEADDR, 0xF9);
    	break;
    	case '2':
    		Xil_Out32(XPAR_AXI_7SEGSAN_GPIO_BASEADDR, 0xFD);
    		Xil_Out32 (XPAR_AXI_7SEGS_GPIO_BASEADDR, 0xA4);
    	break;
    	case '3':
    		Xil_Out32(XPAR_AXI_7SEGSAN_GPIO_BASEADDR, 0xFB);
    		Xil_Out32 (XPAR_AXI_7SEGS_GPIO_BASEADDR, 0xB0);
    	break;
    	case '4':
    		Xil_Out32(XPAR_AXI_7SEGSAN_GPIO_BASEADDR, 0xF7);
    		Xil_Out32 (XPAR_AXI_7SEGS_GPIO_BASEADDR, 0x99);
    	break;
    	case '5':
    		Xil_Out32(XPAR_AXI_7SEGSAN_GPIO_BASEADDR, 0xEF);
    		Xil_Out32 (XPAR_AXI_7SEGS_GPIO_BASEADDR, 0x92);
    	break;
    	case '6':
    			Xil_Out32(XPAR_AXI_7SEGSAN_GPIO_BASEADDR, 0xDF);
    			Xil_Out32 (XPAR_AXI_7SEGS_GPIO_BASEADDR, 0x82);
    		break;
    	case '7':
    			Xil_Out32(XPAR_AXI_7SEGSAN_GPIO_BASEADDR, 0xBF);
    			Xil_Out32 (XPAR_AXI_7SEGS_GPIO_BASEADDR, 0xF8);
    		break;
    	case '8':
    			Xil_Out32(XPAR_AXI_7SEGSAN_GPIO_BASEADDR, 0x7F);
    			Xil_Out32 (XPAR_AXI_7SEGS_GPIO_BASEADDR, 0x80);
    		break;
    	}

}
