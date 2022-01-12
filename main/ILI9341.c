#include <string.h>
#include <dirent.h>
#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "freertos/task.h"
#include "ILI9341.h"
#include "screen_constants.h"
#include "sys_constants.h"


static void send_cmd(const uint8_t cmd);
static void send_data(const uint8_t *data, int len);


spi_device_handle_t ili9341;

DRAM_ATTR static const screen_init_cmd_t ili_init_cmd[]={
    {SWRESET, {0}, 0, 250},
    {PWCTRLA, {0x39, 0x2C, 0x00, 0x34, 0x02}, 5, 0},
    {PWCTRLB, {0x00, 0xC1, 0x30}, 3, 0},
    {DTCTRLA, {0x85, 0x00, 0x78}, 3, 0},
    {DTCTRLB, {0x00, 0x00} , 2, 0},
    {PWSCTRL, {0x64, 0x03, 0x12, 0x81}, 4, 0},
    {PRCTRL, {0x20}, 1, 0},
    {PWCTRL1, {0x23}, 1, 0},
    {PWCTRL2, {0x10}, 1, 0},
    {VMCTRL1, {0x3E, 0x28}, 2, 0},
    {VMCTRL2, {0x86}, 2, 0},
    
    {MADCTL, {MADCTL_PARAM}, 1, 0},
    {PIXSET, {0x66}, 1, 0},
    {DINVOFF, {0}, 0 , 0},
    {FRMCTR1, {0x00, 0x18}, 2 , 0},
    {DISCTRL, {0x08, 0x82, 0x27, 0x00}, 4, 0},
    {NORON, {0}, 0, 10},

    {EN3G, {0x00}, 1, 0},
    {GAMSET, {0x01}, 1, 0},
    {PGAMCTRL, {0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00}, 15, 0},
    {NGAMCTRL, {0x00 , 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F}, 15, 0},

    {SLPOUT, {0}, 0, 100},
    {DISPON, {0}, 0 , 100},
    {NOP, {0}, 0 , 10}
};

static void send_cmd(const uint8_t cmd){
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=8;                     //Command is 8 bits
    t.tx_buffer=&cmd;               //The data is the cmd itself
    t.user=(void*)0;                //D/C needs to be set to 0
    ret=spi_device_polling_transmit(ili9341, &t);  //Transmit!
    assert(ret==ESP_OK);            //Should have had no issues.
    //printf("cmd: %d\n", t.length);
}

static void send_data(const uint8_t *data, int len){
    esp_err_t ret;
    spi_transaction_t t;
    if (len==0) return;             //no need to send anything
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=len*8;                 //Len is in bytes, transaction length is in bits.
    //printf("data: %d\n", t.length);
    t.tx_buffer=data;               //Data
    t.user=(void*)1;                //D/C needs to be set to 1
    ret=spi_device_polling_transmit(ili9341, &t);  //Transmit!
    assert(ret==ESP_OK);            //Should have had no issues.
}


void ili_set_address_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
  uint16_t abs_x0 = abs(x0-319);
  uint16_t abs_x1 = abs(x1-319);
  uint16_t n_x0 = abs_x0;
  uint16_t n_x1 = abs_x1;
  if(abs_x0 > abs_x1){
    n_x0 = abs_x1;
    n_x1 = abs_x0;
  }else{
    n_x0 = abs_x0;
    n_x1 = abs_x1;
  }
  uint8_t columnStartMSB[1] = {n_x0 >> 8};
  uint8_t columnStartLSB[1] = {n_x0};

  uint8_t columnEndMSB[1] = {n_x1 >> 8};
  uint8_t columnEndLSB[1] = {n_x1};

  uint8_t rowStartMSB[1] = {y0 >> 8};
  uint8_t rowStartLSB[1] = {y0};

  uint8_t rowEndMSB[1] = {y1 >> 8};
  uint8_t rowEndLSB[1] = {y1};
  //printf("n_x0: %d n_x1: %d\n", n_x0, n_x1);
  send_cmd(CASET); // Column addr set
  send_data(columnStartMSB, 1);
  send_data(columnStartLSB, 1);     // XSTART
  send_data(columnEndMSB, 1);
  send_data(columnEndLSB, 1);     // XEND

  send_cmd(PASET); // Row addr set
  send_data(rowStartMSB, 1);
  send_data(rowStartLSB, 1);     // YSTART
  send_data(rowEndMSB, 1);
  send_data(rowEndLSB, 1);     // YEND

  send_cmd(RAMWR); // write to RAM
}

void push_color(uint32_t color) {
  uint8_t red[1] = {(uint8_t)((color & RED_MASK) >> 10)};
  uint8_t green[1] = {(uint8_t)((color & GREEN_MASK) >> 4)};
  uint8_t blue[1] = {(uint8_t)(color & BLUE_MASK << 2)};
  send_data(red, 1);
  send_data(green, 1);
  send_data(blue, 1);
}

void draw_pixel(uint8_t x, uint8_t y, uint32_t colour){
  ili_set_address_window(x, y, x, y);
  push_color(colour);
}

void draw_char(int16_t x, int16_t y, char c, uint32_t textColor, uint8_t size){
  uint8_t line; // horizontal row of pixels of character
  if(((x + 5*size - 1) >= LCD_LENGTH)  || // Clip right
     ((y + 8*size - 1) >= LCD_WIDTH) || // Clip bottom
     ((x + 5*size - 1) < 0)        || // Clip left
     ((y + 8*size - 1) < 0)){         // Clip top
    return;
  }
  uint8_t red_val[2] = {(uint8_t)((textColor & RED_MASK) >> 10), (uint8_t)((background & RED_MASK) >> 10)};
  uint8_t green_val[2] = {(uint8_t)((textColor & GREEN_MASK) >> 4), (uint8_t)((background & GREEN_MASK) >> 4)};
  uint8_t blue_val[2] = {(uint8_t)((textColor & BLUE_MASK) << 2), (uint8_t)(background & BLUE_MASK << 2)};
  uint8_t char_buffer[5*size*8*size][3];
  line = 0x01;
  uint16_t buffer_ind = 0;
  ili_set_address_window(x, y, x + 5*size-1, y+8*size-1);
  for(int i = y; i < y+8*size; i+=size){
    for(int j = 0; j < size; j++){
      //ili_set_address_window(x, i+j, x + 5*size, i+j);
      for(int col = 4; col >= 0; col--){
        uint8_t pixel_type = 1;
        if(Font[(c*5)+col]&line){
          pixel_type = 0;
        }
        for(int k = 0; k < size; k++){
          char_buffer[buffer_ind][0] = red_val[pixel_type];
          char_buffer[buffer_ind][1] = green_val[pixel_type];
          char_buffer[buffer_ind][2] = blue_val[pixel_type];
          buffer_ind+=1;
        }
      } 
    }
    line = line<<1;
  }
  spi_device_acquire_bus(ili9341,portMAX_DELAY);
  send_data(char_buffer, sizeof(char_buffer));
  spi_device_release_bus(ili9341);
}


uint32_t draw_string_no_newline(uint16_t x, uint16_t y, char *pt, uint32_t textColor, int8_t size, size_t array_size){
  //printf("draw string\n");
  uint32_t count = 0;
  uint16_t x_ind = x;
  uint16_t y_ind = y;
  uint16_t length = (uint16_t)array_size;
  for(int i = 0; i < length-1; i++){
    if(pt[i] == 10){
      break;
    }
    if(pt[i] != ' '){
      draw_char(x_ind, y_ind, pt[i], textColor, size);
    }
    x_ind = (x_ind + 6*size-1);
    count++;
  }
  return count;  // number of characters printed
}

void fill_screen(uint32_t textColor){
//  printf("fill screen\n");
  uint16_t xstart = 0;
  uint16_t xend = LCD_LENGTH-1;
  uint16_t ystart = 0;
  uint16_t yend = LCD_WIDTH-1;
  uint8_t clear_frame[LCD_LENGTH*2][3];
  spi_device_acquire_bus(ili9341,portMAX_DELAY);
  for(int j = 0; j < LCD_WIDTH; j+=2){
    //push_color(textColor);
    ili_set_address_window(xstart, j, xend, j+1);
    for(int i = 0; i < LCD_LENGTH*2; i++){
      uint8_t red = {(uint8_t)((textColor & RED_MASK) >> 10)};
      uint8_t green = {(uint8_t)((textColor & GREEN_MASK) >> 4)};
      uint8_t blue = {(uint8_t)(textColor & BLUE_MASK << 2)};
      clear_frame[i][0] = red;
      clear_frame[i][1] = green;
      clear_frame[i][2] = blue;
    }
    send_data(clear_frame, LCD_LENGTH*3*2);
  }
  spi_device_release_bus(ili9341);
}

void draw_rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t textColor){
//  printf("fill screen\n");
  uint16_t rect_length = x1 - x0 + 1;
  uint16_t rect_width = y1 - y0 + 1;
  uint8_t clear_frame[rect_length*2][3];
  spi_device_acquire_bus(ili9341,portMAX_DELAY);
  for(int j = y0; j <= y1; j+=2){
    ili_set_address_window(x0, j, x1, j+1);
    for(uint8_t i = 0; i < rect_length*2; i+=1){
      uint8_t red = {(uint8_t)((textColor & RED_MASK) >> 10)};
      uint8_t green = {(uint8_t)((textColor & GREEN_MASK) >> 4)};
      uint8_t blue = {(uint8_t)(textColor & BLUE_MASK << 2)};
      clear_frame[i][0] = red;
      clear_frame[i][1] = green;
      clear_frame[i][2] = blue;
    }
    send_data(clear_frame, rect_length*3*2);
  }
  spi_device_release_bus(ili9341);
}

void lcd_spi_pre_transfer_callback(spi_transaction_t *t){
    int dc=(int)t->user;
    gpio_set_level(A0, dc);
}

void ili_startup(){
  //frame_buffer = malloc(sizeof(LCD_LENGTH*LCD_WIDTH*3));
  esp_err_t ret;
  spi_device_interface_config_t devcfg={
        .clock_speed_hz=SPI_MASTER_FREQ_16M,
        .mode=0,                                //SPI mode 0
        .spics_io_num=LCD_CS,               //CS pin
        .queue_size=7,                          //We want to be able to queue 7 transactions at a time
        .pre_cb=lcd_spi_pre_transfer_callback,  //Specify pre-transfer callback to handle D/C line
  };
  //ret=spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO);
  //ESP_ERROR_CHECK(ret);
  ret=spi_bus_add_device(LCD_HOST, &devcfg, &ili9341);
  ESP_ERROR_CHECK(ret);
  int cmd=0;
  const screen_init_cmd_t* lcd_init_cmds;

  gpio_set_direction(A0, GPIO_MODE_OUTPUT);
  gpio_set_direction(RESET, GPIO_MODE_OUTPUT);
  gpio_set_direction(BACKLIGHT, GPIO_MODE_OUTPUT);

  gpio_set_level(RESET, 0);
  vTaskDelay(150 / portTICK_RATE_MS);
  gpio_set_level(RESET, 1);
  vTaskDelay(150 / portTICK_RATE_MS);
  lcd_init_cmds = ili_init_cmd;
  spi_device_acquire_bus(ili9341,portMAX_DELAY);
  while (lcd_init_cmds[cmd].cmd !=0x00) {
    send_cmd(lcd_init_cmds[cmd].cmd);
    //printf("sent command: %x", lcd_init_cmds[cmd].cmd);
    send_data(lcd_init_cmds[cmd].data, lcd_init_cmds[cmd].databytes);
    if (lcd_init_cmds[cmd].delay != 0) {
      vTaskDelay(lcd_init_cmds[cmd].delay / portTICK_RATE_MS);
    }
    cmd++;
  }
  spi_device_release_bus(ili9341);
  gpio_set_level(BACKLIGHT, 1);
}


