#ifndef CAT_ANIM3_H
#define CAT_ANIM3_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define CAT_ANIM3_WIDTH       64U
#define CAT_ANIM3_HEIGHT      64U
#define CAT_ANIM3_PAGES       8U
#define CAT_ANIM3_FRAME_BYTES (CAT_ANIM3_WIDTH * CAT_ANIM3_PAGES)
#define CAT_ANIM3_FRAME_COUNT 8U

extern const uint8_t CatAnim3_Frames[CAT_ANIM3_FRAME_COUNT][CAT_ANIM3_FRAME_BYTES];
extern const uint16_t CatAnim3_FrameDelays[CAT_ANIM3_FRAME_COUNT];

#ifdef __cplusplus
}
#endif

#endif /* CAT_ANIM3_H */
