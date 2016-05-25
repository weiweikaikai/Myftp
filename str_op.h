/*************************************************************************
	> File Name: str_op.h
	> Author: wk
	> Mail: 18402927708@163.com
	> Created Time: Wed 18 May 2016 06:20:35 PM CST
 ************************************************************************/

#ifndef _STR_OP_H
#define _STR_OP_H


//ȥ��\r\n
void str_trim_ctrl(char *str);
//�ַ����ָ�
void str_split(const char *str,char *cmd,char *arg,char c);
int isall_space(const char *str);
//�ַ���ת��Ϊ��д��ʽ
void str_upper(char *str);
//�ַ���ת��ΪСд��ʽ
void str_lower(char *str);
//���ַ���ת��Ϊlong long
long long str_to_longlong(const char *str);
//���ַ���(�˽���)ת��Ϊ�޷�������
unsigned int str_octal_to_uint(const char*str);


#endif
