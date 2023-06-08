#include <wiringPi.h>
#include <stdio.h>
#include <stdbool.h>
#include "trigger_switch.h"

void trigger(void* arg)
{	
    TriggerEvent* triggerEvent = (TriggerEvent*)arg;
    triggerEvent->triggered = false;
	int button = 4; // 板子的pin16对应GPIO4---->wiringPi的4
	if (wiringPiSetup() == -1) // 加载wiringPi
	{
		printf("wiringPi setup fail\r\n");
        triggerEvent->triggered = false;
	}
	pullUpDnControl(button, PUD_UP); // 设置pin内部上拉
	printf("pro running...\r\n");
	while (1)
	{
		int level = digitalRead(button); // 获取pin电平，当按下时为低电平，触发打印
		if (level == 0)
		{
			triggerEvent->triggered = true;
            printf("button pressed\r\n");
			break;
		}else{
            triggerEvent->triggered = false;
        }
		delay(200);
	}
}