#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#include "lvgl.h"
#include "lvgl_helpers.h"


#define LV_TICK_PERIOD_MS 1

#define INDEV_BTN_CONFIRM 35

//EVENTOS LVGL
#define KEY_OK 0x01
#define EVENT_CHANGE_ADC 0x02
 

#define INIT_SC     BIT0
#define MAIN_SC     BIT1
#define SENSOR_SC   BIT2

enum 
{
    INIT = 0,
    MAIN,
    SENSOR
}check_screen_t ;

static lv_style_t btn_style, label_info_style;
static lv_disp_buf_t disp_buf;

lv_color_t buf1[DISP_BUF_SIZE], *buf2 = NULL;
lv_indev_drv_t indev_drv;
lv_indev_t* indev_btn;
lv_disp_drv_t disp_drv;

static const adc_bits_width_t width = ADC_WIDTH_BIT_12;
static const adc_channel_t    ch    = ADC_CHANNEL_5;
static esp_adc_cal_characteristics_t *adc_chars;

SemaphoreHandle_t xGuiSemaphore;
EventGroupHandle_t event_screens ;

/* DECLARANDO OBJETOS */
lv_obj_t* main_job_timer;
lv_obj_t* init_main_bg;
lv_obj_t* init_screen_btn;
lv_obj_t* init_board_bg;
lv_obj_t* franzininho_logo;
lv_obj_t* label_btn;


lv_obj_t* meter;
 
bool is_press = false;
long long map_adc;
int check_sc ;

LV_IMG_DECLARE(Franzininho_Logo);

long map(long x, long in_min, long in_max, long out_min, long out_max) 
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void init_adc()
{
    adc1_config_width(width);
    adc1_config_channel_atten(ch,ADC_ATTEN_DB_0);
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_0, width, 1100, adc_chars);
}

void read_ldr(lv_obj_t* evt)
{
    uint32_t adc_read = 0;
    for(int i = 0; i<32; i++)
    {
        adc_read +=adc1_get_raw(ch);
    }

    adc_read /=32;
    
    lv_res_t ret;
    map_adc = map((long)adc_read,0,4096,0,100);
    ret = lv_event_send(evt,EVENT_CHANGE_ADC,NULL); 
    if(ret  != LV_RES_OK)
    {
        ESP_LOGE("LVGL", "Evento invalidado");
    }
    vTaskDelay(pdMS_TO_TICKS(100));
}

void button_init(void)
{
    gpio_set_direction(INDEV_BTN_CONFIRM,GPIO_MODE_INPUT);
}

void read_btn(lv_obj_t* evt)
{
    if(!gpio_get_level(INDEV_BTN_CONFIRM) && !is_press)
    {
        is_press = true;
        printf("Press Confirm\n");
        lv_event_send(evt,KEY_OK,NULL); 
    }
    else if (gpio_get_level(INDEV_BTN_CONFIRM) && is_press)
    {
        is_press = false;
    }
   
}

static void lv_tick_task(void *arg)
{
    lv_tick_inc(LV_TICK_PERIOD_MS);
}

static void guiTask(void *pvParam)
{
    xGuiSemaphore = xSemaphoreCreateMutex();

   
    lv_init();
    lvgl_driver_init();
    lv_disp_buf_init(&disp_buf, buf1, buf2, DISP_BUF_SIZE);

   
    
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 320;
    disp_drv.ver_res = 240;
    disp_drv.flush_cb = disp_driver_flush;
    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"};
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

    
    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY)) 
        {
            lv_task_handler();
            xSemaphoreGive(xGuiSemaphore);
        }
       
    }

    vTaskDelete(NULL);
}

void style_init()
{
    /* Estilo para os botões */
    lv_style_init(&btn_style);

    lv_style_set_border_color(&btn_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_border_opa(&btn_style, LV_STATE_DEFAULT, LV_OPA_40);
    lv_style_set_border_width(&btn_style, LV_STATE_DEFAULT, 3);

    /* Estilo para as fontes*/

    lv_style_init(&label_info_style);


}

static void button_event_cb(lv_obj_t * obj, lv_event_t event)
{
    if(event == KEY_OK && check_sc == INIT)
    {
        xEventGroupSetBits(event_screens, MAIN_SC);
    }
    else if(event == KEY_OK && check_sc == MAIN)
    {
        xEventGroupSetBits(event_screens, SENSOR_SC);
    }
    
    else if(event == KEY_OK && check_sc == SENSOR)
    {
        xEventGroupSetBits(event_screens, MAIN_SC);
    }

}

void init_screen(lv_obj_t* init_obj)
{
    init_board_bg = lv_obj_create(init_obj, NULL);
    lv_obj_set_size(init_board_bg, 320, 240);
    lv_obj_set_style_local_bg_color(init_board_bg, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GREEN);

    init_main_bg = lv_obj_create(init_board_bg, NULL);
    lv_obj_set_size(init_main_bg, 300, 300);
    lv_obj_align(init_main_bg,LV_ALIGN_CENTER,NULL,0,0);

    franzininho_logo = lv_img_create(init_main_bg, NULL);
    lv_img_set_src(franzininho_logo, &Franzininho_Logo);
    lv_obj_align(franzininho_logo, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

    // init_screen_btn = lv_btn_create(init_main_bg,NULL);
    // lv_obj_add_style(init_screen_btn,LV_BTN_PART_MAIN,&btn_style);
    // lv_obj_align(init_screen_btn,NULL,LV_ALIGN_IN_BOTTOM_MID,0,-10);
    // lv_btn_set_checkable(init_screen_btn,true);
    // lv_btn_set_fit2(init_screen_btn, LV_FIT_NONE, LV_FIT_TIGHT);
    // lv_obj_set_event_cb(init_screen_btn,button_event_cb);

    // label_btn = lv_label_create(init_screen_btn,NULL);
    // lv_label_set_text(label_btn,"Iniciar");
    // lv_obj_align(label_btn,NULL,LV_ALIGN_CENTER,0,0);
}

void main_screen(lv_obj_t* main_obj)
{
   
    lv_obj_t* main_board = lv_obj_create(main_obj, NULL);
    lv_obj_set_size(main_board,240,240);
    lv_obj_set_event_cb(main_obj,button_event_cb);
    
    lv_obj_t* main_title_bg  = lv_obj_create(main_board, NULL);
    lv_obj_set_size(main_title_bg, 100, 10);
    lv_obj_align(main_title_bg,NULL,LV_ALIGN_IN_TOP_MID,0,30);
    lv_obj_set_style_local_bg_color(main_title_bg, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(43,197,29));

    lv_obj_t* main_title_label = lv_label_create(main_board,NULL);
    lv_label_set_text(main_title_label, "INFOS");
    lv_obj_align(main_title_label,NULL,LV_ALIGN_IN_TOP_MID,0,10);
    lv_obj_set_style_local_text_font(main_title_label,LV_LABEL_PART_MAIN,LV_STATE_DEFAULT,&lv_font_montserrat_16);

    
    lv_obj_t* main_bg  = lv_obj_create(main_board, NULL);
    lv_obj_set_size(main_bg, 220, 100);
    lv_obj_align(main_bg,LV_ALIGN_CENTER,NULL,0,0);
    lv_obj_set_style_local_bg_color(main_bg, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(43,197,29));

    
    lv_obj_t* label_info = lv_label_create(main_bg,NULL);
    lv_label_set_recolor(label_info,true);
    lv_label_set_text(label_info,"#ffffff Board# #000000  : Franzininho WiFi#\n"
                                 "#ffffff Site#  #000000  : franzininho.com.br#\n"
                                 "#ffffff Author# #000000 : Diana and Fabio#\n"
                                 "#ffffff Chip#  #000000  : ESP32 S2#\n");
    lv_obj_align(label_info,NULL,LV_ALIGN_CENTER,5,10);
    lv_obj_set_style_local_text_font(label_info,LV_LABEL_PART_MAIN,LV_STATE_DEFAULT,&lv_font_montserrat_14);
    
    lv_obj_t* label_info_circular = lv_label_create(main_board,NULL);
    lv_label_set_long_mode(label_info_circular,LV_LABEL_LONG_SROLL_CIRC);
    lv_obj_set_width(label_info_circular, 200);
    lv_label_set_text(label_info_circular, "VENHA PARA A COMUNIDADE !!!");
    lv_obj_align(label_info_circular,NULL,LV_ALIGN_IN_BOTTOM_MID,0,-10);
    lv_obj_set_style_local_text_font(label_info_circular,LV_LABEL_PART_MAIN,LV_STATE_DEFAULT,&lv_font_montserrat_16);
    
}

static void sensor_event_cb(lv_obj_t * obj, lv_event_t event)
{
    if(event == EVENT_CHANGE_ADC)
    {
        lv_linemeter_set_value(meter, (uint32_t)map_adc);
    }
    
}

void sensor_screen(lv_obj_t* sensor_obj)
{
   
    lv_obj_t* sensor_board = lv_obj_create(sensor_obj, NULL);
    lv_obj_set_size(sensor_board,240,240);
    lv_obj_set_event_cb(sensor_obj,button_event_cb);

    lv_obj_t* sensor_title_bg  = lv_obj_create(sensor_board, NULL);
    lv_obj_set_size(sensor_title_bg, 110, 10);
    lv_obj_align(sensor_title_bg,NULL,LV_ALIGN_IN_TOP_MID,0,30);
    lv_obj_set_style_local_bg_color(sensor_title_bg, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(43,197,29));

    lv_obj_t* sensor_title_label = lv_label_create(sensor_board,NULL);
    lv_label_set_text(sensor_title_label, "SENSOR");
    lv_obj_align(sensor_title_label,NULL,LV_ALIGN_IN_TOP_MID,0,10);
    lv_obj_set_style_local_text_font(sensor_title_label,LV_LABEL_PART_MAIN,LV_STATE_DEFAULT,&lv_font_montserrat_16);

    meter = lv_linemeter_create(sensor_obj,NULL);
    lv_linemeter_set_range(meter,0,100);
    lv_linemeter_set_scale(meter,270,20);
    lv_obj_set_size(meter,150,150);
    lv_linemeter_set_value(meter, 50);
    lv_obj_align(meter,NULL,LV_ALIGN_CENTER,0,20);
    lv_obj_set_event_cb(meter,sensor_event_cb);
}

void app_main(void)
{
    
    xTaskCreatePinnedToCore(guiTask, "gui", 4096 * 2, NULL, 0, NULL, 1);
    vTaskDelay(pdMS_TO_TICKS(5000));
    
    event_screens = xEventGroupCreate();
    EventBits_t ux;
    
    button_init();
    init_adc();

    style_init();
    vTaskDelay(pdMS_TO_TICKS(200));
    
    lv_obj_t* current_sc  = lv_obj_create(NULL,NULL);
    lv_obj_t* trigger_btn = NULL;

    init_screen(lv_scr_act());
    vTaskDelay(pdMS_TO_TICKS(1000));
    //trigger_btn = init_screen_btn;
    //xEventGroupSetBits(event_screens, INIT_SC);
    while(1)
    {
        // ux = xEventGroupGetBits(event_screens);
        // if(ux & INIT_SC)
        // {
        //     lv_scr_load(current_sc);
        //     vTaskDelay(pdMS_TO_TICKS(500));
            
        //     check_sc = INIT;
        //     xEventGroupClearBits(event_screens, INIT_SC);
        // }

        // else if(ux & MAIN_SC)
        // {
        //     main_screen(current_sc);
        //     lv_scr_load(current_sc);

        //     trigger_btn = current_sc;
        //     check_sc = MAIN;

        //     vTaskDelay(pdMS_TO_TICKS(500));
        //     xEventGroupClearBits(event_screens, MAIN_SC);
            
        // }
        // else if(ux & SENSOR_SC)
        // {
        //     sensor_screen(current_sc);
        //     lv_scr_load(current_sc);
            
        //     check_sc = SENSOR;
        //     vTaskDelay(pdMS_TO_TICKS(500));
        //     xEventGroupClearBits(event_screens, SENSOR_SC);
        // } 

        // read_btn(trigger_btn);
        
        // read_ldr(meter);
        
        vTaskDelay(1);
    }
}