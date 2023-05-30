// #include "../gripper/gripper.h"
// #include "../aruco_location/aruco_location.h"
// #include "../data_transmission/test_data_transmission.h"
// #include <libmemcached/memcached.h>
// // #include "test_data_transmission.h"
// #include "dji_logger.h"
// #include "dji_platform.h"
// #include "utils/util_misc.h"
// #include "dji_low_speed_data_channel.h"
// #include "dji_high_speed_data_channel.h"
// #include "dji_aircraft_info.h"
// #include "widget_interaction_test/test_widget_interaction.h"

// #define DATA_TRANSMISSION_TASK_STACK_SIZE (2048)
// #define DATA_TRANSMISSION_TASK_FREQ (1)
// /* Private variables ---------------------------------------------------------*/
// static T_DjiTaskHandle s_userDataTransmissionThread;
// static T_DjiAircraftInfoBaseInfo s_aircraftInfoBaseInfo;

// static void circleLocationAndSendDataToMobile(void *arg);
// int write_to_memcached(const char *key, const char *value);
// uint8_t circle_data[1000] = "DJI Data Transmission Test Data.";
// bool run_circle;
// typedef struct
// {
//     double x;
//     double y;
// } circle;
// void throwBall()
// {
//     float threadhold = 0.2;
//     memcached_server_st *servers = NULL;
//     memcached_st *memc;
//     memcached_return rc;
//     // memcached_server_st *memcached_servers_parse(char *server_strings);
//     memc = memcached_create(NULL);

//     servers = memcached_server_list_append(servers, "localhost", 11211, &rc);
//     rc = memcached_server_push(memc, servers);

//     T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
//     size_t value_length;
//     uint32_t flags;
//     circle *c = (circle *)malloc(sizeof(circle));
//     write_to_memcached("find_circle", "1");
//     printf("circle: ");
//     run_circle = true;
//     // while (run_circle)
//     // {
//         // osalHandler->TaskSleepMs(300);
//         double x_coor = getMemData2Double("x_coor");
//         double y_coor = getMemData2Double("y_coor");
//         c->x = x_coor;
//         c->y = y_coor;
//         printf("x_coor: %.3f, y_coor: %.3f\n", x_coor, y_coor);
//         if (-threadhold < x_coor && x_coor < threadhold && -threadhold < y_coor && y_coor < threadhold && (x_coor != 0 || y_coor != 0))
//         {
//             gripperSwitch(1);
//             write_to_memcached("find_circle", "0");
//             return;
//         }
//         sprintf(circle_data, "%f,%f", x_coor, y_coor);
//         printf("dataToBeSent: %s\n", circle_data);
//         if (osalHandler->TaskCreate("circle_transmission_task", circleLocationAndSendDataToMobile,
//                                     DATA_TRANSMISSION_TASK_STACK_SIZE, NULL, &s_userDataTransmissionThread) !=
//             DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
//         {
//             USER_LOG_ERROR("user data transmission task create error.");
//             return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
//         }
//     // }
// }
// void stopCircle()
// {
//     run_circle = false;
//     write_to_memcached("find_circle", "0");
// }
// static void circleLocationAndSendDataToMobile(void *arg)
// {
//     write_to_memcached("find_circle", "1");
//     size_t value_length;
//     uint32_t flags;
//     circle *c = (circle *)arg;
//     T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

//     T_DjiReturnCode djiStat;
//     T_DjiDataChannelState state = {0};
//     E_DjiChannelAddress channelAddress;
//     run_circle = true;
//     channelAddress = DJI_CHANNEL_ADDRESS_MASTER_RC_APP;
//     djiStat = DjiLowSpeedDataChannel_SendData(channelAddress, circle_data, sizeof(circle_data));
//     if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
//         USER_LOG_ERROR("send data to mobile error.");

//     djiStat = DjiLowSpeedDataChannel_GetSendDataState(channelAddress, &state);
//     if (djiStat == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
//     {
//         USER_LOG_DEBUG(
//             "send to mobile state: realtimeBandwidthBeforeFlowController: %d, realtimeBandwidthAfterFlowController: %d, busyState: %d.",
//             state.realtimeBandwidthBeforeFlowController, state.realtimeBandwidthAfterFlowController,
//             state.busyState);
//     }
//     else
//     {
//         USER_LOG_ERROR("get send to mobile channel state error.");
//     }

//     if (s_aircraftInfoBaseInfo.mountPosition == DJI_MOUNT_POSITION_PAYLOAD_PORT_NO1 ||
//         s_aircraftInfoBaseInfo.mountPosition == DJI_MOUNT_POSITION_PAYLOAD_PORT_NO2 ||
//         s_aircraftInfoBaseInfo.mountPosition == DJI_MOUNT_POSITION_PAYLOAD_PORT_NO3)
//     {
//         channelAddress = DJI_CHANNEL_ADDRESS_EXTENSION_PORT;
//         djiStat = DjiLowSpeedDataChannel_SendData(channelAddress, circle_data, sizeof(circle_data));
//         if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
//             USER_LOG_ERROR("send data to onboard computer error.");

//         djiStat = DjiLowSpeedDataChannel_GetSendDataState(channelAddress, &state);
//         if (djiStat == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
//         {
//             USER_LOG_DEBUG(
//                 "send to onboard computer state: realtimeBandwidthBeforeFlowController: %d, realtimeBandwidthAfterFlowController: %d, busyState: %d.",
//                 state.realtimeBandwidthBeforeFlowController, state.realtimeBandwidthAfterFlowController,
//                 state.busyState);
//         }
//         else
//         {
//             USER_LOG_ERROR("get send to onboard computer channel state error.");
//         }

//         if (DjiPlatform_GetSocketHandler() != NULL)
//         {
//             djiStat = DjiHighSpeedDataChannel_SendDataStreamData(circle_data, sizeof(circle_data));
//             if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
//                 USER_LOG_ERROR("send data to data stream error.");

//             djiStat = DjiHighSpeedDataChannel_GetDataStreamState(&state);
//             if (djiStat == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
//             {
//                 USER_LOG_DEBUG(
//                     "data stream state: realtimeBandwidthLimit: %d, realtimeBandwidthBeforeFlowController: %d, busyState: %d.",
//                     state.realtimeBandwidthLimit, state.realtimeBandwidthBeforeFlowController, state.busyState);
//             }
//             else
//             {
//                 USER_LOG_ERROR("get data stream state error.");
//             }
//         }
//     }
//     else if (s_aircraftInfoBaseInfo.mountPosition == DJI_MOUNT_POSITION_EXTENSION_PORT)
//     {
//         channelAddress = DJI_CHANNEL_ADDRESS_PAYLOAD_PORT_NO1;
//         djiStat = DjiLowSpeedDataChannel_SendData(channelAddress, circle_data, sizeof(circle_data));
//         if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
//             USER_LOG_ERROR("send data to onboard computer error.");

//         djiStat = DjiLowSpeedDataChannel_GetSendDataState(channelAddress, &state);
//         if (djiStat == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
//         {
//             USER_LOG_DEBUG(
//                 "send to onboard computer state: realtimeBandwidthBeforeFlowController: %d, realtimeBandwidthAfterFlowController: %d, busyState: %d.",
//                 state.realtimeBandwidthBeforeFlowController, state.realtimeBandwidthAfterFlowController,
//                 state.busyState);
//         }
//         else
//         {
//             USER_LOG_ERROR("get send to onboard computer channel state error.");
//         }
//     }
// }
// int write_to_memcached(const char *key, const char *value)
// {
//     memcached_st *memc;
//     memcached_return rc;
//     memcached_server_st *servers;
//     // 创建 memcached 连接
//     memc = memcached_create(NULL);
//     servers = memcached_server_list_append(NULL, "localhost", 11211, &rc);
//     rc = memcached_server_push(memc, servers);
//     // 执行写入操作
//     rc = memcached_set(memc, key, strlen(key), value, strlen(value), (time_t)0, (uint32_t)0);
//     // 清理资源
//     memcached_free(memc);
//     memcached_server_list_free(servers);
//     if (rc == MEMCACHED_SUCCESS)
//     {
//         return 1; // 写入成功
//     }
//     else
//     {
//         return 0; // 写入失败
//     }
// }