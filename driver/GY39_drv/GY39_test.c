#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{	//全部测试：返回24字节
	unsigned char cmd[3]={0xA5,0x83,0x28};
	//单独测试光照，返回9字节
	unsigned char cmd2[3]={0xA5,0x51,0xF6};
	//单独测试温度，湿度，气压，海拔，返回15字节
	unsigned char cmd3[3]={0xA5,0x52,0xF7};
	unsigned char buf[24]={0};
	int i,n;
	int fd = open("/dev/ttySAC2", O_RDWR);
	n = write(fd, cmd3, 3);
	printf("have write %d bytes\n", n);
	while(1)
	{
		n = read(fd, buf, 24);
		if (n == -1)
		{
			perror("read error");
			continue;
		}
		printf("have read %d bytes\n", n);
		for(i=0;i<24;i++)
			printf("0x%x ",buf[i]);
		printf("\n");
		sleep(1);
	}
	return 0;
}
