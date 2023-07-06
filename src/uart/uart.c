/*
 * uart.c
 *
 *  Created on: Jul 4, 2023
 *      Author: y zy
 */
#include "uart.h"

uint8_t U1_RxBuff[U1_RX_SIZE];
uint8_t U1_TxBuff[U1_TX_SIZE];
UCB_CB U1CB;
UCB_SB U1SB;

static void u0rx_ptr_init(void)
{
    /* IN指针指向SE指针对结构体数组的0号成员 */
    U1CB.URxDataIN = &U1CB.URxDataPtr[0];
    /* OUT指针指向SE指针对结构体数组的0号成员 */
    U1CB.URxDataOUT = &U1CB.URxDataPtr[0];
    /* END指针指向SE指针对结构体数组的最后一个成员 */
    U1CB.URxDataEND = &U1CB.URxDataPtr[URxNUM-1];
    /* 使用 IN 指针指向的SE指针对中的S指针标记第一次接收的起始位置 */
    U1CB.URxDataIN->start = U1_RxBuff;
    /* 积累接收数量清零 */
    U1CB.URxCounter = 0;
    /* 中断状态位清零 */
    U1SB.enter_RSV = 0;
    U1SB.enter_TOUT = 0;
}

void uart_init(void)
{
    /* 配置串口1：先配置IO口模式，再配置串口 */
    GPIOA_SetBits(GPIO_Pin_9);
    GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);      // RXD-配置上拉输入
    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA); // TXD-配置推挽输出，注意先让IO口输出高电平
    UART1_DefInit();

    /* 串口中断配置 */
    UART1_ByteTrigCfg(UART_7BYTE_TRIG);
    UART1_INTCfg(ENABLE, RB_IER_RECV_RDY | RB_IER_LINE_STAT);
    PFIC_EnableIRQ(UART1_IRQn);

    u0rx_ptr_init();
}


__INTERRUPT
__HIGH_CODE
void UART1_IRQHandler(void)
{
    volatile uint8_t i;
    uint32_t* reg;

    switch(UART1_GetITFlag())
    {
        case UART_II_LINE_STAT: // 线路状态错误
            break;

        case UART_II_RECV_RDY: // 数据达到设置触发点
                UART1_RecvString(U1CB.URxDataIN->start);
                U1CB.URxCounter += 7;
            break;

        case UART_II_RECV_TOUT: // 接收超时，暂时一帧数据接收完成
            i = UART1_RecvString(U1CB.URxDataIN->start);
            U1CB.URxCounter += i;
            U1SB.enter_TOUT = 1;
            break;

        case UART_II_THR_EMPTY: // 发送缓存区空，可继续发送
            break;

        case UART_II_MODEM_CHG: // 只支持串口0
            break;

        default:
            U1SB.enter_RSV =1;
            break;
    }

    if((R8_UART1_RBR==0&&R8_UART1_RFC==0)||U1SB.enter_TOUT){
        U1SB.enter_TOUT = 0;

        U1CB.URxDataIN->end = &U1_RxBuff[U1CB.URxCounter - 1];
        U1CB.URxDataIN++;

        /* 后移至END标记的位置 */
        if(U1CB.URxDataIN == U1CB.URxDataEND){
            /* 回卷,重新指向0号成员 */
            U1CB.URxDataIN = &U1CB.URxDataPtr[0];
        }
        /* 如果剩余空间量大于等于单次接收最大量 */
        if(U1_RX_SIZE - U1CB.URxCounter >= U1_RX_MAX){
            U1CB.URxDataIN->start = &U1_RxBuff[U1CB.URxCounter];
        }else{
            /* 回卷,下次的接收位置返回缓冲区的起始位置 */
            U1CB.URxDataIN->start = U1_RxBuff;
            /* 累计值清零 */
            U1CB.URxCounter = 0;
        }
    }
}

void u1_event_handle(uint8_t *data,uint16_t datalen)
{
    u1_printf("datalen:%d\r\n",datalen);
//    UART1_SendString(data,datalen);

}

void u1_resive_detection(void)
{
    /* Bootloader命令行程序 */
    if(U1CB.URxDataOUT != U1CB.URxDataIN){
        /* 命令处理 */
        u1_event_handle(U1CB.URxDataOUT->start,U1CB.URxDataOUT->end - U1CB.URxDataOUT->start + 1);
        U1CB.URxDataOUT ++;

        if(U1CB.URxDataOUT == U1CB.URxDataEND){
            U1CB.URxDataOUT = &U1CB.URxDataPtr[0];
        }

    }
}

void u1_printf(char *format,...)
{
    /* 建立一个va_list变量listdata */
    va_list listdata;
    /* 向listdata加载,...代表不定长的参数 */
    va_start(listdata,format);

    /* 格式化输出到缓冲区 U1_TxBuff */
    vsprintf((char *)U1_TxBuff,format,listdata);
    /* 释放lostdata */
    va_end(listdata);

        /* 等待发送缓冲区为0 */
        while(FlagGET(R8_UART1_LSR,RB_LSR_DATA_RDY));
        /* 填入数据 */
        UART1_SendString(U1_TxBuff,strlen((const char*)U1_TxBuff));

    /* 等到最后一个字节数据发送完毕 */
    while(!FlagGET(R8_UART1_LSR,RB_LSR_TX_ALL_EMP));

}
