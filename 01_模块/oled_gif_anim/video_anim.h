#ifndef VIDEO_ANIM_H
#define VIDEO_ANIM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define VIDEO_ANIM_WIDTH       64U
#define VIDEO_ANIM_HEIGHT      64U
#define VIDEO_ANIM_PAGES       8U
#define VIDEO_ANIM_FRAME_BYTES (VIDEO_ANIM_WIDTH * VIDEO_ANIM_PAGES)
#define VIDEO_ANIM_FRAME_COUNT 414U
#define VIDEO_ANIM_DELAY_MS    167U

extern const uint8_t VideoAnim_Frames[VIDEO_ANIM_FRAME_COUNT][VIDEO_ANIM_FRAME_BYTES];
extern const uint16_t VideoAnim_FrameDelays[VIDEO_ANIM_FRAME_COUNT];

#ifdef __cplusplus
}
#endif

#endif /* VIDEO_ANIM_H */
