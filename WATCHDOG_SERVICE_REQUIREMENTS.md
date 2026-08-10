# Watchdog Service Requirements

## Scope

The Watchdog Service executes an injected watchdog refresh callback according
to a validated refresh decision. It does not directly depend on STM32 HAL or
FreeRTOS.

## Functional requirements

| ID | Requirement |
|---|---|
| WDS-001 | The service shall call the refresh callback exactly once when the refresh decision is 1. |
| WDS-002 | The service shall not call the refresh callback when the refresh decision is 0. |
| WDS-003 | The service shall reject refresh decisions other than 0 or 1. |
| WDS-004 | The service shall reject a null callback or null context pointer. |
| WDS-005 | The service shall pass the supplied context pointer to the refresh callback. |

## Test traceability

| Requirement | Test |
|---|---|
| WDS-001 | Test_AllowedDecisionCallsRefresh |
| WDS-002 | Test_BlockedDecisionDoesNotCallRefresh |
| WDS-003 | Test_InvalidDecisionIsRejected |
| WDS-004 | Test_NullDependenciesAreRejected |
| WDS-005 | Test_AllowedDecisionCallsRefresh |
