/*************************************************************************
	> File Name: session.c
	> Author: wk
	> Mail: 18402927708@163.com
	> Created Time: Wed 18 May 2016 09:15:43 AM CST
 ************************************************************************/

#include"session.h"
#include"privsock.h"


void begin_session(session_t *sess)
{
  priv_sock_init(sess);
  active_oobinline(sess->ctrl_fd);
   pid_t pid =fork();
   if(pid < 0)
	{
          ERR_EXIT("fork");
    }
	if(pid == 0) //�ӽ���   
	{
	   //ftp ����������   ����FTP���Э��Ľ��� �������Ӻ���������
		   priv_sock_set_child_context(sess);
		   handle_child(sess);  
	}
	else //������
	{
      priv_sock_set_parent_context(sess);
	   handle_parent(sess);
	}
	priv_sock_close(sess);
}

