/********************************** (C) COPYRIGHT *******************************
 * File Name          : Main.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2020/08/06
 * Description        : 串口1收发演示
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "uart.h"

uint8_t buff[10] = {'1','2','3','1','2','3','1','2','3'};

int main()
{


    SetSysClock(CLK_SOURCE_PLL_60MHz);

    uart_init();
    u1_printf("hello");
    while(1){
        u1_resive_detection();
//        UART1_SendString(buff,9);
//        mDelaymS(1000);

    }
}



