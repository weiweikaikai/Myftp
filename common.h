/*************************************************************************
	> File Name: common.h
	> Author: wk
	> Mail: 18402927708@163.com
	> Created Time: Tue 17 May 2016 08:07:53 PM CST
 ************************************************************************/

#ifndef _COMMON_H
#define _COMMON_H
#include<unistd.h>
#include<errno.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<netinet/in.h>
#include<netdb.h>
#include<fcntl.h>
#include<arpa/inet.h>
#include<string.h>
#include<signal.h>
#include<pwd.h>
#include<time.h>
#include<sys/sendfile.h>

#include"str_op.h"
#include"strings.h"



#define MAX_COMMND_LINE 1024
#define MAX_COMMND 32
#define MAX_ARG 1024
#define FTP_CONF_PATH "./myftp.conf"




#define ERR_EXIT(m)\
   do\
   {\
     perror(m);\
	 exit(EXIT_FAILURE);\
   }while(0)



#endif
