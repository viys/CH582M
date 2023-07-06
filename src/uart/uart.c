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
    /* INָ��ָ��SEָ��Խṹ�������0�ų�Ա */
    U1CB.URxDataIN = &U1CB.URxDataPtr[0];
    /* OUTָ��ָ��SEָ��Խṹ�������0�ų�Ա */
    U1CB.URxDataOUT = &U1CB.URxDataPtr[0];
    /* ENDָ��ָ��SEָ��Խṹ����������һ����Ա */
    U1CB.URxDataEND = &U1CB.URxDataPtr[URxNUM-1];
    /* ʹ�� IN ָ��ָ���SEָ����е�Sָ���ǵ�һ�ν��յ���ʼλ�� */
    U1CB.URxDataIN->start = U1_RxBuff;
    /* ���۽����������� */
    U1CB.URxCounter = 0;
    /* �ж�״̬λ���� */
    U1SB.enter_RSV = 0;
    U1SB.enter_TOUT = 0;
}

void uart_init(void)
{
    /* ���ô���1��������IO��ģʽ�������ô��� */
    GPIOA_SetBits(GPIO_Pin_9);
    GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);      // RXD-������������
    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA); // TXD-�������������ע������IO������ߵ�ƽ
    UART1_DefInit();

    /* �����ж����� */
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
        case UART_II_LINE_STAT: // ��·״̬����
            break;

        case UART_II_RECV_RDY: // ���ݴﵽ���ô�����
                UART1_RecvString(U1CB.URxDataIN->start);
                U1CB.URxCounter += 7;
            break;

        case UART_II_RECV_TOUT: // ���ճ�ʱ����ʱһ֡���ݽ������
            i = UART1_RecvString(U1CB.URxDataIN->start);
            U1CB.URxCounter += i;
            U1SB.enter_TOUT = 1;
            break;

        case UART_II_THR_EMPTY: // ���ͻ������գ��ɼ�������
            break;

        case UART_II_MODEM_CHG: // ֻ֧�ִ���0
            break;

        default:
            U1SB.enter_RSV =1;
            break;
    }

    if((R8_UART1_RBR==0&&R8_UART1_RFC==0)||U1SB.enter_TOUT){
        U1SB.enter_TOUT = 0;

        U1CB.URxDataIN->end = &U1_RxBuff[U1CB.URxCounter - 1];
        U1CB.URxDataIN++;

        /* ������END��ǵ�λ�� */
        if(U1CB.URxDataIN == U1CB.URxDataEND){
            /* �ؾ�,����ָ��0�ų�Ա */
            U1CB.URxDataIN = &U1CB.URxDataPtr[0];
        }
        /* ���ʣ��ռ������ڵ��ڵ��ν�������� */
        if(U1_RX_SIZE - U1CB.URxCounter >= U1_RX_MAX){
            U1CB.URxDataIN->start = &U1_RxBuff[U1CB.URxCounter];
        }else{
            /* �ؾ�,�´εĽ���λ�÷��ػ���������ʼλ�� */
            U1CB.URxDataIN->start = U1_RxBuff;
            /* �ۼ�ֵ���� */
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
    /* Bootloader�����г��� */
    if(U1CB.URxDataOUT != U1CB.URxDataIN){
        /* ����� */
        u1_event_handle(U1CB.URxDataOUT->start,U1CB.URxDataOUT->end - U1CB.URxDataOUT->start + 1);
        U1CB.URxDataOUT ++;

        if(U1CB.URxDataOUT == U1CB.URxDataEND){
            U1CB.URxDataOUT = &U1CB.URxDataPtr[0];
        }

    }
}

void u1_printf(char *format,...)
{
    /* ����һ��va_list����listdata */
    va_list listdata;
    /* ��listdata����,...���������Ĳ��� */
    va_start(listdata,format);

    /* ��ʽ������������� U1_TxBuff */
    vsprintf((char *)U1_TxBuff,format,listdata);
    /* �ͷ�lostdata */
    va_end(listdata);

        /* �ȴ����ͻ�����Ϊ0 */
        while(FlagGET(R8_UART1_LSR,RB_LSR_DATA_RDY));
        /* �������� */
        UART1_SendString(U1_TxBuff,strlen((const char*)U1_TxBuff));

    /* �ȵ����һ���ֽ����ݷ������ */
    while(!FlagGET(R8_UART1_LSR,RB_LSR_TX_ALL_EMP));

}
