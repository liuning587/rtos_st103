/**
 *****************************************************************************
 * @file      zk.c
 * @brief     C Source file of zk.c.
 * @details   This file including all API functions's 
 *            implement of zk.c.
 *
 * @copy      Copyrigth(C), 2008-2012,Sanxing Electric Co.,Ltd.
 */

/*-----------------------------------------------------------------------------
 Section: Includes
 -----------------------------------------------------------------------------*/
#include "types.h"
#include "string.h"
#include "shell.h"
#include "flash.h"
#include "zk.h"
#include <maths.h>

/*-----------------------------------------------------------------------------
 Section: Macro Definitions
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Type Definitions
 -----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Constant Definitions
 -----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Global Variables
 -----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Local Variables
 -----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Local Function Prototypes
 -----------------------------------------------------------------------------*/
static int32_t
zk_data_of16_8(uint8_t letter, uint8_t* platticedata);
static int32_t
zk_inversbit(uint8_t* pdata, int32_t width);
static int32_t
zk_data_of16_16(uint16_t hz, uint8_t* platticedata);

/*-----------------------------------------------------------------------------
 Section: Function Definitions
 -----------------------------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief      获取字模的对应数据.
 * @param[in]  font  FONT_SIZE_16  16*16 或则16*8
 *                   FONT_SIZE_12  12*12 或则12*8
 * @param[in]
 *
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 *******************************************************************************
 */
extern int32_t
zk_generate_data(uint8_t font,
        const uint8_t* pstr,
        int32_t len,
        uint8_t* plattice_data)
{
    int32_t src = 0;
    int32_t index = 0;
    uint16_t wd;
    uint8_t data[32];

    while (src < len)
    {
        if (font == FONT_SIZE_16)
        {
            if (pstr[src] < 0x80u)
            {
                zk_data_of16_8(pstr[src], data);
                zk_inversbit(data, 8);
                memcpy(plattice_data + index, data, 16u);
                index += 16;
                src++;
            }
            else
            {
                wd = (uint16_t)((uint16_t)pstr[src] << 8) | pstr[src + 1];
                zk_data_of16_16(wd, data);
                zk_inversbit(data, 16);
                memcpy(plattice_data + index, data, 32u);
                index += 32;

                src += 2;
            }
        }
        if (font == FONT_SIZE_12)
        {
            /*TODO*/
        }
    }
    return index;
}

/**
 *******************************************************************************
 * @brief      获取16*16的字符字模.
 * @param[in]  hz 汉字
 * @param[out] platticedata 对应字符的字模
 * @retval     None
 *
 * @details
 *
 * @note
 *******************************************************************************
 */
static int32_t
zk_data_of16_16(uint16_t hz, uint8_t* platticedata)
{
    uint8_t qh;
    uint8_t wh;
    uint32_t location;

    qh = ((hz & 0xFF00u) >> 8) - 0xA0u;
    wh = (hz & 0x00FFu) - 0xA0u;

    location = (uint32_t)((94u * ((uint32_t)qh - 1u)) + ((uint32_t)wh - 1u)) * 32u;

    if (location > 267584u)
    {
        memset(platticedata, 0x00, 32u);
    }
    else
    {
        FLFlash* pflash = flash_init();
        pflash->read(pflash, ZK_OFFSET_16_16 + location, platticedata, 32u);
    }

    return 32;
}

/**
 *******************************************************************************
 * @brief      .
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 *******************************************************************************
 */
static int32_t
zk_inversbit(uint8_t* pdata, int32_t width)
{
    uint32_t i;
    uint32_t j;
    uint8_t tmp[32];

    bzero(tmp, 32);
    if (width == 16)
    {

        for (i = 0u; i < 8u; i++)
        {
            for (j = 0u; j < 8u; j++)
                tmp[i] += (BITS(pdata[j * 2], 7u - i) << j);
        }
        for (i = 8u; i < 16u; i++)
        {
            for (j = 0u; j < 8u; j++)
                tmp[i] += (BITS(pdata[(j * 2) + 1], 15 - i) << j);
        }
        for (i = 16u; i < 24u; i++)
        {
            for (j = 0u; j < 8u; j++)
                tmp[i] += (BITS(pdata[16 + (j * 2)], 23 - i) << j);
        }
        for (i = 24u; i < 32u; i++)
        {
            for (j = 0u; j < 8u; j++)
                tmp[i] += (BITS(pdata[16 + (j * 2) + 1], 31 - i) << j);
        }
        memcpy(pdata, tmp, 32);
        return 32;
    }
    else if (width == 8)
    {
        for (i = 0u; i < 8u; i++)
        {
            for (j = 0u; j < 8u; j++)
                tmp[i] += (BITS(pdata[j], 7 - i) << j);
        }
        for (i = 8u; i < 16u; i++)
        {
            for (j = 0u; j < 8u; j++)
                tmp[i] += (uint8_t) (BITS(pdata[j + 8], (15u - i)) << (uint32_t)j);
        }
        memcpy(pdata, tmp, 16);
        return 16;
    }
    else
    {
        return -1;
    }

}
/**
 *******************************************************************************
 * @brief      获取16*8的字符字模.
 * @param[in]  letter 字母
 * @param[out] platticedata 对应字符的字模
 * @retval     None
 *
 * @details
 *
 * @note
 *******************************************************************************
 */
static int32_t
zk_data_of16_8(uint8_t letter, uint8_t* platticedata)
{
    FLFlash* pflash = flash_init();
    pflash->read(pflash, ZK_OFFSET_16_8 + (letter * 16), platticedata, 16);
    return 16;
}

/*----------------------------zk.c--------------------------------*/
