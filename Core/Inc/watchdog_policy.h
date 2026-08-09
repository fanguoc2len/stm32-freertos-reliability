#ifndef WATCHDOG_POLICY_H
#define WATCHDOG_POLICY_H

#include <stdint.h>

#include "task_supervisor.h"

uint8_t WatchdogPolicy_IsRefreshAllowed(
    TaskSupervisorState_t supervisor_state);

#endif
