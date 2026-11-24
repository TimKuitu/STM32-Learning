/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "gpio.h"
#include "stm32f407xx.h"
#include "stm32f4xx_hal_gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ascii_fonts.h"
#include "cat_anim.h"
#include "cat_anim2.h"
#include "cat_anim3.h"
#include "video_anim.h"
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
#define OLED_ADDR 0x78
#define OLED_WIDTH 128
#define OLED_PAGES 8

void OLED_WriteCmd(uint8_t cmd) {
    HAL_I2C_Mem_Write(&hi2c1, OLED_ADDR, 0x00, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 100);
}

// 设置光标：page (0~7), col (0~127)
void OLED_SetCursor(uint8_t page, uint8_t col) {
    OLED_WriteCmd(0xB0 | (page & 0x07));
    OLED_WriteCmd(0x00 | (col & 0x0F));
    OLED_WriteCmd(0x10 | ((col & 0xF0) >> 4));
}

// 清屏（必须清屏，否则屏幕上电会显示随机杂点）
void OLED_Clear(void) {
    uint8_t zero_buf[OLED_WIDTH] = {0};
    for (uint8_t page = 0; page < OLED_PAGES; page++) {
        OLED_SetCursor(page, 0);
        HAL_I2C_Mem_Write(&hi2c1, OLED_ADDR, 0x40, I2C_MEMADD_SIZE_8BIT, zero_buf, OLED_WIDTH, 100);
    }
}

void OLED_Init(void) {
    HAL_Delay(100);
    OLED_WriteCmd(0xAE);  // 关显示
    OLED_WriteCmd(0x8D);  // 电荷泵设置
    OLED_WriteCmd(0x14);  // 开启内部电荷泵
    OLED_WriteCmd(0x20);  // 内存地址模式
    OLED_WriteCmd(0x02);  // 页地址模式
    OLED_WriteCmd(0xA1);  // 左右正常方向（若镜像请改为0xA0）
    OLED_WriteCmd(0xC8);  // 上下正常方向（若倒置请改为0xC0）
    OLED_WriteCmd(0x81);  // 对比度
    OLED_WriteCmd(0xCF);
    OLED_WriteCmd(0xAF);  // 开显示
    OLED_Clear();         // 清空显存
}

// 在指定页(page)和列(col)显示一个指定字号的 ASCII 字符
void OLED_DrawChar(uint8_t page, uint8_t col, char ch, const AsciiFont_t* font) {
    if (font == NULL || col + font->width > OLED_WIDTH || page + font->pages > OLED_PAGES) {
        return;
    }

    const uint8_t* glyph = AsciiFont_GetCharData(font, ch);
    if (glyph == NULL) {
        return;
    }

    for (uint8_t p = 0; p < font->pages; p++) {
        OLED_SetCursor(page + p, col);
        HAL_I2C_Mem_Write(&hi2c1, OLED_ADDR, 0x40, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&glyph[p * font->width], font->width, 100);
    }
}

// 在指定页(page)和列(col)显示 ASCII 字符串（支持 '\n' 与自动换行）
void OLED_DrawString(uint8_t page, uint8_t col, const char* str, const AsciiFont_t* font) {
    if (str == NULL || font == NULL) {
        return;
    }

    uint8_t cur_page = page;
    uint8_t cur_col = col;

    while (*str != '\0') {
        if (*str == '\r') {
            str++;
            continue;
        }
        if (*str == '\n') {
            cur_col = 0;
            cur_page += font->pages;
            str++;
            continue;
        }

        if (cur_col + font->width > OLED_WIDTH) {
            cur_col = 0;
            cur_page += font->pages;
        }
        if (cur_page + font->pages > OLED_PAGES) {
            break;
        }

        OLED_DrawChar(cur_page, cur_col, *str, font);
        cur_col += font->width;
        str++;
    }
}

// 在屏幕正中央（水平与垂直均居中）显示单行 ASCII 字符串
void OLED_DrawStringCentered(const char* str, const AsciiFont_t* font) {
    if (str == NULL || font == NULL || font->width == 0 || font->pages >= OLED_PAGES) {
        return;
    }

    size_t len = strlen(str);
    uint16_t text_width = (uint16_t)(len * font->width);
    uint8_t col = (text_width < OLED_WIDTH) ? (uint8_t)((OLED_WIDTH - text_width) / 2U) : 0U;
    uint8_t page = (uint8_t)((OLED_PAGES - font->pages) / 2U);

    OLED_DrawString(page, col, str, font);
}

// 在指定页(page)和列(col)显示指定尺寸的位图（列行式、低位在上 LSB-Top）
void OLED_DrawBitmap(uint8_t page, uint8_t col, uint8_t width, uint8_t pages, const uint8_t* data) {
    if (data == NULL || width == 0 || pages == 0 || col + width > OLED_WIDTH || page + pages > OLED_PAGES) {
        return;
    }

    for (uint8_t p = 0; p < pages; p++) {
        OLED_SetCursor(page + p, col);
        HAL_I2C_Mem_Write(&hi2c1, OLED_ADDR, 0x40, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&data[(uint16_t)p * width], width, 100);
    }
}

typedef struct
{
    uint8_t col;
    uint8_t width;
    uint8_t pages;
    uint16_t frame_bytes;
    uint16_t frame_count;
    const uint8_t* frames;
    const uint16_t* delays;
    uint16_t frame_idx;
    uint32_t last_tick;
} OledAnimPlayer_t;

void OLED_AnimInit(OledAnimPlayer_t* player, uint32_t now) {
    player->frame_idx = 0;
    player->last_tick = now;
    OLED_DrawBitmap(0, player->col, player->width, player->pages, player->frames);
}

void OLED_AnimStep(OledAnimPlayer_t* player, uint32_t now) {
    if ((uint32_t)(now - player->last_tick) >= player->delays[player->frame_idx]) {
        player->last_tick = now;
        player->frame_idx = (uint16_t)((player->frame_idx + 1U) % player->frame_count);
        OLED_DrawBitmap(0, player->col, player->width, player->pages,
                        &player->frames[(uint32_t)player->frame_idx * player->frame_bytes]);
    }
}

typedef enum
{
    OLED_MODE_CAT0 = 0,
    OLED_MODE_CAT1,
    OLED_MODE_CAT2,
    OLED_MODE_3CATS,
    OLED_MODE_COUNT
} OledDisplayMode_t;

// 非阻塞 PA0 按键消抖检测（自动适配外部上拉或下拉电路，仅在按下瞬间返回 1）
uint8_t Key_PA0_Pressed(GPIO_PinState idle_state, uint32_t now) {
    static GPIO_PinState last_raw = GPIO_PIN_RESET;
    static GPIO_PinState stable_state = GPIO_PIN_RESET;
    static uint8_t inited = 0;
    static uint32_t change_tick = 0;

    GPIO_PinState raw = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
    if (!inited) {
        inited = 1;
        last_raw = raw;
        stable_state = idle_state;
        change_tick = now;
        return 0;
    }

    if (raw != last_raw) {
        last_raw = raw;
        change_tick = now;
    }

    if ((uint32_t)(now - change_tick) >= 20U && raw != stable_state) {
        stable_state = raw;
        if (stable_state != idle_state) {
            return 1;
        }
    }
    return 0;
}
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_I2C1_Init();
    /* USER CODE BEGIN 2 */
    /* USER CODE BEGIN 2 */
    // 1. 确保 L2, L3, L4 全部熄灭（输出高电平）
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_SET);
    // 默认先熄灭 L1
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);

    // 2. 检测屏幕是否在线
    uint8_t oled_ready = 0;
    OledDisplayMode_t display_mode = OLED_MODE_CAT0;
    GPIO_PinState pa0_idle = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);

    OledAnimPlayer_t anim_cool = {
            .col = (OLED_WIDTH - CAT_ANIM3_WIDTH) / 2U,
            .width = CAT_ANIM3_WIDTH,
            .pages = CAT_ANIM3_PAGES,
            .frame_bytes = CAT_ANIM3_FRAME_BYTES,
            .frame_count = CAT_ANIM3_FRAME_COUNT,
            .frames = (const uint8_t*)CatAnim3_Frames,
            .delays = CatAnim3_FrameDelays,
    };
    OledAnimPlayer_t anim_cat1 = {
            .col = (OLED_WIDTH - CAT_ANIM_WIDTH) / 2U,
            .width = CAT_ANIM_WIDTH,
            .pages = CAT_ANIM_PAGES,
            .frame_bytes = CAT_ANIM_FRAME_BYTES,
            .frame_count = CAT_ANIM_FRAME_COUNT,
            .frames = (const uint8_t*)CatAnim_Frames,
            .delays = CatAnim_FrameDelays,
    };
    OledAnimPlayer_t anim_cat2 = {
            .col = (OLED_WIDTH - CAT_ANIM2_WIDTH) / 2U,
            .width = CAT_ANIM2_WIDTH,
            .pages = CAT_ANIM2_PAGES,
            .frame_bytes = CAT_ANIM2_FRAME_BYTES,
            .frame_count = CAT_ANIM2_FRAME_COUNT,
            .frames = (const uint8_t*)CatAnim2_Frames,
            .delays = CatAnim2_FrameDelays,
    };
    OledAnimPlayer_t anim_video = {
            .col = (OLED_WIDTH - VIDEO_ANIM_WIDTH) / 2U,
            .width = VIDEO_ANIM_WIDTH,
            .pages = VIDEO_ANIM_PAGES,
            .frame_bytes = VIDEO_ANIM_FRAME_BYTES,
            .frame_count = VIDEO_ANIM_FRAME_COUNT,
            .frames = (const uint8_t*)VideoAnim_Frames,
            .delays = VideoAnim_FrameDelays,
    };

    if (HAL_I2C_IsDeviceReady(&hi2c1, OLED_ADDR, 3, 100) == HAL_OK) {
        // 通信成功：只点亮 L1 (PC5 输出低电平)
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

        OLED_Init();
        oled_ready = 1;

        // 开机默认只居中显示墨镜猫
        OLED_AnimInit(&anim_cool, HAL_GetTick());
    }
    /* USER CODE END 2 */
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1) {
        if (oled_ready) {
            uint32_t now = HAL_GetTick();
            if (Key_PA0_Pressed(pa0_idle, now)) {
                display_mode = (OledDisplayMode_t)((display_mode + 1U) % OLED_MODE_COUNT);
                OLED_Clear();
                switch (display_mode) {
                case OLED_MODE_CAT0:
                    OLED_AnimInit(&anim_cool, now);
                    break;
                case OLED_MODE_CAT1:
                    OLED_AnimInit(&anim_cat1, now);
                    break;
                case OLED_MODE_CAT2:
                    OLED_AnimInit(&anim_cat2, now);
                    break;
                case OLED_MODE_3CATS:
                    OLED_AnimInit(&anim_video, now);
                    break;
                default:
                    break;
                }
            }

            switch (display_mode) {
            case OLED_MODE_CAT0:
                OLED_AnimStep(&anim_cool, now);
                break;
            case OLED_MODE_CAT1:
                OLED_AnimStep(&anim_cat1, now);
                break;
            case OLED_MODE_CAT2:
                OLED_AnimStep(&anim_cat2, now);
                break;
            case OLED_MODE_3CATS:
                OLED_AnimStep(&anim_video, now);
                break;
            default:
                break;
            }
        }
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
  */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 4;
    RCC_OscInitStruct.PLL.PLLN = 168;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 4;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
  */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line) {
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
