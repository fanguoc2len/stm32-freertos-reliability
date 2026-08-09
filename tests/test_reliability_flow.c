#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "task_supervisor.h"
#include "watchdog_policy.h"

int main(void)
{
  TaskSupervisor_t supervisor;

  /* Startup: chưa xác nhận hệ thống khỏe. */
  assert(TaskSupervisor_Init(&supervisor) ==
         TASK_SUPERVISOR_OK);
  assert(supervisor.state ==
         TASK_SUPERVISOR_STATE_INIT);
  assert(WatchdogPolicy_IsRefreshAllowed(
             supervisor.state) == 0U);

  /* Nhận đủ heartbeat: cho phép refresh. */
  assert(TaskSupervisor_ProcessWindow(
             &supervisor,
             TASK_SUPERVISOR_WINDOW_COMPLETE) ==
         TASK_SUPERVISOR_OK);
  assert(supervisor.state ==
         TASK_SUPERVISOR_STATE_HEALTHY);
  assert(WatchdogPolicy_IsRefreshAllowed(
             supervisor.state) == 1U);

  /* Fault injection: heartbeat bị timeout. */
  assert(TaskSupervisor_ProcessWindow(
             &supervisor,
             TASK_SUPERVISOR_WINDOW_TIMEOUT) ==
         TASK_SUPERVISOR_OK);
  assert(supervisor.state ==
         TASK_SUPERVISOR_STATE_FAULT);
  assert(WatchdogPolicy_IsRefreshAllowed(
             supervisor.state) == 0U);

  /* Heartbeat trở lại: hệ thống phục hồi. */
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
