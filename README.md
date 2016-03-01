# ipip-c
IP数据库解析代码 支持dat和datx格式 (https://www.ipip.net/)

```shell
gcc ip_core.c test.c -O2 -g -D benchmark
./a.out datx ../17monipdb.datx
./a.out dat ../17monipdb.dat
```

基本用法:
```C
ipip_t * hip=ip_init(type,path); //初始化 type=0 为dat格式 type=1为datx格式
ip_find(hip,"8.8.8.8",buffer,sizeof(buffer));//查找单个ip
ip_find_u(hip,3395848193,buffer,sizeof(buffer));//主机顺序整形ip 查找
ip_uninit(hip);//反初始化
```
性能:
```
dat 格式 500万次随机ip查询  1569毫秒
datx 格式 500万次随机ip查询 293毫秒
机器  i7-3770 CPU @ 3.40GHz CentOS 7.1
```
