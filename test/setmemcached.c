#include <stdio.h>
#include <stdlib.h>
#include <libmemcached/memcached.h>

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
int read_memcached(const char *key)
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
    char *return_value = memcached_get(memc, key, strlen(key), &value_length, &flags, &rc);
    printf("return_value: %s\n", return_value);
    if (rc == MEMCACHED_SUCCESS)
    {
        // 清理资源
        // memcached_free(memc);
        // memcached_server_list_free(servers);
        if (return_value == "null")
        {
            // free(return_value);
            return 0;
        }
        return strtod(return_value, NULL);
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
int main()
{
    char *circle;
    getMemData("circle", &circle);
    if (circle != NULL)
    {
        printf("circle: %s\n", circle);
    }
    else
    {
        printf("circle is null\n");
    }

    return 0;
}
