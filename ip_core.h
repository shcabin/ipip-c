
#ifndef __IP_CORE_H__
#define __IP_CORE_H__ 
typedef struct ipip_s ipip_t;

ipip_t * ip_init(int type,char *path);	//type 0=dat 1=datx
void ip_uninit(ipip_t *hip);
int ip_find(ipip_t *h,char* str_ip,char *output,int output_len);

#endif
