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
	//�Ự����
	session_t sess =
	{
		//uid
		  0,
		 //���������׽���
			-1,
		 //��������
	     "","","",
		 //�����׽���
		 NULL,
		  -1,
		  -1,
	     //���ӽ���ͨ��
			-1,-1,
		//FTPЭ��״̬
			0,
			0,
			NULL
	};
      signal(SIGCHLD,SIG_IGN); //���⽩ʬ���̵ĳ���
	  //signal(SIGPIPE,SIG_IGN);
	 parseconf_load_file_path("./myftp.conf"); //���������ļ�
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
		if(pid == 0) //�ӽ��̿����Ự
		{
		   close(listenfd);
		   sess.ctrl_fd=conn;
		   begin_session(&sess);  //����ͻ��˵����ӳ���Ϊһ���Ự
		}
		else //������
		{
		  close(conn);
		}
  
    }


    return 0;
}

