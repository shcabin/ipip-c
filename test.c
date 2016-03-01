#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include "ip_core.h"
int main(int argc,char *argv[])
{
	ipip_t *hip;
	int type=-1;
	char buffer[256];
	int k;
	if(argc==3)
	{
		if( (strcmp(argv[1],"dat")==0) )
		{
			type=0;
		}
		else if(strcmp(argv[1],"datx")==0 )
		{
			type=1;
		}
		else
		{
			fprintf(stderr,"param error,usage: exec dat|datx path\n");
			return -1;
		}
		hip=ip_init(type,argv[2]);
	}
	else
	{
		hip=ip_init(0,"./17monipdb.dat");
	}
	if(hip==NULL)
	{
		fprintf(stderr,"usage: exec dat|datx path\n");
		return -1;
	}
	
	ip_find(hip,"203.208.62.0",buffer,sizeof(buffer));
	printf("info:%s\n",buffer);
	ip_find(hip,"203.208.62.1",buffer,sizeof(buffer));
	printf("info:%s\n",buffer);
	ip_find(hip,"209.85.228.223",buffer,sizeof(buffer));
	printf("info:%s\n",buffer);
	ip_find_u(hip,202*256*256*256+104*256*256+136*256+197,buffer,sizeof(buffer));
	printf("info:%s\n",buffer);
	ip_find(hip,"12.33.3.1",buffer,sizeof(buffer));
	printf("info:%s\n",buffer);

#ifdef benchmark
	struct timeval tpstart,tpend; 
	float timeuse; 
	gettimeofday(&tpstart,NULL); 
	unsigned int uip;
	for(k=0;k<10000*100;k++)
	{
	#if 0
		ip_find(hip,"8.8.8.8",buffer,sizeof(buffer));
		ip_find(hip,"192.168.0.1",buffer,sizeof(buffer));
		ip_find(hip,"255.255.255.255",buffer,sizeof(buffer));
	
		ip_find(hip,"1.2.3.4",buffer,sizeof(buffer));
		ip_find(hip,"111.222.333.444",buffer,sizeof(buffer));
	#else
		uip=rand()<<2;
		ip_find_u(hip,uip,buffer,sizeof(buffer));
		uip=rand()<<2;
		ip_find_u(hip,uip,buffer,sizeof(buffer));
		uip=rand()<<2;
		ip_find_u(hip,uip,buffer,sizeof(buffer));
		uip=rand()<<2;
		ip_find_u(hip,uip,buffer,sizeof(buffer));
		uip=rand()<<2;
		ip_find_u(hip,uip,buffer,sizeof(buffer));
	#endif
	}
	gettimeofday(&tpend,NULL); 
	timeuse=1000*(tpend.tv_sec-tpstart.tv_sec)+ (tpend.tv_usec-tpstart.tv_usec)/1000; 
	printf("used uime:%f(ms)\n",timeuse); 
#endif
	ip_uninit(hip);
	return 0;
}

