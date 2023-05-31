#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
	unsigned char cmd[10]={0xFF,0x1,0x86,0,0,0,0,0,0x79};
	unsigned char buf[10]={0};
	int i,n;
	int fd = open("/dev/ttySAC1", O_RDWR);
	n = write(fd, cmd, 9);
	printf("have write %d bytes\n", n);
	while(1)
	{
		n = read(fd, buf, 9);
		printf("have read %d bytes\n", n);
		for(i=0;i<9;i++)
			printf("0x%x ",buf[i]);
		printf("\n");
		sleep(1);
	}
	return 0;
}
