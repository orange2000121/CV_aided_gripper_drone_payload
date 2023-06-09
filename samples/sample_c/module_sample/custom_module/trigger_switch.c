#include <wiringPi.h>
#include <stdio.h>
#include "trigger_switch.h"
#include "../gripper/gripper.h"
int lastTriggerStatus = 0;

void trigger(void *arg)
{
	// TriggerEvent* triggerEvent = (TriggerEvent*)arg;
	// triggerEvent->triggered = false;
	char *data = (char *)arg;
	if(lastTriggerStatus == 0){
		sprintf((char *)data, "%s", "[{\"triggered\": false}]");
	}else{
		sprintf((char *)data, "%s", "[{\"triggered\": true}]");
	}
	int button = 4;			   // 板子的pin16对应GPIO4---->wiringPi的4
	if (wiringPiSetup() == -1) // 加载wiringPi
	{
		printf("wiringPi setup fail\r\n");
		sprintf((char *)data, "%s", "[{\"triggered\": false}]");
		lastTriggerStatus = 0;
	}
	pullUpDnControl(button, PUD_UP); // 设置pin内部上拉
	printf("pro running...\r\n");
	int level = digitalRead(button); // 获取pin电平，当按下时为低电平，触发打印
	if (level == 0)
	{
		sprintf((char *)data, "%s", "[{\"triggered\": true}]");
		gripperSwitch(0);
		lastTriggerStatus = 1;
		printf("button pressed\r\n");
		// break;
	}
}