/*********************MAIN INTERRUPTS SERVICE ROUTINES*************************/
/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx_it.h"
#include "mxchipWNET.h"
#include "stdint.h"
#include "stdlib.h"
#include "flash_configurations.h"
#include "stdio.h"

extern config_param_t config_param;								//Estructura tipo config_param_t para guardado de parametros

int flag;
/* Valores para el led que indica el módulo con el que se interactúa----------*/
char Flag_Led;																		//Flag para bloquear el parpadeo del led verde a un máximo de 4 seg (TIM4)
static int Static_Contador_Led;										//Contador para timer 4
static uint8_t Static_Led_Conexion;							//variable para led de actividad

/* Variables de entrada-------------------------------------------------------*/
extern int 	Nbits_Html;															//Kasami 255 o 1023
extern char Sec_Trans_1, Sec_Trans_2, Sec_Trans_3, Sec_Trans_4, Sec_Trans_5;				//Secuencia kasami 0 a 15 para 255 0 a 31 para 1023
extern int  Frecuencia_Html; 	      								// 12 muestras/ciclo en el inicio
extern int  Ciclos_Simbolo_Html;        						// 2ciclos/forma en el inicio
extern int  Periodo_Emision_Html;

/* Valores aislados actuales--------------------------------------------------*/
static int Static_Nbits;
static char Static_Sec_Ka;
static int Static_Frecuencia;
static int Static_Ciclos_Simbolo;
static int Static_Periodo_Emision;
static char Static_Numero_Transductor = 1;

/* Valores para timer3-------------------------------------------------------*/
static int8_t Static_Kasami_Modulada=0;						//Aux para valor actual del DAC
static uint32_t Static_Canal_Mux = 1;							//Variable de selección de canal mux
static uint32_t Bit0_mux, Bit1_mux, Bit2_mux;			//Para máscara de selección de canal mux
char Flag_Actualizacion_Param;										//Flag para la correcta actualización de parámetros

/* Valores para Timer---------------------------------------------------------*/
static uint32_t Contador_Timer2;									//Valor contador timer 2
static char Static_Flag_Timer;										//Flag aux. para asegurar las debidas interrupciones entre TIM2 y TIM3

/* Valores que están en Timer y Timer 3---------------------------------------*/
static uint32_t Static_Cuentas_Tim2 = 3060;			//Valor inicial hasta donde contar tim2
extern int8_t Kasami_Modulada[];
extern void Genera_Kasami_Modulada(char n_fila_kasami, int n_bits_kasami, char muestras_ciclo, char ciclos_simbolo);

/*********************************Funciones OTA****************************************/
void NMI_Handler(void){
}

__asm void asm_dump(void){
	 IMPORT hard_fault_handler_c
	 TST LR, #4
	 ITE EQ
	 MRSEQ R0, MSP
	 MRSNE R0, PSP
	 B hard_fault_handler_c
 }
void HardFault_Handler(void){
	asm_dump();
}

void MemManage_Handler(void){
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

void BusFault_Handler(void){
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

void UsageFault_Handler(void){
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

void SVC_Handler(void){
}


void DebugMon_Handler(void){
}

void PendSV_Handler(void){
}
void SysTick_Handler(void){
	
	systick_irq();    
	NoOS_systick_irq();

}

/*********************************TIM2_IRQHandler Añadida***IT para generación señal****/
void TIM2_IRQHandler (void){	
	TIM2->SR &= ~(TIM_SR_UIF);																			//Flag del manejador
	
	Static_Kasami_Modulada = Kasami_Modulada[Contador_Timer2++];		//Incremento del siguiente valor de la secuencia actual
	
	DAC_SetChannel1Data(DAC_Align_8b_R, Static_Kasami_Modulada);		//Valor de salida del DAC
	
  if (Contador_Timer2 >= (Static_Cuentas_Tim2) && (Static_Flag_Timer == 0)){		//No se pueden meter más valores que este num de cuentas
		Static_Flag_Timer = 1;																				
		//TIM_SetCounter(TIM2, 0);																			//Reseteo de cuenta del Timer 2 
		Contador_Timer2=0;																						//Contador_Timer2 => Se actualiza al valor del tamaño de Kasami_Modulad[]
		DAC_SetChannel1Data(DAC_Align_8b_R, 0x00);										//Valor de salida del DAC  	
		Genera_Kasami_Modulada(Static_Sec_Ka, Static_Nbits, 12, Static_Ciclos_Simbolo);		//Generación de la siguiente secuencia
		Bit0_mux = (0x00000001 & (Static_Canal_Mux-1)); 							//Se adquiere cada bit para la multiplexación
		Bit1_mux = (0x00000002 & (Static_Canal_Mux-1)); 
		Bit2_mux = (0x00000004 & (Static_Canal_Mux-1));
		GPIOB->ODR &= 0xFFFFFF1F;																			//1º se resetean los bits implicados del mux
		GPIOB->ODR |= ((Bit0_mux<<5)|(Bit1_mux<<5)|(Bit2_mux<<5));		//2º se ponen en el puerto los bits adquiridos de la multiplexación
		TIM_Cmd(TIM2, DISABLE);																				//Habilitación del periférico TIM2 y cuenta
	}
}
/*********************************TIM3_IRQHandler Añadida*******Periodo de Emision******/
void TIM3_IRQHandler (void){	//Establecido con prescaler para múltiplos de 1ms
	TIM_TimeBaseInitTypeDef Tim2_TimeBaseInit;										//Creación de una estructura para TIM2 por si hubiera que realizar cambios
	Static_Flag_Timer = 0;																				//Flag aux. para asegurar las debidas interrupciones entre TIM2 y TIM3
	TIM3->SR &= ~(TIM_SR_UIF);																		//Flag del manejador	
		
	if(Flag_Actualizacion_Param == 0){													//Guardado en memoria y actualización de parámetros
			Flag_Actualizacion_Param = 1;															
			config_param.ciclos_simbolo		 = Ciclos_Simbolo_Html;			
			config_param.frecuencia		 		 = Frecuencia_Html;
			config_param.numero_bits 			 = Nbits_Html;
			config_param.periodo_emision	 = Periodo_Emision_Html;
			config_param.secuencia_transductor1 = Sec_Trans_1;
			config_param.secuencia_transductor2 = Sec_Trans_2;
			config_param.secuencia_transductor3 = Sec_Trans_3;
			config_param.secuencia_transductor4 = Sec_Trans_4;
			config_param.secuencia_transductor5 = Sec_Trans_5;
			Guarda_Config(&config_param);															//Guardado de parametros en memoria
			Static_Ciclos_Simbolo  = Ciclos_Simbolo_Html;							//Los parámetros de modulación se pasan a vbles estáticas
			Static_Frecuencia  = Frecuencia_Html;
			Static_Periodo_Emision = Periodo_Emision_Html;
			Static_Nbits = Nbits_Html;
			if(Static_Frecuencia == 40){
				//TIM_Cmd(TIM2, DISABLE);																			//Habilitación del periférico TIM2 y cuenta
				//NVIC_ClearPendingIRQ(TIM2_IRQn);														//Quitar si está pendiente la interrupción del TIM2	
				//NVIC_DisableIRQ(TIM2_IRQn);																	//Deshabilitación de la interrupción
				Tim2_TimeBaseInit.TIM_Prescaler = 0x0000;										//Prescaler
				Tim2_TimeBaseInit.TIM_CounterMode = TIM_CounterMode_Down;		//Cuenta descendente
				Tim2_TimeBaseInit.TIM_Period = 60e6/480e3-1;								//Frec. perif/frec interrup
				Tim2_TimeBaseInit.TIM_ClockDivision = TIM_CKD_DIV1;					//Division por 1
				Tim2_TimeBaseInit.TIM_RepetitionCounter = 0x0000;
				TIM_TimeBaseInit(TIM2, &Tim2_TimeBaseInit);

				//NVIC_EnableIRQ(TIM2_IRQn);																	//Habilitación de la interrupción
			}
			else if (Static_Frecuencia == 41){
				//TIM_Cmd(TIM2, DISABLE);																			//Habilitación del periférico TIM2 y cuenta
				//NVIC_ClearPendingIRQ(TIM2_IRQn);														//Quitar si está pendiente la interrupción del TIM2	
				//NVIC_DisableIRQ(TIM2_IRQn);																	//Deshabilitación de la interrupción	
				Tim2_TimeBaseInit.TIM_Prescaler = 0x0000;										//Prescaler
				Tim2_TimeBaseInit.TIM_CounterMode = TIM_CounterMode_Down;		//Cuenta descendente
				Tim2_TimeBaseInit.TIM_Period = 60e6/500e3-1;								//Frec. perif/frec interrup
				Tim2_TimeBaseInit.TIM_ClockDivision = TIM_CKD_DIV1;					//Division por 1
				Tim2_TimeBaseInit.TIM_RepetitionCounter = 0x0000;
				TIM_TimeBaseInit(TIM2, &Tim2_TimeBaseInit);
				
				//NVIC_EnableIRQ(TIM2_IRQn);																	//Habilitación de la interrupción
			}
			TIM_SetAutoreload(TIM3,(Static_Periodo_Emision-1));					//Se actualiza el periodo de emisión
			TIM_SetCounter(TIM2, 0);																		//Reseteo de cuenta del Timer 2 
			
		}
		TIM_Cmd(TIM2, ENABLE);																			//Habilitación del periférico TIM2 y cuenta
		Static_Cuentas_Tim2 = Static_Nbits*12*Static_Ciclos_Simbolo;	//Actualización del número de elementos del array Kasami_Modulad
		switch (Static_Numero_Transductor){								//Se carga una secuencia en el transductor que corresponda
			case 1:Static_Sec_Ka = Sec_Trans_2;break;
			case 2:Static_Sec_Ka = Sec_Trans_3;break;
			case 3:Static_Sec_Ka = Sec_Trans_4;break;
			case 4:Static_Sec_Ka = Sec_Trans_5;break;
			case 5:Static_Sec_Ka = Sec_Trans_1;break;
		}		
		Static_Canal_Mux++;														//Static_Canal_Mux = Número de canal del multiplexor
		Static_Numero_Transductor++;									//Transductor siguiente
		if (Static_Canal_Mux >= 6){										//Reinicio de variables para selección de canal
			Static_Numero_Transductor = 1;
			Static_Canal_Mux=1;	
		}		
		Static_Led_Conexion = GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_1);
		if(Static_Led_Conexion)															//Led rojo
			GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_RESET);			//PB14 = pin 13 del emw
		else
			GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_SET);				//PB14 = pin 13 del emw		

}
/*********************************TIM2_IRQHandler Añadida***IT para generación señal****/
void TIM4_IRQHandler (void){//PB8 (Pin10 del EMW) Se enciende y apaga led verde durante 4 segundos 3 veces por segundo
	TIM4->SR &= ~(TIM_SR_UIF);																	//Flag del manejador	
		
	GPIO_ToggleBits(GPIOB, GPIO_Pin_8);													//Toogle Pin 10 EMW

	if((Static_Contador_Led++) >= 24){													//Retardo
		TIM4->CR1 &= ~(0x1);																			//Para cuenta
		GPIOB->ODR &= 0xFFFFFEFF;																	//Led PB8 encendido pin 10 del EMW
		Static_Contador_Led=0;
		Flag_Led=0;																								//Flag para activación de TIM4
	}
}
void EXTI15_10_IRQHandler(void){
  gpio_irq();
}
void EXTI0_IRQHandler(void){
	gpio_irq(); //SDIO OOB interrupt
}
 /**************************SDIO_IRQHandler********************************************/
void SDIO_IRQHandler(void){
  /* Process All SDIO Interrupt Sources */
	NVIC_ClearPendingIRQ(TIM2_IRQn);
  sdio_irq();
}

void DMA2_Stream3_IRQHandler(void){
  NVIC_ClearPendingIRQ(TIM2_IRQn); 
	dma_irq();
}

/*void PPP_IRQHandler(void){
}*/
/**********************DMA2_Stream2_IRQHandler*****************************************/
//void DMA2_Stream2_IRQHandler(void){
//}
//void USART6_IRQHandler(void){ 
//}
//void USART1_IRQHandler(void){
//}
//void USART2_IRQHandler(void){
//}
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

//TIM2->CNT = 0;		

				//TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
				//TIM_UpdateRequestConfig(TIM2, TIM_UpdateSource_Regular);		//Interrupcion por final de cuenta
				//TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
				//TIM_UpdateRequestConfig(TIM2, TIM_UpdateSource_Regular);		//Interrupcion por final de cuenta
