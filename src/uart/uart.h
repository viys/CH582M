/*
 * uart.h
 *
 *  Created on: Jul 4, 2023
 *      Author: y zy
 */

#ifndef SRC_UART_UART_H_
#define SRC_UART_UART_H_

#include "CH58x_common.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"

#define FlagSET(Flag,bit)   (Flag |= bit)   //Flag置位
#define FlagCLR(Flag,bit)   (Flag &= ~bit)  //Flag清位
#define FlagGET(Flag,bit)   (Flag & bit)    //Flag获取
#define ReggSET(reg,bit)   (((uint32_t*)reg) |= bit)   //Flag置位
#define ReggCLR(reg,bit)   (((uint32_t*)reg) &= ~bit)  //Flag清位
#define RegGET(reg,bit)   (((uint32_t*)reg) & bit)    //Flag获取

#define U1_RX_SIZE  2048
#define U1_TX_SIZE  2048
#define U1_RX_MAX   256

#define URxNUM      10

/* uart control blok */
typedef struct{
    /* s用于标记起始位置 */
    uint8_t *start;
    /* e用于标记结束位置 */
    uint8_t *end;
}UCB_URxBuffPtr;

/* 串口控制区指针结构体 */
typedef struct{
    /* 积累接受数据量 */
    uint16_t URxCounter;
    /* se指针对于结构体数组 */
    UCB_URxBuffPtr URxDataPtr[URxNUM];
    /* IN指针用于标记接收数据 */
    UCB_URxBuffPtr *URxDataIN;
    /* OUT指针用于提取接受的数据 */
    UCB_URxBuffPtr *URxDataOUT;
    /* IN 和 OUT 指针的结尾标志 */
    UCB_URxBuffPtr *URxDataEND;
}UCB_CB;

/* 中断类型判断结构体 */
/* 防止因一包数据为7的整数倍而丢失 */

typedef struct{
    /* 进入过UART_II_RECV_TOUT中断标志位 */
    uint8_t enter_TOUT;
    uint8_t enter_RSV;
}UCB_SB;

extern UCB_CB U1CB;
extern uint8_t U1_RxBuff[U1_RX_SIZE];

/* 串口初始化函数 */
void uart_init(void);
void u1_printf(char *format,...);
void u1_resive_detection(void);

#endif /* SRC_UART_UART_H_ */
