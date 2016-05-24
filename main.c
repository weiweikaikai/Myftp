/*************************************************************************
	> File Name: main.c
	> Author: wk
	> Mail: 18402927708@163.com
	> Created Time: Tue 17 May 2016 07:42:40 PM CST
 ************************************************************************/

#include"common.h"
#include"session.h"
#include"parseconfig.h"
#include"config.h"

int main()
{ 
   if(getuid() != 0)
	{
     fprintf(stderr,"miniftpd must be start by root user\n");
      ERR_EXIT("getuid");
	}
	//会话变量
	session_t sess =
	{
		//uid
		  0,
		 //控制连接套接字
			-1,
		 //控制连接
	     "","","",
		 //数据套接字
		 NULL,
		  -1,
		  -1,
	     //父子进程通道
			-1,-1,
		//FTP协议状态
			0,
			0,
			NULL
	};
      signal(SIGCHLD,SIG_IGN); //避免僵尸进程的出现
	  //signal(SIGPIPE,SIG_IGN);
	 parseconf_load_file_path("./myftp.conf"); //加载配置文件
	 printf("%s   %d\n",tunable_listen_address,tunable_listen_port);
  // int listenfd = tcp_server(tunable_listen_address,tunable_listen_port);
      int listenfd = tcp_server("121.42.180.114" ,9001);
    int conn=0;
	pid_t pid;
 while(1)
	{
	 int waittime=5;
	 struct sockaddr_in clientaddr;
	 memset(&clientaddr,0,sizeof(clientaddr));
    conn=accept_timeout(listenfd,&clientaddr,waittime);
      if(conn == -1 && errno ==ETIMEDOUT )
		{
	      printf("accept_timeout\n");
		  continue;
	    }
		else if(conn == -1)
		{
		     ERR_EXIT("accept_timeout");
		}
		printf("client:%s  online\n",inet_ntoa(clientaddr.sin_addr));
		pid = fork();
         if(pid == -1)
		{
		  ERR_EXIT("fork"); 
		}
		if(pid == 0) //子进程开启会话
		{
		   close(listenfd);
		   sess.ctrl_fd=conn;
		   begin_session(&sess);  //将与客户端的连接抽象为一个会话
		}
		else //父进程
		{
		  close(conn);
		}
  
    }


    return 0;
}

