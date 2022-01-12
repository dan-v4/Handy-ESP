#include <string.h>
#include <stdio.h>
#include "ILI9341.h"
#include "sys_constants.h"


static char* path_for_menu();
static char* path_for_settings();
static void menu(char* path);
static void update_menu_scroll(uint8_t scroll);
static void colour_menu(char *path);
static void colourset_ss_update(int scroll, bool inc);
static void set_bgc();
static void set_ftc();



uint8_t list_count;
int cur_list_index;

int page = 0;
int scroll = 1;
int side_scroll = 1;
int max_show = 4;

uint8_t temp_red = 0;
uint8_t temp_green = 0;
uint8_t temp_blue = 0;

void menu_up_handler(){
    if(scroll == 1){
        scroll = max_show;
    }else if(scroll > 1 && scroll <= max_show){
        scroll--;
    }
    update_menu_scroll(scroll);
}

void menu_left_handler(){
    if(page == 31 || page == 32){
        colourset_ss_update(scroll,false);
    }
}

void menu_down_handler(){
    if(scroll == max_show){
        scroll = 1;
    }else if(scroll >= 1 && scroll < max_show){
        scroll++;
    }
    update_menu_scroll(scroll);
}

void menu_right_handler(){
    if(page == 31 || page == 32){
        colourset_ss_update(scroll,true);
    }
}

void menu_select_handler(){

}

void menu_start_handler(){

}

void menu_a_handler(){
    if(page == 0){
        menu(path_for_menu());
        page = scroll;
        scroll = 1;
        update_menu_scroll(scroll);
    }else if(page == 3){
        colour_menu(path_for_settings());
        page = 3*10 + scroll;
        scroll = 1;
        update_menu_scroll(scroll);
    }else if(page == 31){
        set_bgc();
        update_menu_scroll(scroll);
    }else if(page == 32){
        set_ftc();
        update_menu_scroll(scroll);
    }
}

void menu_b_handler(){
    if(page > 0 && page <= 3){
        scroll = page;
        page = 0;
        menu(menul_path);
        update_menu_scroll(scroll);
    }else if(page > 3*10){
        scroll = page - 3*10;
        page = 3;
        menu(settingsl_path);
        update_menu_scroll(scroll);
    }
}

static char* path_for_menu(){
    switch(scroll){
        case 1:
            return appl_path;
            break;
        case 2:
            return filel_path;
            break;
        case 3:
            return settingsl_path;
            break;
    }
    return "";
}

static char* path_for_settings(){
    switch(scroll){
        case 1:
            return bgs_path;
            break;
        case 2:
            return fts_path;
            break;
    }
    return "";
}

static void menu(char* path){
    fill_screen(background);
    //spi_device_acquire_bus(3);
    char select[3] = "  ";
    char unselect[3] = "  ";
    FILE* appl = fopen(path, "rb");
    if(appl == NULL){
        fclose(appl);
        return;
    }
    max_show = 4;
    cur_list_index = 0;
    char count_char[20];
    if(!fgets(count_char, sizeof(count_char), appl)){
        fclose(appl);
        return;
    }
    char temp_name[12];
    char temp_name2[14];
    uint32_t count = atoi(count_char);
    if(count > 255){
        list_count = 255;
    }else{
        list_count = count;
    }
    char apps_txt[20];
    char ss_txt [] = "<>";
    if(!fgets(apps_txt, sizeof(apps_txt), appl)){
        fclose(appl);
         return;
    }
    draw_string_no_newline(0, 0, apps_txt, font_colour, font_size, sizeof(apps_txt) / sizeof(apps_txt[0]));
    draw_string_no_newline(LCD_LENGTH-3*5*font_size+(5*font_size-(font_size-1)), 0, ss_txt, font_colour, font_size, sizeof(ss_txt) / sizeof(ss_txt[0]));
    if(count == 0){
        char empty[8] = "<EMPTY>";
        draw_string_no_newline(20 , 12*font_size, empty, font_colour, font_size, sizeof(empty) / sizeof(empty[0]));
        fclose(appl);
        return;
    }
    for(int i = cur_list_index; i < list_count; i++){
        if(fgets(temp_name, sizeof(temp_name), appl) != NULL ){
            //puts(temp_name);
            if(i == 0){
                snprintf(temp_name2, sizeof(temp_name2),"%s%s", select, temp_name);
            }else{
                snprintf(temp_name2, sizeof(temp_name2),"%s%s", unselect, temp_name);
            }
            //puts(temp_name2);
            draw_string_no_newline(20 , 12*font_size + i*8*font_size + 5, temp_name2, font_colour, font_size, sizeof(temp_name2) / sizeof(temp_name2[0]));
            if(i >= cur_list_index + max_show){
                break;
            }
        }
    }
    if(list_count > max_show){
        char sel_func[7] = "SRCH";
        char str_func[5] = "INFO";
        draw_string_no_newline(0 , LCD_WIDTH-8*font_size, sel_func, font_colour, font_size, sizeof(sel_func)/sizeof(sel_func[0]));
        draw_string_no_newline(LCD_LENGTH-(4*5*(font_size) + 3*font_size) , LCD_WIDTH-8*font_size, str_func, font_colour, font_size, sizeof(str_func)/sizeof(str_func[0]));
    }
    if(list_count <= max_show){
        cur_list_index = -1;
        max_show = list_count;
    }else{
        max_show = 4;
        cur_list_index = cur_list_index + max_show;
    }
    fclose(appl);
}

static void update_menu_scroll(uint8_t scroll){
    if(list_count != 0){
        char select[] = "> ";
        size_t se_size = sizeof(select) / sizeof(select[0]);
        uint16_t x0 = 20;
        uint16_t x1 = x0 + 5*font_size;
        uint16_t y0 = 12*font_size + 5;
        uint16_t y1 = y0 + 4*8*font_size;
        draw_rect(x0, y0, x1,y1 + 5*font_size, background);
        draw_string_no_newline(x0, y0 + (scroll-1)*8*font_size, select, font_colour, font_size, se_size);
    }
}

static void colour_menu(char *path){
    fill_screen(background);
    FILE* colour_settings = fopen(path, "rb");
    char bg[20] = "";
    fgets(bg, sizeof(bg), colour_settings);
    list_count = 3;
    max_show = 3;
    char red[10] = "  R:";
    char green[10] = "  G:";
    char blue[10] = "  B:";
    char r_value[25] = "";
    char g_value[25] = "";
    char b_value[25] = "";
    char r[10] = "";
    char g[10] = "";
    char b[10] = "";
    fgets(r, sizeof(r), colour_settings);
    fgets(g, sizeof(g), colour_settings);
    fgets(b, sizeof(b), colour_settings);
    snprintf(r_value, sizeof(r_value), "%s  %s", red, r);
    snprintf(g_value, sizeof(g_value), "%s  %s", green, g);
    snprintf(b_value, sizeof(b_value), "%s  %s", blue,b);
    temp_red = strtol(r, NULL,10 );
    temp_green = strtol(g, NULL,10 );
    temp_blue = strtol(b, NULL,10 );
    draw_string_no_newline(0, 0, bg, font_colour, font_size, sizeof(bg) / sizeof(bg[0]));
    draw_string_no_newline(20, 12*font_size + 5, r_value, font_colour, font_size, sizeof(r_value) / sizeof(r_value[0]));
    draw_string_no_newline(20, 12*font_size + 8*font_size + 5, g_value, font_colour, font_size, sizeof(g_value) / sizeof(g_value[0]));
    draw_string_no_newline(20, 12*font_size + 2*8*font_size+5, b_value, font_colour, font_size, sizeof(b_value) / sizeof(b_value[0]));
    fclose(colour_settings);
}


static void colourset_ss_update(int scroll, bool inc){
    int increment;
    if(inc){
        increment = 1;
    }else{
        increment = -1;
    }
    char value[9] = "";
    if(scroll == 1){
        if(temp_red + increment >= 256){
            temp_red = 0;
        }else if(temp_red + increment < 0){
            temp_red = 255;
        }else{
            temp_red = temp_red + increment;
        }
        snprintf(value, sizeof(value), "%d", temp_red);        
    }else if(scroll == 2){
        if(temp_green + increment >= 256){
            temp_green = 0;
        }else if(temp_green + increment < 0){
            temp_green = 255;
        }else{
            temp_green = temp_green + increment;
        }
        snprintf(value, sizeof(value), "%d", temp_green);
    }else if(scroll == 3){
        if(temp_blue + increment >= 256){
            temp_blue = 0;
        }else if(temp_blue + increment < 0){
            temp_blue = 255;
        }else{
            temp_blue = temp_blue + increment;
        }
        snprintf(value, sizeof(value), "%d", temp_blue);
    }
    draw_string_no_newline(40*font_size-2, 12*font_size + (scroll-1)*8*font_size + 5, value, font_colour, font_size, sizeof(value) / sizeof(value[0]));
}

static void set_bgc(){
    background = (uint16_t)temp_red << 10 | (uint16_t) temp_green <<4 | (uint16_t)temp_blue << 2;
    FILE* bg = fopen(bgs_path, "w+");
    fprintf(bg, "BACKGROUND\n");
    fprintf(bg, "%d\n", temp_red);
    fprintf(bg, "%d\n", temp_green);
    fprintf(bg, "%d\n", temp_blue);
    fclose(bg);
    colour_menu(bgs_path);
}

static void set_ftc(){
    font_colour = (uint16_t)temp_red << 10 | (uint16_t)temp_green << 4| (uint16_t)temp_blue << 2;
    FILE* ft = fopen(fts_path, "w+");
    fprintf(ft, "FONT COLOUR\n");
    fprintf(ft, "%d\n", temp_red);
    fprintf(ft, "%d\n", temp_green);
    fprintf(ft, "%d\n", temp_blue);
    fclose(ft);
    colour_menu(fts_path);
}

void displays_startup(uint16_t background){
    ili_startup();
    menu(menul_path);
    update_menu_scroll(scroll);
}