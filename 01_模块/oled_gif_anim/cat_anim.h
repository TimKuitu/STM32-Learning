#ifndef CAT_ANIM_H
#define CAT_ANIM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define CAT_ANIM_WIDTH       64U
#define CAT_ANIM_HEIGHT      64U
#define CAT_ANIM_PAGES       8U
#define CAT_ANIM_FRAME_BYTES (CAT_ANIM_WIDTH * CAT_ANIM_PAGES)
#define CAT_ANIM_FRAME_COUNT 28U

extern const uint8_t CatAnim_Frames[CAT_ANIM_FRAME_COUNT][CAT_ANIM_FRAME_BYTES];
extern const uint16_t CatAnim_FrameDelays[CAT_ANIM_FRAME_COUNT];

#ifdef __cplusplus
}
#endif

#endif /* CAT_ANIM_H */
