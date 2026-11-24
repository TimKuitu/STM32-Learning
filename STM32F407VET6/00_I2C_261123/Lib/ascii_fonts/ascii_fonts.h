#ifndef ASCII_FONTS_H
#define ASCII_FONTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

/**
 * @brief 可配置启用的字号（不需要的字号可设为 0 以节省 Flash）
 */
#ifndef ASCII_FONT_ENABLE_6X8
#define ASCII_FONT_ENABLE_6X8 1
#endif

#ifndef ASCII_FONT_ENABLE_8X16
#define ASCII_FONT_ENABLE_8X16 1
#endif

/**
 * @brief ASCII 字库描述符结构体（列行式、低位在上 LSB-Top，适配 SSD1306/SH1106 页模式）
 */
typedef struct {
    uint8_t width;        /* 单字符宽度（像素 / 列数） */
    uint8_t height;       /* 单字符高度（像素，8 的倍数） */
    uint8_t pages;        /* 占用页数（height / 8） */
    uint8_t bytes_per_ch; /* 单字符总字节数（width * pages） */
    char first_char;      /* 起始 ASCII 字符（' ' = 0x20） */
    char last_char;       /* 结束 ASCII 字符（'~' = 0x7E） */
    const uint8_t* data;  /* 连续字模数组 */
} AsciiFont_t;

#if ASCII_FONT_ENABLE_6X8
extern const AsciiFont_t ASCII_Font6x8;
#endif

#if ASCII_FONT_ENABLE_8X16
extern const AsciiFont_t ASCII_Font8x16;
#endif

/**
 * @brief 获取指定字符在字库中的点阵首地址
 * @param font 字库指针（如 &ASCII_Font8x16、&ASCII_Font6x8）
 * @param ch   ASCII 字符（超出 0x20~0x7E 范围将安全回退到空格 ' '）
 * @return const uint8_t* 指向该字符点阵数据的指针；若 font 无效则返回 NULL
 */
const uint8_t* AsciiFont_GetCharData(const AsciiFont_t* font, char ch);

#ifdef __cplusplus
}
#endif

#endif /* ASCII_FONTS_H */
