#pragma once

// C++
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <deque>
#include <stdexcept>
#include <exception>
#include <iomanip>

// C
#include <cstdio>
#include <cassert>
#include <cstddef>

// another
#include <sys/stat.h>
#include <sys/types.h>

// platform detection

#define     PLATFORM_WINDOWS            1
#define     PLATFORM_MAC_OS_X           2
#define     PLATFORM_UNIX               3

#if defined(WIN32)

#define PLATFORM PLATFORM_WINDOWS

#elif defined(__APPLE__)

#define PLATFORM PLATFORM_MAC_OS_X

#else

#define PLATFORM PLATFORM_UNIX

#endif // platform detection

#if PLATFORM == PLATFORM_WINDOWS

#include <winsock2.h>
#include <ws2tcpip.h>

#define CANNOT_CREATE_SOCKET INVALID_SOCKET

#elif PLATFORM == PLATFORM_MAC_OS_X || PLATFORM == PLATFORM_UNIX

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>

#define SOCKET int
#define CANNOT_CREATE_SOCKET -1

#endif