# ascii_fonts 独立可移植 ASCII 字库

零外部依赖（仅需 `<stdint.h>` 和 `<stddef.h>`）的轻量级 ASCII 点阵字库，专为 OLED（SSD1306 / SH1106 等）页地址模式设计。

## 1. 跨项目复制方法

只需将本目录下的 **2 个核心文件** 复制到任意 C/C++ 工程中并加入编译列表与头文件搜索路径即可：

- `ascii_fonts.h`
- `ascii_fonts.c`

## 2. 字模格式与内置字号

- **字符范围**：完整可打印 ASCII 字符 `' '` (`0x20`) ~ `'~'` (`0x7E`)，共 95 个字符（超出范围自动回退到空格 `' '`）。
- **取模方式**：逐列式（列行式），低位在上（LSB-Top），单字节 bit0 为最上方像素、bit7 为最下方像素。
- **内置字号**：
  - `ASCII_Font6x8`：6×8 像素（每字符 6 字节，占 1 个 Page，128×64 屏可显示 8 行 × 21 列）
  - `ASCII_Font8x16`：8×16 像素（每字符 16 字节，前 8 字节为上半 Page，后 8 字节为下半 Page，128×64 屏可显示 4 行 × 16 列）

## 3. 裁剪配置

可在包含 `ascii_fonts.h` 前（或直接在编译器宏定义中）关闭不需要的字号以节省 Flash：

```c
#define ASCII_FONT_ENABLE_6X8   0
#define ASCII_FONT_ENABLE_8X16  1
```

## 4. 调用示例

```c
#include "ascii_fonts.h"

const uint8_t* glyph = AsciiFont_GetCharData(&ASCII_Font8x16, 'A');
// 按 font->pages 与 font->width 写入 OLED 显存即可
```
