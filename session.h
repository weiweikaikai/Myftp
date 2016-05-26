/*************************************************************************
	> File Name: session.h
	> Author: wk
	> Mail: 18402927708@163.com
	> Created Time: Wed 18 May 2016 09:15:08 AM CST
 ************************************************************************/

#ifndef _SESSION_H
#define _SESSION_H
#include"common.h"

typedef struct session
{
	//uid
		uid_t uid;
	 //���������׽���
	    int ctrl_fd; 
      //��������
      char cmdline[MAX_COMMND_LINE];
	  char cmd[MAX_COMMND];
	  char arg[MAX_ARG];
	  //����
	  unsigned int upload_max_rate;
	  unsigned int dowload_max_rate;
	  long start_transfer_time_sec;
	  long start_transfer_time_usec;
	  //���������׽���
       struct sockaddr_in  *port_addr;
	   int pasv_listenfd;
	   int datasock;
	 //���ӽ���ͨ��
	 int parent_fd;
	 int child_fd;
	 //FTPЭ��״̬
	 int is_ascii;
	 long long restart_pos;//���ڶϵ�����
	 char *rnfr_name;
	 int abor_recived;
	 //�Ƿ������� ���ػ����ϴ�
	 int isdata_transfer;
}session_t;

void begin_session(session_t *sess);

#endif
