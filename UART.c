#include "lib/include.h"

//configuración UART4, Baud-rate 4800
//U4Tx->PC5
//U4Rx->PC4
extern void Configurar_UART0(void)
{
    SYSCTL->RCGCUART  = (1<<4); //inicializar y habilitar el UART (RCGCUART) p. 902,344
    SYSCTL->RCGCGPIO |= (1<<2); //habilitar el reloj para el módulo GPIO (RCGCGPIO) p. 902,1351,340
    //habilitar reloj del puerto C
    GPIOC->AFSEL = (1<<5) | (1<<4); //habilitar funciones alternativas p. 902,1350,672
    GPIOC->PCTL = (GPIOC->PCTL&0xFF00FFFF) | 0x00110000;    //configurar GPIO port control (GPIOPCTL)
    //GPIO Port Control PC4-> U4Rx PC5-> U4Tx p. 688
    //(1<<16) | (1<<20);
    GPIOC->DEN = (1<<5) | (1<<4);//habilitar funciones digitales del pin (GPIODEN) p. 688 
    //PC5 PC4
    UART4->CTL = (0<<9) | (0<<8) | (0<<0); //deshabilitar el UART -> UART control (UARTCTL) p. 918

    // UART Integer Baud-Rate Divisor (UARTIBRD) p. 914 ***
    /*
    BRD = 10,000,000 / (16*4800) = 130.2083
    UARTFBRD[DIVFRAC] = integer(.2083 * 64 + 0.5) se redondea hacia arriba 
    */
    UART4->IBRD = 130;
    UART4->FBRD = 14; // UART Fractional Baud-Rate Divisor (UARTFBRD) p. 915
    UART4->LCRH = (0x3<<5)|(1<<4); //configurar UART Line Control (UARTLCRH) p. 916
    //se transmiten/reciben 8 bits, habilitar FIFO buffers
    UART4->CC =(0<<0);  //configurar reloj -> UART Clock Configuration (UARTCC) p. 939
    UART4->CTL = (1<<9) | (1<<8) | (1<<0); //habilitar el UART p. 918
}

extern char readChar(void) //retorna un valor de tipo char
{
    //UART flag register (UARTFR) p. 911
    //UART data register (UARTDR) p. 906
    int v;
    char c;
    while((UART0->FR & (1<<4)) != 0 ); //mientras se reciba un dato, se va a almacenar
    //1 -> el receiver no está vacio
    //0 -> el receiver FIFO esta vacio 
    v = UART0->DR & 0xFF; //se escriben los datos que se van a transmitir
    //0xFF = 1111 1111
    c = v;
    return c;
}

extern void printChar(char c) //recibe a c
{
    while((UART4->FR & (1<<5)) != 0 ); 
    //1 -> el transmit FIFO is full 
    //0 -> el transmitter is not full
    UART4->DR = c;
}

extern void printString(char* string)
{
    while(*string)
    {
        printChar(*(string++));
    }
}

extern char * readString(char delimitador)
{

   int i=0;
   char *string = (char *)calloc(10,sizeof(char));
   char c = readChar();
   while(c != delimitador)
   {
       *(string+i) = c;
       i++;
       if(i%10==0)
       {
           string = realloc(string,(i+10)*sizeof(char));
       }
       c = readChar();
   }

   return string;

}
//Experimento 2

//El envio es su nombre  (rave) 

// invertirlo y regresarlo con numeros consecutivos
// entre letras (e1v2a3r) 

