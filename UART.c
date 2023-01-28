#include "lib/include.h"

//configuración UART4, Baud-rate 4800
//U4Tx->PC5 (se conecta al Rx del modulo)
//U4Rx->PC4

//el modulo aparece como "USB serial mode"

/*PASOS
* 1. en una nueva terminal, escribit python
* 2. import serial as s
* 3. ser=s.Serial('COM11',4800)
* 4. ser.write(b'n')->prende rosa
* 5. ser.write(b'nombre.')
* 6. ser.read(#) (el total de letras y numeros)
*/
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

extern char readChar(void) //función que lee un caracter 
{
    int v;
    char c;
    while((UART4->FR & (1<<4)) != 0 );
    v = UART4->DR & 0xFF;
    c = v; //lo guarda en 'c'
    return c; //lo regresa 
}

extern void printChar(char c) //función para imprimir un caracter 
{
    while((UART4->FR & (1<<5)) != 0 );
    UART4->DR = c;
}

extern void printString(char* string, int longitud) //función para imprimir una cadena (el nombre) 
{
    int i=0;
    while(string[i]>47) //el 47 es por el ascii (evitar que le lleguen cosas raras)
    {
        printChar(string[i]); //va imprimiendo cada caracter
        i++;
    }
}

extern int readString(char delimitador, char *string) //función para leer una cadena 
{
   int i = 0;
   char c = readChar();
   while(c != delimitador) //cuando sea igual al delimitador (.) se detiene
   {
       string[i] = c;
       i++;
       c = readChar(); //va leyendo cada letra que le llega, la cual va guardando en un arreglo
   }
   return i; //regresa el # de elementos (el numero de letras que tiene el nombre)
}

extern void Invert(char *Nombre, int longitud) //funcion para invertir el nombre 
{
    int i = 1;
    int numero = 1; //empezamos con el # 1
    int b = 0;
    int bandera = 1;
    int Size;
    Size = longitud;
    char invertido[longitud]; //arreglo para guardar el nombre con los numeros
   
    if (longitud < 10) //cuanta memoria necesitamos para guardar el nombre con los numeros 
    
        longitud = longitud + (longitud-1); //arreglos pequeños 
    else
    {
        longitud = 18 + ((longitud - 9)*3); //arreglos grandes 
    }     

    while(i<longitud)
    {
        if (bandera == 1 )
        {
	        invertido[b] = Nombre[Size-i]; //se va guardando cada letra 
	
            i = i + 1; //para poder ir recorriendo el arreglo
	        b = b + 1;
    
            bandera = 0; //se pone en 0 para que entre al else y agregue un numero 
        }
        else
        {
	        invertido[b] = numero + '0'; 
	        numero = numero + 1;
	        b = b + 1;

            bandera = 1;
        }
    }

    for(int j = 0; j <= (longitud - 1); j++)
    {
        Nombre[j]=invertido[j]; //se guarda el nombre invertido en el arreglo original
    } 
    
    for(int k = longitud ;k <= 30; k++) //rellenar los espacios faltantes del arreglo
        Nombre[k] = 0;
    return;
} 