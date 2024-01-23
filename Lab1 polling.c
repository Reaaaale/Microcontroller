 #include <stdio.h>
#include "platform.h"
#include "xil_printf.h"


int main()
{
    init_platform();

    int *led;
    led=(int*)0x40000000; //E' l'indirizzo sulla gpio dei led
    volatile int *Switches; //inizializzazione degli switches
    volatile int *button; //Perchè sono valori che cambiano
    button=(volatile int*)0x40010000; //assegnamo alla variabile l'indirizzo in memoria dedicato ai bottoni
    Switches=(volatile int*)0x40020000; //assegnamo alla variabile l'indirizzo in memoria dedicato agli switches
    while (1) //ciclo while infinito perchè la condizione (1) è sempre vera
    { *led=*button;
    *led=*Switches; //assegno al puntato da LED il valore del puntato switches e bottoni


    }
    cleanup_platform();
    return 0;
}
