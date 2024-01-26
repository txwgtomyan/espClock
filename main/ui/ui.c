/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

// This demo UI is adapted from LVGL official example: https://docs.lvgl.io/master/examples.html#scatter-chart

#include "ui.h"

// static void draw_event_cb(lv_event_t *e)
// {
//     lv_obj_draw_part_dsc_t *dsc = lv_event_get_draw_part_dsc(e);
//     if (dsc->part == LV_PART_ITEMS) {
//         lv_obj_t *obj = lv_event_get_target(e);
//         lv_chart_series_t *ser = lv_chart_get_series_next(obj, NULL);
//         uint32_t cnt = lv_chart_get_point_count(obj);
//         /*Make older value more transparent*/
//         dsc->rect_dsc->bg_opa = (LV_OPA_COVER *  dsc->id) / (cnt - 1);

//         /*Make smaller values blue, higher values red*/
//         lv_coord_t *x_array = lv_chart_get_x_array(obj, ser);
//         lv_coord_t *y_array = lv_chart_get_y_array(obj, ser);
//         /*dsc->id is the tells drawing order, but we need the ID of the point being drawn.*/
//         uint32_t start_point = lv_chart_get_x_start_point(obj, ser);
//         uint32_t p_act = (start_point + dsc->id) % cnt; /*Consider start point to get the index of the array*/
//         lv_opa_t x_opa = (x_array[p_act] * LV_OPA_50) / 200;
//         lv_opa_t y_opa = (y_array[p_act] * LV_OPA_50) / 1000;

//         dsc->rect_dsc->bg_color = lv_color_mix(lv_palette_main(LV_PALETTE_RED),
//                                                lv_palette_main(LV_PALETTE_BLUE),
//                                                x_opa + y_opa);
//     }
// }

// static void add_data(lv_timer_t *timer)
// {
//     lv_obj_t *chart = timer->user_data;
//     lv_chart_set_next_value2(chart, lv_chart_get_series_next(chart, NULL), lv_rand(0, 200), lv_rand(0, 1000));
// }

// void example_lvgl_demo_ui(lv_disp_t *disp)
// {
//     lv_obj_t *scr = lv_disp_get_scr_act(disp);
//     lv_obj_t *chart = lv_chart_create(scr);
//     lv_obj_set_size(chart, 400, 300);
//     lv_obj_align(chart, LV_ALIGN_CENTER, 0, 0);
//     lv_obj_add_event_cb(chart, draw_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);
//     lv_obj_set_style_line_width(chart, 0, LV_PART_ITEMS);   /*Remove the lines*/

//     lv_chart_set_type(chart, LV_CHART_TYPE_SCATTER);

//     lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_X, 5, 5, 5, 1, true, 30);
//     lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 6, 5, true, 50);

//     lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_X, 0, 200);
//     lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 1000);

//     lv_chart_set_point_count(chart, 50);

//     lv_chart_series_t *ser = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
//     for (int i = 0; i < 50; i++) {
//         lv_chart_set_next_value2(chart, ser, lv_rand(0, 200), lv_rand(0, 1000));
//     }

//     lv_timer_create(add_data, 100, chart);
// }
// LV_IMG_DECLARE(ui_img_pic1_png);    // assets\pic1.png
// LV_IMG_DECLARE(ui_img_pic2_png);    // assets\pic1.png
// LV_IMG_DECLARE(ui_img_pic3_png);    // assets\pic1.png

extern const lv_img_dsc_t ui_img_pic1_png;
extern const lv_img_dsc_t ui_img_pic2_png;
extern const lv_img_dsc_t ui_img_pic3_png;

lv_obj_t *ui_Screen1; // 创建图片对象
lv_obj_t *ui_Screen1_Image1;
uint16_t count;
const lv_img_dsc_t *ui_imgs[3] = {
    &ui_img_pic1_png, &ui_img_pic2_png, &ui_img_pic3_png};
void ui_event_Screen1_Image1(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_CLICKED)
    {
        count++;
        lv_img_set_src(ui_Screen1_Image1, ui_imgs[count % 3]);
    }
}
void ui_Screen1_screen_init(void)
{
    count = 0;

    ui_Screen1 = lv_obj_create(NULL);

    lv_obj_clear_flag(ui_Screen1, LV_OBJ_FLAG_SCROLLABLE); /// Flags
    lv_obj_align(ui_Screen1, LV_ALIGN_CENTER, 0, 0);

    ui_Screen1_Image1 = lv_img_create(ui_Screen1);
    lv_img_set_src(ui_Screen1_Image1, &ui_img_pic1_png);
    lv_obj_set_width(ui_Screen1_Image1, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(ui_Screen1_Image1, LV_SIZE_CONTENT); /// 1
    // lv_img_set_zoom(ui_Screen1_Image1,128);//缩放图片

    lv_obj_set_align(ui_Screen1_Image1, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Screen1_Image1, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_ADV_HITTEST); /// Flags
    lv_obj_clear_flag(ui_Screen1_Image1, LV_OBJ_FLAG_SCROLLABLE);                        /// Flags

    lv_obj_add_event_cb(ui_Screen1_Image1, ui_event_Screen1_Image1, LV_EVENT_ALL, NULL);
}

lv_obj_t *ui____initial_actions0;
void example_lvgl_demo_ui(lv_disp_t *disp)
{
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_LIGHT_BLUE), lv_palette_main(LV_PALETTE_RED),
                                              false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    ui_Screen1_screen_init();
    ui____initial_actions0 = lv_obj_create(NULL);
    lv_disp_load_scr(ui_Screen1);
}
