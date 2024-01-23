
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xgpio_l.h"
void myISR(void) __attribute__((interrupt_handler));
void myISR () {
	int *led;
	int *switches;
	int *bottone;

	led=(int*)0x40000000;
	switches=(int*) 0x40020000;
	bottone=(int*)0x40010000;

	if (*(unsigned int*)0x41200004==0x2) { //il secondo bit dell'intC indica che l'interrupt è stato servito dagli switch
		if(*switches==0) *led=0x0; //l'interrupt viene servito ogni volta che abbiamo una variazione nei valori degli switch, quando tutti gli switch sono a zero vogliamo che i led siano tutti spenti
		else { for(int i=0; i<20; i++){
			if(i % 2==0) *led=0xaaaa;
			else *led=0x5555;  //introduciamo un contatore con un ciclo for, così da avere alternanza nella configurazione di accensione dei led quando l'interrupt è servito dagli switch, la configurazione è scritta in modo che i led si accendano alternatamente tra bit pari e bit dispari.

		for (int time=0; time<1000000; time++){

		}
		*led=0x0;
		} //introduciamo un nuovo contatore per far spegnere i led dopo un periodo di tempo
		}


		*((unsigned int*)0x40020120)=0x1;  //
		*((unsigned int*)0x4120000C) = 0x2;  //acknowledge IAR

	}
	else if(*(unsigned int*)0x41200004==0x1){
		*led=0xffff;
		for (int time=0; time<10000000; time++){}
		*led=0x0;
		*((unsigned int*)0x40010120)=0x1;
		*((unsigned int*)0x4120000C) = 0x1; //acknowledge

	}


}
int main()
{
	init_platform();
	//enable gpio2
	*((unsigned int*)0x4002011C)=0x80000000; //global interrupt enable register
	*((unsigned int*)0x40020128)=0x1; // IP interrupt enable register
	//enable gpio1
	*((unsigned int*)0x4001011C)=0x80000000; //global interrupt enable register
	*((unsigned int*)0x40010128)=0x1; // IP interrupt enable register
	//enable inrtc
	*((unsigned int*)0x4120001C)=0x3; // Master enable Register
	*((unsigned int*)0x41200008)=0x3; //Interrupt Enable Register

	 microblaze_enable_interrupts();


	// while (1) {};/



    cleanup_platform();
    return 0;
}
