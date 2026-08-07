# Task Supervisor Requirements

## Functional requirements

| ID | Requirement |
|---|---|
| TSR-001 | Initialization shall set the supervisor state to INIT and reset all counters to zero. |
| TSR-002 | Initialization with a null pointer shall return TASK_SUPERVISOR_NULL_POINTER. |
| TSR-003 | A complete heartbeat window shall set the state to HEALTHY and increment healthy_window_count. |
| TSR-004 | A timed-out heartbeat window shall set the state to FAULT and increment timeout_count. |
| TSR-005 | An invalid window result shall return TASK_SUPERVISOR_INVALID_RESULT without modifying supervisor data. |
| TSR-006 | Counters shall saturate at UINT32_MAX instead of wrapping to zero. |
| TSR-007 | Processing a window with a null supervisor pointer shall return TASK_SUPERVISOR_NULL_POINTER. |

## Test traceability

| Requirement | Test |
|---|---|
| TSR-001 | Test_InitValidObject |
| TSR-002 | Test_InitNullPointer |
| TSR-003 | Test_CompleteWindowSetsHealthy |
| TSR-004 | Test_TimeoutWindowSetsFault |
| TSR-005 | Test_InvalidResultDoesNotModifySupervisor |
| TSR-006 | Test_CountersSaturateAtUint32Max |
| TSR-007 | Test_ProcessWindowNullPointer |