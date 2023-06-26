#ifndef __CONSTANTS_HPP__
#define __CONSTANTS_HPP__

// Socket configurations
#define CLOSED_SOCKET_ID -1
#define SOCKET_PROTOCOL 0 // any

// Server capacity configurations
#define BUFFER_SIZE 1024
#define MAX_CONNECTIONS 5000
#define MAX_ASYNC_EVENTS_QUEUE_SIZE 1000
#define THREADS_POOL_SIZE 4

#define END_OF_TCP_STREAM "\0"

const char STATIC_ASSET_PATH[] = "public/";

#endif
