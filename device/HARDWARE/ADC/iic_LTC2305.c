
/* ˵��
- ���ļ�����ͨ��IIC��LTC2305оƬ������ʹ�õĹ���ʵ��;
- ����LTC2305оƬ�Ĳ���˵��
*/


#include "iic_fun.h"
#include "iic_LTC2305.h"
#include "bms_state.h"


/* ���������ź�ת��
����:��ָ����ַ�����ź�,���������źŵ�ת��
����:����ʼ�ź�+7λ��ַ+���ź�λ+��ȡӦ��λ+ֹͣ�źš�;
����:x:ѡ��IIC,addr:оƬ��ַ;
����ֵ:�ӻ��Ƿ�Ӧ��,��Ӧ��Ϊ0,��Ӧ��Ϊ1;
*/
u8 LTC2305_start_convert(u8 x, u8 addr){
	u8 ack_flag = 1;  //��Ӧ���ź�
	u8 byte = 0;
	byte = x<<1 | 0x01;
	
	IIC_Start(x);  //��ʼ�ź�
	IIC_Send_Byte(x,byte);  //���͵�ַ+���ź�λ
	ack_flag = IIC_Wait_Ack(x);  //��ȡ����Ӧ���ź�
	IIC_Stop(x);  //ֹͣ�ź�
	
	return ack_flag;
}

/* �ı�оƬת��ģʽ
����:�ı�ָ����ַоƬ�Ĺ���ģʽ(�������ģʽ,CH0����ģʽ,CH1����ģʽ);
����:����ʼ�ź�+7λ��ַ+д�ź�λ+��ȡӦ��λ+6λDin+2������λ+��ȡӦ��λ+ֹͣ�źš�;
����:x:ѡ��IIC,addr:оƬ��ַ,Din:����ָ��оƬ�Ĺ���ģʽ;
����ֵ:���ݴӻ�Ӧ��״���ж��Ƿ�д������ɹ�,1Ϊд��ɹ�,0Ϊд��ʧ��;
*/
u8 LTC2305_change_mode(u8 x, u8 addr, u8 din){
	u8 ack_flag1 = 1;
	u8 ack_flag2 = 1;
	u8 byte;
	
	IIC_Start(x);  //��ʼ�ź�
	byte = x<<1 | 0x00;  //7λ��ַ+д�ź�λ
	IIC_Send_Byte(x,byte);  //���͵�ַ+д�ź�λ
	ack_flag1 = IIC_Wait_Ack(x);  //��ȡ����Ӧ��λ(��Ϊ0)
	IIC_Send_Byte(x,din);  //д��DINģʽ
	ack_flag2 = IIC_Wait_Ack(x);  //��ȡ����Ӧ��λ(��Ϊ0)
	IIC_Stop(x);
	
	if((ack_flag1+ack_flag2)==0)  //���δӻ�����Ӧ��
		return 1;  //д��ɹ�
	else
		return 0;  //д��ʧ��
	
}

/* ��ȡ����ת�����
����:��ȡָ��оƬ��ַ��ת�����
����:����ʼ�ź�+7λ��ַ+���ź�λ+��ȡ�ӻ�Ӧ��λ+�����8λ+��������ACK+�����4λ+����4�����õ�λ+��������NACK+ֹͣ�źš�;
����:x:ѡ��IIC,addr:оƬ��ַ;
����ֵ:data:��ȡ���(16λ������,����ģʽ��ת��Ϊ��Ӧ���޷��Ż��з�����);
*/
u16 LTC2305_read_result(u8 x, u8 addr) {
	u16 result = 0;
	u8 data = 0;
	u8 byte;
	u8 ack_flag = 1;
	
	IIC_Start(x);  //��ʼ�ź�
	byte = x<<1 | 0x01;  //7λ��ַ+���ź�λ
	IIC_Send_Byte(x,byte);  // ����7λ��ַ+���ź�λ
	ack_flag = IIC_Wait_Ack(x);  // ��ȡ����Ӧ��λ(��Ϊ0)
	
	data = IIC_Read_Byte(x,1);  //�����8λ,����Ack
	result= data<<4;
	data = IIC_Read_Byte(x,0);  //�����4λ,����NAck
	IIC_Stop(x);
	result = result | (data>>4);
	return result;
	
}























