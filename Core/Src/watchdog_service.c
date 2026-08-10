#include "watchdog_service.h"

#include <stddef.h>

WatchdogServiceStatus_t WatchdogService_Execute(
    uint8_t refresh_allowed,
    WatchdogRefreshCallback_t refresh_callback,
    void *context)
{
  WatchdogServiceStatus_t status =
      WATCHDOG_SERVICE_NULL_POINTER;

  if ((refresh_callback != NULL) && (context != NULL))
  {
    if (refresh_allowed == 0U)
    {
      status = WATCHDOG_SERVICE_OK;
    }
    else if (refresh_allowed == 1U)
    {
      refresh_callback(context);
      status = WATCHDOG_SERVICE_OK;
    }
    else
    {
      status = WATCHDOG_SERVICE_INVALID_DECISION;
    }
  }

  return status;
}
