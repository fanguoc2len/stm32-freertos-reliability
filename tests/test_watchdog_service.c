#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

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

static void Test_AllowedDecisionCallsRefresh(void)
{
  FakeWatchdog_t fake_watchdog = {0U};

  assert(WatchdogService_Execute(
             1U,
             FakeWatchdog_Refresh,
             &fake_watchdog) ==
         WATCHDOG_SERVICE_OK);

  assert(fake_watchdog.refresh_call_count == 1U);
}

static void Test_BlockedDecisionDoesNotCallRefresh(void)
{
  FakeWatchdog_t fake_watchdog = {0U};

  assert(WatchdogService_Execute(
             0U,
             FakeWatchdog_Refresh,
             &fake_watchdog) ==
         WATCHDOG_SERVICE_OK);

  assert(fake_watchdog.refresh_call_count == 0U);
}

static void Test_InvalidDecisionIsRejected(void)
{
  FakeWatchdog_t fake_watchdog = {0U};

  assert(WatchdogService_Execute(
             2U,
             FakeWatchdog_Refresh,
             &fake_watchdog) ==
         WATCHDOG_SERVICE_INVALID_DECISION);

  assert(fake_watchdog.refresh_call_count == 0U);
}

static void Test_NullDependenciesAreRejected(void)
{
  FakeWatchdog_t fake_watchdog = {0U};

  assert(WatchdogService_Execute(
             1U,
             NULL,
             &fake_watchdog) ==
         WATCHDOG_SERVICE_NULL_POINTER);

  assert(WatchdogService_Execute(
             1U,
             FakeWatchdog_Refresh,
             NULL) ==
         WATCHDOG_SERVICE_NULL_POINTER);

  assert(fake_watchdog.refresh_call_count == 0U);
}

int main(void)
{
  Test_AllowedDecisionCallsRefresh();
  Test_BlockedDecisionDoesNotCallRefresh();
  Test_InvalidDecisionIsRejected();
  Test_NullDependenciesAreRejected();

  puts("WATCHDOG SERVICE TEST: PASS");
  return 0;
}
