#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "task_supervisor.h"
#include "watchdog_policy.h"

int main(void)
{
  TaskSupervisor_t supervisor;
  /* Startup: system health has not been confirmed. */
  assert(TaskSupervisor_Init(&supervisor) ==
         TASK_SUPERVISOR_OK);
  assert(supervisor.state ==
         TASK_SUPERVISOR_STATE_INIT);
  assert(WatchdogPolicy_IsRefreshAllowed(
             supervisor.state) == 0U);

  /* Complete heartbeat window: allow refresh. */
  assert(TaskSupervisor_ProcessWindow(
             &supervisor,
             TASK_SUPERVISOR_WINDOW_COMPLETE) ==
         TASK_SUPERVISOR_OK);
  assert(supervisor.state ==
         TASK_SUPERVISOR_STATE_HEALTHY);
  assert(WatchdogPolicy_IsRefreshAllowed(
             supervisor.state) == 1U);

  /* Fault injection: heartbeat window times out. */
  assert(TaskSupervisor_ProcessWindow(
             &supervisor,
             TASK_SUPERVISOR_WINDOW_TIMEOUT) ==
         TASK_SUPERVISOR_OK);
  assert(supervisor.state ==
         TASK_SUPERVISOR_STATE_FAULT);
  assert(WatchdogPolicy_IsRefreshAllowed(
             supervisor.state) == 0U);

  /* Heartbeat recovers: restore healthy behavior. */
  assert(TaskSupervisor_ProcessWindow(
             &supervisor,
             TASK_SUPERVISOR_WINDOW_COMPLETE) ==
         TASK_SUPERVISOR_OK);
  assert(supervisor.state ==
         TASK_SUPERVISOR_STATE_HEALTHY);
  assert(WatchdogPolicy_IsRefreshAllowed(
             supervisor.state) == 1U);

  puts("RELIABILITY FLOW TEST: PASS");
  return 0;
}
