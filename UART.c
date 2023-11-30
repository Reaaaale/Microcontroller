//LOOPBACK
#include <stdio.h>

#include "platform.h"

#include "xil_printf.h"

#include "xil_io.h"

#include "xuartlite_l.h"

u32 my_XUartLite_RecvByte2(UINTPTR BaseAddress)  //questa funzione legge il carattere in arrvo sulla fifo, in questo codice ho inserito la parte per ignorare il Cr nel main e non nella funzione
{
    if (XUartLite_IsReceiveEmpty(BaseAddress) == 1) //condizione if per controllare che il bit della fifo di ricezione sia vuoto, se vuoi non sta arrivando niente.
    {

        return 0;
    }
    else
    {
        u8 carattere1 = (u8)XUartLite_ReadReg(BaseAddress, XUL_RX_FIFO_OFFSET);
        return (u32)carattere1;
    }
}
int main()
{
    init_platform();

    u32 uart;


    print("Digita qualcosa\n\r");

    while (1)
    {


        uart = my_XUartLite_RecvByte2(0x40600000);

        // Stampa il carattere ricevuto solo se è diverso da 0 e da \r
        if (uart != 0 && uart != '\r') //diverso da \r per evitare il CR
        {
            xil_printf("Carattere ricevuto: %c\r", (char)uart);
        }

    }

    cleanup_platform();

    return 0;
}


//SECONDA PARTE, ACCENDE I LED IN BASE AL CARATTERE INSERITO IN CONSOLE ( 'a'+ numeri)

#define UART (u8)2


u32 my_XUartLite_RecvByte1(UINTPTR BaseAddress) {  //funzione di lettura della uart con l'implementazione della condizione che mi permette di ignorare il CR
    u8 carattere;

    if (XUartLite_IsReceiveEmpty(BaseAddress) == 1) {

        return 0;
    } else {
        carattere = (u8)XUartLite_ReadReg(BaseAddress, XUL_RX_FIFO_OFFSET);

        // Ignora il carattere di ritorno
        if (carattere == '\r') {
            return 0;

        }

        return (u32)carattere;
    }
}

void update_leds1(u32 interr, u8 mode) {  //in questa parte sto definendo una funzione che viene richiamata nel main che definisce il modo in cui i led vengono accesi
    int *led;
    led = (int *)0x40000000; //indirizzo dei led



    switch (interr) {  //per ogni case dello switch faccio accendere i led con una configurazione diversa
    case 0x00:
        *led = 0x00;
        for (int i = 0; i < 10000000; i++);
        break;
    case 0x31:
		*led=0x1;
    for (int i = 0; i < 10000000; i++);
     break;
    case 0x32:
  		*led=0x2;
      for (int i = 0; i < 10000000; i++);
       break;
    case 0x33:
  		*led=0x3;
      for (int i = 0; i < 10000000; i++);
       break;
    case 0x34:
  		*led=0x4;
      for (int i = 0; i < 10000000; i++);
       break;
    case 0x35:
  		*led=0x5;
      for (int i = 0; i < 10000000; i++);
       break;
    case 0x36:
  		*led=0x6;
      for (int i = 0; i < 10000000; i++);
       break;
    case 0x37:
  		*led=0x7;
      for (int i = 0; i < 10000000; i++);
       break;
    case 0x38:
  		*led=0x8;
      for (int i = 0; i < 10000000; i++);
       break;
    case 0x39:
  		*led=0x9;
      for (int i = 0; i < 10000000; i++);
       break;
    case 0x61:
  		*led=0xFFFF;
      for (int i = 0; i < 10000000; i++);
       break;


    }
    }


int main() {
    init_platform();

    u32 uart;

    while (1) {

        uart = my_XUartLite_RecvByte1(0x40600000);
        update_leds1(uart, UART); //richiamo la funzione che contiene le configurazioni dei led che si devono accendere


    }

    cleanup_platform();

    return 0;
}
//FUNZIONE NON BLOCKING
#define UART (u8)2

#define SWITCHES (u8)1

#define BUTTONS (u8)0



u32 my_XUartLite_RecvByte(UINTPTR BaseAddress)

{
u32 carattere;
	if (XUartLite_IsReceiveEmpty(BaseAddress)==0)
	{
		carattere= (u8)XUartLite_ReadReg(BaseAddress, XUL_RX_FIFO_OFFSET);
		xil_printf("ricevuto=%x", carattere); //stampo a schermo il carattere che sto mandando alla uart
		return carattere;
	}
	else
		return 0xd; //mando indietro il carattere '/r', che per come è stato scritto il programma verrà ignorato (0xd è la codifica ascii del cr)
}



void update_leds(u32 interr, u8 mode){
 int *led;

led=(int*)0x40000000;
	switch (mode){
	case 0:
			interr=interr & 0x000F; //prima faccio maschera per mettere a 0 tutti i bit tranne i 4 meno significativi
			interr=interr<<4; //shifto di 4, voglio usare i secondi 4 led e controllarli coi bottoni
			*led=*led & 0xFF0F; //faccio una maschera per mettere a 0 i bit che mi servono senza cambiare gli altri
			*led=*led | interr;  //faccio un or logico perchè non voglio che gli altri bit della stringa dei led vengano contaminati, carico nei led i valori dei bottoni



			break;
	case 1: //voglio controllare i primi 4 led con gli switch
		interr=interr & 0x000F;
		*led=*led & 0xFFF0;
		*led= *led | interr;

		break;
	case 2:
		if (interr != 0xd) //condizione if per ignorare il carrier return
		{
		interr=interr & 0x00FF;
		interr=interr<<8; //shifto di 8 perchè voglio che gli ultimi 8 bit, vengano controllati in funzione del carattere che sto mandando alla uart
		*led=*led & 0x00FF;
		*led= *led | interr;
		}
		break;

}

}

int main()

{

    init_platform();

    u32 uart;

    u32 buttons;

    u32 switches;

    while(1){

    buttons=Xil_In32(0x40010000); //LEGGE IL VALORE CHE C'E' NEL REGISTRO GPIO1

    update_leds(buttons, BUTTONS);

    switches=Xil_In32(0x40020000); //LEGGE IL VALORE CHE C'E' NEL REGISTRO GPIO2

    update_leds(switches, SWITCHES);

    uart=my_XUartLite_RecvByte(0x40600000);

    update_leds(uart, UART);


    }



    cleanup_platform();

    return 0;
}
