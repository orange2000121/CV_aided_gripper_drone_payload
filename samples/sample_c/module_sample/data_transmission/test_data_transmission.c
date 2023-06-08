/**
 ********************************************************************
 * @file    test_data_transmission.c
 * @brief
 *
 * @copyright (c) 2021 DJI. All rights reserved.
 *
 * All information contained herein is, and remains, the property of DJI.
 * The intellectual and technical concepts contained herein are proprietary
 * to DJI and may be covered by U.S. and foreign patents, patents in process,
 * and protected by trade secret or copyright law.  Dissemination of this
 * information, including but not limited to data and other proprietary
 * material(s) incorporated within the information, in any form, is strictly
 * prohibited without the express written consent of DJI.
 *
 * If you receive this source code without DJI’s authorization, you may not
 * further disseminate the information, and you must immediately remove the
 * source code and notify DJI of its removal. DJI reserves the right to pursue
 * legal actions against you for any loss(es) or damage(s) caused by your
 * failure to do so.
 *
 *********************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "../gripper/gripper.h"
#include "../aruco_location/aruco_location.h"
// #include "../custom_module/throw_ball.h"
#include "test_data_transmission.h"
#include "dji_logger.h"
#include "dji_platform.h"
#include "utils/util_misc.h"
#include "dji_low_speed_data_channel.h"
#include "dji_high_speed_data_channel.h"
#include "dji_aircraft_info.h"
#include "widget_interaction_test/test_widget_interaction.h"

/* Private constants ---------------------------------------------------------*/
#define DATA_TRANSMISSION_TASK_FREQ (1)
#define DATA_TRANSMISSION_TASK_STACK_SIZE (2048)

/* Private types -------------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/
static void *UserDataTransmission_Task(void *arg);
void throwBall(void* arg);
void stopCircle();
static T_DjiReturnCode ReceiveDataFromMobile(const uint8_t *data, uint16_t len);
static T_DjiReturnCode ReceiveDataFromOnboardComputer(const uint8_t *data, uint16_t len);
static T_DjiReturnCode ReceiveDataFromPayload(const uint8_t *data, uint16_t len);

/* Private variables ---------------------------------------------------------*/
static T_DjiTaskHandle s_userDataTransmissionThread;
static T_DjiAircraftInfoBaseInfo s_aircraftInfoBaseInfo;

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode DjiTest_DataTransmissionStartService(void)
{
    T_DjiReturnCode djiStat;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    E_DjiChannelAddress channelAddress;
    const T_DjiDataChannelBandwidthProportionOfHighspeedChannel bandwidthProportionOfHighspeedChannel =
        {10, 60, 30};
    char ipAddr[DJI_IP_ADDR_STR_SIZE_MAX];
    uint16_t port;

    djiStat = DjiLowSpeedDataChannel_Init();
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("init data transmission module error.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    djiStat = DjiAircraftInfo_GetBaseInfo(&s_aircraftInfoBaseInfo);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("get aircraft base info error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    channelAddress = DJI_CHANNEL_ADDRESS_MASTER_RC_APP;
    djiStat = DjiLowSpeedDataChannel_RegRecvDataCallback(channelAddress, ReceiveDataFromMobile);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("register receive data from mobile error.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }
    // data receive mission
    if (s_aircraftInfoBaseInfo.mountPosition == DJI_MOUNT_POSITION_PAYLOAD_PORT_NO1 ||
        s_aircraftInfoBaseInfo.mountPosition == DJI_MOUNT_POSITION_PAYLOAD_PORT_NO2 ||
        s_aircraftInfoBaseInfo.mountPosition == DJI_MOUNT_POSITION_PAYLOAD_PORT_NO3)
    {
        channelAddress = DJI_CHANNEL_ADDRESS_EXTENSION_PORT;
        djiStat = DjiLowSpeedDataChannel_RegRecvDataCallback(channelAddress, ReceiveDataFromOnboardComputer);
        if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
        {
            USER_LOG_ERROR("register receive data from onboard coputer error.");
            return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
        }

        djiStat = DjiHighSpeedDataChannel_SetBandwidthProportion(bandwidthProportionOfHighspeedChannel);
        if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
        {
            USER_LOG_ERROR("Set data channel bandwidth width proportion error.");
            return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
        }

        djiStat = DjiHighSpeedDataChannel_GetDataStreamRemoteAddress(ipAddr, &port);
        if (djiStat == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
        {
            USER_LOG_DEBUG("Get data stream remote address: %s, port: %d", ipAddr, port);
        }
        else
        {
            USER_LOG_ERROR("get data stream remote address error.");
        }
    }
    else if (s_aircraftInfoBaseInfo.mountPosition == DJI_MOUNT_POSITION_EXTENSION_PORT)
    {
        channelAddress = DJI_CHANNEL_ADDRESS_PAYLOAD_PORT_NO1;
        djiStat = DjiLowSpeedDataChannel_RegRecvDataCallback(channelAddress, ReceiveDataFromPayload);
        if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
        {
            USER_LOG_ERROR("register receive data from payload NO1 error.");
            return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
        }
    }
    else
    {
        return DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT;
    }
    // data transmission
    // if (osalHandler->TaskCreate("user_transmission_task", UserDataTransmission_Task,
    //                             DATA_TRANSMISSION_TASK_STACK_SIZE, NULL, &s_userDataTransmissionThread) !=
    //     DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    // {
    //     USER_LOG_ERROR("user data transmission task create error.");
    //     return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    // }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Private functions definition-----------------------------------------------*/
#ifndef __CC_ARM
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wreturn-type"
#endif

uint8_t dataToBeSent[1000] = "DJI Data Transmission Test Data.";
static void *UserDataTransmission_Task(void *arg)
{
    T_DjiReturnCode djiStat;
    // uint8_t dataToBeSent[100] = "DJI Data Transmission Test Data.";
    // uint8_t dataToBeSentDemo[] = "DJI Data Transmission Test Data. ";
    T_DjiDataChannelState state = {0};
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    E_DjiChannelAddress channelAddress;

    USER_UTIL_UNUSED(arg);
    // int number = 1;
    while (1)
    {
        osalHandler->TaskSleepMs(500 / DATA_TRANSMISSION_TASK_FREQ);
        // sprintf(dataToBeSent,"%s%d", dataToBeSentDemo, number);
        // printf("%s\n",dataToBeSent);
        // number++;
        channelAddress = DJI_CHANNEL_ADDRESS_MASTER_RC_APP;
        djiStat = DjiLowSpeedDataChannel_SendData(channelAddress, dataToBeSent, sizeof(dataToBeSent));
        if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
            USER_LOG_ERROR("send data to mobile error.");

        djiStat = DjiLowSpeedDataChannel_GetSendDataState(channelAddress, &state);
        if (djiStat == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
        {
            USER_LOG_DEBUG(
                "send to mobile state: realtimeBandwidthBeforeFlowController: %d, realtimeBandwidthAfterFlowController: %d, busyState: %d.",
                state.realtimeBandwidthBeforeFlowController, state.realtimeBandwidthAfterFlowController,
                state.busyState);
        }
        else
        {
            USER_LOG_ERROR("get send to mobile channel state error.");
        }

        if (s_aircraftInfoBaseInfo.mountPosition == DJI_MOUNT_POSITION_PAYLOAD_PORT_NO1 ||
            s_aircraftInfoBaseInfo.mountPosition == DJI_MOUNT_POSITION_PAYLOAD_PORT_NO2 ||
            s_aircraftInfoBaseInfo.mountPosition == DJI_MOUNT_POSITION_PAYLOAD_PORT_NO3)
        {
            channelAddress = DJI_CHANNEL_ADDRESS_EXTENSION_PORT;
            djiStat = DjiLowSpeedDataChannel_SendData(channelAddress, dataToBeSent, sizeof(dataToBeSent));
            if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
                USER_LOG_ERROR("send data to onboard computer error.");

            djiStat = DjiLowSpeedDataChannel_GetSendDataState(channelAddress, &state);
            if (djiStat == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
            {
                USER_LOG_DEBUG(
                    "send to onboard computer state: realtimeBandwidthBeforeFlowController: %d, realtimeBandwidthAfterFlowController: %d, busyState: %d.",
                    state.realtimeBandwidthBeforeFlowController, state.realtimeBandwidthAfterFlowController,
                    state.busyState);
            }
            else
            {
                USER_LOG_ERROR("get send to onboard computer channel state error.");
            }

            if (DjiPlatform_GetSocketHandler() != NULL)
            {
                djiStat = DjiHighSpeedDataChannel_SendDataStreamData(dataToBeSent, sizeof(dataToBeSent));
                if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
                    USER_LOG_ERROR("send data to data stream error.");

                djiStat = DjiHighSpeedDataChannel_GetDataStreamState(&state);
                if (djiStat == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
                {
                    USER_LOG_DEBUG(
                        "data stream state: realtimeBandwidthLimit: %d, realtimeBandwidthBeforeFlowController: %d, busyState: %d.",
                        state.realtimeBandwidthLimit, state.realtimeBandwidthBeforeFlowController, state.busyState);
                }
                else
                {
                    USER_LOG_ERROR("get data stream state error.");
                }
            }
        }
        else if (s_aircraftInfoBaseInfo.mountPosition == DJI_MOUNT_POSITION_EXTENSION_PORT)
        {
            channelAddress = DJI_CHANNEL_ADDRESS_PAYLOAD_PORT_NO1;
            djiStat = DjiLowSpeedDataChannel_SendData(channelAddress, dataToBeSent, sizeof(dataToBeSent));
            if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
                USER_LOG_ERROR("send data to onboard computer error.");

            djiStat = DjiLowSpeedDataChannel_GetSendDataState(channelAddress, &state);
            if (djiStat == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
            {
                USER_LOG_DEBUG(
                    "send to onboard computer state: realtimeBandwidthBeforeFlowController: %d, realtimeBandwidthAfterFlowController: %d, busyState: %d.",
                    state.realtimeBandwidthBeforeFlowController, state.realtimeBandwidthAfterFlowController,
                    state.busyState);
            }
            else
            {
                USER_LOG_ERROR("get send to onboard computer channel state error.");
            }
        }
    }
}

// uint8_t dataToBeSent[100] = "DJI Data Transmission Test Data.";
static void sendDataToMobile(void *arg)
{
    T_DjiReturnCode djiStat;
    T_DjiDataChannelState state = {0};
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    E_DjiChannelAddress channelAddress;

    channelAddress = DJI_CHANNEL_ADDRESS_MASTER_RC_APP;
    djiStat = DjiLowSpeedDataChannel_SendData(channelAddress, dataToBeSent, sizeof(dataToBeSent));
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
        USER_LOG_ERROR("send data to mobile error.");

    djiStat = DjiLowSpeedDataChannel_GetSendDataState(channelAddress, &state);
    if (djiStat == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_DEBUG(
            "send to mobile state: realtimeBandwidthBeforeFlowController: %d, realtimeBandwidthAfterFlowController: %d, busyState: %d.",
            state.realtimeBandwidthBeforeFlowController, state.realtimeBandwidthAfterFlowController,
            state.busyState);
    }
    else
    {
        USER_LOG_ERROR("get send to mobile channel state error.");
    }

    if (s_aircraftInfoBaseInfo.mountPosition == DJI_MOUNT_POSITION_PAYLOAD_PORT_NO1 ||
        s_aircraftInfoBaseInfo.mountPosition == DJI_MOUNT_POSITION_PAYLOAD_PORT_NO2 ||
        s_aircraftInfoBaseInfo.mountPosition == DJI_MOUNT_POSITION_PAYLOAD_PORT_NO3)
    {
        channelAddress = DJI_CHANNEL_ADDRESS_EXTENSION_PORT;
        djiStat = DjiLowSpeedDataChannel_SendData(channelAddress, dataToBeSent, sizeof(dataToBeSent));
        if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
            USER_LOG_ERROR("send data to onboard computer error.");

        djiStat = DjiLowSpeedDataChannel_GetSendDataState(channelAddress, &state);
        if (djiStat == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
        {
            USER_LOG_DEBUG(
                "send to onboard computer state: realtimeBandwidthBeforeFlowController: %d, realtimeBandwidthAfterFlowController: %d, busyState: %d.",
                state.realtimeBandwidthBeforeFlowController, state.realtimeBandwidthAfterFlowController,
                state.busyState);
        }
        else
        {
            USER_LOG_ERROR("get send to onboard computer channel state error.");
        }

        if (DjiPlatform_GetSocketHandler() != NULL)
        {
            djiStat = DjiHighSpeedDataChannel_SendDataStreamData(dataToBeSent, sizeof(dataToBeSent));
            if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
                USER_LOG_ERROR("send data to data stream error.");

            djiStat = DjiHighSpeedDataChannel_GetDataStreamState(&state);
            if (djiStat == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
            {
                USER_LOG_DEBUG(
                    "data stream state: realtimeBandwidthLimit: %d, realtimeBandwidthBeforeFlowController: %d, busyState: %d.",
                    state.realtimeBandwidthLimit, state.realtimeBandwidthBeforeFlowController, state.busyState);
            }
            else
            {
                USER_LOG_ERROR("get data stream state error.");
            }
        }
    }
    else if (s_aircraftInfoBaseInfo.mountPosition == DJI_MOUNT_POSITION_EXTENSION_PORT)
    {
        channelAddress = DJI_CHANNEL_ADDRESS_PAYLOAD_PORT_NO1;
        djiStat = DjiLowSpeedDataChannel_SendData(channelAddress, dataToBeSent, sizeof(dataToBeSent));
        if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
            USER_LOG_ERROR("send data to onboard computer error.");

        djiStat = DjiLowSpeedDataChannel_GetSendDataState(channelAddress, &state);
        if (djiStat == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
        {
            USER_LOG_DEBUG(
                "send to onboard computer state: realtimeBandwidthBeforeFlowController: %d, realtimeBandwidthAfterFlowController: %d, busyState: %d.",
                state.realtimeBandwidthBeforeFlowController, state.realtimeBandwidthAfterFlowController,
                state.busyState);
        }
        else
        {
            USER_LOG_ERROR("get send to onboard computer channel state error.");
        }
    }
}
#ifndef __CC_ARM
#pragma GCC diagnostic pop
#endif

// double prev_location[4] = {0};
double pre_time = 0;
static T_DjiReturnCode ReceiveDataFromMobile(const uint8_t *data, uint16_t len)
{
    char *printData = NULL;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    printData = osalHandler->Malloc(len + 1);
    if (printData == NULL)
    {
        USER_LOG_ERROR("malloc memory for printData fail.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED;
    }

    strncpy(printData, (const char *)data, len);
    printData[len] = '\0';
    USER_LOG_INFO("receive data from mobile: %s, len:%d.", printData, len);
    DjiTest_WidgetLogAppend("receive data: %s, len:%d.", printData, len);
    /* --------------------------- 接收到y就打开夹爪，接收到n就关闭夹爪 -------------------------- */
    if (strcmp(printData, "y") == 0)
    {
        gripperSwitch(1);
    }
    else if (strcmp(printData, "n") == 0)
    {
        gripperSwitch(0);
    }
    else if (strcmp(printData, "location") == 0)
    {
        // 获取当前位置
        // double location[7] = {0};
        char *aruco;
        double time;
        getLocationFromMemcache(&aruco, &time);
        if (time != pre_time)
        {
            pre_time = time;
            sprintf((char *)dataToBeSent, "%s", aruco);
            printf("location: %s\n", dataToBeSent);
            printf("string length: %d\n", strlen(dataToBeSent));
            if (osalHandler->TaskCreate("user_transmission_task", sendDataToMobile,
                                        DATA_TRANSMISSION_TASK_STACK_SIZE, NULL, &s_userDataTransmissionThread) !=
                DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
            {
                USER_LOG_ERROR("user data transmission task create error.");
                return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
            }
        }
        else
        {
            sprintf((char *)dataToBeSent, "null");
            printf("location: %s\n", dataToBeSent);
            if (osalHandler->TaskCreate("user_transmission_task", sendDataToMobile,
                                        DATA_TRANSMISSION_TASK_STACK_SIZE, NULL, &s_userDataTransmissionThread) !=
                DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
            {
                USER_LOG_ERROR("user data transmission task create error.");
                return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
            }
        }
    }
    else if (strcmp(printData, "circle") == 0)
    {
        write_to_memcached("find_circle", "1");
        char* circle;
        getMemData("circle", &circle);
        if(circle != NULL){
            printf("circle: %s\n", circle);
        }else{
            printf("circle is null\n");
        }
        sprintf((char *)dataToBeSent, "%s", circle);
        printf("dataToBeSent: %s\n", dataToBeSent);
        if (osalHandler->TaskCreate("circle_transmission_task", sendDataToMobile,
                                    DATA_TRANSMISSION_TASK_STACK_SIZE, NULL, &s_userDataTransmissionThread) !=
            DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
        {
            USER_LOG_ERROR("user data transmission task create error.");
            return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
        }
    }
    else if (strcmp(printData, "throw") ==0){
        if (osalHandler->TaskCreate("throwBall", throwBall,
                                    DATA_TRANSMISSION_TASK_STACK_SIZE, NULL, &s_userDataTransmissionThread) !=
            DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
        {
            USER_LOG_ERROR("user data transmission task create error.");
            return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
        }
    }
    else if (strcmp(printData, "stop_circle") == 0)
    {
        stopCircle();
    }

    osalHandler->Free(printData);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode ReceiveDataFromOnboardComputer(const uint8_t *data, uint16_t len)
{
    char *printData = NULL;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    printData = osalHandler->Malloc(len + 1);
    if (printData == NULL)
    {
        USER_LOG_ERROR("malloc memory for printData fail.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED;
    }

    strncpy(printData, (const char *)data, len);
    printData[len] = '\0';
    USER_LOG_INFO("receive data from onboard computer: %s, len:%d.", printData, len);
    DjiTest_WidgetLogAppend("receive data: %s, len:%d.", printData, len);

    osalHandler->Free(printData);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode ReceiveDataFromPayload(const uint8_t *data, uint16_t len)
{
    char *printData = NULL;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    printData = osalHandler->Malloc(len + 1);
    if (printData == NULL)
    {
        USER_LOG_ERROR("malloc memory for printData fail.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED;
    }

    strncpy(printData, (const char *)data, len);
    printData[len] = '\0';
    USER_LOG_INFO("receive data from payload port: %s, len:%d.", printData, len);
    DjiTest_WidgetLogAppend("receive data: %s, len:%d.", printData, len);

    osalHandler->Free(printData);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
void throwBall(void* arg)
{
    while(1){
        float threadhold = 0.2;
        write_to_memcached("find_circle", "1");
        double x_coor = getMemData2Double("x_coor");
        double y_coor = getMemData2Double("y_coor");
        printf("x_coor: %.3f, y_coor: %.3f\n", x_coor, y_coor);
        if (-threadhold < x_coor && x_coor < threadhold && -threadhold < y_coor && y_coor < threadhold && (x_coor != 0 || y_coor != 0))
        {
            gripperSwitch(1);
            write_to_memcached("find_circle", "0");
            return;
        }
    }
}
void stopCircle()
{
    // run_circle = false;
    write_to_memcached("find_circle", "0");
}