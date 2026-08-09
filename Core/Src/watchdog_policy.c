#include "watchdog_policy.h"

uint8_t WatchdogPolicy_IsRefreshAllowed(
    TaskSupervisorState_t supervisor_state)
{
  uint8_t refresh_allowed = 0U;

  if (supervisor_state == TASK_SUPERVISOR_STATE_HEALTHY)
  {
    refresh_allowed = 1U;
  }

  return refresh_allowed;
}
