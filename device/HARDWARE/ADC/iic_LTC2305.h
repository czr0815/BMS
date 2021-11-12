#ifndef __IIC_LTC2305_H
#define __IIC_LTC2305_H

#include "sys.h"

/*Din��ģʽ�趨
- Din��6λ����Ϊ8λ(1���ֽ�),����ʱ�Ӹ�λ��ʼ����;
- Din��ʽ: 
  - ��ʶ|S/D|O/S| X | X |UNI|SLP| X | X |
  - λ��  8   7   6   5   4   3   2   1
- SLPλ:Ӧ����Ϊ0,��оƬ�ϵ�����NAPģʽ;
- UNIλ:��Ϊ����,��Ϊ˫��
- S/D��O/S:
- 0 0 : CH0+, CH1- ->���ϵ�Ĭ��״̬Ϊ��������״̬;
- 1 0 : CH0+  +���� ->����B+��ѹ
- 1 1 : CH1+  +���� ->����HV1��ѹ
- ��������:CH0+ CH1- ˫�� -> 0x00
- ��ѹ���B+:CH0+ ���� -> 10001000
- ��ѹ���HV1:CH1+ ���� -> 11001000
*/
#define DIN_BIP_MODE       0x00
#define DIN_UNIP_CH0_MODE  0x88
#define DIN_UNIP_CH1_MODE  0xC8


/*ʹ�õļ���LTC2305оƬ��ַ
- BCU��:
  - IIC3 �������:AD0=LOW,AD1=LOW,Addr=0001000=0x08;
  - IIC4 ��ѹ���:
    - оƬ1:AD0=LOW,AD1=LOW, Addr=0001000=0x08;
	- оƬ2:AD0=LOW,AD1=HIGH,Addr=0011010=0x1A;
- BMU��:
  - IIC4 ��������(ѡ��):AD0=LOW,AD1=LOW,Addr=0001000=0x08;
*/
#define LT2305_ADDR_CURR  0x08
#define LT2305_ADDR_HVCHIP0   0x08
#define LT2305_ADDR_HVCHIP1   0x1A
#define LT2305_ADDR_SMOKE 0x08




//��������
u8 LTC2305_start_convert(u8 x, u8 addr);  //�����ź�ת��
u8 LTC2305_change_mode(u8 x, u8 addr, u8 din);  //�ı�оƬ����ģʽ
u16 LTC2305_read_result(u8 x, u8 addr);  //��ȡ����ת�����






























#endif /* __IIC_LTC2305_H */
