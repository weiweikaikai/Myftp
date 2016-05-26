/*************************************************************************
	> File Name: config.c
	> Author: wk
	> Mail: 18402927708@163.com
	> Created Time: Wed 18 May 2016 08:03:05 PM CST
 ************************************************************************/

#include"config.h"
#include"common.h"

//如果配置文件中没有配置这些值 就会 使用默认值
int tunable_pasv_enable = 1;
int tunable_port_enable = 1;
unsigned short tunable_listen_port = 9001;
unsigned int tunable_max_clients = 20000;
unsigned int tunable_max_per_ip = 50;
unsigned int tunable_accept_timeout = 60;
unsigned int tunable_connect_timeout= 300;
unsigned int tunable_idle_session_timeout =10;
unsigned int tunable_local_umask = 0777;
unsigned int tunable_upload_max_rate = 102400; //字节为单位 100k
unsigned int tunable_download_max_rate = 204800; //200k
char tunable_listen_address[20]={'\0'};


Mapconfig_t array[]=
	   {
		   {"pasv_enable",&tunable_pasv_enable},
		   {"port_enable",&tunable_port_enable},
		   {"listen_port",&tunable_listen_port},
		   {"max_clients",&tunable_max_clients},
		   {"max_per_ip",&tunable_max_per_ip},
		   {"accept_timeout",&tunable_accept_timeout},
		   {"connect_timeout",&tunable_connect_timeout},
		   {"session_timeout",&tunable_idle_session_timeout},
		   {"local_umask",&tunable_local_umask},
		   {"upload_max_rate",&tunable_upload_max_rate},
		   {"download_max_rate",&tunable_download_max_rate},
		   {"listen_addres",(const char**)&tunable_listen_address},
		   {NULL,NULL}
       };
