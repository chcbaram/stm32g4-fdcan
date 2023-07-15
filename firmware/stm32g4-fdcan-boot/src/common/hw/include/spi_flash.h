#ifndef SPI_FLASH_H_
#define SPI_FLASH_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"


#ifdef _USE_HW_SPI_FLASH



typedef struct {
 uint32_t FlashSize;          /*!< Size of the flash */
 uint32_t EraseSectorSize;    /*!< Size of sectors for the erase operation */
 uint32_t EraseSectorsNumber; /*!< Number of sectors for the erase operation */
 uint32_t ProgPageSize;       /*!< Size of pages for the program operation */
 uint32_t ProgPagesNumber;    /*!< Number of pages for the program operation */

 uint8_t  device_id[20];
} spi_flash_info_t;


bool spiFlashInit(void);
bool spiFlashIsInit(void);
bool spiFlashReset(void);

bool spiFlashRead(uint32_t addr, uint8_t *p_data, uint32_t length);
bool spiFlashWrite(uint32_t addr, uint8_t *p_data, uint32_t length);
bool spiFlashErase(uint32_t addr, uint32_t length);
bool spiFlashEraseBlock(uint32_t block_addr);
bool spiFlashEraseSector(uint32_t sector_addr);
bool spiFlashEraseChip(void);
bool spiFlashGetStatus(void);
bool spiFlashGetInfo(spi_flash_info_t* p_info);
uint32_t spiFlashGetAddr(void);
uint32_t spiFlashGetLength(void);

#endif

#ifdef __cplusplus
}
#endif



#endif 