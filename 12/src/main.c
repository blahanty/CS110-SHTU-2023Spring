#include "lcd/lcd.h"
#include <string.h>
#include "utils.h"

#define BOARD_TEST_ON 0

void Inp_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOC);

    gpio_init(GPIOA, GPIO_MODE_IPD, GPIO_OSPEED_50MHZ, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_6);
    gpio_init(GPIOC, GPIO_MODE_IPD, GPIO_OSPEED_50MHZ, GPIO_PIN_13);;
}

void IO_init(void)
{
    Inp_init(); // inport init
    Lcd_Init(); // LCD init
}

void Board_self_test(void)
{
    while(1)
    {
        LCD_ShowString(60,25,"TEST",WHITE);
        if (Get_Button(JOY_LEFT))
        {
            LCD_ShowString(5,25,"L", BLUE);
            //continue;
        }
        if (Get_Button(JOY_DOWN))
        {
            LCD_ShowString(25,45,"D", BLUE);
        }
        LCD_ShowString(5,5,"U:INOP",RED);
        if (Get_Button(JOY_RIGHT))
        {
            LCD_ShowString(45,25,"R", BLUE);
        }
        if (Get_Button(JOY_CTR))
        {
            LCD_ShowString(25,25,"C", BLUE);
        }
        if (Get_Button(BUTTON_1))
        {
            LCD_ShowString(60,5,"SW1", BLUE);
        }
        LCD_ShowString(60,45,"SW2:INOP",RED);
        LCD_Clear(BLACK);
        delay_1ms(10);
    }
}

int main(void)
{
    IO_init();         // init OLED
    #if BOARD_TEST_ON
    Board_self_test();
    #else
    LCD_Clear(BLACK);
    while(1)
    {
    delay_1ms(10);
    LCD_ShowString(5,0,"Hello World",WHITE);
    LCD_ShowString(5,15,"Hello World",WHITE);
    LCD_ShowString(5,30,"Hello World",WHITE);
    LCD_ShowString(5,45,"Hello World",WHITE);
    LCD_ShowString(5,60,"Hello World",WHITE);
    if (Get_BOOT0()) break;
    }
    LCD_Clear(BLACK);
    while (1)
    {
        delay_1ms(10);
        draw();
    }
    #endif
}
