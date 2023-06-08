#include <libmemcached/memcached.h>

double getMemData2Double(char *key);
/*
arr = [x, y, z, time]
*/
void getLocationFromMemcache(char** aruco, double *time)
{
    memcached_server_st *servers = NULL;
    memcached_st *memc;
    memcached_return rc;
    // memcached_server_st *memcached_servers_parse(char *server_strings);
    memc = memcached_create(NULL);

    servers = memcached_server_list_append(servers, "localhost", 11211, &rc);
    rc = memcached_server_push(memc, servers);

    size_t value_length;
    uint32_t flags;

    *aruco = memcached_get(memc, "aruco", strlen("aruco"), &value_length, &flags, &rc);
    printf("aruco: %s\n", *aruco);
    *time = getMemData2Double("time");
}

double getMemData2Double(char *key)
{
    memcached_st *memc;
    memcached_return rc;
    memcached_server_st *servers;
    // 创建 memcached 连接
    memc = memcached_create(NULL);
    servers = memcached_server_list_append(NULL, "localhost", 11211, &rc);
    rc = memcached_server_push(memc, servers);
    // 执行读取操作
    size_t value_length;
    uint32_t flags;
    char* return_value = memcached_get(memc, key, strlen(key), &value_length, &flags, &rc);
    if (rc == MEMCACHED_SUCCESS)
    {
        // 清理资源
        memcached_free(memc);
        memcached_server_list_free(servers);
        if(strcmp(return_value, "null") == 0){
            free(return_value);
            return 0;
        }
        double result = strtod(return_value, NULL);
        // printf("result: %f\n", result);
        return result;
    }
    // 释放 memcached_get 分配的内存
    // free(return_value);
    return 0;
}

void getMemData(const char *key, char** return_value)
{
    memcached_st *memc;
    memcached_return rc;
    memcached_server_st *servers;
    // 创建 memcached 连接
    memc = memcached_create(NULL);
    servers = memcached_server_list_append(NULL, "localhost", 11211, &rc);
    rc = memcached_server_push(memc, servers);
    // 执行读取操作
    size_t value_length;
    uint32_t flags;
    *return_value = memcached_get(memc, key, strlen(key), &value_length, &flags, &rc);
    // if (rc == MEMCACHED_SUCCESS)
    // {
    //     printf("getMemData: %s\n", return_value);
    // }
}
int write_to_memcached(const char *key, const char *value)
{
    memcached_st *memc;
    memcached_return rc;
    memcached_server_st *servers;
    // 创建 memcached 连接
    memc = memcached_create(NULL);
    servers = memcached_server_list_append(NULL, "localhost", 11211, &rc);
    rc = memcached_server_push(memc, servers);
    // 执行写入操作
    rc = memcached_set(memc, key, strlen(key), value, strlen(value), (time_t)0, (uint32_t)0);
    // 清理资源
    memcached_free(memc);
    memcached_server_list_free(servers);
    if (rc == MEMCACHED_SUCCESS)
    {
        return 1; // 写入成功
    }
    else
    {
        return 0; // 写入失败
    }
}