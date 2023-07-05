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

#define FlagSET(Flag,bit)   (Flag |= bit)   //Flag��λ
#define FlagCLR(Flag,bit)   (Flag &= ~bit)  //Flag��λ
#define FlagGET(Flag,bit)   (Flag & bit)    //Flag��ȡ

#define U1_RX_SIZE  2048
#define U1_TX_SIZE  2048
#define U1_RX_MAX   256

#define URxNUM      10

/* uart control blok */
typedef struct{
    /* s���ڱ����ʼλ�� */
    uint8_t *start;
    /* e���ڱ�ǽ���λ�� */
    uint8_t *end;
}UCB_URxBuffPtr;

/* ���ڿ�����ָ��ṹ�� */
typedef struct{
    /* ���۽��������� */
    uint16_t URxCounter;
    /* seָ����ڽṹ������ */
    UCB_URxBuffPtr URxDataPtr[URxNUM];
    /* INָ�����ڱ�ǽ������� */
    UCB_URxBuffPtr *URxDataIN;
    /* OUTָ��������ȡ���ܵ����� */
    UCB_URxBuffPtr *URxDataOUT;
    /* IN �� OUT ָ��Ľ�β��־ */
    UCB_URxBuffPtr *URxDataEND;
}UCB_CB;

extern UCB_CB U1CB;
extern uint8_t U1_RxBuff[U1_RX_SIZE];

/* ���ڳ�ʼ������ */
void uart_init(void);
void u1_printf(char *format,...);
void u1_resive_detection(void);

#endif /* SRC_UART_UART_H_ */
