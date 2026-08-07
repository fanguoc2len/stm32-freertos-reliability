#include "task_supervisor.h"

#include <stddef.h>

TaskSupervisorStatus_t TaskSupervisor_Init(
    TaskSupervisor_t *supervisor)
{
  TaskSupervisorStatus_t status = TASK_SUPERVISOR_NULL_POINTER;

  if (supervisor != NULL)
  {
    supervisor->state = TASK_SUPERVISOR_STATE_INIT;
    supervisor->healthy_window_count = 0U;
    supervisor->timeout_count = 0U;

    status = TASK_SUPERVISOR_OK;
  }

  return status;
}

TaskSupervisorStatus_t TaskSupervisor_ProcessWindow(
    TaskSupervisor_t *supervisor,
    TaskSupervisorWindowResult_t window_result)
{
  TaskSupervisorStatus_t status = TASK_SUPERVISOR_NULL_POINTER;

  if (supervisor != NULL)
  {
    switch (window_result)
    {
      case TASK_SUPERVISOR_WINDOW_COMPLETE:
        supervisor->state = TASK_SUPERVISOR_STATE_HEALTHY;

        if (supervisor->healthy_window_count < UINT32_MAX)
        {
          supervisor->healthy_window_count++;
        }

        status = TASK_SUPERVISOR_OK;
        break;

      case TASK_SUPERVISOR_WINDOW_TIMEOUT:
        supervisor->state = TASK_SUPERVISOR_STATE_FAULT;

        if (supervisor->timeout_count < UINT32_MAX)
        {
          supervisor->timeout_count++;
        }

        status = TASK_SUPERVISOR_OK;
        break;

      default:
        status = TASK_SUPERVISOR_INVALID_RESULT;
        break;
    }
  }

  return status;
}
