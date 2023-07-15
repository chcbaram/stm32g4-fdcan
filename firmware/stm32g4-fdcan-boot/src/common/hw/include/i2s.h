#ifndef I2S_H_
#define I2S_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_I2S


bool i2sInit(void);
bool i2sIsInit(void);
bool i2sStart(void);
bool i2sStop(void);
bool i2sIsBusy(void);
bool i2sCfgLoad(void);
bool i2sCfgSave(void);

int8_t   i2sGetEmptyChannel(void);
uint32_t i2sAvailableForWrite(uint8_t ch);
bool     i2sWrite(uint8_t ch, int16_t *p_data, uint32_t length);
bool     i2sPlayNote(int8_t octave, int8_t note, uint16_t volume, uint32_t time_ms);
bool     i2sPlayBeep(uint32_t freq_hz, uint16_t volume, uint32_t time_ms);
uint32_t i2sGetFrameSize(void);
bool     i2sSetSampleRate(uint32_t sample_rate);
uint32_t i2sGetSampleRate(void);
int16_t  i2sGetVolume(void);
bool     i2sSetVolume(int16_t volume);


#endif

#ifdef __cplusplus
}
#endif

#endif 
