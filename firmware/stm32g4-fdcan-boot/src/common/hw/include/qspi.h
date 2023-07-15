#ifndef QSPI_H_
#define QSPI_H_

#ifdef __cplusplus
 extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_QSPI

typedef struct {
 uint32_t FlashSize;          /*!< Size of the flash */
 uint32_t EraseSectorSize;    /*!< Size of sectors for the erase operation */
 uint32_t EraseSectorsNumber; /*!< Number of sectors for the erase operation */
 uint32_t ProgPageSize;       /*!< Size of pages for the program operation */
 uint32_t ProgPagesNumber;    /*!< Number of pages for the program operation */

 uint8_t  device_id[20];
} qspi_info_t;


bool qspiInit(void);
bool qspiIsInit(void);
bool qspiReset(void);

bool qspiRead(uint32_t addr, uint8_t *p_data, uint32_t length);
bool qspiWrite(uint32_t addr, uint8_t *p_data, uint32_t length);
bool qspiErase(uint32_t addr, uint32_t length);
bool qspiEraseBlock(uint32_t block_addr);
bool qspiEraseSector(uint32_t sector_addr);
bool qspiEraseChip(void);
bool qspiGetStatus(void);
bool qspiGetInfo(qspi_info_t* p_info);
bool qspiEnableMemoryMappedMode(void);

uint32_t qspiGetAddr(void);
uint32_t qspiGetLength(void);

#endif

#ifdef __cplusplus
}
#endif

#endif 