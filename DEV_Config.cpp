/*****************************************************************************
* | File      	:   DEV_Config.c
* | Author      :
* | Function    :   Hardware underlying interface
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2025-04-22
* | Info        :
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of theex Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
******************************************************************************/
#include "DEV_Config.h"

uint bl_slice_num;
uint beep_slice_num;
uint dma_tx;
dma_channel_config c;

/**
 * GPIO read and write
 **/
void DEV_Digital_Write(uint16_t Pin, uint8_t Value)
{
    digitalWrite(Pin, Value);
}

uint8_t DEV_Digital_Read(uint16_t Pin)
{
    return digitalRead(Pin);
}

/**
 * SPI
 **/
void DEV_SPI_WriteByte(uint8_t Value)
{
    SPI1.transfer(Value);
}

void DEV_SPI_Write_nByte(uint8_t pData[], uint32_t Len)
{
     SPI1.transfer(pData, Len);
}

/**
 * I2C
 **/
void DEV_I2C_Write_Byte(uint8_t addr, uint8_t reg, uint8_t Value)
{
    Wire1.beginTransmission(addr);
    Wire1.write(reg);
    Wire1.write(Value);
    Wire1.endTransmission();
}

void DEV_I2C_Write_Register(uint8_t addr, uint8_t reg, uint16_t value)
{
    uint8_t tmpi[3];
    tmpi[0] = reg;
    tmpi[1] = (value >> 8) & 0xFF;
    tmpi[2] = value & 0xFF;
    Wire1.beginTransmission(addr);
    Wire1.write(tmpi,3);
    Wire1.endTransmission();
}

void DEV_I2C_Write_nByte(uint8_t addr,uint8_t *pData, uint32_t Len)
{
    Wire1.beginTransmission(addr);
    Wire1.write(pData,Len);
    Wire1.endTransmission();
}

uint8_t DEV_I2C_Read_Byte(uint8_t addr, uint8_t reg)
{
    uint8_t value;
  
    Wire1.beginTransmission(addr);
    Wire1.write((byte)reg);
    Wire1.endTransmission();
  
    Wire1.requestFrom(addr, (byte)1);
    value = Wire1.read();
  
    return value;
}

void DEV_I2C_Read_Register(uint8_t addr, uint8_t reg, uint16_t *value)
{
    uint8_t tmpi[2];
    
    Wire1.beginTransmission(addr);
    Wire1.write(reg);
    // Wire1.endTransmission();
    Wire1.requestFrom(addr, 2);
  
    uint8_t i = 0;
    for(i = 0; i < 2; i++) {
      tmpi[i] =  Wire1.read();
    }
    Wire1.endTransmission();
    *value = (((uint16_t)tmpi[0] << 8) | (uint16_t)tmpi[1]);
}

void DEV_I2C_Read_nByte(uint8_t addr, uint8_t reg, uint8_t *pData, uint32_t Len)
{
    Wire1.beginTransmission(addr);
    Wire1.write(reg);
    Wire1.endTransmission();
    
    Wire1.requestFrom(addr, Len);
  
    uint8_t i = 0;
    for(i = 0; i < Len; i++) {
      pData[i] =  Wire1.read();
    }
    Wire1.endTransmission();
}

/**
 * GPIO Mode
 **/
void DEV_GPIO_Mode(uint16_t Pin, uint16_t Mode)
{

    if (Mode == 0 || Mode == GPIO_IN)
    {
        pinMode(Pin, INPUT);
    }
    else
    {
        pinMode(Pin, OUTPUT);
    }
}

/**
 * KEY Config
 **/
void DEV_KEY_Config(uint16_t Pin)
{
    pinMode(Pin,INPUT_PULLUP);
}

/*
** PWM
*/
void DEV_SET_PWM(uint8_t Value)
{
    if (Value < 0 || Value > 100)
    {
        printf("DEV_SET_PWM Error \r\n");
    }
    else
    {
        pwm_set_chan_level(bl_slice_num, PWM_CHAN_B, Value);
    }
}

/**
 * delay x ms
 **/
void DEV_Delay_ms(uint32_t xms)
{
    delay(xms);
}

void DEV_Delay_us(uint32_t xus)
{
    delayMicroseconds(xus);
}

void DEV_GPIO_Init(void)
{
    DEV_GPIO_Mode(LCD_RST_PIN, 1);
    DEV_GPIO_Mode(LCD_DC_PIN, 1);
    DEV_GPIO_Mode(LCD_CS_PIN, 1);
    DEV_GPIO_Mode(Touch_RST_PIN, 1);
    DEV_GPIO_Mode(BAT_PWR_PIN, 1);
    DEV_GPIO_Mode(BEEP_PIN, 1);

    DEV_Digital_Write(LCD_CS_PIN, 1);
    DEV_Digital_Write(LCD_DC_PIN, 0);
    DEV_Digital_Write(BAT_PWR_PIN, 1);
}

/**
 * IRQ
 **/
void DEV_IRQ_SET(uint gpio, uint32_t events, gpio_irq_callback_t callback)
{
    gpio_set_irq_enabled_with_callback(gpio,events,true,callback);
}

/******************************************************************************
function:	Module Initialize, the library and initialize the pins, SPI protocol
parameter:
Info:
******************************************************************************/
uint8_t DEV_Module_Init(void)
{
    Serial.begin(115200);
    DEV_Delay_ms(100);
    // GPIO Config
    DEV_GPIO_Init();
    gpio_set_function(LCD_BL_PIN, GPIO_FUNC_PWM);
    bl_slice_num = pwm_gpio_to_slice_num(LCD_BL_PIN);
    pwm_set_wrap(bl_slice_num, 100);
    pwm_set_chan_level(bl_slice_num, PWM_CHAN_B, 0);
    pwm_set_clkdiv(bl_slice_num, 50);
    pwm_set_enabled(bl_slice_num, true);
    // PWM Config -- BEEP
    gpio_set_function(BEEP_PIN, GPIO_FUNC_PWM);
    beep_slice_num = pwm_gpio_to_slice_num(BEEP_PIN);
    pwm_set_wrap(beep_slice_num, 2000);
    pwm_set_chan_level(beep_slice_num, PWM_CHAN_A, 1000);
    pwm_set_clkdiv(beep_slice_num, 200);
    pwm_set_enabled(beep_slice_num, false);
    // SPI Config
    SPI1.setRX(LCD_MISO_PIN);
    SPI1.setCS(LCD_CS_PIN);
    SPI1.setSCK(LCD_CLK_PIN);
    SPI1.setTX(LCD_MOSI_PIN);
    SPI1.begin();
    SPI1.beginTransaction(SPISettings(66500000, MSBFIRST, SPI_MODE0));
    // DMA Config
    dma_tx = dma_claim_unused_channel(true);
    c = dma_channel_get_default_config(dma_tx);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8); 
    channel_config_set_dreq(&c, spi_get_dreq(LCD_SPI_PORT, true));
    // I2C Config
    Wire1.setSDA(DEV_SDA_PIN);
    Wire1.setSCL(DEV_SCL_PIN);
    Wire1.setClock(400 * 1000);
    Wire1.begin();
    // KEY
    DEV_KEY_Config(PWR_KEY_PIN);
    // IRQ Config
    DEV_KEY_Config(Touch_INT_PIN);
    DEV_KEY_Config(RTC_INT_PIN);

    Serial.println("DEV_Module_Init OK");
    return 0;
}

/******************************************************************************
function:	Module exits, closes SPI and BCM2835 library
parameter:
Info:
******************************************************************************/
void DEV_Module_Exit(void)
{
    SPI1.end();
    Wire1.end();
}
