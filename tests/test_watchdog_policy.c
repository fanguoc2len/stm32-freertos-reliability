#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "watchdog_policy.h"

static void Test_HealthyAllowsRefresh(void)
{
  assert(WatchdogPolicy_IsRefreshAllowed(
             TASK_SUPERVISOR_STATE_HEALTHY) == 1U);
}

static void Test_NonHealthyStatesBlockRefresh(void)
{
  assert(WatchdogPolicy_IsRefreshAllowed(
             TASK_SUPERVISOR_STATE_INIT) == 0U);

  assert(WatchdogPolicy_IsRefreshAllowed(
             TASK_SUPERVISOR_STATE_FAULT) == 0U);

  assert(WatchdogPolicy_IsRefreshAllowed(
             (TaskSupervisorState_t)99) == 0U);
}

int main(void)
{
  Test_HealthyAllowsRefresh();
  Test_NonHealthyStatesBlockRefresh();

  puts("WATCHDOG POLICY TEST: PASS");
  return 0;
}
