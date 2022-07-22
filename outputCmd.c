/******************** (C) COPYRIGHT 2015 INCUBECN *****************************
* File Name          : outputCmd.c
* Author             : Tiko Zhong
* Date First Issued  : 12/01/2015
* Description        : This file provides a set of functions needed to manage the
*                      
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "outputCmd.h"
#include "string.h"
#include "stdarg.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define FUN_LEN 64
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const char OUTPUT_HELP[] = {
	"output commands:"
	"\n x.output.help()"
	"\n x.output.readPin()/(indx)/(indx0,indx1)"
	"\n x.output.writePin(status)/(indx, b)/(indx0,bit0,indx1,bit1)"
	"\n x.output.togglePin()/(indx)/(indx0,indx1)"
	"\n"
};

/* Private function prototypes -----------------------------------------------*/

/*******************************************************************************
* Function Name  : outputCmd
* Description    : gpio function commands
* Input          : - huart : using this uart to print
									: - cmdStr : the command string
* Output         : None
* Return         : None 

output.writepin(%pin, %state)
output.toggle(%pin)
*******************************************************************************/
u8 outputCmd(OUTPUT_DEV_T* pDev, const char* CMD, u8 brdAddr, char* buff, u16 len){
	u32 i,j,ii,jj;
	char strTmp[100];
	const char* line;
	OUTPUT_RSRC_T *pRsrc = &pDev->rsrc;
		
	if(strncmp(CMD, pRsrc->name, strlen(pRsrc->name)) != 0)	return 0;
	line = &CMD[strlen(pRsrc->name)];
	
	//output.help()
	if(strncmp(line, ".help", strlen(".help")) == 0)	
		strFormat(buff, len, "%s+ok@%d.%s.help()\r\n", OUTPUT_HELP,brdAddr,pRsrc->name);
	//output.readPin()/(indx)/(indx0,indx1)
	else if(sscanf(line, ".readpin %d %d ", &i, &j)==2){
		if(i>=pRsrc->gpioLen || j>=pRsrc->gpioLen){	
			strFormat(buff, len, "+err@%d.%s.readpin(\"overflow\")\r\n", brdAddr, pRsrc->name);
			return 1;	
		}
		ii = 0;	jj = 0;
		if(pRsrc->status & BIT(i))	ii = 1;
		if(pRsrc->status & BIT(j))	jj = 1;
		strFormat(buff, len, "+ok@%d.%s.readpin(%d,%d,%d,%d)\r\n", brdAddr, pRsrc->name, i, ii, j, jj);
	}
	else if(sscanf(line, ".readpin %d ", &i)==1){
		if(i>=pRsrc->gpioLen){	strFormat(buff, len, "+err@%d.%s.readpin(\"overflow\")\r\n", brdAddr, pRsrc->name);	return 1;	}
		ii = 0;	
		if(pRsrc->status & BIT(i))	ii = 1;
		strFormat(buff, len, "+ok@%d.%s.readpin(%d,%d)\r\n", brdAddr, pRsrc->name, i, ii);
	}
	else if(strncmp(line, ".readpin  ", strlen(".readpin  ")) == 0){
		strFormat(buff, len, "+ok@%d.%s.readpin(0x%x)\r\n", brdAddr, pRsrc->name, pRsrc->status);
	}
	
	//output.writePin(status)/(indx, b)/(indx0,bit0,indx1,bit1)
	else if(sscanf(line, ".writepin %d %d %d %d ", &i, &j, &ii, &jj)==4){
		if(i>=pRsrc->gpioLen || ii>=pRsrc->gpioLen){	
			strFormat(buff, len, "+err@%d.%s.writepin(\"overflow\")\r\n", brdAddr, pRsrc->name);	
			return 1;	
		}
		if(j)		pDev->WritePin(pRsrc, i, PIN_SET);
		else pDev->WritePin(pRsrc, i, PIN_RESET);
		if(jj)	pDev->WritePin(pRsrc, ii, PIN_SET);
		else pDev->WritePin(pRsrc, ii, PIN_RESET);
		strFormat(buff, len, "+ok@%d.%s.writepin(%d,%d,%d,%d)\r\n", brdAddr, pRsrc->name, i, j, ii, jj);
	}
	else if(sscanf(line, ".writepin %d %d ", &i, &j)==2){
		if(i>=pRsrc->gpioLen){	
			strFormat(buff, len, "+err@%d.%s.writepin(\"overflow\")\r\n", brdAddr, pRsrc->name);
			return 1;	
		}
		if(j)		pDev->WritePin(pRsrc, i, PIN_SET);
		else pDev->WritePin(pRsrc, i, PIN_RESET);
		strFormat(buff, len, "+ok@%d.%s.writepin(%d,%d)\r\n", brdAddr, pRsrc->name, i, j);
	}
	else if(sscanf(line, ".writepin 0x%x ", &i)==1){
		pDev->WritePinHEX(pRsrc, i);
		strFormat(buff, len, "+ok@%d.%s.writepin(0x%x)\r\n", brdAddr, pRsrc->name, i);
	}	
	else if(sscanf(line, ".writepin %d ", &i)==1){
		pDev->WritePinHEX(pRsrc, i);
		strFormat(buff, len, "+ok@%d.%s.writepin(%d)\r\n", brdAddr, pRsrc->name, i);
	}

	//output.togglePin()/(indx)/(indx0,indx1)
	else if(sscanf(line, ".togglepin %d %d ", &i, &j)==2){
		if(i>=pRsrc->gpioLen || j>=pRsrc->gpioLen){	
			strFormat(buff, len, "+err@%d.%s.readpin(\"overflow\")\r\n", brdAddr, pRsrc->name);	
			return 1;	
		}
		pDev->TogglePin(pRsrc, i);
		pDev->TogglePin(pRsrc, j);
		strFormat(buff, len, "+ok@%d.%s.togglepin(%d,%d)\r\n", brdAddr, pRsrc->name, i,j);
	}
	else if(sscanf(line, ".togglepin %d ", &i)==1){
		if(i>=pRsrc->gpioLen){	
			strFormat(buff, len, "+err@%d.%s.togglepin(\"overflow\")\r\n", brdAddr, pRsrc->name);	
			return 1;	
		}
		pDev->TogglePin(pRsrc, i);
		strFormat(buff, len, "+ok@%d.%s.togglepin(%d)\r\n", brdAddr, pRsrc->name, i);
	}
	else if(strncmp(line, ".togglepin  ", strlen(".togglepin  ")) == 0){
		for(i=0;i<pRsrc->gpioLen;i++)	pDev->TogglePin(pRsrc, i);
		strFormat(buff, len, "+ok@%d.%s.togglepin()\r\n", brdAddr, pRsrc->name);
	}
	else{
		memset(strTmp,0,100);
		strcpy(strTmp,CMD);
		strFormat(buff, len, "+unknown@%d.%s", brdAddr, strTmp);
	}
	
	return 1;
}

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
