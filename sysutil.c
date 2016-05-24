/*************************************************************************
	> File Name: sysutil.c
	> Author: wk
	> Mail: 18402927708@163.com
	> Created Time: Tue 17 May 2016 07:43:25 PM CST
 ************************************************************************/


#include"sysutil.h"
#include"session.h"
/*
   tcp_server-启动tcp服务器
   @host: 服务器ip地址或者主机名
   @port：服务器端口号
    成功返回监听套接字
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
	if((listenfd = socket(AF_INET,SOCK_STREAM,0)) < 0)   //这里少了一个括号导致错误出现setsockopt: Socket operation on non-socket在此处记录一下
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
int getlocalip(char *ip) //获取本地IP
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
    activate_nonblock  -设置文件描述符为阻塞模式
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
    deactivate_nonblock:设置去掉非阻塞属性
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
 * read_timeout - 读超时检测函数，不含读操作
 * @fd: 文件描述符
 * @wait_seconds: 等待超时秒数，如果为0表示不检测超时
 * 成功（未超时）返回0，失败返回-1，超时返回-1并且errno = ETIMEDOUT
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
		
		//select返回值三态
		//1 若timeout时间到（超时），没有检测到读事件 ret返回=0
		//2 若ret返回<0 &&  errno == EINTR 说明select的过程中被别的信号中断（可中断睡眠原理）
		//2-1 若返回-1，select出错
		//3 若ret返回值>0 表示有read事件发生，返回事件发生的个数
		
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
 * write_timeout - 写超时检测函数，不含写操作
 * @fd: 文件描述符
 * @wait_seconds: 等待超时秒数，如果为0表示不检测超时
 * 成功（未超时）返回0，失败返回-1，超时返回-1并且errno = ETIMEDOUT
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
    accept_timeout   读超时检测函数 不含读操作
     @fd  文件描述符
	 @wait_seconds 等待超时秒数如果为0 表示不检测超时
	 成功 返回 已经连接的套接字 失败返回-1  超时返回-1 并且 errno=ETIEDOUT
*/
int accept_timeout(int fd,struct sockaddr_in *addr,unsigned int wait_seconds)
{
    int ret = 0;
	if(wait_seconds > 0)
	{
	    fd_set read_fdset;
		struct timeval timeout;

		FD_ZERO(&read_fdset); //清零读时间描述符集合
		FD_SET(fd,&read_fdset); //将文件描述符fd加入select监听集合中

		timeout.tv_sec = wait_seconds; //秒
		timeout.tv_usec = 0;  //毫秒
           
        do
		{
		   ret= select(fd+1,&read_fdset,NULL,NULL,&timeout);//加入读事件
		
		}while(ret < 0 && errno == EINTR);//如果没有时间发生且有中断就绪select监听
        if(ret ==-1)
		{
		 return -1;
		}
		else if(ret == 0) //没有时间发生 超时
		{
         errno = ETIMEDOUT;
		 return -1;
		}
	}
        //没有超时 说明时间发生可以直接accept 不用阻塞
			if(addr != NULL) //服务器对客户端套接字信息感兴趣
	        {
			   socklen_t addrlen = sizeof(struct sockaddr_in );
		      ret = accept(fd,(struct sockaddr*)addr,&addrlen);
		    }
			else //客户端的套接字不关心
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
 * @fd: 套接字
 * @addr: 要连接的对方地址
 * @wait_seconds: 等待超时秒数，如果为0表示正常模式
 * 成功（未超时）返回0，失败返回-1，超时返回-1并且errno = ETIMEDOUT
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
			// 一但连接建立，则套接字就可写  所以connect_fdset放在了写集合中
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
			/* ret返回为1（表示套接字可写），可能有两种情况，一种是连接建立成功，一种是套接字产生错误，*/
			/* 此时错误信息不会保存至errno变量中，因此，需要调用getsockopt来获取。 */
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
 * readn - 读取固定字节数
 * @fd: 文件描述符
 * @buf: 接收缓冲区
 * @count: 要读取的字节数
 * 成功返回count，失败返回-1，读到EOF返回<count
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
 * writen - 发送固定字节数
 * @fd: 文件描述符
 * @buf: 发送缓冲区
 * @count: 要读取的字节数
 * 成功返回count，失败返回-1
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
		else if (nwritten == 0) //没写进去再尝试一下
			continue;

		bufp += nwritten;
		nleft -= nwritten;
	}
   printf("writen count:%d\n",count);
	return count;
 
}
/*
 * recv_peek - 仅仅查看套接字缓冲区数据，但不移除数据
 * @sockfd: 套接字
 * @buf: 接收缓冲区
 * @len: 长度
 * 成功返回>=0，失败返回-1
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
   the_lock.l_whence =SEEK_SET;//从哪开始
   the_lock.l_start =0; //加锁开始的偏移
   the_lock.l_len =0; //加锁的字节数
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
   the_lock.l_whence =SEEK_SET;//从哪开始
   the_lock.l_start =0; //加锁开始的偏移
   the_lock.l_len =0; //加锁的字节数
	int flag=fcntl(fd,F_SETLK,&the_lock);
    return flag;
}