#include "lcd/lcd.h"
#include <string.h>
#include "utils.h"

#define LAST_LEVEL  3
#define EMPTY       0
#define WALL        1
#define PLAYER      2
#define TARGET      3
#define BOX         4
#define BOX_T       5

void Inp_init(void) {
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOC);

    gpio_init(GPIOA, GPIO_MODE_IPD, GPIO_OSPEED_50MHZ,
              GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_6);
    gpio_init(GPIOC, GPIO_MODE_IPD, GPIO_OSPEED_50MHZ, GPIO_PIN_13);
}

void IO_init(void) {
    Inp_init(); // inport init
    Lcd_Init(); // LCD init
}

void LCD_DrawGrid(u16 x, u16 y, u16 *p) {
    for (u16 i = 0; i < 6; ++i)
        for (u16 j = 0; j < 6; ++j)
            LCD_DrawPoint(x * 6 + i, y * 6 + j, p[i * 6 + j]);
}

typedef struct {
    unsigned init_grid[20][10], box_num;
    u16 pos_x, pos_y;
} Config;

int main(void) {
    IO_init();         // init OLED
    LCD_Clear(BLACK);
    u16 level = 1, box = 2, min_box = 1, max_box = 3, step = 0, done = 0, pos_x, pos_y, occupied = EMPTY;
    unsigned init_grid[3][20][10] = {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                                     EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                                     EMPTY, EMPTY, WALL, WALL, WALL, WALL, WALL, WALL, WALL, EMPTY,
                                     EMPTY, EMPTY, WALL, EMPTY, EMPTY, TARGET, EMPTY, EMPTY, WALL, EMPTY,
                                     EMPTY, EMPTY, WALL, EMPTY, TARGET, TARGET, BOX, EMPTY, WALL, EMPTY,
                                     EMPTY, EMPTY, WALL, WALL, EMPTY, BOX, EMPTY, EMPTY, WALL, EMPTY,
                                     EMPTY, EMPTY, EMPTY, WALL, WALL, WALL, EMPTY, EMPTY, WALL, EMPTY,
                                     EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL, EMPTY, EMPTY, WALL, EMPTY,
                                     EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL, EMPTY, EMPTY, WALL, EMPTY,
                                     EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                                     EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                                     WALL, WALL, WALL, EMPTY, PLAYER, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                                     EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                                     EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, BOX, WALL, WALL, WALL,
                                     EMPTY, EMPTY, EMPTY, WALL, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                                     EMPTY, EMPTY, EMPTY, WALL, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                                     EMPTY, EMPTY, EMPTY, WALL, WALL, WALL, EMPTY, EMPTY, EMPTY, EMPTY,
                                     EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL, EMPTY, EMPTY, EMPTY, EMPTY,
                                     EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL, EMPTY, EMPTY, EMPTY, EMPTY,
                                     EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL, EMPTY, EMPTY, EMPTY, EMPTY,
                                     EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                                     EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                                     EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL, EMPTY, EMPTY, EMPTY, EMPTY,
                                     WALL, WALL, EMPTY, EMPTY, EMPTY, WALL, EMPTY, EMPTY, EMPTY, EMPTY,
                                     EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL, EMPTY, EMPTY, EMPTY, EMPTY,
                                     EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL, EMPTY, EMPTY, EMPTY, EMPTY,
                                     EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL, PLAYER, EMPTY, EMPTY, EMPTY,
                                     EMPTY, EMPTY, EMPTY, WALL, WALL, WALL, WALL, WALL, WALL, WALL,
                                     EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                                     EMPTY, EMPTY, EMPTY, WALL, EMPTY, BOX, BOX, BOX, BOX, EMPTY,
                                     EMPTY, EMPTY, EMPTY, WALL, EMPTY, EMPTY, BOX, EMPTY, EMPTY, EMPTY,
                                     EMPTY, EMPTY, EMPTY, WALL, EMPTY, EMPTY, EMPTY, WALL, WALL, WALL,
                                     EMPTY, EMPTY, EMPTY, WALL, WALL, EMPTY, WALL, WALL, TARGET, TARGET,
                                     EMPTY, EMPTY, EMPTY, WALL, EMPTY, EMPTY, WALL, EMPTY, EMPTY, TARGET,
                                     EMPTY, EMPTY, EMPTY, WALL, EMPTY, EMPTY, EMPTY, TARGET, TARGET, TARGET,
                                     EMPTY, EMPTY, EMPTY, WALL, WALL, EMPTY, EMPTY, BOX, EMPTY, EMPTY,
                                     EMPTY, EMPTY, EMPTY, EMPTY, WALL, WALL, EMPTY, EMPTY, WALL, EMPTY,
                                     EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL, WALL, WALL, WALL, EMPTY,
                                     EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                                     EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                                     EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                                     EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                                     EMPTY, EMPTY, EMPTY, TARGET, EMPTY, TARGET, EMPTY, TARGET, EMPTY, TARGET,
                                     EMPTY, EMPTY, EMPTY, EMPTY, TARGET, BOX, TARGET, EMPTY, TARGET, EMPTY,
                                     EMPTY, EMPTY, EMPTY, TARGET, BOX, TARGET, WALL, WALL, WALL, WALL,
                                     EMPTY, EMPTY, EMPTY, EMPTY, TARGET, BOX, TARGET, EMPTY, TARGET, EMPTY,
                                     WALL, WALL, WALL, WALL, WALL, TARGET, BOX, TARGET, BOX, EMPTY,
                                     EMPTY, EMPTY, EMPTY, EMPTY, TARGET, BOX, TARGET, WALL, WALL, WALL,
                                     EMPTY, EMPTY, EMPTY, EMPTY, BOX, TARGET, BOX, TARGET, EMPTY, EMPTY,
                                     EMPTY, EMPTY, EMPTY, EMPTY, TARGET, BOX, TARGET, EMPTY, TARGET, EMPTY,
                                     WALL, WALL, WALL, WALL, WALL, WALL, BOX, BOX, BOX, EMPTY,
                                     EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                                     EMPTY, EMPTY, EMPTY, BOX, EMPTY, BOX, EMPTY, BOX, EMPTY, EMPTY,
                                     EMPTY, EMPTY, EMPTY, WALL, WALL, WALL, WALL, WALL, WALL, WALL,
                                     EMPTY, EMPTY, EMPTY, BOX, EMPTY, BOX, EMPTY, BOX, EMPTY, EMPTY,
                                     EMPTY, EMPTY, EMPTY, BOX, EMPTY, BOX, EMPTY, BOX, EMPTY, EMPTY,
                                     EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                                     EMPTY, EMPTY, WALL, WALL, WALL, WALL, WALL, WALL, WALL, EMPTY,
                                     EMPTY, EMPTY, WALL, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                                     PLAYER, EMPTY, WALL, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY};
    unsigned grid[20][10];
    u16 score_board[3][3] = {0};
    Config cfg[3];
    u16 welcome_icon[3600] = {0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9,
                              0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9,
                              0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9,
                              0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9,
                              0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9,
                              0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9,
                              0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9,
                              0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9,
                              0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0,
                              0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9,
                              0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9,
                              0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9,
                              0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9,
                              0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9,
                              0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9,
                              0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9,
                              0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0,
                              0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9,
                              0xfed9, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xfed9,
                              0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0,
                              0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xfed9,
                              0xfed9, 0xfed9, 0xfed9};
    u16 empty_pic[36] = {BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,
                         BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,
                         BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK};
    u16 wall_pic[36] = {0x4249, 0x4249, 0x4249, 0x4249, 0x4249, 0x4249, 0x4249, 0x7bf0, 0xad97, 0xad97, 0x7bf0, 0x7bf0,
                        0x4249, 0x7bf0, 0x7bf0, 0x7bf0, 0x7bf0, 0x7bf0, 0x4249, 0x4249, 0x7bf0, 0x7bf0, 0x7bf0, 0x7bf0,
                        0xad97, 0xad97, 0xad97, 0x4249, 0x4249, 0x4249, 0x7bf0, 0x7bf0, 0x7bf0, 0x7bf0, 0xad97, 0xad97};
    u16 player_pic[36] = {0x0000, 0x0000, 0xff55, 0xff55, 0x0000, 0x0000, 0x0000, 0x0000, 0xff55, 0xff55, 0x0000,
                          0x0000, 0x0000, 0x4dbe, 0x4dbe, 0x4dbe, 0x4dbe, 0x0000, 0x0000, 0x4dbe, 0x4dbe, 0x4dbe,
                          0x4dbe, 0x0000, 0x0000, 0xff55, 0x7761, 0x7761, 0xff55, 0x0000, 0x0000, 0x0000, 0x7761,
                          0x7761, 0x0000, 0x0000};
    u16 target_pic[36] = {0xc72d, 0xc72d, 0xc72d, 0xc72d, 0xc72d, 0xc72d, 0xc72d, 0xc72d, 0xc72d, 0xc72d, 0xc72d,
                          0xc72d, 0xc72d, 0xc72d, 0x9694, 0x9694, 0xc72d, 0xc72d, 0xc72d, 0xc72d, 0x9694, 0x9694,
                          0xc72d, 0xc72d, 0xc72d, 0xc72d, 0xc72d, 0xc72d, 0xc72d, 0xc72d, 0xc72d, 0xc72d, 0xc72d,
                          0xc72d, 0xc72d, 0xc72d};
    u16 box_pic[36] = {0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xccc0, 0xfed9, 0xccc0, 0xccc0, 0xfed9, 0xccc0,
                       0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xccc0, 0xccc0,
                       0xccc0, 0xfed9, 0xccc0, 0xccc0, 0xfed9, 0xccc0, 0xfed9, 0xccc0, 0xccc0, 0xccc0, 0xccc0, 0xfed9};
    u16 box_targeted_pic[36] = {0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xfed9, 0xccc0, 0xfed9, 0xfed9, 0xccc0,
                                0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0, 0xfed9, 0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xccc0,
                                0xfed9, 0xfed9, 0xfed9, 0xccc0, 0xfed9, 0xfed9, 0xccc0, 0xfed9, 0xccc0, 0xfed9, 0xfed9,
                                0xfed9, 0xfed9, 0xccc0};
    cfg[0].box_num = 3;
    cfg[0].pos_x = 11;
    cfg[0].pos_y = 4;
    cfg[1].box_num = 6;
    cfg[1].pos_x = 6;
    cfg[1].pos_y = 6;
    cfg[2].box_num = 21;
    cfg[2].pos_x = 19;
    cfg[2].pos_y = 0;
    LCD_ShowString(20, 30, "CS110 Project 4", WHITE);
    delay_1ms(3000);
    LCD_Clear(BLACK);
    LCD_ShowString(32, 30, "yanty & yuwf", WHITE);
    delay_1ms(3000);
    LCD_Clear(BLACK);
    LCD_ShowString(32, 20, "ShanghaiTech", WHITE);
    LCD_ShowString(40, 40, "University", WHITE);
    delay_1ms(3000);
    LCD_Clear(BLACK);
    for (int i = 0; i < 60; ++i)
        for (int j = 0; j < 60; ++j)
            LCD_DrawPoint(50 + j, 10 + i, welcome_icon[i * 60 + j]);
    delay_1ms(3000);
    LCD_Clear(BLACK);
    LCD_ShowString(52, 20, "SOKOBAN", WHITE);
    LCD_ShowString(52, 56, "LOADING", WHITE);
    delay_1ms(1000);
    LCD_DrawLine(44, 16, 116, 16, RED);
    LCD_DrawLine(50, 78, 55, 78, WHITE);
    delay_1ms(1000);
    LCD_DrawLine(116, 16, 116, 40, BLUE);
    LCD_DrawLine(55, 78, 65, 78, WHITE);
    delay_1ms(1000);
    LCD_DrawLine(44, 40, 116, 40, GREEN);
    LCD_DrawLine(65, 78, 70, 78, WHITE);
    delay_1ms(1000);
    LCD_DrawLine(44, 16, 44, 40, YELLOW);
    LCD_DrawLine(70, 78, 80, 78, WHITE);
    delay_1ms(500);
    for (int i = 0; i < 3; ++i) {
        delay_1ms(500);
        LCD_DrawRectangle(44, 16, 116, 40, BLACK);
        delay_1ms(500);
        LCD_ShowString(52, 20, "SOKOBAN", WHITE);
        LCD_DrawLine(44, 16, 116, 16, RED);
        LCD_DrawLine(116, 16, 116, 40, BLUE);
        LCD_DrawLine(44, 40, 116, 40, GREEN);
        LCD_DrawLine(44, 16, 44, 40, YELLOW);
        LCD_DrawLine(80 + i * 10, 78, 90 + i * 10, 78, WHITE);
    }
    delay_1ms(500);
    LCD_ShowString(52, 56, "LOADING", BLACK);
    LCD_DrawLine(50, 78, 110, 78, BLACK);
    LCD_ShowString(0, 60, "PRESS BOOT TO START", WHITE);
    while (114514) {
        if (Get_BOOT0()) {
            LCD_Clear(BLACK);
            delay_1ms(300);
            while ("Start") {
                while ("Difficulty Selection") {
                    LCD_ShowString(0, 0, "LEVEL", WHITE);
                    if (level > 1)
                        LCD_ShowString(52, 0, "D", WHITE);
                    else
                        LCD_ShowString(52, 0, "D", BLACK);
                    LCD_ShowNum(76, 0, level, 1, YELLOW);
                    if (level < LAST_LEVEL)
                        LCD_ShowString(92, 0, "C", WHITE);
                    else
                        LCD_ShowString(92, 0, "C", BLACK);
                    LCD_ShowString(0, 20, "BOXES", WHITE);
                    if (box > cfg[level - 1].box_num - 2)
                        LCD_ShowString(52, 20, "L", WHITE);
                    else
                        LCD_ShowString(52, 20, "L", BLACK);
                    LCD_ShowNum(68, 20, box, 2, BROWN);
                    if (box < cfg[level - 1].box_num)
                        LCD_ShowString(92, 20, "R", WHITE);
                    else
                        LCD_ShowString(92, 20, "R", BLACK);
                    LCD_ShowString(112, 0, "L C R", WHITE);
                    LCD_ShowString(128, 20, "D", WHITE);
                    LCD_ShowString(0, 60, "PRESS BOOT TO START", WHITE);
                    if (Get_BOOT0()) {
                        delay_1ms(300);
                        break;
                    }
                    if (Get_Button(JOY_CTR) && level < LAST_LEVEL) {
                        ++level;
                        min_box = cfg[level - 1].box_num - 2;
                        max_box = cfg[level - 1].box_num;
                        box = min_box + 1;
                        continue;
                    }
                    if (Get_Button(JOY_DOWN) && level > 1) {
                        --level;
                        min_box = cfg[level - 1].box_num - 2;
                        max_box = cfg[level - 1].box_num;
                        box = min_box + 1;
                        continue;
                    }
                    if (Get_Button(JOY_LEFT) && box > min_box) {
                        --box;
                        continue;
                    }
                    if (Get_Button(JOY_RIGHT) && box < max_box) {
                        ++box;
                        continue;
                    }
                }
                while ("Initialization") {
                    LCD_Clear(BLACK);
                    for (int i = 0; i < 20; ++i)
                        for (int j = 0; j < 10; ++j)
                            grid[i][j] = init_grid[level - 1][i][j];
                    switch (level) {
                        case 1:
                            if (box < 3) {
                                grid[4][4] = EMPTY;
                                grid[5][5] = EMPTY;
                                if (box < 2) {
                                    grid[4][5] = EMPTY;
                                    grid[4][6] = EMPTY;
                                }
                            }
                            break;
                        case 2:
                            if (box < 6) {
                                grid[9][6] = EMPTY;
                                grid[12][8] = EMPTY;
                                if (box < 5) {
                                    grid[9][7] = EMPTY;
                                    grid[13][9] = EMPTY;
                                }
                            }
                            break;
                        case 3:
                            if (box < 21) {
                                grid[8][6] = EMPTY;
                                grid[9][8] = EMPTY;
                                if (box < 20) {
                                    grid[6][6] = EMPTY;
                                    grid[8][7] = EMPTY;
                                }
                            }
                            break;
                        default:
                            break;
                    }
                    step = 0;
                    done = 0;
                    pos_x = cfg[level - 1].pos_x;
                    pos_y = cfg[level - 1].pos_y;
                    LCD_ShowString(40, 30, "LOADING", WHITE);
                    for (int i = 0; i < 3; ++i) {
                        delay_1ms(500 * level);
                        LCD_ShowString(96 + 8 * i, 30, ".", WHITE);
                    }
                    delay_1ms(1000 * level);
                    LCD_Clear(BLACK);
                    LCD_ShowString(0, 60, "EXIT:", WHITE);
                    LCD_ShowString(48, 60, "SW1", BLUE);
                    LCD_ShowString(80, 60, "BOX   /", WHITE);
                    LCD_ShowNum(136, 60, box, 2, BROWN);
                    LCD_ShowString(120, 0, "LV", WHITE);
                    LCD_ShowNum(144, 0, level, 1, YELLOW);
                    LCD_ShowString(120, 20, "STEP", WHITE);
                    for (int i = 0; i < 20; ++i)
                        for (int j = 0; j < 10; ++j)
                            switch (grid[i][j]) {
                                case WALL:
                                    LCD_DrawGrid(i, j, wall_pic);
                                    break;
                                case PLAYER:
                                    LCD_DrawGrid(i, j, player_pic);
                                    break;
                                case TARGET:
                                    LCD_DrawGrid(i, j, target_pic);
                                    break;
                                case BOX:
                                    LCD_DrawGrid(i, j, box_pic);
                                    break;
                                default:
                                    LCD_DrawGrid(i, j, empty_pic);
                                    break;
                            }
                    break;
                }
                while ("Ongoing") {
                    if (Get_Button(BUTTON_1))
                        break;
                    if (step > 9999) {
                        for (int i = 0; i < 3; ++i) {
                            LCD_Clear(BLACK);
                            delay_1ms(500);
                            LCD_ShowString(44, 30, "YOU LOSE!", RED);
                            delay_1ms(500);
                        }
                        delay_1ms(2500);
                        break;
                    }
                    LCD_ShowNum(112, 60, done, 2, GREEN);
                    LCD_ShowNum(120, 40, step, 4, RED);
                    if (done == box) {
                        delay_1ms(300);
                        for (int i = 0; i < 3; ++i) {
                            LCD_Clear(BLACK);
                            delay_1ms(500);
                            LCD_ShowString(48, 20, "YOU WIN!", RED);
                            delay_1ms(500);
                        }
                        delay_1ms(500);
                        LCD_ShowString(24, 40, "YOUR STEP:", WHITE);
                        delay_1ms(1000);
                        LCD_ShowNum(104, 40, step, 4, GREEN);
                        delay_1ms(3000);
                        if (!score_board[level - 1][2] || step < score_board[level - 1][2]) {
                            score_board[level - 1][2] = step;
                            if (!score_board[level - 1][1] || score_board[level - 1][2] < score_board[level - 1][1]) {
                                step = score_board[level - 1][1];
                                score_board[level - 1][1] = score_board[level - 1][2];
                                score_board[level - 1][2] = step;
                                if (!score_board[level - 1][0] ||
                                    score_board[level - 1][1] < score_board[level - 1][0]) {
                                    step = score_board[level - 1][0];
                                    score_board[level - 1][0] = score_board[level - 1][1];
                                    score_board[level - 1][1] = step;
                                }
                            }
                        }
                        break;
                    }
                    if (Get_Button(JOY_CTR)) {
                        if (pos_y) {
                            if (grid[pos_x][pos_y - 1] == EMPTY || grid[pos_x][pos_y - 1] == TARGET) {
                                if (occupied == EMPTY) {
                                    grid[pos_x][pos_y] = EMPTY;
                                    LCD_DrawGrid(pos_x, pos_y, empty_pic);
                                } else if (occupied == TARGET) {
                                    grid[pos_x][pos_y] = TARGET;
                                    LCD_DrawGrid(pos_x, pos_y, target_pic);
                                }
                                occupied = grid[pos_x][--pos_y];
                                grid[pos_x][pos_y] = PLAYER;
                                LCD_DrawGrid(pos_x, pos_y, player_pic);
                                ++step;
                                continue;
                            } else if (grid[pos_x][pos_y - 1] == BOX && pos_y > 1) {
                                if (grid[pos_x][pos_y - 2] == EMPTY) {
                                    if (occupied == EMPTY) {
                                        grid[pos_x][pos_y] = EMPTY;
                                        LCD_DrawGrid(pos_x, pos_y, empty_pic);
                                    } else if (occupied == TARGET) {
                                        grid[pos_x][pos_y] = TARGET;
                                        LCD_DrawGrid(pos_x, pos_y, target_pic);
                                    }
                                    occupied = EMPTY;
                                    grid[pos_x][--pos_y] = PLAYER;
                                    LCD_DrawGrid(pos_x, pos_y, player_pic);
                                    grid[pos_x][pos_y - 1] = BOX;
                                    LCD_DrawGrid(pos_x, pos_y - 1, box_pic);
                                    ++step;
                                    continue;
                                } else if (grid[pos_x][pos_y - 2] = TARGET) {
                                    if (occupied == EMPTY) {
                                        grid[pos_x][pos_y] = EMPTY;
                                        LCD_DrawGrid(pos_x, pos_y, empty_pic);
                                    } else if (occupied == TARGET) {
                                        grid[pos_x][pos_y] = TARGET;
                                        LCD_DrawGrid(pos_x, pos_y, target_pic);
                                    }
                                    occupied = EMPTY;
                                    grid[pos_x][--pos_y] = PLAYER;
                                    LCD_DrawGrid(pos_x, pos_y, player_pic);
                                    grid[pos_x][pos_y - 1] = BOX_T;
                                    LCD_DrawGrid(pos_x, pos_y - 1, box_targeted_pic);
                                    ++done;
                                    ++step;
                                    continue;
                                }
                            } else if (grid[pos_x][pos_y - 1] == BOX_T && pos_y > 1) {
                                if (grid[pos_x][pos_y - 2] == EMPTY) {
                                    if (occupied == EMPTY) {
                                        grid[pos_x][pos_y] = EMPTY;
                                        LCD_DrawGrid(pos_x, pos_y, empty_pic);
                                    } else if (occupied == TARGET) {
                                        grid[pos_x][pos_y] = TARGET;
                                        LCD_DrawGrid(pos_x, pos_y, target_pic);
                                    }
                                    occupied = TARGET;
                                    grid[pos_x][--pos_y] = PLAYER;
                                    LCD_DrawGrid(pos_x, pos_y, player_pic);
                                    grid[pos_x][pos_y - 1] = BOX;
                                    LCD_DrawGrid(pos_x, pos_y - 1, box_pic);
                                    --done;
                                    ++step;
                                    continue;
                                } else if (grid[pos_x][pos_y - 2] == TARGET) {
                                    if (occupied == EMPTY) {
                                        grid[pos_x][pos_y] = EMPTY;
                                        LCD_DrawGrid(pos_x, pos_y, empty_pic);
                                    } else if (occupied == TARGET) {
                                        grid[pos_x][pos_y] = TARGET;
                                        LCD_DrawGrid(pos_x, pos_y, target_pic);
                                    }
                                    occupied = TARGET;
                                    grid[pos_x][--pos_y] = PLAYER;
                                    LCD_DrawGrid(pos_x, pos_y, player_pic);
                                    grid[pos_x][pos_y - 1] = BOX_T;
                                    LCD_DrawGrid(pos_x, pos_y - 1, box_targeted_pic);
                                    ++step;
                                    continue;
                                }
                            }
                        }
                    }
                    if (Get_Button(JOY_DOWN)) {
                        if (pos_y < 9) {
                            if (grid[pos_x][pos_y + 1] == EMPTY || grid[pos_x][pos_y + 1] == TARGET) {
                                if (occupied == EMPTY) {
                                    grid[pos_x][pos_y] = EMPTY;
                                    LCD_DrawGrid(pos_x, pos_y, empty_pic);
                                } else if (occupied == TARGET) {
                                    grid[pos_x][pos_y] = TARGET;
                                    LCD_DrawGrid(pos_x, pos_y, target_pic);
                                }
                                occupied = grid[pos_x][++pos_y];
                                grid[pos_x][pos_y] = PLAYER;
                                LCD_DrawGrid(pos_x, pos_y, player_pic);
                                ++step;
                                continue;
                            } else if (grid[pos_x][pos_y + 1] == BOX && pos_y < 8) {
                                if (grid[pos_x][pos_y + 2] == EMPTY) {
                                    if (occupied == EMPTY) {
                                        grid[pos_x][pos_y] = EMPTY;
                                        LCD_DrawGrid(pos_x, pos_y, empty_pic);
                                    } else if (occupied == TARGET) {
                                        grid[pos_x][pos_y] = TARGET;
                                        LCD_DrawGrid(pos_x, pos_y, target_pic);
                                    }
                                    occupied = EMPTY;
                                    grid[pos_x][++pos_y] = PLAYER;
                                    LCD_DrawGrid(pos_x, pos_y, player_pic);
                                    grid[pos_x][pos_y + 1] = BOX;
                                    LCD_DrawGrid(pos_x, pos_y + 1, box_pic);
                                    ++step;
                                    continue;
                                } else if (grid[pos_x][pos_y + 2] == TARGET) {
                                    if (occupied == EMPTY) {
                                        grid[pos_x][pos_y] = EMPTY;
                                        LCD_DrawGrid(pos_x, pos_y, empty_pic);
                                    } else if (occupied == TARGET) {
                                        grid[pos_x][pos_y] = TARGET;
                                        LCD_DrawGrid(pos_x, pos_y, target_pic);
                                    }
                                    occupied = EMPTY;
                                    grid[pos_x][++pos_y] = PLAYER;
                                    LCD_DrawGrid(pos_x, pos_y, player_pic);
                                    grid[pos_x][pos_y + 1] = BOX_T;
                                    LCD_DrawGrid(pos_x, pos_y + 1, box_targeted_pic);
                                    ++done;
                                    ++step;
                                    continue;
                                }
                            } else if (grid[pos_x][pos_y + 1] == BOX_T && pos_y < 8) {
                                if (grid[pos_x][pos_y + 2] == EMPTY) {
                                    if (occupied == EMPTY) {
                                        grid[pos_x][pos_y] = EMPTY;
                                        LCD_DrawGrid(pos_x, pos_y, empty_pic);
                                    } else if (occupied == TARGET) {
                                        grid[pos_x][pos_y] = TARGET;
                                        LCD_DrawGrid(pos_x, pos_y, target_pic);
                                    }
                                    occupied = TARGET;
                                    grid[pos_x][++pos_y] = PLAYER;
                                    LCD_DrawGrid(pos_x, pos_y, player_pic);
                                    grid[pos_x][pos_y + 1] = BOX;
                                    LCD_DrawGrid(pos_x, pos_y + 1, box_pic);
                                    --done;
                                    ++step;
                                    continue;
                                } else if (grid[pos_x][pos_y + 2] = TARGET) {
                                    if (occupied == EMPTY) {
                                        grid[pos_x][pos_y] = EMPTY;
                                        LCD_DrawGrid(pos_x, pos_y, empty_pic);
                                    } else if (occupied == TARGET) {
                                        grid[pos_x][pos_y] = TARGET;
                                        LCD_DrawGrid(pos_x, pos_y, target_pic);
                                    }
                                    occupied = TARGET;
                                    grid[pos_x][++pos_y] = PLAYER;
                                    LCD_DrawGrid(pos_x, pos_y, player_pic);
                                    grid[pos_x][pos_y + 1] = BOX_T;
                                    LCD_DrawGrid(pos_x, pos_y + 1, box_targeted_pic);
                                    ++step;
                                    continue;
                                }
                            }
                        }
                    }
                    if (Get_Button(JOY_LEFT)) {
                        if (pos_x) {
                            if (grid[pos_x - 1][pos_y] == EMPTY || grid[pos_x - 1][pos_y] == TARGET) {
                                if (occupied == EMPTY) {
                                    grid[pos_x][pos_y] = EMPTY;
                                    LCD_DrawGrid(pos_x, pos_y, empty_pic);
                                } else if (occupied == TARGET) {
                                    grid[pos_x][pos_y] = TARGET;
                                    LCD_DrawGrid(pos_x, pos_y, target_pic);
                                }
                                occupied = grid[--pos_x][pos_y];
                                grid[pos_x][pos_y] = PLAYER;
                                LCD_DrawGrid(pos_x, pos_y, player_pic);
                                ++step;
                                continue;
                            } else if (grid[pos_x - 1][pos_y] == BOX && pos_x > 1) {
                                if (grid[pos_x - 2][pos_y] == EMPTY) {
                                    if (occupied == EMPTY) {
                                        grid[pos_x][pos_y] = EMPTY;
                                        LCD_DrawGrid(pos_x, pos_y, empty_pic);
                                    } else if (occupied == TARGET) {
                                        grid[pos_x][pos_y] = TARGET;
                                        LCD_DrawGrid(pos_x, pos_y, target_pic);
                                    }
                                    occupied = EMPTY;
                                    grid[--pos_x][pos_y] = PLAYER;
                                    LCD_DrawGrid(pos_x, pos_y, player_pic);
                                    grid[pos_x - 1][pos_y] = BOX;
                                    LCD_DrawGrid(pos_x - 1, pos_y, box_pic);
                                    ++step;
                                    continue;
                                } else if (grid[pos_x - 2][pos_y] == TARGET) {
                                    if (occupied == EMPTY) {
                                        grid[pos_x][pos_y] = EMPTY;
                                        LCD_DrawGrid(pos_x, pos_y, empty_pic);
                                    } else if (occupied == TARGET) {
                                        grid[pos_x][pos_y] = TARGET;
                                        LCD_DrawGrid(pos_x, pos_y, target_pic);
                                    }
                                    occupied = EMPTY;
                                    grid[--pos_x][pos_y] = PLAYER;
                                    LCD_DrawGrid(pos_x, pos_y, player_pic);
                                    grid[pos_x - 1][pos_y] = BOX_T;
                                    LCD_DrawGrid(pos_x - 1, pos_y, box_targeted_pic);
                                    ++done;
                                    ++step;
                                    continue;
                                }
                            } else if (grid[pos_x - 1][pos_y] == BOX_T && pos_x > 1) {
                                if (grid[pos_x - 2][pos_y] == EMPTY) {
                                    if (occupied == EMPTY) {
                                        grid[pos_x][pos_y] = EMPTY;
                                        LCD_DrawGrid(pos_x, pos_y, empty_pic);
                                    } else if (occupied == TARGET) {
                                        grid[pos_x][pos_y] = TARGET;
                                        LCD_DrawGrid(pos_x, pos_y, target_pic);
                                    }
                                    occupied = TARGET;
                                    grid[--pos_x][pos_y] = PLAYER;
                                    LCD_DrawGrid(pos_x, pos_y, player_pic);
                                    grid[pos_x - 1][pos_y] = BOX;
                                    LCD_DrawGrid(pos_x - 1, pos_y, box_pic);
                                    --done;
                                    ++step;
                                    continue;
                                } else if (grid[pos_x - 2][pos_y] == TARGET) {
                                    if (occupied == EMPTY) {
                                        grid[pos_x][pos_y] = EMPTY;
                                        LCD_DrawGrid(pos_x, pos_y, empty_pic);
                                    } else if (occupied == TARGET) {
                                        grid[pos_x][pos_y] = TARGET;
                                        LCD_DrawGrid(pos_x, pos_y, target_pic);
                                    }
                                    occupied = TARGET;
                                    grid[--pos_x][pos_y] = PLAYER;
                                    LCD_DrawGrid(pos_x, pos_y, player_pic);
                                    grid[pos_x - 1][pos_y] = BOX_T;
                                    LCD_DrawGrid(pos_x - 1, pos_y, box_targeted_pic);
                                    ++step;
                                    continue;
                                }
                            }
                        }
                    }
                    if (Get_Button(JOY_RIGHT)) {
                        if (pos_x < 19) {
                            if (grid[pos_x + 1][pos_y] == EMPTY || grid[pos_x + 1][pos_y] == TARGET) {
                                if (occupied == EMPTY) {
                                    grid[pos_x][pos_y] = EMPTY;
                                    LCD_DrawGrid(pos_x, pos_y, empty_pic);
                                } else if (occupied == TARGET) {
                                    grid[pos_x][pos_y] = TARGET;
                                    LCD_DrawGrid(pos_x, pos_y, target_pic);
                                }
                                occupied = grid[++pos_x][pos_y];
                                grid[pos_x][pos_y] = PLAYER;
                                LCD_DrawGrid(pos_x, pos_y, player_pic);
                                ++step;
                                continue;
                            } else if (grid[pos_x + 1][pos_y] == BOX && pos_x < 18) {
                                if (grid[pos_x + 2][pos_y] == EMPTY) {
                                    if (occupied == EMPTY) {
                                        grid[pos_x][pos_y] = EMPTY;
                                        LCD_DrawGrid(pos_x, pos_y, empty_pic);
                                    } else if (occupied == TARGET) {
                                        grid[pos_x][pos_y] = TARGET;
                                        LCD_DrawGrid(pos_x, pos_y, target_pic);
                                    }
                                    occupied = EMPTY;
                                    grid[++pos_x][pos_y] = PLAYER;
                                    LCD_DrawGrid(pos_x, pos_y, player_pic);
                                    grid[pos_x + 1][pos_y] = BOX;
                                    LCD_DrawGrid(pos_x + 1, pos_y, box_pic);
                                    ++step;
                                    continue;
                                } else if (grid[pos_x + 2][pos_y] == TARGET) {
                                    if (occupied == EMPTY) {
                                        grid[pos_x][pos_y] = EMPTY;
                                        LCD_DrawGrid(pos_x, pos_y, empty_pic);
                                    } else if (occupied == TARGET) {
                                        grid[pos_x][pos_y] = TARGET;
                                        LCD_DrawGrid(pos_x, pos_y, target_pic);
                                    }
                                    occupied = EMPTY;
                                    grid[++pos_x][pos_y] = PLAYER;
                                    LCD_DrawGrid(pos_x, pos_y, player_pic);
                                    grid[pos_x + 1][pos_y] = BOX_T;
                                    LCD_DrawGrid(pos_x + 1, pos_y, box_targeted_pic);
                                    ++done;
                                    ++step;
                                    continue;
                                }
                            } else if (grid[pos_x + 1][pos_y] == BOX_T && pos_x < 18) {
                                if (grid[pos_x + 2][pos_y] == EMPTY) {
                                    if (occupied == EMPTY) {
                                        grid[pos_x][pos_y] = EMPTY;
                                        LCD_DrawGrid(pos_x, pos_y, empty_pic);
                                    } else if (occupied == TARGET) {
                                        grid[pos_x][pos_y] = TARGET;
                                        LCD_DrawGrid(pos_x, pos_y, target_pic);
                                    }
                                    occupied = TARGET;
                                    grid[++pos_x][pos_y] = PLAYER;
                                    LCD_DrawGrid(pos_x, pos_y, player_pic);
                                    grid[pos_x + 1][pos_y] = BOX;
                                    LCD_DrawGrid(pos_x + 1, pos_y, box_pic);
                                    --done;
                                    ++step;
                                    continue;
                                } else if (grid[pos_x + 2][pos_y] == TARGET) {
                                    if (occupied == EMPTY) {
                                        grid[pos_x][pos_y] = EMPTY;
                                        LCD_DrawGrid(pos_x, pos_y, empty_pic);
                                    } else if (occupied == TARGET) {
                                        grid[pos_x][pos_y] = TARGET;
                                        LCD_DrawGrid(pos_x, pos_y, target_pic);
                                    }
                                    occupied = TARGET;
                                    grid[++pos_x][pos_y] = PLAYER;
                                    LCD_DrawGrid(pos_x, pos_y, player_pic);
                                    grid[pos_x + 1][pos_y] = BOX_T;
                                    LCD_DrawGrid(pos_x + 1, pos_y, box_targeted_pic);
                                    ++step;
                                    continue;
                                }
                            }
                        }
                    }
                }
                LCD_Clear(BLACK);
                while ("Scoreboard") {
                    LCD_ShowString(0, 0, "SCOREBOARD", WHITE);
                    for (u16 i = 0; i < 3; ++i) {
                        LCD_ShowNum(0, i * 20 + 20, i + 1, 1, WHITE);
                        for (u16 j = 0; j < 3; ++j)
                            if (score_board[i][j])
                                LCD_ShowNum(j * 40 + 16, i * 20 + 20, score_board[i][j], 4, WHITE);
                    }
                    if (Get_Button(JOY_CTR) || Get_Button(JOY_DOWN) || Get_Button(JOY_LEFT) ||
                        Get_Button(JOY_RIGHT) || Get_Button(BUTTON_1)) {
                        LCD_Clear(BLACK);
                        break;
                    }
                    if (Get_BOOT0()) {
                        LCD_Clear(BLACK);
                        delay_1ms(300);
                        break;
                    }
                }
            }
        }
    }
}
