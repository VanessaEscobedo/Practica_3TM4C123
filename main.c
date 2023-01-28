#include "lib/include.h"

int main(void)
{
    char c='5'; //variables
    int longitud=30; //largo del arreglo
    char Nombre[longitud]; //para almacenar el nombre se crea un arreglo de 60

    Configurar_PLL(_10MHZ); //configuracion de velocidad de reloj (me tocó 10 MHz)
    Configurar_UART0(); //configuracion UART4
    Configurar_GPIO(); //configuracion GPIO

    while(1)
    {
         c = readChar(); //leer el valor 
         switch(c) //dependiendo del valor que recibe el uart, entra al case o al default 
         {
           case 'n': //se le envia una n para el nombre 
                 GPIOF->DATA = (1<<2) | (1<<1); //prende color rosa
                 longitud = readString('.',&Nombre[0]); //calcular longitud del arreglo
                 //'.' es el delimitador
                 Invert(&Nombre[0],longitud); //invertir el nombre y agregar numeros 
                 printString(&Nombre[0],longitud); //imprimir la cadena
                 break;

            default:
                GPIOF->DATA = (1<<2); //prende solo el color azul 
                break;
         }
    }

}
