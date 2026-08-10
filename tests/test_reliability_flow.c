#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "task_supervisor.h"
#include "watchdog_policy.h"
#include "watchdog_service.h"

typedef struct
{
  uint32_t refresh_call_count;
} FakeWatchdog_t;

static void FakeWatchdog_Refresh(void *context)
{
  FakeWatchdog_t *fake_watchdog =
      (FakeWatchdog_t *)context;

  fake_watchdog->refresh_call_count++;
}

int main(void)
{
  TaskSupervisor_t supervisor;
  uint8_t refresh_allowed;
  FakeWatchdog_t fake_watchdog =
  {
    .refresh_call_count = 0U
  };
  /* Startup: system health has not been confirmed. */
  assert(TaskSupervisor_Init(&supervisor) ==
         TASK_SUPERVISOR_OK);
  assert(supervisor.state ==
         TASK_SUPERVISOR_STATE_INIT);

  refresh_allowed =
      WatchdogPolicy_IsRefreshAllowed(supervisor.state);

  assert(refresh_allowed == 0U);
  assert(WatchdogService_Execute(
             refresh_allowed,
             FakeWatchdog_Refresh,
             &fake_watchdog) == WATCHDOG_SERVICE_OK);
  assert(fake_watchdog.refresh_call_count == 0U);


  /* Complete heartbeat window: allow refresh. */
  assert(TaskSupervisor_ProcessWindow(
             &supervisor,
             TASK_SUPERVISOR_WINDOW_COMPLETE) ==
         TASK_SUPERVISOR_OK);
  assert(supervisor.state ==
         TASK_SUPERVISOR_STATE_HEALTHY);

  refresh_allowed =
      WatchdogPolicy_IsRefreshAllowed(supervisor.state);

  assert(refresh_allowed == 1U);
  assert(WatchdogService_Execute(
             refresh_allowed,
             FakeWatchdog_Refresh,
             &fake_watchdog) == WATCHDOG_SERVICE_OK);
  assert(fake_watchdog.refresh_call_count == 1U);


  /* Fault injection: heartbeat window times out. */
  assert(TaskSupervisor_ProcessWindow(
             &supervisor,
             TASK_SUPERVISOR_WINDOW_TIMEOUT) ==
         TASK_SUPERVISOR_OK);
  assert(supervisor.state ==
         TASK_SUPERVISOR_STATE_FAULT);

  refresh_allowed =
      WatchdogPolicy_IsRefreshAllowed(supervisor.state);

  assert(refresh_allowed == 0U);
  assert(WatchdogService_Execute(
             refresh_allowed,
             FakeWatchdog_Refresh,
             &fake_watchdog) == WATCHDOG_SERVICE_OK);
  assert(fake_watchdog.refresh_call_count == 1U);


  /* Heartbeat recovers: restore healthy behavior. */
  assert(TaskSupervisor_ProcessWindow(
             &supervisor,
             TASK_SUPERVISOR_WINDOW_COMPLETE) ==
         TASK_SUPERVISOR_OK);
  assert(supervisor.state ==
         TASK_SUPERVISOR_STATE_HEALTHY);

  refresh_allowed =
      WatchdogPolicy_IsRefreshAllowed(supervisor.state);

  assert(refresh_allowed == 1U);
  assert(WatchdogService_Execute(
             refresh_allowed,
             FakeWatchdog_Refresh,
             &fake_watchdog) == WATCHDOG_SERVICE_OK);
  assert(fake_watchdog.refresh_call_count == 2U);

  puts("RELIABILITY FLOW TEST: PASS");
  return 0;
}
