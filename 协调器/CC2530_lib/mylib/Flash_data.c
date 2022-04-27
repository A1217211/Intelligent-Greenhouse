#include "hal_defs.h"
#include "hal_flash.h"
#include "Flash_data.h"

#define OSAL_NV_WORD_SIZE 4

 
void writeWordM( uint8 pg, uint16 offset, uint8 *buf, uint16 cnt )
{
    offset = (offset >> 2) + ((uint16)pg << 9);
    HalFlashWrite(offset, buf, cnt);
 
}


void writeWord( uint8 pg, uint16 offset, uint8 *buf )
{
    offset = (offset / HAL_FLASH_WORD_SIZE) + ((uint16)pg * (HAL_FLASH_PAGE_SIZE / HAL_FLASH_WORD_SIZE));   
    HalFlashWrite(offset, buf, 1);
}


extern void writeBuf( uint8 dstPg, uint16 dstOff, uint16 len, uint8 *buf )
{
    uint8 rem = dstOff % OSAL_NV_WORD_SIZE;
    uint8 tmp[OSAL_NV_WORD_SIZE];

      if ( rem )
      {
        dstOff = (dstOff / OSAL_NV_WORD_SIZE) * OSAL_NV_WORD_SIZE;
        HalFlashRead(dstPg, dstOff, tmp, OSAL_NV_WORD_SIZE);

        while ( (rem < OSAL_NV_WORD_SIZE) && len )
        {
             tmp[rem++] = *buf++;
              len--;
        }

            writeWord( dstPg, dstOff, tmp );
            dstOff += OSAL_NV_WORD_SIZE;
    }

    rem = len % OSAL_NV_WORD_SIZE;
    len /= OSAL_NV_WORD_SIZE;

    if ( len )
    {
        writeWordM( dstPg, dstOff, buf, len );
        dstOff += OSAL_NV_WORD_SIZE * len;
        buf += OSAL_NV_WORD_SIZE * len;
    }

    if ( rem )
    {
        uint8 idx = 0;
        HalFlashRead(dstPg, dstOff, tmp, OSAL_NV_WORD_SIZE);
        while ( rem-- )
        {
          tmp[idx++] = *buf++;
        }
        writeWord( dstPg, dstOff, tmp );
        }
    }


void delay(int time )
{
    uint16 cntr1,cntr2;
    for (cntr1=0;cntr1<time;cntr1++)
    {
        for(cntr2=0;cntr2<30000;cntr2++);
    }
}