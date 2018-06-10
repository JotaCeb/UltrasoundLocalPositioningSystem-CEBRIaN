#include "stdio.h"
#include "ctype.h"
#include "platform.h"
#include "mxchipWNET.h"
#include "http_process.h"
#include "flash_configurations.h"
/* Includes añadidos-----------------------------------------------------------------*/
#include "stm32f2xx_dac.h"
#include "math.h"
#include "stdlib.h"
#include "kasami_secuencias.h"
#define APP_INFO "mxchipWNet: Web server"
/* Defines añadidos------------------------------------------------------------------*/
#define AP_NAME           "depeca"	//"VONETS_1884FC"					//"MOVISTAR_25BC"			//"VONETS_1884FC"		//
#define AP_PASSWORD       "renfe"		//"12345678"							//"9UPA9FU7UA7EC7CVJUJJ" 	//"12345678"				//

#define F_Muestreo 500e3								// Frecuencia de salida de muestra en el DAC por defecto
#define F_TIM2 60e6/F_Muestreo					//Número de cuentas para TIM2 con prescaler a 0
#define F_TIM3 1												//Número de cuentas para TIM3 con prescaler dividiendo por 60e3
#define F_TIM4 200											//Número de cuentas para TIM4
#define Pi 3.1416

int8_t Kasami_Modulada[1023*12*2];				//Array para almacenar secuenciasKasami_Modulada
config_param_t config_param;							//Estructura tipo config_param_t para establecer parámetros guardados si los hay
NVIC_InitTypeDef NVIC_Prioridades;				//Estructura de configuracion de prioridades

/* Parámetros de modulación por defecto-----------------------------------------------*/
char Sec_Trans_1=0, Sec_Trans_2=1, Sec_Trans_3=2, Sec_Trans_4=3, Sec_Trans_5=4;				//Secuencia kasami 0 a 15 para 255 0 a 31 para 1023
int  Nbits_Html								= 255;									//kasami 255
int  Frecuencia_Html 					= 41; 	      					// 41.66 kHz 
int  Ciclos_Simbolo_Html 			= 2;        						// 2ciclos/símbolo en el inicio
int  Periodo_Emision_Html		 	= 20;

/* Función de generación de secuencias------------------------------------------------*/
void Genera_Kasami_Modulada(char n_fila_kasami, int n_bits_kasami, char muestras_ciclo, char ciclos_simbolo);

/* Referencias de funciones de Demo Webserver OTA-------------------------------------*/
config_t configParas;
network_InitTypeDef_st wNetConfig;				//Estructura para configuración de inicio de la red
void print_msg(void);

/* Funciones de inicio ---------------------------------------------------------------*/
static void TIM4_Init(void);
static void TIM3_Init(void);
static void TIM2_Init(void);
static void GPIOB_Init(void);
static void DAC1_Init(void);
static void Prioridades (void);

/*Función principal--------------------------------------------------------------------*/
int main(void){
	
	mxchipInit();
  
	//readConfig(&configParas);												//Función de la librería mxchip 
	Lee_Config_Param (&config_param);								//Lectura de parámetros de modulación guardados en flash
	
#ifdef LowPowerMode
  enable_ps_mode(SLEEP_UNIT_MS, 100, 100);
#endif	
	
  memset(&wNetConfig, 0x0, sizeof(network_InitTypeDef_st));					//Reserva de bloque de memoria para configuración de red
	wNetConfig.wifi_mode = Station;																		//Para conectarse a un rúter como equipo
	strcpy(wNetConfig.wifi_ssid, AP_NAME);														//Nombre SSID
	strcpy(wNetConfig.wifi_key, AP_PASSWORD);													//Contraseña de la SSID a la que se conecta
	sprintf(wNetConfig.local_ip_addr, "192.168.75.248");							//IP "192.168.1.1"//"192.168.253.41"
	sprintf(wNetConfig.net_mask, "255.255.252.0");										//Máscara de red "255.255.255.0"
	sprintf(wNetConfig.gateway_ip_addr, "192.168.75.254");						//IP del rúter "192.168.1.1"//"192.168.253.254"
	sprintf(wNetConfig.address_pool_start, "192.168.75.100");				//"192.168.1.100"//"192.168.253.100"
	sprintf(wNetConfig.address_pool_end, "192.168.75.255");					//"192.168.1.200"//"192.168.253.200"
	wNetConfig.dhcpMode = DHCP_Disable;																//Dinamic Host...
	wNetConfig.wifi_retry_interval = 1000;														//Intervalo de comprobación de la conexión (ms)
	StartNetwork(&wNetConfig);																				//Configuración de inicio de la red
	
  set_tcp_keepalive(3, 60);

	if (config_param.ciclos_simbolo != 0xFFFFFFFF){										//Si hay valores en memoria se actualizan las vbles globales
		Ciclos_Simbolo_Html = config_param.ciclos_simbolo;	
		Frecuencia_Html = config_param.frecuencia;
		Nbits_Html = config_param.numero_bits;
		Periodo_Emision_Html = config_param.periodo_emision;
		Sec_Trans_1 = config_param.secuencia_transductor1;
		Sec_Trans_2 = config_param.secuencia_transductor2;
		Sec_Trans_3= config_param.secuencia_transductor3;
		Sec_Trans_4 = config_param.secuencia_transductor4;
		Sec_Trans_5 = config_param.secuencia_transductor5;
	}		
	
	http_init();																					//Autenticacion de acceso a credenciales
		
	//Funciones de inicialización de Periféricos
	DAC1_Init();
	GPIOB_Init();
	TIM2_Init();
	TIM3_Init();
	TIM4_Init();
	
	SystemCoreClockUpdate();															//Inicialización de la configuración elegida para PLL
	
	RCC_PCLK1Config (RCC_HCLK_Div4);											//Se divide por 4 el bus APB1
	
	Prioridades();																				//Configuración de prioridades

  while(1){
    mxchipTick();																				//Funciones de la librería mxchip
		http_tick();
	}
}
/*Funciones de inicialización----------------------------------------------------------*/
void GPIOB_Init(void){
	
	GPIO_InitTypeDef GPIOB_Struct;
	GPIOB_Struct.GPIO_Mode = GPIO_Mode_OUT;													//Modo salida
	GPIOB_Struct.GPIO_OType = GPIO_OType_PP;												//Tipo de salida
	GPIOB_Struct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_14;		//Mux: 5,6,7; Leds rojo y verde
	GPIOB_Struct.GPIO_PuPd = GPIO_PuPd_NOPULL;											//Ni pull up ni pull down no pull
	GPIO_Init(GPIOB, &GPIOB_Struct);																//Configuración de inicio para la estructura elegida

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);						//Habilitar muestreo de la parte del bus AHB1 para GPIOB
	
	GPIOB->ODR &= 0xFFFFFEFF;																				//Led PB8 encendido pin 10
}//PB5 = pin9 del emw3162, PB6 = pin1 del emw3162, PB7 = pin2 del emw3162, PB8 = pin10 del emw, PB14 = pin 13 del emw
void DAC1_Init(void){
	//Estructuras para Puerto A, para pin PA4 = pin6 del EMW3162 y estructura para configuración del DAC
	GPIO_InitTypeDef GPIOA_Struct;														//Estructura puerto A
	DAC_InitTypeDef DAC1_Struct;															//Estructura para DAC1
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);				//Para el DAC habilitar su parte del bus
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);			//Para el PA4 habilitar su parte del bus
	
	GPIOA_Struct.GPIO_Mode = GPIO_Mode_AN;										//Modo analógico para PA4
	GPIOA_Struct.GPIO_OType = GPIO_OType_PP;									//Tipo de salida
	GPIOA_Struct.GPIO_Pin = GPIO_Pin_4;												//Pin para DAC, puerto A
	GPIOA_Struct.GPIO_PuPd = GPIO_PuPd_NOPULL;								//Ni pull up ni pull down, no pull
	GPIO_Init(GPIOA, &GPIOA_Struct);													//Configuración de inicio para la estructura elegida
	
	DAC1_Struct.DAC_Trigger = DAC_Trigger_None;								//Sin trigger. La salida de muestra se hace a través de DAC_SetChannel1Data
	DAC1_Struct.DAC_WaveGeneration = DAC_WaveGeneration_None;	//Sin generaciones específicas de señal
	DAC1_Struct.DAC_OutputBuffer = DAC_OutputBuffer_Enable;		//Mejora de la impedancia de salida del puerto
	
	DAC_Init(DAC_Channel_1, &DAC1_Struct);										//Configuración de inicio para la estructura elegida
	DAC_Cmd(DAC_Channel_1, ENABLE);														//Habilitación de canal 1 del DAC	
}//PA4 = pin6 del EMW3162*********************************************************
void TIM2_Init(void){	
	TIM_TimeBaseInitTypeDef Tim2_TimeBaseInit;									//Estructura base de configuración del TIM2
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);				//Habilita reloj de bus APB1 para tim2	
	
	Tim2_TimeBaseInit.TIM_Prescaler = 0x0000;										//Prescaler
	Tim2_TimeBaseInit.TIM_CounterMode = TIM_CounterMode_Down;		//Cuenta descendente
	Tim2_TimeBaseInit.TIM_Period = F_TIM2-1; 										//Numero de cuentas 125
	Tim2_TimeBaseInit.TIM_ClockDivision = TIM_CKD_DIV1;					//Division por 1
	Tim2_TimeBaseInit.TIM_RepetitionCounter = 0x0000;						//Realizar la interrupción 1 vez
	TIM_TimeBaseInit(TIM2, &Tim2_TimeBaseInit);									//Función para llevar a cabo la configuración
	
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);									//Habilitación de la fuente de interrupción
	TIM_UpdateRequestConfig(TIM2, TIM_UpdateSource_Regular);		//Interrupcion por final de cuenta
	TIM_Cmd(TIM2, ENABLE);																			//Habilitación del periférico TIM2
}
void TIM3_Init(void){
	
	TIM_TimeBaseInitTypeDef Tim3_TimeBaseInit;									//Estructura base de configuración del TIM3
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);				//Habilita reloj de bus APB1 para tim3	
	
	Tim3_TimeBaseInit.TIM_Prescaler = 0xEA60;										//Prescaler divide por 60000
	Tim3_TimeBaseInit.TIM_CounterMode = TIM_CounterMode_Down;		//Cuenta descendente
	Tim3_TimeBaseInit.TIM_Period = Periodo_Emision_Html-1;			//Numero de cuentas 1
	Tim3_TimeBaseInit.TIM_ClockDivision = TIM_CKD_DIV1;					//Division por 1
	Tim3_TimeBaseInit.TIM_RepetitionCounter = 0x0000;						//Realizar la interrupción 1 vez
	TIM_TimeBaseInit(TIM3, &Tim3_TimeBaseInit);									//Función para llevar a cabo la configuración
	
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);									//Habilitación de la fuente de interrupción
	TIM_UpdateRequestConfig(TIM3, TIM_UpdateSource_Regular);		//Interrupción tipica por final de cuenta
	TIM_Cmd(TIM3, ENABLE);																			//Habilitación del periférico TIM3
}
void TIM4_Init(void){
	
	TIM_TimeBaseInitTypeDef Tim4_TimeBaseInit;									//Estructura base de configuración del TIM4

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);				//Habilita reloj de bus APB1 para tim4	
	
	Tim4_TimeBaseInit.TIM_Prescaler = 0xC350;										//Prescaler dividido por 50000 => Luego interrup tim4 a 1/(1200Hz)
	Tim4_TimeBaseInit.TIM_CounterMode = TIM_CounterMode_Down;		//Cuenta descendente
	Tim4_TimeBaseInit.TIM_Period = F_TIM4-1; 										//Numero de cuentas 
	Tim4_TimeBaseInit.TIM_ClockDivision = TIM_CKD_DIV1;					//Division por 1
	Tim4_TimeBaseInit.TIM_RepetitionCounter = 0x0000;						//Realizar la interrupción 1 vez
	TIM_TimeBaseInit(TIM4, &Tim4_TimeBaseInit);									//Función para llevar a cabo la configuración
	
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);									//Habilitación de la fuente de interrupción
	TIM_UpdateRequestConfig(TIM4, TIM_UpdateSource_Regular);		//Interrupcion tipica por final de cuenta	
	TIM_Cmd(TIM4, ENABLE);																			//Habilitación del periférico TIM4
}//PB8(Pin10 del EMW) se enciende 4 segundos nada más iniciar y cuando se interactúa a través de la web

/*Establecimiento de prioridades-------------------------------------------------------*/
void Prioridades(void){
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);							//0 niveles de subprioridad
	
	NVIC_Prioridades.NVIC_IRQChannel = 28;											//TIM2
	NVIC_Prioridades.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Prioridades.NVIC_IRQChannelPreemptionPriority = 4;
	NVIC_Prioridades.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_Prioridades);
	
	NVIC_Prioridades.NVIC_IRQChannel = 29;											//TIM3
	NVIC_Prioridades.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Prioridades.NVIC_IRQChannelPreemptionPriority = 0;			//Más prioritaria
	NVIC_Prioridades.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_Prioridades);
	
	NVIC_Prioridades.NVIC_IRQChannel = 30;											//TIM4
	NVIC_Prioridades.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Prioridades.NVIC_IRQChannelPreemptionPriority = 5;
	NVIC_Prioridades.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_Prioridades);
}
/*Función de generación de la modulación-----------------------------------------------*/
void Genera_Kasami_Modulada(char n_fila_kasami, int n_bits_kasami, char muestras_ciclo, char ciclos_simbolo){
	int j,i,indice=0;	
	
	int8_t Simbolo_A[12*4];										// Maximo	numero de elementos que puede tomar Simbolo
	int8_t Simbolo_B[12*4];										// Maximo	numero de elementos que puede tomar Simbolo	
	
	for(i=0;i<muestras_ciclo*ciclos_simbolo;i++)Simbolo_A[i]=(127+127*sin(2*Pi*i/muestras_ciclo));		//Simbolo A 
	for(j=0;j<muestras_ciclo*ciclos_simbolo;j++)Simbolo_B[j]=(127-127*sin(2*Pi*j/muestras_ciclo));		//Simbolo B
	
	if(n_bits_kasami==255){
		for(i=0;i<n_bits_kasami;i++){
			if(Kasami255[n_fila_kasami][i]==+1)
				for(j=0;j<muestras_ciclo*ciclos_simbolo;j++) 
			Kasami_Modulada[indice++]=Simbolo_A[j];	
			else
				for(j=0;j<muestras_ciclo*ciclos_simbolo;j++)
			Kasami_Modulada[indice++]=Simbolo_B[j];			
		}
	}
	else if (n_bits_kasami==1023){
		for(i=0;i<n_bits_kasami;i++){
			if(Kasami1023[n_fila_kasami][i]==+1)	
				for(j=0;j<muestras_ciclo*ciclos_simbolo;j++) 
			Kasami_Modulada[indice++]=Simbolo_A[j];
			else	  
				for(j=0;j<muestras_ciclo*ciclos_simbolo;j++) 
			Kasami_Modulada[indice++]=Simbolo_B[j];															
		}
	}
}

	//Systick_Init();													//Esta función hace que la velocidad de respuesta del servidor sea de 40s
	//SysTick->LOAD  = (uint32_t)(F_Systick - 1UL);                         /* set reload register */
  //SysTick->VAL   = 0UL;                                             		/* Load the SysTick Counter Value */
//sprintf(wNetConfig.dnsServer_ip_addr, "SB1");					//"192.168.1.1"
//void RptConfigmodeRslt(network_InitTypeDef_st *nwkpara){
//}
//void userWatchDog(void){
//}
//void ApListCallback(ScanResult *pApList){
//}
/*Definición de funciones de Demo Webserver OTA---------------------------------------*/
//void system_version(char *str, int len){
//  snprintf( str, len, "%s", APP_INFO);
//}    

//void WifiStatusHandler(int event){
//  switch (event) {
//    case MXCHIP_WIFI_UP:
//      printf("Wi-Fi up \r\n");
//      break;
//    case MXCHIP_WIFI_DOWN:
//      printf("Wi-Fi down \r\n");
//      break;
//    default:
//      break;
//  }
//  return;
//}

//void NetCallback(net_para_st *pnet){
//	printf("IP address: %s \r\n", pnet->ip);
//	printf("NetMask address: %s \r\n", pnet->mask);
//	printf("Gateway address: %s \r\n", pnet->gate);
//	printf("DNS server address: %s \r\n", pnet->dns);
//  printf("MAC address: %s \r\n", pnet->mac);
//}
//COMPROBAR LAS FUNCIONES ANTERIORES DE LA DEMO OTA ***************************
//printf("Establish soft ST: %s.....\r\n", wNetConfig.wifi_ssid);		//("Establish soft AP: %s.....\r\n", wNetConfig.wifi_ssid) COMPROBAR****
//DAC1_Struct.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;	//COMPROBA***************
//DAC_SoftwareTriggerCmd(DAC_Channel_1, ENABLE);						//COMPROBAR***************************


	//UART_Init();																	//COMPROBAR********************
  //printf("\r\n%s\r\nmxchipWNet library version: %s\r\n", APP_INFO, system_lib_version());
