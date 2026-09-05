#ifndef LV_CONF_H
#define LV_CONF_H

#define LV_COLOR_DEPTH 16
#define LV_HOR_RES_MAX 320
#define LV_VER_RES_MAX 480
#define LV_USE_ARABIC_PERSIAN_CHARS 1

/*====================
   Fonts - ONLY ENABLE WHAT YOU NEED
 *====================*/
#define LV_FONT_MONTSERRAT_12 0
#define LV_FONT_MONTSERRAT_14 1  // Default - keep this
#define LV_FONT_MONTSERRAT_16 0
#define LV_FONT_MONTSERRAT_18 1  // For messages
#define LV_FONT_MONTSERRAT_20 0
#define LV_FONT_MONTSERRAT_22 0
#define LV_FONT_MONTSERRAT_24 1  // For titles
#define LV_FONT_MONTSERRAT_26 0
#define LV_FONT_MONTSERRAT_28 0
#define LV_FONT_MONTSERRAT_30 0
#define LV_FONT_MONTSERRAT_32 0
#define LV_FONT_MONTSERRAT_34 0
#define LV_FONT_MONTSERRAT_36 0
#define LV_FONT_MONTSERRAT_38 0
#define LV_FONT_MONTSERRAT_40 0

#define LV_FONT_DEFAULT &lv_font_montserrat_14

/*====================
   Other settings
 *====================*/
#define LV_MEM_SIZE (32 * 1024U)
#define LV_DISP_DEF_REFR_PERIOD 30

#endif