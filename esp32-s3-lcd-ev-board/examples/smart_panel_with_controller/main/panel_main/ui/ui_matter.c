/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <esp_log.h>
#include "ui_main.h"
#include "ui_matter.h"

/* UI function declaration */
ui_func_desc_t ui_matter_func = {
    .name = "Matter Controller",
    .init = ui_matter_init,
    .show = ui_matter_show,
    .hide = ui_matter_hide,
};

static const char *TAG = "ui_matter";
static ui_state_t ui_matter_state = ui_state_dis;
static lv_obj_t *obj_page_matter = NULL;
static lv_obj_t *_tips_text = NULL;
static lv_obj_t *_qr_code = NULL;
static int16_t row_align_y = 80;
static int16_t button_width = 130;
static int16_t button_height = 160;
static int16_t button_dis = 140;

typedef struct {
    const char *name;
    lv_img_dsc_t const *img_on;
    lv_img_dsc_t const *img_off;
} btn_img_src_t;

LV_IMG_DECLARE(QRcode);
LV_IMG_DECLARE(light_on);
LV_IMG_DECLARE(light_off);
LV_IMG_DECLARE(switch_on);
LV_IMG_DECLARE(switch_off);
LV_IMG_DECLARE(plug_on);
LV_IMG_DECLARE(plug_off);

static const btn_img_src_t img_src_list[] = {
    {.name = "Light", .img_on = &light_on, .img_off = &light_off},
    {.name = "Plug", .img_on = &plug_on, .img_off = &plug_off},
    {.name = "Switch", .img_on = &switch_on, .img_off = &switch_off}
};

static ui_matter_state_t g_matter_state = UI_MATTER_EVT_WAITING_SETUP;
extern device_to_control_t device_to_control;

static lv_obj_t *ui_obj_create(lv_obj_t *par, lv_coord_t width, lv_coord_t height, lv_style_int_t radius)
{
    lv_obj_t *_obj = lv_obj_create(par, NULL);

    lv_obj_set_size(_obj, width, height);
    lv_obj_set_style_local_radius(_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, radius);
    lv_obj_set_style_local_bg_color(_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_border_color(_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, COLOR_THEME);
    lv_obj_set_style_local_border_color(_obj, LV_OBJ_PART_MAIN, LV_STATE_FOCUSED, COLOR_THEME);

    return _obj;
}

void ui_set_onoff_state(lv_obj_t *g_func_btn, size_t size_type, bool state)
{
    if (NULL == g_func_btn) {
        return;
    }
    lv_obj_t *img = (lv_obj_t *)g_func_btn->user_data;
    // ui_acquire();
    if (state) {
        lv_obj_add_state(g_func_btn, LV_STATE_CHECKED);
        lv_img_set_src(img, img_src_list[size_type].img_on);
    } else {
        lv_obj_clear_state(g_func_btn, LV_STATE_CHECKED);
        lv_img_set_src(img, img_src_list[size_type].img_off);
    }
    // ui_release();
}

static void btn_obj_click_cb(lv_obj_t *obj, lv_event_t event)
{
    if (LV_EVENT_CLICKED == event) {
        matter_ctrl_change_state(obj);
    }
}

static void ui_list_device(void)
{
    uint8_t num_of_device[4] = {0, 0, 0, 0};
    uint8_t kind_to_show = 0;
    uint8_t online_no = 0;
    uint8_t offline_no = device_to_control.online_num;
    matter_device_list_lock();
    node_endpoint_id_list_t *ptr = device_to_control.dev_list;

    while (ptr) {
        lv_obj_t *g_func_btn = ui_obj_create(obj_page_matter, button_width, button_height, 15);
        ptr->lv_obj = g_func_btn;
        ++kind_to_show;
        ++num_of_device[ptr->device_type];
        lv_obj_t *img = lv_img_create(g_func_btn, NULL);
        lv_obj_align(img, g_func_btn, LV_ALIGN_CENTER, 10, -35);
        lv_obj_set_user_data(g_func_btn, (void *)img);
        if (ptr->is_online) {
            if (ptr->OnOff) {
                // ESP_LOGI(TAG, "device %llx is on", ptr->node_id);
                lv_img_set_src(img, img_src_list[ptr->device_type].img_on);
                lv_obj_add_state(g_func_btn, LV_STATE_CHECKED);
            } else {
                // ESP_LOGI(TAG, "device %llx is off", ptr->node_id);
                lv_img_set_src(img, img_src_list[ptr->device_type].img_off);
                lv_obj_clear_state(g_func_btn, LV_STATE_CHECKED);
            }
            lv_obj_set_style_local_border_width(g_func_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 2);
            lv_obj_set_style_local_bg_color(g_func_btn, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, COLOR_THEME);
            lv_obj_t *label = lv_label_create(g_func_btn, NULL);
            lv_label_set_text_fmt(label, "%s %d\nonline", img_src_list[ptr->device_type].name, num_of_device[ptr->device_type]);
            lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
            lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &font_en_18);
            lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(40, 40, 40));
            lv_obj_align(label, g_func_btn, LV_ALIGN_CENTER, 0, 40);
            lv_obj_set_event_cb(g_func_btn, btn_obj_click_cb);

            lv_obj_set_pos(g_func_btn, button_dis * online_no + 10, row_align_y);
            ++online_no;
        } else {
            lv_obj_set_style_local_border_color(g_func_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, COLOR_BG);
            lv_obj_set_style_local_border_color(g_func_btn, LV_OBJ_PART_MAIN, LV_STATE_FOCUSED, COLOR_BG);
            lv_img_set_src(img, img_src_list[ptr->device_type].img_off);
            lv_obj_t *label = lv_label_create(g_func_btn, NULL);
            lv_label_set_text_fmt(label, "%s %d\noffline", img_src_list[ptr->device_type].name, num_of_device[ptr->device_type]);
            lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
            lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &font_en_18);
            lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(220, 220, 220));
            lv_obj_align(label, g_func_btn, LV_ALIGN_CENTER, 0, 40);
            
            lv_obj_set_pos(g_func_btn, button_dis * offline_no + 10, row_align_y);
            ++offline_no;
        }
        ptr = ptr->next;
    }

    if (0u == kind_to_show) {
        if (!_tips_text) {
            _tips_text = lv_label_create(obj_page_matter, NULL);
            lv_obj_set_style_local_text_font(_tips_text, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &font_en_32);
            lv_obj_align(_tips_text, NULL, LV_ALIGN_CENTER, -240, -16);
        }
        ESP_LOGE(TAG, "No device list");
        lv_label_set_text(_tips_text, "No device list, please refresh");
        lv_obj_set_hidden(_tips_text, false);
    }
    matter_device_list_unlock();
}

void ui_matter_config_update_cb(ui_matter_state_t state)
{
    g_matter_state = state;

    if (!obj_page_matter) {
        return;
    }
    bsp_display_lock(0);
    switch (state) {
    case UI_MATTER_EVT_WAITING_SETUP:
        _tips_text = lv_label_create(obj_page_matter, NULL);
        lv_obj_set_style_local_text_font(_tips_text, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &font_en_32);
        lv_obj_align(_tips_text, NULL, LV_ALIGN_CENTER, -200, -16);
        lv_label_set_text(_tips_text, "Waiting for matter setting up");
        break;
    case UI_MATTER_EVT_LOADING:
        _qr_code = lv_img_create(obj_page_matter, NULL);
        lv_img_set_src(_qr_code, &QRcode);
        lv_obj_align(_qr_code, NULL, LV_ALIGN_IN_LEFT_MID, 20, 0);
        lv_obj_align(_tips_text, _qr_code, LV_ALIGN_OUT_RIGHT_MID, 40, -16);
        lv_obj_set_hidden(_tips_text, false);
        lv_label_set_text(_tips_text, "Scan the QR code\nby ESP Rainmaker App");
        break;
    case UI_MATTER_EVT_START_COMMISSION:
        lv_obj_set_hidden(_tips_text, false);
        lv_label_set_text(_tips_text, "Start Commission ...");
        break;
    case UI_MATTER_EVT_FAILED_COMMISSION:
        if (_qr_code) {
            lv_obj_set_hidden(_qr_code, false);
        }

        lv_obj_set_hidden(_tips_text, false);
        lv_label_set_text(_tips_text, "Failed commission ...");
        break;
    case UI_MATTER_EVT_COMMISSIONCOMPLETE:
    case UI_MATTER_EVT_REFRESH:
        if (_qr_code) {
            lv_obj_set_hidden(_qr_code, true);
        }
        if (_tips_text) {
            lv_obj_set_hidden(_tips_text, true);
        }
        ui_list_device();
        break;
    default:
        break;
    }
    bsp_display_unlock();
}

static void matter_state_update_task(lv_task_t *task)
{
    if (!whether_get_matter_fabric()) {
        ESP_LOGW(TAG, "Not commissioned, show QR code!");
        ui_matter_config_update_cb(UI_MATTER_EVT_LOADING);
    } else {
        ESP_LOGW(TAG, "Has been commissioned, waiting for device list!");
    }
    lv_task_del(task);
}


void ui_matter_init(void *data)
{
    if (ui_state_dis == ui_matter_state) {
        ui_page_show("Matter Controller");
    }
    obj_page_matter = lv_obj_create(ui_page_get_obj(), NULL);

    lv_obj_set_click(obj_page_matter, false);
    lv_obj_set_size(obj_page_matter, 750, 300);
    lv_obj_set_style_local_border_width(obj_page_matter, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_bg_color(obj_page_matter, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_align(obj_page_matter, NULL, LV_ALIGN_CENTER, 0, 30);

    // _qr_code = lv_img_create(obj_page_matter, NULL);
    // lv_img_set_src(_qr_code, &QRcode);
    // lv_obj_align(_qr_code, NULL, LV_ALIGN_IN_LEFT_MID, 20, 0);

    // _tips_text = lv_label_create(obj_page_matter, NULL);
    // lv_obj_set_style_local_text_font(_tips_text, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &font_en_32);
    // lv_obj_align(_tips_text, _qr_code, LV_ALIGN_OUT_RIGHT_MID, 40, -16);
    lv_task_create(matter_state_update_task, 500, 1, NULL);
    ui_matter_config_update_cb(g_matter_state);
    ui_matter_state = ui_state_show;
}

void ui_matter_show(void *data)
{
    if (ui_state_dis == ui_matter_state) {
        ui_matter_init(data);
    } else if (ui_state_hide == ui_matter_state) {
        lv_obj_set_hidden(obj_page_matter, false);
        ui_page_show("Matter Controller");
        ui_matter_state = ui_state_show;
    }
}

void ui_matter_hide(void *data)
{
    if (ui_state_show == ui_matter_state) {
        lv_obj_set_hidden(obj_page_matter, true);
        ui_page_hide(NULL);
        ui_matter_state = ui_state_hide;
    }
}

void clean_screen(void)
{
    if (!obj_page_matter) {
        return;
    }
    bsp_display_lock(0);
    lv_obj_clean(obj_page_matter);
    lv_obj_set_hidden(obj_page_matter, false);
    _qr_code = NULL;
    _tips_text = NULL;
    bsp_display_unlock();
}