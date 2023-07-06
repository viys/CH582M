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
uint8_t RxBuff[100];
int main()
{
    uint16_t len;
    uint16_t len1;

    SetSysClock(CLK_SOURCE_PLL_60MHz);

    uart_init();
    u1_printf("hello");

//    while(1)
//    {
//        len = UART1_RecvString(RxBuff);
////        len1 = R8_UART1_RFC;
//        if(len)
//        {
////            u1_printf("datalen:%d\r\n",len1);
//            UART1_SendString(RxBuff, len);
//
//////            UART1_SendByte('1');
//        }
//    }
    while(1){
        u1_resive_detection();

//        mDelaymS(1000);

    }
}



