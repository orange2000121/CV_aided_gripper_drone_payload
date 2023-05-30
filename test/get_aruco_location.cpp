#include <iostream>
#include <libmemcached/memcached.h>

using namespace std;

int main(int argc, char *argv[])
{
    memcached_server_st *servers = NULL;
    memcached_st *memc;
    memcached_return rc;
    char *key= "x";
    char *value= "10";

    memcached_server_st *memcached_servers_parse (char *server_strings);
    memc = memcached_create(NULL);

    servers = memcached_server_list_append(servers, "localhost", 11211, &rc);
    rc = memcached_server_push(memc, servers);

    if (rc == MEMCACHED_SUCCESS)
        cout << "Added server successfully" << endl;
    else
        cout << "Couldn't get value: " << memcached_strerror(memc, rc) << endl;

    // get value
    size_t value_length;
    uint32_t flags;

    char *return_value = memcached_get(memc, key, strlen(key), &value_length, &flags, &rc);
    if (rc == MEMCACHED_SUCCESS)
        cout << "Get value successfully" << endl;
    else
        cout << "Couldn't get value: " << memcached_strerror(memc, rc) << endl;

    while (return_value != NULL)
    {
        return_value = memcached_get(memc, key, strlen(key), &value_length, &flags, &rc);
        double x;
        memcpy(&x, value, sizeof(double));
        cout << "return_value: " << x << endl;

    }

    return 0;
}
