/**
 * @file lv_openvg.c
 *
 */

#ifndef LVGL_VG_GPU
#if LV_USE_OPENVG != 0

/*********************
 *      INCLUDES
 *********************/

//#include "../lv_misc/lv_debug.h"
#include "../core/lv_theme.h"
#include "lv_openvg.h"
#include "lv_label.h"

#include "vg_driver.h"

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_openvg"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void refr_img(lv_obj_t * openvg);
static void lv_openvg_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_openvg_ext_t g_ext = {0};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a openvg object
 * @param par pointer to an object, it will be the parent of the new openvg
 * @param copy pointer to a openvg object, if not NULL then the new object will be copied from
 * it
 * @return pointer to the created openvg
 */
lv_obj_t * lv_openvg_create(lv_obj_t * par)
{
    LV_LOG_TRACE("openvg create started");

    /*Create the ancestor of openvg*/
    lv_obj_t * openvg = lv_obj_create(par);
    if(openvg == NULL)
		return NULL;

    lv_obj_add_event_cb(openvg, lv_openvg_event_cb, LV_EVENT_ALL, NULL);

	lv_theme_apply(openvg);

    LV_LOG_INFO("openvg created");

    return openvg;
}


lv_openvg_ext_t *lv_openvg_get_ext(lv_obj_t * obj)
{
	return &g_ext;
}


/*=====================
 * Setter functions
 *====================*/

/**
 * Set images for a background of the openvg
 * @param openvg pointer to an openvg object
 * @param src pointer to an image source (a C array or path to a file)
 */
void lv_openvg_set_bg_src(lv_obj_t * openvg, const void * src)
{
    LV_ASSERT_OBJ(openvg, LV_OBJX_NAME);

    lv_openvg_ext_t *ext = lv_openvg_get_ext(openvg);

    ext->img_src_bg = src;

    refr_img(openvg);
}

/**
 * Set param for the openvg
 * @param openvg pointer to an openvg object
 * @param param
 */
void lv_openvg_set_param(lv_obj_t * openvg, void *param)
{
    LV_ASSERT_OBJ(openvg, LV_OBJX_NAME);

    lv_openvg_ext_t *ext = lv_openvg_get_ext(openvg);

    ext->param = param;

    refr_img(openvg);
}


/*=====================
 * Getter functions
 *====================*/

/**
 * Get the background image
 * @param openvg pointer to an openvg object
 * @return pointer to an image source (a C array or path to a file)
 */
const void * lv_openvg_get_bg_src(lv_obj_t * openvg)
{
    LV_ASSERT_OBJ(openvg, LV_OBJX_NAME);

    lv_openvg_ext_t *ext = lv_openvg_get_ext(openvg);

    return ext->img_src_bg;
}

/**
 * Get the openvg param
 * @param openvg pointer to an openvg object
 * @return param
 */
void *lv_openvg_get_param(lv_obj_t * openvg)
{
    LV_ASSERT_OBJ(openvg, LV_OBJX_NAME);

    lv_openvg_ext_t *ext = lv_openvg_get_ext(openvg);

    return ext->param;
}


/*=====================
 * Other functions
 *====================*/

/*
 * New object specific "other" functions come here
 */

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void lv_openvg_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
//    lv_obj_t * kb = lv_event_get_user_data(e);

	if(code == LV_EVENT_DRAW_MAIN_BEGIN) {
		lv_openvg_ext_t *ext = lv_openvg_get_ext(ta);
		if (ext->param) {
			xm_vg_draw_prepare(ext->param);
			xm_vg_draw_start();
		}
	}
}

static void refr_img(lv_obj_t * obj)
{
    lv_openvg_ext_t *ext = lv_openvg_get_ext(obj);
    lv_img_header_t header;

    const void * src = ext->img_src_bg;
    if(src == NULL) {
		lv_obj_invalidate(obj);
		return;
    }

    lv_res_t info_res = LV_RES_OK;
    if(lv_img_src_get_type(src) == LV_IMG_SRC_SYMBOL) {
        const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
        header.h = lv_font_get_line_height(font);
        header.w = lv_txt_get_width(src, (uint16_t)strlen(src), font, 0, LV_TEXT_FLAG_NONE);
        header.always_zero = 0;
        header.cf = LV_IMG_CF_ALPHA_1BIT;
    }
    else {
        info_res = lv_img_decoder_get_info(src, &header);
    }

    if(info_res == LV_RES_OK) {
        ext->act_cf = header.cf;
        lv_obj_set_size(obj, header.w, header.h);
    }
    else {
        ext->act_cf = LV_IMG_CF_UNKNOWN;
    }

    lv_obj_invalidate(obj);
}

#endif

#endif
