#ifndef CAT_ANIM2_H
#define CAT_ANIM2_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define CAT_ANIM2_WIDTH       64U
#define CAT_ANIM2_HEIGHT      64U
#define CAT_ANIM2_PAGES       8U
#define CAT_ANIM2_FRAME_BYTES (CAT_ANIM2_WIDTH * CAT_ANIM2_PAGES)
#define CAT_ANIM2_FRAME_COUNT 27U

extern const uint8_t CatAnim2_Frames[CAT_ANIM2_FRAME_COUNT][CAT_ANIM2_FRAME_BYTES];
extern const uint16_t CatAnim2_FrameDelays[CAT_ANIM2_FRAME_COUNT];

#ifdef __cplusplus
}
#endif

#endif /* CAT_ANIM2_H */
