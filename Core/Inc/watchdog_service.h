#ifndef WATCHDOG_SERVICE_H
#define WATCHDOG_SERVICE_H

#include <stdint.h>

typedef void (*WatchdogRefreshCallback_t)(void *context);

typedef enum
{
  WATCHDOG_SERVICE_OK = 0,
  WATCHDOG_SERVICE_NULL_POINTER,
  WATCHDOG_SERVICE_INVALID_DECISION
} WatchdogServiceStatus_t;

WatchdogServiceStatus_t WatchdogService_Execute(
    uint8_t refresh_allowed,
    WatchdogRefreshCallback_t refresh_callback,
    void *context);

#endif
