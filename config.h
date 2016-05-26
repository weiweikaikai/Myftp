/*************************************************************************
	> File Name: config.h
	> Author: wk
	> Mail: 18402927708@163.com
	> Created Time: Wed 18 May 2016 08:02:54 PM CST
 ************************************************************************/

#ifndef _CONFIG_H
#define _CONFIG_H


typedef struct Mapconfig
{
  char *key;
  char**value;
}Mapconfig_t;

extern int tunable_pasv_enable;
extern int tunable_port_enable;
extern unsigned short tunable_listen_port;
extern unsigned int tunable_max_clients;
extern unsigned int tunable_max_per_ip;
extern unsigned int tunable_accept_timeout;
extern unsigned int tunable_connect_timeout;
extern unsigned int tunable_idle_session_timeout;
extern unsigned int tunable_local_umask;
extern unsigned int tunable_upload_max_rate;
extern unsigned int tunable_download_max_rate;
extern char tunable_listen_address[20];

extern Mapconfig_t array[];

#endif
