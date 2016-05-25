/*************************************************************************
	> File Name: str_op.c
	> Author: wk
	> Mail: 18402927708@163.com
	> Created Time: Wed 18 May 2016 06:20:41 PM CST
 ************************************************************************/

#include"common.h"


//ȥ��\r\n
void str_trim_ctrl(char *str)
{
   if(str == NULL)
   {
	   return;
   }
   char *pcur = &str[strlen(str)-1];
   if(*pcur == '\r' || *pcur == '\n')
	{
	   *pcur--='\0';
	   if(*pcur == '\r' || *pcur == '\n')
		{
	   *pcur='\0';
	    }
    }
	//printf("hahahh: [%s]\n",str);
}

//�ַ����ָ�
void str_split(const char *str,char *cmd,char *arg,char c)
{
	if(str == NULL)
	{
	 return ;
	}
     char *p = strchr(str,c); //���ҳ���C�ַ���λ��
	 if(p == NULL)
	{
	   strcpy(cmd,str); 
	}
	else
	{
	  strncpy(cmd,str,p-str);
	  strcpy(arg,p+1);
	}
}
//�ж��Ƿ�ȫ���ǿհ��ַ�
int isall_space(const char *str)
{
	while(*str != '\0')
	{
	   if(*str != ' ')
		{
	     return 0;
	   }
	   str++;
	}
   return 1;
}
//�ַ���ת��Ϊ��д��ʽ
void str_upper(char *str)
{
	if(str == NULL)
	{
	 return ;
	}
   while(*str != '\0')
	{
      *str &=~0x20;
	  str++;
    }
}
//�ַ���ת��ΪСд��ʽ
void str_lower(char *str)
{
	if(str == NULL)
	{
	 return ;
	}
   while(*str != '\0')
	{
      *str |= 0x20;
	   str++;
    }
}
//���ַ���ת��Ϊlong long ������
long long str_to_longlong(const char *str)
{
	if(str == NULL)
	{
		return 0;
	}
    long long result=0;
	unsigned int len = strlen(str);
	unsigned int i=0;
	if(len > 15)
	{
	  return 0;
	}
	for(i=0;i<len;++i)
	{
		if(str[i] <'0' || str[i] >'9')
		{
		 return result;
		}
		
	  result = result*10 +str[i]-'0';
	}
	return result;
}
//���ַ���(�˽���)ת��Ϊ�޷�������
unsigned int str_octal_to_uint(const char*str)
{
  if(str == NULL)
	{
    return 0;
    }
    long long result=0;
	unsigned int len = strlen(str);
	unsigned int i=0;
	for(i=1;i<len;++i)
	{
		if(str[i] <'0' || str[i] >'7')
		{
		 return result;
		}
		
	  result = result*8 +str[i]-'0';
	}
	return result;
}


