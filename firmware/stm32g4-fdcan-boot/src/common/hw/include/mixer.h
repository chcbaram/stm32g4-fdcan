/*
 * mixer.h
 *
 *  Created on: 2020. 8. 8.
 *      Author: Baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_MIXER_H_
#define SRC_COMMON_HW_INCLUDE_MIXER_H_


#ifdef __cplusplus
extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_MIXER


#define MIXER_MAX_CH        HW_MIXER_MAX_CH
#define MIXER_MAX_BUF_LEN   HW_MIXER_MAX_BUF_LEN



typedef struct
{
  uint32_t length;
  uint32_t in;
  uint32_t out;
  int16_t  buf[MIXER_MAX_BUF_LEN];
} mixer_buf_t;

typedef struct
{
  int32_t volume;
  mixer_buf_t buf[MIXER_MAX_CH];
} mixer_t;


bool mixerInit(mixer_t *p_mixer);
bool mixerWrite(mixer_t *p_mixer, uint8_t ch, int16_t *p_data, uint32_t length);
bool mixerRead(mixer_t *p_mixer, int16_t *p_data, uint32_t length);
bool mixerIsEmpty(mixer_t *p_mixer, uint8_t ch);
bool mixerSetVolume(mixer_t *p_mixer, int32_t volume);
int32_t mixerGetVolume(mixer_t *p_mixer);

uint32_t mixerAvailable(mixer_t *p_mixer);
uint32_t mixerAvailableForWrite(mixer_t *p_mixer, uint8_t ch);
int8_t   mixerGetEmptyChannel(mixer_t *p_mixer);
int8_t   mixerGetValidChannel(mixer_t *p_mixer, uint32_t length);

int16_t mixerSamples(int16_t a, int16_t b);


#ifdef __cplusplus
}
#endif


#endif



#endif /* SRC_COMMON_HW_INCLUDE_MIXER_H_ */
