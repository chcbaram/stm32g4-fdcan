/*
 * resize.h
 *
 *  Created on: 2020. 2. 1.
 *      Author: Baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_RESIZE_H_
#define SRC_COMMON_HW_INCLUDE_RESIZE_H_


#ifdef __cplusplus
extern "C" {
#endif


#include "def.h"

typedef struct
{
  int32_t  w;
  int32_t  h;
  int32_t  x;
  int32_t  y;
  int32_t  stride;
  uint16_t *p_data;
} resize_image_t;


void resizeImage(resize_image_t *src, resize_image_t *dest);
void resizeImageNearest(resize_image_t *src, resize_image_t *dest);
void resizeImageFast(resize_image_t *src, resize_image_t *dest);
void resizeImageFastOffset(resize_image_t *src, resize_image_t *dest);
void resizeImageFastPxp(resize_image_t *src, resize_image_t *dest);

void resizeImageFastGray(resize_image_t *src, resize_image_t *dest);


#ifdef __cplusplus
}
#endif



#endif /* SRC_COMMON_HW_INCLUDE_RESIZE_H_ */
