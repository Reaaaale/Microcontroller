#include "platform.h"
#include "xstatus.h"
#include "xtmrctr_l.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xgpio_l.h"
#include "xintc_l.h"
#include "xuartlite_l.h"
#include "xsysmon_hw.h"

#ifndef SDT
#define TMRCTR_BASEADDR     XPAR_TMRCTR_0_BASEADDR
#else
#define TMRCTR_BASEADDR     XPAR_XTMRCTR_0_BASEADDR
#endif
#define TIMER_COUNTER_0     0

int cont = 0;
int timer_start_value = 0; // Valore di partenza predefinito

void ISR()__attribute__((interrupt_handler));
void Audio(int cont);
void timer(void);
int volatile potenz;

int main() {
    init_platform();
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IER_OFFSET, 0b100); // enable timer
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_MER_OFFSET, 0b11);  // Attivo entrambi i canali master enable register

    microblaze_enable_interrupts();
    timer();
    potenz = XSysMon_ReadReg(XPAR_XADC_WIZ_0_BASEADDR, XSM_AUX03_OFFSET);

    cleanup_platform();
    return 0;
}


void ISR() {
    if (Xil_In32(XPAR_AXI_INTC_0_BASEADDR) == 0b100) { // if interrupt da Timer
        cont = ~cont; //inverto la variabile timer in modo da accendere e spegnere continuamente l'audio pwm
        Audio(cont);
    }

        // Acknowledge del Timer
        XTmrCtr_SetControlStatusReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0, (XTmrCtr_GetControlStatusReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0)) | (XTC_CSR_INT_OCCURED_MASK));
        XIntc_Out32((XPAR_AXI_INTC_0_BASEADDR) + XIN_IAR_OFFSET, (0b100)); // Acknowledge timer
    }


void Audio(int cont) {
    int switch_state = Xil_In32(XPAR_AXI_SWITHES_GPIO_BASEADDR);

   switch (switch_state) {
   case 0x00:
	   timer_start_value= 0; //Audio OFF quando nessuno switch è attivato
       break;
   case 0x01:
	   timer_start_value= 191131; //Do
	   break;
   case 0x02:
	   timer_start_value=180375; //Do#
	   break;
   case 0x04:
	   timer_start_value=170241; //Re
	   break;
   case 0x08:
   	   timer_start_value= 160720; //Re#
   	   break;
   case 0x10:
   	   timer_start_value=151699; //Mi
   	   break;
   case 0x20:
   	   timer_start_value=143184; //Fa
   	   break;
   case 0x40:
  	   timer_start_value= 135135; //Fa#
  	   break;
   case 0x80:
  	   timer_start_value=127551; //Sol
  	   break;
   case 0x100:
  	   timer_start_value=120395; //Sol#
  	   break;
   case 0x200:
       timer_start_value= 113636; //La
       break;
   case 0x400:
       timer_start_value=107250; //La#
       break;
   case 0x800:
       timer_start_value=101235; //Si
       break;

   }

    // Applica il nuovo valore di partenza del timer
    XTmrCtr_SetLoadReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0, timer_start_value);

    int c=10;
    int mode=0;  //SETTA A 1 PER ABILITARE IL POTENZIOMETRO

        	potenz = XSysMon_ReadReg(XPAR_XADC_WIZ_0_BASEADDR, XSM_AUX03_OFFSET);
        	 if (mode==1) {
        	timer_start_value=potenz+10* c * timer_start_value;
        	XTmrCtr_SetLoadReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0, timer_start_value);
        	 }

    // Controlla la variabile cont per accendere o spegnere l'AUDIO PWM
    if (cont != 0) {
        Xil_Out32(XPAR_AXI_PWM_AUDIO_BASEADDR, 1);  // Accendi l'AUDIO PWM
    } else {
        Xil_Out32(XPAR_AXI_PWM_AUDIO_BASEADDR, 3);  // Spegni l'AUDIO PWM
    }

}

	void timer(){
		 /******** IMPOSTAZIONE TIMER *********/
		    // Inizializzazione e impostazioni del timer
	    u32 ControlStatus;

	    XTmrCtr_SetControlStatusReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0, 0x52);

	    XTmrCtr_SetLoadReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0, timer_start_value);
	    XTmrCtr_LoadTimerCounterReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0);
	    ControlStatus = XTmrCtr_GetControlStatusReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0);
	    XTmrCtr_SetControlStatusReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0, ControlStatus & (~XTC_CSR_LOAD_MASK));
	    XTmrCtr_Enable(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0);

	    XTmrCtr_WriteReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0, XTC_TCSR_OFFSET,(XTmrCtr_ReadReg(XPAR_AXI_TIMER_0_BASEADDR, TIMER_COUNTER_0, XTC_TCSR_OFFSET) | XTC_CSR_ENABLE_TMR_MASK));


	}
