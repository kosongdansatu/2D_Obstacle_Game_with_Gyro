/*****************************************************************************
* | File        :   LCD_1in69_Touch_test.c
* | Author      :   Waveshare team / Modded for Flappy Bird (Super Easy)
* | Function    :   1.69inch LCD LVGL Flappy Bird Game via Gyro
******************************************************************************/
#include "LCD_Test.h"
#include "LCD_1in69.h"
#include "LVGL_example.h" 
#include "QMI8658.h" 

int press_time = 0;

// ==========================================
// 1. VARIABEL & OBJEK GAME FLAPPY BIRD
// ==========================================
lv_obj_t * bird_obj;
lv_obj_t * pipe_top;
lv_obj_t * pipe_bottom;
lv_obj_t * score_label;
lv_obj_t * game_over_label;

float bird_y = 140.0;
float velocity = 0.0;

// [SUPER EASY MODE] Pengaturan Fisika
const float gravity = 0.2;        // Jatuh super lambat (seperti di bulan)
const float jump_strength = -3.8; // Lompatan sangat halus dan pendek

int pipe_x = 240;
int pipe_gap_y = 80;
const int pipe_gap_size = 140;    // Celah raksasa (setengah tinggi layar)
const int pipe_width = 45;
const int pipe_speed = 2;         // Pipa berjalan sangat pelan (siput)

int score = 0;
bool game_over = false;
unsigned long last_jump_time = 0;

// [SUPER EASY MODE] Sensitivitas Ayunan/Hentakan
const float JUMP_GYRO_THRESHOLD = 30.0; // Senggol/ayun sedikit saja sudah lompat

// ==========================================
// 2. FUNGSI LOGIKA GAME
// ==========================================
void reset_game() {
    bird_y = 140.0;
    velocity = 0.0;
    pipe_x = 240;
    score = 0;
    game_over = false;
    pipe_gap_y = random(20, 120); 
    lv_label_set_text_fmt(score_label, "Score: %d", score);
    lv_obj_add_flag(game_over_label, LV_OBJ_FLAG_HIDDEN); 
}

void FlappyBird_Init() {
    lv_obj_clean(lv_scr_act());
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x87CEEB), 0); 

    pipe_top = lv_obj_create(lv_scr_act());
    lv_obj_set_style_bg_color(pipe_top, lv_color_hex(0x00FF00), 0);
    lv_obj_set_style_border_width(pipe_top, 2, 0);
    lv_obj_set_style_radius(pipe_top, 0, 0);

    pipe_bottom = lv_obj_create(lv_scr_act());
    lv_obj_set_style_bg_color(pipe_bottom, lv_color_hex(0x00FF00), 0);
    lv_obj_set_style_border_width(pipe_bottom, 2, 0);
    lv_obj_set_style_radius(pipe_bottom, 0, 0);

    bird_obj = lv_obj_create(lv_scr_act());
    lv_obj_set_size(bird_obj, 24, 24);
    lv_obj_set_style_bg_color(bird_obj, lv_color_hex(0xFFFF00), 0);
    lv_obj_set_style_radius(bird_obj, 12, 0); 

    score_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(score_label, &lv_font_montserrat_18, 0);
    lv_label_set_text(score_label, "Score: 0");
    lv_obj_align(score_label, LV_ALIGN_TOP_MID, 0, 20);

    game_over_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(game_over_label, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(game_over_label, lv_color_hex(0xFF0000), 0);
    lv_label_set_text(game_over_label, "GAME OVER\nAyun utk Reset");
    lv_obj_set_style_text_align(game_over_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(game_over_label, LV_ALIGN_CENTER, 0, 0);
    
    reset_game();
}

void update_flappy_bird() {
    float acc[3], gyro[3];
    unsigned int tim_count;
    QMI8658_read_xyz(acc, gyro, &tim_count); 
    
    float gyro_y = gyro[1]; 

    if (!game_over) {
        if ((gyro_y > JUMP_GYRO_THRESHOLD || gyro_y < -JUMP_GYRO_THRESHOLD) && (millis() - last_jump_time > 300)) {
            velocity = jump_strength;
            last_jump_time = millis();
        }

        velocity += gravity;
        bird_y += velocity;

        if (bird_y < 0) bird_y = 0;
        if (bird_y > 280 - 24) game_over = true;

        pipe_x -= pipe_speed;
        if (pipe_x < -pipe_width) {
            pipe_x = 240;
            pipe_gap_y = random(20, 120);
            score++;
            lv_label_set_text_fmt(score_label, "Score: %d", score);
        }

        if (pipe_x <= 40 + 20 && pipe_x + pipe_width >= 40 + 4) {
            if (bird_y + 4 < pipe_gap_y || bird_y + 20 > pipe_gap_y + pipe_gap_size) {
                game_over = true;
            }
        }

        lv_obj_set_pos(bird_obj, 40, (int)bird_y);
        
        lv_obj_set_pos(pipe_top, pipe_x, 0);
        lv_obj_set_size(pipe_top, pipe_width, pipe_gap_y);
        
        lv_obj_set_pos(pipe_bottom, pipe_x, pipe_gap_y + pipe_gap_size);
        lv_obj_set_size(pipe_bottom, pipe_width, 280 - (pipe_gap_y + pipe_gap_size));

    } else {
        lv_obj_clear_flag(game_over_label, LV_OBJ_FLAG_HIDDEN); 
        
        if (gyro_y > JUMP_GYRO_THRESHOLD || gyro_y < -JUMP_GYRO_THRESHOLD) {
            reset_game();
            delay(500); 
        }
    }
}

// ==========================================
// 3. FUNGSI POWER OFF (SHUTDOWN)
// ==========================================
void power_off_device() {
    // 1. Tampilkan pesan mati di layar
    lv_obj_clean(lv_scr_act());
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x000000), 0); // Layar Hitam
    
    lv_obj_t * pwroff_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(pwroff_label, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(pwroff_label, lv_color_hex(0xFFFFFF), 0);
    lv_label_set_text(pwroff_label, "Powering Off...");
    lv_obj_align(pwroff_label, LV_ALIGN_CENTER, 0, 0);
    
    lv_timer_handler(); // Paksa render ke layar
    delay(800); // Tahan agar tulisan terbaca sebentar

    // 2. Matikan Backlight (Lampu LCD)
    DEV_SET_PWM(0); 
    
    // 3. Putus arus dari baterai
    DEV_Digital_Write(BAT_PWR_PIN, 0); 
    
    // 4. Masuk ke infinite loop (berjaga-jaga jika dicolok USB)
    while(1) {
        delay(100);
    }
}

// ==========================================
// 4. SETUP & MAIN LOOP
// ==========================================
void setup()
{
    if (DEV_Module_Init() != 0)
        Serial.println("GPIO Init Fail!");
    else
        Serial.println("GPIO Init successful!");
        
    Serial.printf("1.69inch LCD demo...\r\n");
    LCD_1IN69_Init(VERTICAL);
    DEV_SET_PWM(60);
    
    CST816S_init(CST816S_ALL_Mode);
    PCF85063A_Init();
    QMI8658_init();
    
    LVGL_Init();
    
    FlappyBird_Init(); 
}

void loop()
{
    update_flappy_bird(); 

    lv_timer_handler(); 
    delay(5); 
    
    // Logika Tombol Power
    if(DEV_Digital_Read(PWR_KEY_PIN) == 0)
    {
        press_time++;
        // Jika ditekan lebih dari ~1 detik (200 * 5ms)
        if(press_time > 200)
        {
            press_time = 0;
            power_off_device(); // Panggil fungsi shutdown
        }
    }
    else
    {
        press_time = 0; // Reset counter jika tombol dilepas
    }
}