#include <stdio.h>
#include "flash_configurations.h"


void Guarda_Config (config_param_t *pConfig){	//Almacena los valores de los par醡etros de modulaci髇 a partir de 0x08008000
	uint32_t configStartAddress, configEndAddress;
	
	configStartAddress = CONFIG_START_ADDRESS;																			//Direcci髇 de inicio
	configEndAddress =  CONFIG_END_ADDRESS;																					//Direcci髇 de final
	
	FLASH_If_Init();																																//Actualizaci髇 de flags para escribir en flash
	FLASH_If_Erase(configStartAddress, configEndAddress);														//Borrado de sector 2 del STM
	FLASH_If_Write(&configStartAddress, (u32 *)pConfig, sizeof(config_param_t));		//Escritura en sector 2 del STM
	FLASH_Lock();																																		//Bloqueo de flash	
}

void Lee_Config_Param(config_param_t *pConfig){	//Funci髇 para leer de inicio los par醡etros de la se馻l
	u32 configInFlash;
	configInFlash = CONFIG_START_ADDRESS;
	memcpy(pConfig, (void *)configInFlash, sizeof(config_param_t));									//Copia de la flash, a partir de CONFIG_START_ADDRESS,
}																																									//los valores en la estructura

//void readConfig(config_t *pConfig)
//{
//	u32 configInFlash;
//	configInFlash = PARA_START_ADDRESS;
//	memcpy(pConfig, (void *)configInFlash, sizeof(config_t));
//	if(pConfig->magicNumber != MAGIC_FLAG){
//		restoreConfig();
//		NVIC_SystemReset();
//	}
//}

//void updateConfig(config_t *pConfig)
//{
//	uint32_t paraStartAddress, paraEndAddress;
//	
//	paraStartAddress = PARA_START_ADDRESS;
//	paraEndAddress = PARA_END_ADDRESS;
//	
//	FLASH_If_Init();
//	FLASH_If_Erase(paraStartAddress, paraEndAddress);		// 擦除16K 数据保存区;	
//	FLASH_If_Write(&paraStartAddress, (u32 *)pConfig, sizeof(config_t));
//	FLASH_Lock();
//}

//void restoreConfig(void)
//{	
//	config_t	config;
//	net_para_st netPara;
//	uint32_t paraStartAddress, paraEndAddress;
//	paraStartAddress = PARA_START_ADDRESS;
//	paraEndAddress = PARA_END_ADDRESS;
//	
//	getNetPara(&netPara,Station);
//	memset(&config, 0x0, sizeof(config_t));
//	config.magicNumber = MAGIC_FLAG;
//	sprintf((char*)config.wifi_ssid , "MXCHIP_%s", netPara.mac+6);	
//	sprintf((char*)config.wifi_key , "");	
//	
//	FLASH_If_Init();
//	FLASH_If_Erase(paraStartAddress, paraEndAddress);		// 擦除16K 数据保存区;	
//	FLASH_If_Write(&paraStartAddress, (void *)&config, sizeof(config_t));
//	FLASH_Lock();
//}
