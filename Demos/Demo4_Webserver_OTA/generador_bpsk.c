/********************************FUNCIONES DE GENERACION BPSK**************************/
/* Includes añadidos------------------------------------------------------------------*/
//#include "stm32f2xx.h"                  // Device header
#include "stdint.h"
#include "stdlib.h"
#include "kasami_secuencias.h"
#include "math.h"
//#include "stdio.h"
/* Defines añadidos-------------------------------------------------------------------*/
#define Pi 3.14159


void Genera_Kasami_Modulada(char n_fila_kasami, int n_bits_kasami, char muestras_ciclo, char ciclos_simbolo){
	int a,b,j,i,indice=0;	
	
	/* Buffers para la forma de señal (Simbolo_A y B) y para cada uno de los elementos de la amplitud de la señal BPSK completa */
	
	int8_t Simbolo_A[24*4];										// Maximo	numero de elementos que puede tomar Simbolo
	int8_t Simbolo_B[24*4];										// Maximo	numero de elementos que puede tomar Simbolo
	
	for(a=0;a<muestras_ciclo*ciclos_simbolo;a++)Simbolo_A[a]=(255*(1+sin(2*Pi*a/muestras_ciclo))/2);		//Simbolo A 255... amplitud, resolución 8bits
	for(b=0;b<muestras_ciclo*ciclos_simbolo;b++)Simbolo_B[b]=(255*(1-sin(2*Pi*b/muestras_ciclo))/2);		//Simbolo B
	
	if(n_bits_kasami==255){
		for(i=0;i<n_bits_kasami;i++){
			if(Kasami255[n_fila_kasami][i]==+1)
				for(j=0;j<muestras_ciclo*ciclos_simbolo;j++) 
			Kasami_Modulada[indice++]=Simbolo_A[j];	
			else
				for(j=0;j<muestras_ciclo*ciclos_simbolo;j++)
			Kasami_Modulada[indice++]=Simbolo_B[j];			
		}
	}//Elementos limitados en cada entrada en la funcion por muestras_ciclo*ciclos_simbolo de la misma forma que en TIM2
	else if (n_bits_kasami==1023){
		for(i=0;i<n_bits_kasami;i++){
			if(Kasami1023[n_fila_kasami][i]==+1)	
				for(j=0;j<muestras_ciclo*ciclos_simbolo;j++) 
			Kasami_Modulada[indice++]=Simbolo_A[j];
			else	  
				for(j=0;j<muestras_ciclo*ciclos_simbolo;j++) 
			Kasami_Modulada[indice++]=Simbolo_B[j];															
		}
	}//Elementos limitados en cada entrada en la funcion por muestras_ciclo*ciclos_simbolo de la misma forma que en TIM2
	//p_kasami = &Kasami_Modulada[0];
	//return p_kasami;			//Devuelve el puntero al primer elemento del array
}
//	int numero_elementos_actual, valor_maximo_kasami_modulada;
//	
//	valor_maximo_kasami_modulada = 255*24*2;
//	numero_elementos_actual = Longitud*muestras_ciclo*ciclos_simbolo;							//Se rellena el array hasta este valor

//	int8_t kasami255_aux [255], kasami1023_aux[1023];
//	
//	if (n_bits_kasami == 255){
//		for(i=0;i<n_bits_kasami;i++){
//			kasami255_aux [i] = Kasami255[n_fila_kasami] [i];
//		}
//	}
//	else if(n_bits_kasami == 1023){
//		for(i=0;i<n_bits_kasami;i++){
//			kasami1023_aux [i] = Kasami1023[n_fila_kasami] [i];
//		}
//	}

//allocate the array
//	int** kasami_modulada = new int*[row]; 			//ROW = FILA
//	for(int i = 0; i < row; i++)
//   kasami_modulada[i] = new int[col];

	// use the array

	//deallocate the array
//	for(int i = 0; i < row; i++)
//    delete[] arr[i];
//	delete[] arr;

	//free(Kasami_Modulada); free(Simbolo_A); free(Simbolo_B);										//Liberación de memoria
	
	
	//3 Arrays dinámicos
//	Kasami_Modulada = (int8_t *)malloc(Longitud*muestras_ciclo*ciclos_simbolo);
//	Simbolo_A = (int8_t *)malloc(muestras_ciclo*ciclos_simbolo);
//	Simbolo_B = (int8_t *)malloc(muestras_ciclo*ciclos_simbolo);	
