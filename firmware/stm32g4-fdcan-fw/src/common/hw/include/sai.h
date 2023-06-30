#ifndef SAI_H_
#define SAI_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_SAI


bool saiInit(void);
bool saiIsInit(void);
bool saiStart(void);
bool saiStop(void);
bool saiIsBusy(void);
bool saiCfgLoad(void);
bool saiCfgSave(void);

int8_t   saiGetEmptyChannel(void);
uint32_t saiAvailableForWrite(uint8_t ch);
bool     saiWrite(uint8_t ch, int16_t *p_data, uint32_t length);
bool     saiPlayNote(int8_t octave, int8_t note, uint16_t volume, uint32_t time_ms);
bool     saiPlayBeep(uint32_t freq_hz, uint16_t volume, uint32_t time_ms);
uint32_t saiGetFrameSize(void);
bool     saiSetSampleRate(uint32_t sample_rate);
uint32_t saiGetSampleRate(void);
int16_t  saiGetVolume(void);
bool     saiSetVolume(int16_t volume);


#endif

#ifdef __cplusplus
}
#endif

#endif 
