#ifndef TASK_SUPERVISOR_H
#define TASK_SUPERVISOR_H

#include <stdint.h>

typedef enum
{
  TASK_SUPERVISOR_STATE_INIT = 0,
  TASK_SUPERVISOR_STATE_HEALTHY,
  TASK_SUPERVISOR_STATE_FAULT
} TaskSupervisorState_t;

typedef enum
{
  TASK_SUPERVISOR_WINDOW_COMPLETE = 0,
  TASK_SUPERVISOR_WINDOW_TIMEOUT
} TaskSupervisorWindowResult_t;

typedef enum
{
  TASK_SUPERVISOR_OK = 0,
  TASK_SUPERVISOR_NULL_POINTER,
  TASK_SUPERVISOR_INVALID_RESULT
} TaskSupervisorStatus_t;

typedef struct
{
  TaskSupervisorState_t state;
  uint32_t healthy_window_count;
  uint32_t timeout_count;
} TaskSupervisor_t;

TaskSupervisorStatus_t TaskSupervisor_Init(
    TaskSupervisor_t *supervisor);

TaskSupervisorStatus_t TaskSupervisor_ProcessWindow(
    TaskSupervisor_t *supervisor,
    TaskSupervisorWindowResult_t window_result);

#endif
