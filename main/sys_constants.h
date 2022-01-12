#ifndef SYS_CONST
#define SYS_CONST

#define UP_B 39
#define LEFT_B 36
#define DOWN_B 34
#define RIGHT_B 35
#define SELECT_B 32
#define START_B 27
#define A_B 14
#define B_B 13	

#define PIN_NUM_MISO              19
#define PIN_NUM_MOSI              23
#define PIN_NUM_CLK               18
#define SD_CS                     17
#define MOUNT_POINT               "/sdcard"
#if CONFIG_IDF_TARGET_ESP32S2
#define SPI_DMA_CHAN              host.slot
#elif CONFIG_IDF_TARGET_ESP32C3
#define SPI_DMA_CHAN                SPI_DMA_CH_AUTO
#else
#define SPI_DMA_CHAN                1
#endif
#define HOST_SLOT                SPI3_HOST

#define menul_path  "/sdcard/sys/menulist.txt"
#define appl_path   "/sdcard/sys/applist.txt"
#define filel_path  "/sdcard/sys/filelist.txt"
#define settingsl_path  "/sdcard/sys/settings.txt"
#define bgs_path        "/sdcard/sys/settings/background.txt"
#define fts_path        "/sdcard/sys/settings/font.txt"
#define base_menu   "3\nMENU\nApps\nFiles\nSettings\n"
#define base_apps   "3\nAPPS\nSimpleTxT\nCalculator\nPaintPad\n"
#define base_files  "0\nFILES\n"
#define base_settings   "2\nSETTINGS\nBackground\nFont Colour\n"
#define base_set_bg     "BACKGROUND\n0\n0\n0\n"
#define base_set_ft     "FONT COLOUR\n63\n63\n63"


#define LCD_WIDTH                 240
#define LCD_LENGTH                0b101000000


#define RED_MASK                  0b111111000000000000
#define GREEN_MASK                0b000000111111000000
#define BLUE_MASK                 0b000000000000111111

#define WHITE_COLOUR              0xFFFFFF
#define font_size                 4


uint32_t background;
uint32_t font_colour;


#endif

