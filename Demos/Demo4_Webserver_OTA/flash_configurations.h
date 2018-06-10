#ifndef __FLASH_CONFIGURATIONS_H
#define __FLASH_CONFIGURATIONS_H

#include "flash_if.h"
#include "mxchipWNET.h"

#define MAGIC_FLAG 0x12345677

#pragma pack(1)
typedef struct _config_ {
	/*OTA options*/
	boot_table_t bootTable;
  /*EABox configuration*/
	u32 magicNumber;
	char wifi_ssid[32];				
	char wifi_key[32];
}config_t;

typedef struct _config_param{					//Estructura para almacenamiento de parámetros en memoria flash
	int numero_bits;
	int frecuencia;
	int ciclos_simbolo;
	int periodo_emision;
	char secuencia_transductor1;
	char secuencia_transductor2;
	char secuencia_transductor3;
	char secuencia_transductor4;
	char secuencia_transductor5;
}config_param_t;

void readConfig(config_t *pConfig);
void restoreConfig(void);
void updateConfig(config_t *pConfig);
void Guarda_Config (config_param_t *pConfig);			//Función que almacena los valores de los parámetros de la señal en el sector 2 del STM
void Lee_Config_Param (config_param_t *pConfig); 	//Función para leer de inicio los parámetros de la señal

#endif
