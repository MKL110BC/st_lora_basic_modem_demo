#include "driver_flash.h"
#include "main.h"
#include "system_time.h"

/*
 * ����ȫ�ֱ���
*/
static FLASH_EraseInitTypeDef EraseInitStruct;


void EraseFlashPage(uint32_t addr, uint32_t page_num)
{
	uint32_t address = addr+FLASH_BASE_ADDR;
	uint32_t nPageError = 0;
	EraseInitStruct.TypeErase	= FLASH_TYPEERASE_PAGES;	// ҳ����
	EraseInitStruct.PageAddress = address; 					// ��������ʼ��ַ
	EraseInitStruct.NbPages = page_num;

	HAL_FLASH_Unlock(); 						// ����
	HAL_FLASHEx_Erase(&EraseInitStruct, &nPageError); // ����Flash
	HAL_FLASH_Lock();							// ����

	APP_INFO("EraseFlashPage:addr=%x,page_num=%d \n",address,page_num);
}



static uint32_t FlashBuff[FLASH_USER_SIZE/4] = {0};
void WriteFlash(uint32_t startAddr,const uint8_t *pdata, uint32_t nBytes)
{
	uint32_t i = 0;
	uint32_t address = startAddr+FLASH_BASE_ADDR;
	uint32_t WriteSize = nBytes/4 + 1;

	APP_INFO("FLASH Write:Addr=%x,len=%d ",address,nBytes);
	APP_INFO_DUMP_BUF(":", pdata, nBytes);

	HAL_FLASH_Unlock(); 						// ����

	memset(FlashBuff,0,sizeof(FlashBuff));
	memcpy(FlashBuff,pdata,nBytes);

	for(i=0; i<WriteSize;i++)					   // ��ʼд����
	{
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, FlashBuff[i]) == HAL_OK)
		{
			address = address + 4;
		}
	}
	HAL_FLASH_Lock();							// ����

}


void ReadFlash(uint32_t startAddr, uint8_t *pdata, uint32_t nBytes)
{
	uint32_t address = startAddr+FLASH_BASE_ADDR;	
#if 0
		uint16_t i = 0;
		for(i=0; i<length; i++) 			// ��ȡlength������
		{
			pdata[i]=*(uint32_t*)startAddr; // ��ָ����ַ���ݱ��浽pata
			startAddr = startAddr + 4;		// ��ַ��4,����һ��32λ����λ��
		}
#else
		memcpy((uint32_t*)pdata, (uint32_t*)address, nBytes);
#endif

	APP_INFO("FLASH Read:Addr=%x,len=%d ",address,nBytes);
	APP_INFO_DUMP_BUF(":", pdata, nBytes);

}


void FlashDemo()
{
	static uint8_t state = 0;
	static uint32_t timer = 0;
	uint8_t writebuf[10] = {0,1,2,3,4,5,6,7,8,9};
	uint8_t readbuf[10];

	uint32_t addr = 256*500;
	switch(state)
	{
		case 0:
			state = 1;
			timer = get_sys_time();
			EraseFlashPage(addr,1);
			WriteFlash(addr,writebuf,10);
			break;
		case 1:
			if(timepassed(timer, 5*ONE_SEC))
			{
				memset(readbuf,0xFF,sizeof(readbuf));
				ReadFlash(addr,readbuf,10);
				state = 2;
				APP_INFO_DUMP_BUF("read:",readbuf,10);
			}
			break;
		case 2:
			EraseFlashPage(addr,1);
			memset(readbuf,0xFF,sizeof(readbuf));
			ReadFlash(addr,readbuf,10);
			APP_INFO_DUMP_BUF("read:",readbuf,10);
			state = 3;
			break;
	}
}

