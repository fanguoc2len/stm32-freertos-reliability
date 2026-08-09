# Watchdog Policy Requirements

## Scope

The Watchdog Policy determines whether a watchdog refresh request is allowed.
It does not directly access the STM32 hardware watchdog.

## Functional requirements

| ID | Requirement |
|---|---|
| WDP-001 | The policy shall allow watchdog refresh only when the Task Supervisor state is HEALTHY. |
| WDP-002 | The policy shall block watchdog refresh when the Task Supervisor state is INIT or FAULT. |
| WDP-003 | The policy shall block watchdog refresh for any unsupported supervisor state. |

## Test traceability

| Requirement | Test |
|---|---|
| WDP-001 | Test_HealthyAllowsRefresh |
| WDP-002 | Test_NonHealthyStatesBlockRefresh |
| WDP-003 | Test_NonHealthyStatesBlockRefresh |
