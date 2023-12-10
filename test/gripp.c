/*
舵機的頻率一般為頻率為50HZ，也就是一個20ms左右的時基脈衝，而脈衝的高電平部分一般為0.5ms-2.5ms範圍。
所以-90°⇒0.5ms, 90°⇒2.5ms
公式：2000*((2ms/180°)*所轉角度+0.5)/20
72°: 2000*((2ms/180°)*72+0.5)/20=130
108°: 2000*((2ms/180°)*108+0.5)/20=170
*/
#include <wiringPi.h>
// #include <softpwm.h>
#include <stdio.h>
// 定义控制舵机的引脚为：GPIO1
#define PWM_PIN 26
void gripperSwitch(int open)
{
    // printf("wiringPi-C PWM test program\n");
    wiringPiSetup();
    pinMode(PWM_PIN, PWM_OUTPUT); // 设置PWM输出
    pwmSetMode(PWM_MODE_MS);      // 设置传统模式
    pwmSetClock(192);             // 设置分频
    pwmSetRange(2000);            // 设置周期分为2000步
    switch (open)
    {
    case 0:
        printf("close gripper\n");
        pwmWrite(PWM_PIN, 175);
        break;
    case 1:
        printf("open gripper\n");
        pwmWrite(PWM_PIN, 80);
    default:
        break;
    }
}

int main(){
    for(int i=0;i<5;i++){
        gripperSwitch(0);
        delay(1000);
        gripperSwitch(1);
        delay(1000);
    }
    gripperSwitch(1);
    return 0;
}