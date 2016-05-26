/*************************************************************************
	> File Name: parseconfig.c
	> Author: wk
	> Mail: 18402927708@163.com
	> Created Time: Wed 18 May 2016 08:15:42 PM CST
 ************************************************************************/


#include"common.h"
#include"parseconfig.h"
#include"config.h"

void parseconf_load_file_path(const char*path)
{
	  if(path == NULL)
	   {
	     return ;
	    }
       FILE *fp = fopen(path,"r");
	   if(fp == NULL)
	   {
	       ERR_EXIT("fopen");
	   }

	   char setting_line[1024]={'\0'};

	   while( fgets(setting_line,sizeof(setting_line),fp) != NULL)
	    {
	      if(strlen(setting_line) == 0 || setting_line[0]=='#'
			  || isall_space(setting_line) )
			{
		     continue;
		    }
		  str_trim_ctrl(setting_line);
		  //printf("%s\n",setting_line);
		  parseconf_load_setting(setting_line);
		  memset(setting_line,0,sizeof(setting_line));
	   }
	   

}
void parseconf_load_setting(const char* setting)
{

   if(setting == NULL)
	{
     return ;
    }
	//È¥³ý×ó¿Õ¸ñ
	 char key[128]={'\0'};
	 char value[128]={'\0'};
	 str_split(setting,key,value,'=');
	// printf("key:%s  value: %s\n",key,value);
    Mapconfig_t *ptmp=array;
	while(ptmp->key != NULL)
	{
	 if(strcasecmp(ptmp->key,key) == 0)
		{
		  if(strchr(value,'.') != NULL)
		  {
			strcpy(ptmp->value,value);
        // printf("value:%s\n",ptmp->value);
		  }
          else
			{
		  *(ptmp->value) =str_to_longlong(value);
		  //printf("value:%d\n",*(ptmp->value));
			}
		  return ;
	     }
        ptmp++;
	}

}
