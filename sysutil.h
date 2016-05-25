/*************************************************************************
	> File Name: sysutil.h
	> Author: wk
	> Mail: 18402927708@163.com
	> Created Time: Tue 17 May 2016 07:43:33 PM CST
 ************************************************************************/

#ifndef _SYSUTIL_H
#define _SYSUTIL_H
#include"common.h"

int tcp_server(const char *ip,unsigned short port);

int getlocalip(char *ip);  //获取本地IP

void activate_nonblock(int fd); //设置文件描述符为非阻塞
void deactivate_nonblock(int fd);//设置去掉非阻塞属性


//几个阻塞函数的超时函数
int read_timeout(int fd,unsigned int wait_seconds);
int write_timeout(int fd,unsigned int wait_seconds);
int accept_timeout(int fd,struct sockaddr_in *addr,unsigned int wait_seconds);
int connect_timeout(int fd,struct sockaddr_in *addr,unsigned int wait_seconds);


ssize_t readn(int fd,void *buf,size_t count);
ssize_t writen(int fd,const void *buf,size_t count);
ssize_t recv_peek(int sockfd,void *buf,size_t len);
ssize_t readline(int sockfd,void *buf,size_t maxline);

void send_fd(int sock_fd,int fd);
int recv_fd(const int sock_fd);

int lock_file_read(int fd);

int lock_file_write(int fd);
int unlock_file(int fd);

long get_now_time_of_sec();
long get_now_time_of_usec();

void nano_sleep(double sleep_time);

#endif /* _SYS_UIL_H_*/
