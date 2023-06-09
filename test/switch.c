#include <wiringPi.h>
#include <stdio.h>

#define button 4 // 板子的pin16对应GPIO4---->wiringPi的4

int main()
{
	if (wiringPiSetup() == -1) // 加载wiringPi
	{
		printf("wiringPi setup fail\r\n");
		return -1;
	}
	pullUpDnControl(button, PUD_UP); // 设置pin内部上拉

	printf("pro running...\r\n");
	while (1)
	{
		int level = digitalRead(button); // 获取pin电平，当按下时为低电平，触发打印
		if (level == 0)
			printf("button touch!\r\n");
		delay(200);
	}
}