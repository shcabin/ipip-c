#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include "ip_core.h"

struct ipip_s
{
	unsigned char *buffer;
	unsigned char *data;
	unsigned char * offset;
	unsigned int buffer_len;
	unsigned int index;
	unsigned char *last_boundary;	
	int (*ip_find)(ipip_t *h,char *,char *,int);
	int (*ip_find_u)(ipip_t *h,unsigned int ,char *,int);
	void (*ip_dump)(ipip_t *h);
};
ipip_t * ip_init(int type,char *path);	//type 0=dat 1=datx
void ip_dat_dump(ipip_t *h);
void ip_datx_dump(ipip_t *h);
int ip_dat_find(ipip_t *h,char* str_ip,char *output,int output_len);
int ip_datx_find(ipip_t *h,char* str_ip,char *output,int output_len);
int ip_dat_find_u(ipip_t *h,unsigned int uip,char *output,int output_len);
int ip_datx_find_u(ipip_t *h,unsigned int uip,char *output,int output_len);

ipip_t *ip_init(int type,char *path)
{
	struct ipip_s *ipip_info=0;
	struct stat st;
	unsigned long filesize = -1;  
	fprintf(stderr,"enter ip_init %s\n",path);
	if((type!=0)&&(type!=1))
	{
		fprintf(stderr,"type error\n");
		return 0;
	}
	if (stat(path, &st) == -1) 
	{
		fprintf(stderr,"stat error:%s\n",strerror(errno));
		return 0;
	}
	filesize=st.st_size;
	if(filesize>20*1024*1024)
	{
		fprintf(stderr,"File too large\n");
		return 0 ;
	}
	if(filesize<4)
	{//is a joke?
		fprintf(stderr,"File too small\n");
		return 0 ;
	}
	int fd=open(path,O_RDONLY);
	if(fd<=0)
	{
		fprintf(stderr,"open file error:%s\n",strerror(errno));
		return 0;
	}
	unsigned char *buffer=(unsigned char *)malloc(filesize);
	if(buffer==NULL)
	{
		return 0;
	}
	ssize_t nread=read(fd,buffer,filesize);
	if(nread<filesize)
	{
		free(buffer);
		return 0;
	}
	close(fd);
	
	int index_len=(buffer[0]<<24)+(buffer[1]<<16)+(buffer[2]<<8)+buffer[3];
	if(index_len>filesize)
	{
		fprintf(stderr,"File error\n");
		free(buffer);
		return 0;	//文件不完整
	}
	ipip_info=calloc(sizeof(struct ipip_s),1);
	if(ipip_info==NULL)
	{
		free(buffer);
		return 0;
	}
	ipip_info->buffer=buffer;
	ipip_info->buffer_len=filesize;
	ipip_info->offset=buffer+index_len;
	ipip_info->last_boundary=buffer+filesize;
		
	char temp[16];
	if(type==0)
	{
		ipip_info->ip_find=ip_dat_find;
		ipip_info->ip_find_u=ip_dat_find_u;
		ipip_info->ip_dump=ip_dat_dump;
	}
	else
	{
		ipip_info->ip_find=ip_datx_find;
		ipip_info->ip_find_u=ip_datx_find_u;
		ipip_info->ip_dump=ip_datx_dump;
	}
	if(ipip_info->ip_find(ipip_info,"255.255.255.255",temp,sizeof(temp))<0)
	{
		fprintf(stderr,"File error\n");
		free(ipip_info);
		free(buffer);
		return 0;	//文件不完整
	}
	return ipip_info;
}
void ip_uninit(ipip_t *hip)
{
	if(hip)
	{
		free(hip->buffer);
		free(hip);
	}
}
void inline ip_dump(ipip_t *h)
{
	return h->ip_dump(h);
}
int inline ip_find(ipip_t *h,char* str_ip,char *output,int output_len)
{
	return h->ip_find(h,str_ip,output,output_len);	
}
int inline ip_find_u(ipip_t *h,unsigned int uip,char *output,int output_len)
{
	return h->ip_find_u(h,uip,output,output_len);	
}

void ip_dat_dump(ipip_t *h)
{
    ;//not implemented
}

int ip_dat_find_u(ipip_t *h,unsigned int uip,char *output,int output_len)
{
	unsigned char * fip_pos = h->buffer + 4 + (uip>>24)*4;
	int start = (fip_pos[3]<<24)+
				(fip_pos[2]<<16)+
				(fip_pos[1]<<8)+
				fip_pos[0];	//start
	unsigned char *start_pos=h->buffer+1028;
	unsigned char *end_pos = h->offset-1028;
	unsigned int index_offset = 0 , index_length = 0;
	for (start_pos=start_pos+start*8; start_pos<end_pos; start_pos+=8)
	{
		unsigned int cip = 0;
		memcpy(&cip, start_pos, 4);
		cip = ntohl(cip);
		if (cip >= uip) 
		{
			index_offset=(start_pos[6]<<16)+
						(start_pos[5]<<8)+
						start_pos[4];		
			index_length=start_pos[7];
			unsigned char *res_offset = h->offset + index_offset - 1024;
			int len=index_length<(output_len-1)?index_length:(output_len-1);
			memcpy(output,res_offset,len);
			output[len]='\0';
			break;
		}
	}
	return 0;
}

int ip_dat_find(ipip_t *h,char* str_ip,char *output,int output_len)
{
	struct sockaddr_in sockaddr={0};
	if(output_len<=0)
	{
		return 0;
	}
	if(inet_aton(str_ip, &sockaddr.sin_addr) == 0)
	{
		memset(output,'\t',output_len-1);
		output[output_len-1]=0;
		return 0;
	}
	unsigned int uip = ntohl(sockaddr.sin_addr.s_addr);
	return ip_dat_find_u(h,uip,output,output_len);
}

void ip_datx_dump(ipip_t *h)
{
    ;//not implemented
}
int ip_datx_find_u(ipip_t *h,unsigned int uip,char *output,int output_len)
{
	unsigned char * fip_pos = h->buffer + 4 + (uip>>16)*4;
	int start = (fip_pos[3]<<24)+(fip_pos[2]<<16)+
				(fip_pos[1]<<8)+fip_pos[0];	//start
	unsigned char *start_pos=h->buffer+256*1024+4;	//9+262144
	unsigned char *end_pos = h->offset-256*1024-4;
	int index_offset = 0 , index_length = 0;
	for (start_pos=start_pos+start*9; start_pos<end_pos; start_pos+=9)
	{
		unsigned int cip = 0;
		memcpy(&cip, start_pos, 4);
		cip = ntohl(cip);
		if (cip >= uip) 
		{
			index_offset=(start_pos[6]<<16)+
						(start_pos[5]<<8)+
						start_pos[4];
			index_length=(start_pos[7]<<8)+start_pos[8];
			unsigned char *res_offset = h->offset + index_offset - 262144;
			if(res_offset+index_length>h->last_boundary)
			{
				fprintf(stderr,"error last_boundary=%d\n",index_offset);
				memset(output,'\t',output_len-1);				
				output[output_len-1]=0;
				return -1;
			}
			int len=index_length<(output_len-1)?index_length:(output_len-1);
			memcpy(output,res_offset,len);
			output[len]=0;
			break;
		}
	}
	return 0;
}

int ip_datx_find(ipip_t *h,char* str_ip,char *output,int output_len)
{
	struct sockaddr_in sockaddr={0};
	if(output_len<=0)
	{
		return -1;
	}
	if(inet_aton(str_ip, &sockaddr.sin_addr) == 0)
	{
		memset(output,'\t',output_len-1);
		output[output_len-1]=0;
		return -1;
	}
	unsigned int uip = ntohl(sockaddr.sin_addr.s_addr);
	return ip_datx_find_u(h,uip,output,output_len);
}
