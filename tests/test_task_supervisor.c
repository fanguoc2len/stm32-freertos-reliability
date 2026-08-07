#include <assert.h>
#include <stdio.h>

#include "task_supervisor.h"

static void Test_InitValidObject(void)
{
  TaskSupervisor_t supervisor =
  {
    .state = TASK_SUPERVISOR_STATE_FAULT,
    .healthy_window_count = 5U,
    .timeout_count = 7U
  };

  assert(TaskSupervisor_Init(&supervisor) == TASK_SUPERVISOR_OK);
  assert(supervisor.state == TASK_SUPERVISOR_STATE_INIT);
  assert(supervisor.healthy_window_count == 0U);
  assert(supervisor.timeout_count == 0U);
}

static void Test_InitNullPointer(void)
{
  assert(TaskSupervisor_Init(NULL) ==
         TASK_SUPERVISOR_NULL_POINTER);
}


static void Test_CompleteWindowSetsHealthy(void)
{
  TaskSupervisor_t supervisor;

  assert(TaskSupervisor_Init(&supervisor) ==
         TASK_SUPERVISOR_OK);

  assert(TaskSupervisor_ProcessWindow(
             &supervisor,
             TASK_SUPERVISOR_WINDOW_COMPLETE) ==
         TASK_SUPERVISOR_OK);

  assert(supervisor.state ==
         TASK_SUPERVISOR_STATE_HEALTHY);

  assert(supervisor.healthy_window_count == 1U);
  assert(supervisor.timeout_count == 0U);
}

static void Test_TimeoutWindowSetsFault(void)
{
	TaskSupervisor_t supervisor;

	assert(TaskSupervisor_Init(&supervisor) == TASK_SUPERVISOR_OK);
	assert(TaskSupervisor_ProcessWindow(&supervisor,TASK_SUPERVISOR_WINDOW_TIMEOUT) == TASK_SUPERVISOR_OK);
	assert(supervisor.state == TASK_SUPERVISOR_STATE_FAULT);
	assert(supervisor.healthy_window_count == 0U);
	assert(supervisor.timeout_count == 1U);
}

static void Test_InvalidResultDoesNotModifySupervisor(void)
{
  TaskSupervisor_t supervisor;
  TaskSupervisor_t before;

  assert(TaskSupervisor_Init(&supervisor) ==
         TASK_SUPERVISOR_OK);

  assert(TaskSupervisor_ProcessWindow(
             &supervisor,
             TASK_SUPERVISOR_WINDOW_COMPLETE) ==
         TASK_SUPERVISOR_OK);

  before = supervisor;

  assert(TaskSupervisor_ProcessWindow(
             &supervisor,
             (TaskSupervisorWindowResult_t)99) ==
         TASK_SUPERVISOR_INVALID_RESULT);

  assert(supervisor.state == before.state);
  assert(supervisor.healthy_window_count ==
         before.healthy_window_count);
  assert(supervisor.timeout_count ==
         before.timeout_count);
}

static void Test_CountersSaturateAtUint32Max(void)
{
  TaskSupervisor_t supervisor;

  assert(TaskSupervisor_Init(&supervisor) ==
         TASK_SUPERVISOR_OK);

  supervisor.healthy_window_count = UINT32_MAX;

  assert(TaskSupervisor_ProcessWindow(
             &supervisor,
             TASK_SUPERVISOR_WINDOW_COMPLETE) ==
         TASK_SUPERVISOR_OK);

  assert(supervisor.healthy_window_count == UINT32_MAX);

  supervisor.timeout_count = UINT32_MAX;

  assert(TaskSupervisor_ProcessWindow(
             &supervisor,
             TASK_SUPERVISOR_WINDOW_TIMEOUT) ==
         TASK_SUPERVISOR_OK);

  assert(supervisor.timeout_count == UINT32_MAX);
}

static void Test_ProcessWindowNullPointer(void){
	assert(TaskSupervisor_ProcessWindow(NULL,
	         TASK_SUPERVISOR_WINDOW_COMPLETE) == TASK_SUPERVISOR_NULL_POINTER);
}

static void Test_CompleteWindowRecoversFromFault(void)
{
  TaskSupervisor_t supervisor;

  assert(TaskSupervisor_Init(&supervisor) ==
         TASK_SUPERVISOR_OK);

  assert(TaskSupervisor_ProcessWindow(
             &supervisor,
             TASK_SUPERVISOR_WINDOW_TIMEOUT) ==
         TASK_SUPERVISOR_OK);

  assert(supervisor.state ==
         TASK_SUPERVISOR_STATE_FAULT);

  assert(TaskSupervisor_ProcessWindow(
             &supervisor,
             TASK_SUPERVISOR_WINDOW_COMPLETE) ==
         TASK_SUPERVISOR_OK);

  /* Intentionally wrong for the CI failure demonstration. */
  assert(supervisor.state ==
         TASK_SUPERVISOR_STATE_FAULT);
}
int main(void)
{
  Test_InitValidObject();
  Test_InitNullPointer();
  Test_CompleteWindowSetsHealthy();
  Test_TimeoutWindowSetsFault();
  Test_InvalidResultDoesNotModifySupervisor();
  Test_CountersSaturateAtUint32Max();
  Test_ProcessWindowNullPointer();
  Test_CompleteWindowRecoversFromFault();
  puts("TASK SUPERVISOR TEST: PASS");
  return 0;
}
