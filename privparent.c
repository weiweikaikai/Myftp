/*************************************************************************
	> File Name: privparent.c
	> Author: wk
	> Mail: 18402927708@163.com
	> Created Time: Wed 18 May 2016 09:21:43 AM CST
 ************************************************************************/

#include"privparent.h"
#include"privsock.h"
#include"sysutil.h"
#include"config.h"
#include<linux/capability.h>


 /*
    fuction: tcp_client
    success return 0
	failed   return -1
*/
static int tcp_client(unsigned short port)
{
  int sock=0;
  if((sock = socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
    ERR_EXIT("tcp_client");
   }
   if(port > 0)
	{
         int on=1;
	     if(setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(const char*)&on,sizeof(on)) <0)
	      {
	        ERR_EXIT("setsockopt");
	      }
        //char ip[16]={'\0'};
	    //getlocalip(ip);
         char ip[16]="121.42.180.114 ";
	     struct sockaddr_in localaddr;
	     memset(&localaddr,0,sizeof(localaddr));
	     localaddr.sin_family = AF_INET;
         localaddr.sin_port=htons(port);
         localaddr.sin_addr.s_addr=inet_addr(ip);
	    if(bind(sock,(struct sockaddr*)&localaddr,sizeof(localaddr)) <0)
	     {
	     ERR_EXIT("bind");
	     }
    }
	return sock;
}


static void privparent_port_get_data_sock(session_t *sess)
{

   //nobody���̽���ftp���̷��͹����Ķ˿ں�
     int tmp_fd;
     priv_sock_get_int(sess->parent_fd,&tmp_fd);
	 unsigned short port = (unsigned short)tmp_fd;
	  printf("port=%u\n",port);
   //����ip
   char ip[16]={'\0'};
   priv_sock_get_str(sess->parent_fd,ip,sizeof(ip));
   
   printf("port=%u  ip=%s\n",port,ip);
   //������������
	 struct sockaddr_in addr;
	 memset(&addr,0,sizeof(addr));
	 addr.sin_family = AF_INET;
	 addr.sin_port = htons(port);
	 addr.sin_addr.s_addr = inet_addr(ip);
	    //socket->connect
		int fd =tcp_client(20); //����20�˿ڴ�ʱû��Ȩ�ޣ���Ҫnobody����Э����20�˿� ��������Ϊ�ͻ�������
	       if(fd == -1)
	        {
		        priv_send_result(sess->parent_fd,PRIV_SOCK_RESULT_BAD);
			    return ;
		    }
		int ret=0;
	  while((ret=connect_timeout(fd,sess->port_addr,tunable_connect_timeout))< 0)
		   {
		        if(errno == ETIMEDOUT)
			       {
			        printf("connect_timeout\n");
		            continue;
		           }
		           priv_send_result(sess->parent_fd,PRIV_SOCK_RESULT_BAD);
	               close(fd);
		           return;
	       }
       priv_send_result(sess->parent_fd,PRIV_SOCK_RESULT_OK);
	   priv_sock_send_fd(sess->parent_fd,fd);
       close(fd);
}
static void privparent_pasv_active(session_t *sess)
{
   int active=0;
   if(sess->pasv_listenfd != -1)
	{
      active = 1;
    }
     priv_sock_send_int(sess->parent_fd,active);
}
static void privparent_pasv_listen(session_t *sess)
{
	    char ip[16]={'\0'};
		getlocalip(ip);
        int fd= tcp_server(ip,0);
		struct sockaddr_in addr;
	     socklen_t addrlen =sizeof(addr);
		 if(getsockname(fd,(struct sockaddr*)&addr,&addrlen) <0 )
		 {
		  ERR_EXIT("getsockname");
		 }
		 sess->pasv_listenfd=fd;
		 unsigned short port = ntohs(addr.sin_port);
		 priv_sock_send_int(sess->parent_fd,(int)port);

}
static void privparent_pasv_accept(session_t *sess)
{
        int conn=0;
		while((conn=accept_timeout(sess->pasv_listenfd,NULL,tunable_accept_timeout)) < 0)
		{   
		if(errno == ETIMEDOUT)
		{
		 printf("accept_timeout\n");
		  continue;
		  }
         priv_send_result(sess->parent_fd,PRIV_SOCK_RESULT_BAD);
		 close(sess->parent_fd);
		 sess->parent_fd=-1;
	     close(conn);
		  return ;
		}
	    priv_send_result(sess->parent_fd,PRIV_SOCK_RESULT_OK);
		priv_sock_send_fd(sess->parent_fd,conn);
		close(sess->parent_fd);
	    sess->parent_fd=-1;
	    close(conn);

}

static void minimize_privilege()
{
 //nobody ����  ����ftp���������ͻ��˽������������� nobodyֻ��Э�����ڲ�ʹ��
//���ڿͻ��˽�������֮��Ὣ���ӳɹ����ļ�����������ftp����������
//����Ϊ�˰�20�˿� ��ͨ�û���û��Ȩ�ް󶨵�
      struct passwd *pw = getpwnam("nobody"); //�����̸���Ϊnobody
		if(pw == NULL)
	    {
	        return;
	    }
	    if(setegid(pw->pw_gid)<0)//��Ч��id�ĳ�  pw->pw_gid
	     {
	      ERR_EXIT("setegid"); 
	     }
	    if(seteuid(pw->pw_uid)<0)//��Ч�û�id�ĳ�  pw->pw_uid
	    {
	    ERR_EXIT("seteuid"); 
	    }
		//�ý���ӵ�а�������֪�˿ڵķ���
        struct __user_cap_header_struct  cap_header;
        struct __user_cap_data_struct  cap_data;

		cap_header.version = _LINUX_CAPABILITY_VERSION_2;
		cap_header.pid = 0;
        __u32 cap_mask=0;
		cap_mask |= (1<<CAP_NET_BIND_SERVICE);
        cap_data.effective = cap_data.permitted = cap_mask;
        cap_data.inheritable = 0;
		capset(&cap_header,&cap_data);
}

void handle_parent(session_t *sess)
{
    minimize_privilege();
	char cmd='\0';
    while(1)
	{
	//�����ӽ��̷��͹������ڲ�����Э���ӽ������
          priv_get_cmd(sess->parent_fd,&cmd);
		  switch(cmd)
		{
		  case PRIV_SOCK_GET_DATA_SOCK:
			  privparent_port_get_data_sock(sess);
			  break;
		  case PRIV_SOCK_PASV_ACTIVE:
			  privparent_pasv_active(sess);
			  break;
		  case PRIV_SOCK_PASV_LISTEN:
			  privparent_pasv_listen(sess);
			  break;
		  case PRIV_SOCK_PASV_ACCEPT:
			  privparent_pasv_accept(sess);
			  break;
		  }
	}

}
