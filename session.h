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
	 //控制连接套接字
	    int ctrl_fd; 
      //控制连接
      char cmdline[MAX_COMMND_LINE];
	  char cmd[MAX_COMMND];
	  char arg[MAX_ARG];
	  //限速
	  unsigned int upload_max_rate;
	  unsigned int dowload_max_rate;
	  long start_transfer_time_sec;
	  long start_transfer_time_usec;
	  //数据连接套接字
       struct sockaddr_in  *port_addr;
	   int pasv_listenfd;
	   int datasock;
	 //父子进程通道
	 int parent_fd;
	 int child_fd;
	 //FTP协议状态
	 int is_ascii;
	 long long restart_pos;//用于断点续传
	 char *rnfr_name;
	 int abor_recived;
	 //是否传输数据 下载或者上传
	 int isdata_transfer;
}session_t;

void begin_session(session_t *sess);

#endif
