/*************************************************************************
	> File Name: sysutil.c
	> Author: wk
	> Mail: 18402927708@163.com
	> Created Time: Tue 17 May 2016 07:43:25 PM CST
 ************************************************************************/


#include"sysutil.h"
#include"session.h"
/*
   tcp_server-����tcp������
   @host: ������ip��ַ����������
   @port���������˿ں�
    �ɹ����ؼ����׽���
*/

int tcp_server(const char *ip,unsigned short port)
{
	struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(servaddr));
   
	servaddr.sin_family = AF_INET;
	if(ip != NULL)
	{
	   servaddr.sin_addr.s_addr=inet_addr(ip);
	}
	else
	{
	   servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	}
	servaddr.sin_port = htons(port);
     int listenfd = 0;
	if((listenfd = socket(AF_INET,SOCK_STREAM,0)) < 0)   //��������һ�����ŵ��´������setsockopt: Socket operation on non-socket�ڴ˴���¼һ��
	{
	      ERR_EXIT("socket");
	}
     int on=1;
	 if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,(const char*)&on,sizeof(on)) <0)
	  {
	   ERR_EXIT("setsockopt");
	 }
	 if(bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) <0)
	{
	 ERR_EXIT("bind");
	 }
     
	 if(listen(listenfd,SOMAXCONN) < 0)
	 {
	    ERR_EXIT("listen");
	 }
	 return listenfd;
}
int getlocalip(char *ip) //��ȡ����IP
{
	/*
   char host[100]={'\0'};
   if(gethostname(host,sizeof(host)) < 0)
	{
      return -1;
    }
	printf("%s\n",host);
	struct hostent *hp;
	if((hp = gethostbyname(host)) == NULL)
	{
	return -1;
	}
	strcpy(ip,inet_ntoa(*(struct in_addr*)hp->h_addr));
     */
	 strcpy(ip,"121.42.180.114");
	return 0;
}

/* 
    activate_nonblock  -�����ļ�������Ϊ����ģʽ
*/
void activate_nonblock(int fd) 
{
     int ret=0;
	 int flags = fcntl(fd,F_GETFL);
	   if(flags == -1)
	   {
	     ERR_EXIT("fcntl");
	   }
	   flags |= O_NONBLOCK;
	  ret = fcntl(fd,F_SETFL,flags);
	 if(flags == -1)
	   {
	     ERR_EXIT("fcntl");
	   }

}
/*
    deactivate_nonblock:����ȥ������������
*/

void deactivate_nonblock(int fd)
{
    int ret=0;
	 int flags = fcntl(fd,F_GETFL);
	   if(flags == -1)
	   {
	     ERR_EXIT("fcntl");
	   }
      flags &=~O_NONBLOCK;
	  ret=fcntl(fd,F_SETFL,flags);
	  if(ret == -1)
	  {
	     ERR_EXIT("fcntl");
	  } 
}



/**
 * read_timeout - ����ʱ��⺯��������������
 * @fd: �ļ�������
 * @wait_seconds: �ȴ���ʱ���������Ϊ0��ʾ����ⳬʱ
 * �ɹ���δ��ʱ������0��ʧ�ܷ���-1����ʱ����-1����errno = ETIMEDOUT
 */
int read_timeout(int fd,unsigned int wait_seconds)
{
	int ret = 0;
	if (wait_seconds > 0)
	{
		fd_set read_fdset;
		struct timeval timeout;

		FD_ZERO(&read_fdset);
		FD_SET(fd, &read_fdset);

		timeout.tv_sec = wait_seconds;
		timeout.tv_usec = 0;
		
		//select����ֵ��̬
		//1 ��timeoutʱ�䵽����ʱ����û�м�⵽���¼� ret����=0
		//2 ��ret����<0 &&  errno == EINTR ˵��select�Ĺ����б�����ź��жϣ����ж�˯��ԭ��
		//2-1 ������-1��select����
		//3 ��ret����ֵ>0 ��ʾ��read�¼������������¼������ĸ���
		
		do
		{
			ret = select(fd + 1, &read_fdset, NULL, NULL, &timeout);
		} while (ret < 0 && errno == EINTR); 

		if (ret == 0)
		{
			ret = -1;
			errno = ETIMEDOUT;
		}
		else if (ret == 1)
			ret = 0;
	}

	return ret;
}
	/**
 * write_timeout - д��ʱ��⺯��������д����
 * @fd: �ļ�������
 * @wait_seconds: �ȴ���ʱ���������Ϊ0��ʾ����ⳬʱ
 * �ɹ���δ��ʱ������0��ʧ�ܷ���-1����ʱ����-1����errno = ETIMEDOUT
 */
int write_timeout(int fd,unsigned int wait_seconds)
{
int ret = 0;
	if (wait_seconds > 0)
	{
		fd_set write_fdset;
		struct timeval timeout;

		FD_ZERO(&write_fdset);
		FD_SET(fd, &write_fdset);

		timeout.tv_sec = wait_seconds;
		timeout.tv_usec = 0;
		do
		{
			ret = select(fd + 1, NULL, &write_fdset, NULL, &timeout);
		} while (ret < 0 && errno == EINTR);

		if (ret == 0)
		{
			ret = -1;
			errno = ETIMEDOUT;
		}
		else if (ret == 1)
			ret = 0;
	}

	return ret;
}

/*
    accept_timeout   ����ʱ��⺯�� ����������
     @fd  �ļ�������
	 @wait_seconds �ȴ���ʱ�������Ϊ0 ��ʾ����ⳬʱ
	 �ɹ� ���� �Ѿ����ӵ��׽��� ʧ�ܷ���-1  ��ʱ����-1 ���� errno=ETIEDOUT
*/
int accept_timeout(int fd,struct sockaddr_in *addr,unsigned int wait_seconds)
{
    int ret = 0;
	if(wait_seconds > 0)
	{
	    fd_set read_fdset;
		struct timeval timeout;

		FD_ZERO(&read_fdset); //�����ʱ������������
		FD_SET(fd,&read_fdset); //���ļ�������fd����select����������

		timeout.tv_sec = wait_seconds; //��
		timeout.tv_usec = 0;  //����
           
        do
		{
		   ret= select(fd+1,&read_fdset,NULL,NULL,&timeout);//������¼�
		
		}while(ret < 0 && errno == EINTR);//���û��ʱ�䷢�������жϾ���select����
        if(ret ==-1)
		{
		 return -1;
		}
		else if(ret == 0) //û��ʱ�䷢�� ��ʱ
		{
         errno = ETIMEDOUT;
		 return -1;
		}
	}
        //û�г�ʱ ˵��ʱ�䷢������ֱ��accept ��������
			if(addr != NULL) //�������Կͻ����׽�����Ϣ����Ȥ
	        {
			   socklen_t addrlen = sizeof(struct sockaddr_in );
		      ret = accept(fd,(struct sockaddr*)addr,&addrlen);
		    }
			else //�ͻ��˵��׽��ֲ�����
	        {
			     ret =ret = accept(fd,NULL,NULL);
			}
			if(ret == -1)
	          {
			    return -1;
			  }
		   return ret;
}
/*
 * connect_timeout - connect
 * @fd: �׽���
 * @addr: Ҫ���ӵĶԷ���ַ
 * @wait_seconds: �ȴ���ʱ���������Ϊ0��ʾ����ģʽ
 * �ɹ���δ��ʱ������0��ʧ�ܷ���-1����ʱ����-1����errno = ETIMEDOUT
 */
int connect_timeout(int fd,struct sockaddr_in *addr,unsigned int wait_seconds)
{
       int ret=0;
	socklen_t addrlen = sizeof(struct sockaddr_in);

	if (wait_seconds > 0)
		activate_nonblock(fd);

	ret = connect(fd, (struct sockaddr*)addr, addrlen);
		
	if (ret < 0 && errno == EINPROGRESS)
	{
		fd_set connect_fdset;
		struct timeval timeout;
		FD_ZERO(&connect_fdset);
		FD_SET(fd, &connect_fdset);
		timeout.tv_sec = wait_seconds;
		timeout.tv_usec = 0;
		do
		{
			// һ�����ӽ��������׽��־Ϳ�д  ����connect_fdset������д������
			ret = select(fd + 1, NULL, &connect_fdset, NULL, &timeout);
		} while (ret < 0 && errno == EINTR);
		if (ret == 0)
		{		
			ret = -1;
			errno = ETIMEDOUT;
		}
		else if (ret < 0)
		{
			
			return -1;
		}
		else if (ret == 1)
		{	
			/* ret����Ϊ1����ʾ�׽��ֿ�д�������������������һ�������ӽ����ɹ���һ�����׽��ֲ�������*/
			/* ��ʱ������Ϣ���ᱣ����errno�����У���ˣ���Ҫ����getsockopt����ȡ�� */
			int err;
			socklen_t socklen = sizeof(err);
			int sockoptret = getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &socklen);
			if (sockoptret == -1)
			{
				return -1;
			}
			if (err == 0)
			{
				ret = 0;
			}
			else
			{
				errno = err;
				ret = -1;
			}
		}
	}
	if (wait_seconds > 0)
	{
		deactivate_nonblock(fd);
	}

	return ret;
}

/*
 * readn - ��ȡ�̶��ֽ���
 * @fd: �ļ�������
 * @buf: ���ջ�����
 * @count: Ҫ��ȡ���ֽ���
 * �ɹ�����count��ʧ�ܷ���-1������EOF����<count
 */

ssize_t readn(int fd,void *buf,size_t count)
{
    size_t nleft = count;
	ssize_t nread=0;
	char *bufp = (char*)buf;
   
	while(nleft > 0)
	{
	   if((nread = read(fd,bufp,nleft)) < 0)
		{
	      if(errno == EINTR)
			{
		     continue;
		    }
			return -1;
	    }
		else  if(nread == 0)
		     {
			 return count-nleft;
		      }
		
			  bufp += nread;
			  nleft -= nread;
	}
	printf("readn  count %d\n",count);
	return count;
      

}
/*
 * writen - ���͹̶��ֽ���
 * @fd: �ļ�������
 * @buf: ���ͻ�����
 * @count: Ҫ��ȡ���ֽ���
 * �ɹ�����count��ʧ�ܷ���-1
 */
ssize_t writen(int fd,const void *buf,size_t count)
{
   	size_t nleft = count;
	ssize_t nwritten=0;
	char *bufp = (char*)buf;

	while (nleft > 0)
	{
		if ((nwritten = write(fd, bufp, nleft)) < 0)
		{
			if (errno == EINTR)
				continue;
			return -1;
		}
		else if (nwritten == 0) //ûд��ȥ�ٳ���һ��
			continue;

		bufp += nwritten;
		nleft -= nwritten;
	}
   printf("writen count:%d\n",count);
	return count;
 
}
/*
 * recv_peek - �����鿴�׽��ֻ��������ݣ������Ƴ�����
 * @sockfd: �׽���
 * @buf: ���ջ�����
 * @len: ����
 * �ɹ�����>=0��ʧ�ܷ���-1
 */
ssize_t recv_peek(int sockfd,void *buf,size_t len)
{
while (1)
	{
		int ret = recv(sockfd, buf, len, MSG_PEEK);
		if (ret == -1 && errno == EINTR)	
		{
			continue;
		}
		return ret;
	}

}

ssize_t readline(int sockfd, void *buf, size_t maxline)
{
	int ret=0;
	int nread=0;
	char *bufp = buf;
	int nleft = maxline;
	while (1)
	{
		ret = recv_peek(sockfd, bufp, nleft);
		if (ret < 0)
			return ret;
		else if (ret == 0)
			return ret;

		nread = ret;
		int i;
		for (i=0; i<nread; i++)
		{
			if (bufp[i] == '\n')
			{
				ret = readn(sockfd, bufp, i+1);
				if (ret != i+1)
					exit(EXIT_FAILURE);
				return ret;
			}
		}

		if (nread > nleft)
			exit(EXIT_FAILURE);

		nleft -= nread;
		ret = readn(sockfd, bufp, nread);
		if (ret != nread)
			exit(EXIT_FAILURE);

		bufp += nread;

		
	}

	return -1;
}

void send_fd(int sock_fd,int fd)
{
      int ret = 0;
	  struct msghdr msg;
	  struct cmsghdr *p_cmsg;
	  struct iovec vec;
	  char cmsgbuf[CMSG_SPACE(sizeof(fd))];
	  int *p_fds;
	  char sendchar = 0;

	  msg.msg_control = cmsgbuf;
	  msg.msg_controllen = sizeof(cmsgbuf);
	  p_cmsg= CMSG_FIRSTHDR(&msg);
	  p_cmsg->cmsg_level = SOL_SOCKET;
	  p_cmsg->cmsg_type = SCM_RIGHTS;
	  p_cmsg->cmsg_len = CMSG_LEN(sizeof(fd));
	  p_fds = (int*)CMSG_DATA(p_cmsg);
	  *p_fds =fd;

	  msg.msg_name =NULL;
	  msg.msg_namelen = 0;
	  msg.msg_iov = &vec;
	  msg.msg_iovlen = 1;
	  msg.msg_flags = 0;

	  vec.iov_base = &sendchar;
	  vec.iov_len = sizeof(sendchar);
	 ret = sendmsg(sock_fd,&msg,0);
	 if(ret != 1)
	  {
	   ERR_EXIT("sendmsg");
	  } 
}
int recv_fd(const int sock_fd)
{
  int ret =0;
  struct msghdr msg;
  char recvchar;
  struct iovec vec;
  int recvfd;
  char cmsgbuf[CMSG_SPACE(sizeof(recvfd))];
  struct cmsghdr *p_cmsg;
  int *p_fd;
  vec.iov_base = &recvchar;
  vec.iov_len = sizeof(recvchar);
  msg.msg_name = NULL;
  msg.msg_namelen =0;
  msg.msg_iov = &vec;
  msg.msg_iovlen =-1;
  msg.msg_control = cmsgbuf;
  msg.msg_controllen = sizeof(cmsgbuf);
  msg.msg_flags=0;

  p_fd = (int*)CMSG_DATA(CMSG_FIRSTHDR(&msg));
  *p_fd = -1;
  
  ret = recvmsg(sock_fd,&msg,0);
  if(ret != 1)
	{
      ERR_EXIT("recvmsg");
    }
     return recvfd;
}

static int lock_internal(int fd,int locktype)
{
   struct flock the_lock;
   memset(&the_lock,'\0',sizeof(the_lock));
   the_lock.l_type = locktype;
   the_lock.l_whence =SEEK_SET;//���Ŀ�ʼ
   the_lock.l_start =0; //������ʼ��ƫ��
   the_lock.l_len =0; //�������ֽ���
	int flag;
	do
	{
        flag = fcntl(fd,F_SETLKW,&the_lock);
	}
	while ( flag < 0 && errno == EINTR);
    return flag;
}
int lock_file_read(int fd)
{
    return lock_internal(fd,F_RDLCK);
}
int lock_file_write(int fd)
{
    return lock_internal(fd,F_WRLCK);
}
int unlock_file(int fd)
{
     struct flock the_lock;
   memset(&the_lock,'\0',sizeof(the_lock));
   the_lock.l_type = F_UNLCK;
   the_lock.l_whence =SEEK_SET;//���Ŀ�ʼ
   the_lock.l_start =0; //������ʼ��ƫ��
   the_lock.l_len =0; //�������ֽ���
	int flag=fcntl(fd,F_SETLK,&the_lock);
    return flag;
}