# STM32 FreeRTOS Reliability Practice
[![Host Unit Tests](https://github.com/fanguoc2len/stm32-freertos-reliability/actions/workflows/host-tests.yml/badge.svg)](https://github.com/fanguoc2len/stm32-freertos-reliability/actions/workflows/host-tests.yml)

A reliability-focused STM32F103 firmware project demonstrating task scheduling,
inter-task communication, interrupt deferral, synchronization, health monitoring,
memory diagnostics, and host-based unit testing.

## System architecture

```mermaid
flowchart TB
    subgraph INPUT["Input and interrupt path"]
        direction LR
        BTN["PA0 Button"] -->|Rising edge| ISR["EXTI0 ISR"]
        ISR -->|Release| SEM["Binary Semaphore"]
        SEM -->|Wake| EVT["EventTask"]
        EVT -->|Restart| DB["50 ms Debounce Timer"]
        DB --> APP["Validated Button Event"]
    end

    subgraph COMM["Task communication"]
        direction LR
        CTRL["ControlTask<br/>10 ms"] -->|Status| QUEUE["Status Queue"]
        QUEUE --> DIAG["DiagnosticTask<br/>100 ms"]
        CTRL -->|Lock| MUTEX["System Data Mutex"]
        DIAG -->|Lock| MUTEX
    end

    subgraph HEALTH["Health supervision"]
        direction LR
        CTRL -->|Heartbeat bit 0| FLAGS["System Event Flags"]
        DIAG -->|Heartbeat bit 1| FLAGS
        FLAGS -->|Wait all / 200 ms| MON["MonitorTask"]
        MON -->|Complete or timeout| SUP["Task Supervisor"]
        SUP -->|Health state| POLICY["Watchdog Policy"]
        POLICY -->|HEALTHY| WDG["Refresh Request"]
        POLICY -->|INIT / FAULT / invalid| BLOCK["Refresh Blocked"]
    end
```

## Main components
| Component | Responsibility |
|---|---|
| ControlTask | Periodic 10 ms control producer |
| DiagnosticTask | 100 ms queue consumer and diagnostic processing |
| EventTask | Deferred GPIO interrupt processing |
| MonitorTask | Heartbeat supervision and stack monitoring |
| Task Supervisor | HAL/RTOS-independent health state logic |
| Status Queue | Transfers status values between tasks |
| System Event Flags | Synchronizes Control and Diagnostic heartbeats |
| System Data Mutex | Protects shared data from race conditions |
| Debounce Timer | Validates button state after a 50 ms delay |
| Watchdog Policy | Allows refresh only when the Task Supervisor is healthy |

## Project structure

```text
RTOS_Practice_F103/
├── Core/
│   ├── Inc/
│   │   ├── task_supervisor.h
│   │   └── watchdog_policy.h
│   └── Src/
│       ├── main.c
│       ├── freertos.c
│       ├── stm32f1xx_it.c
│       ├── task_supervisor.c
│       └── watchdog_policy.c
├── Middlewares/
├── Drivers/
├── tests/
│   ├── test_task_supervisor.c
│   ├── test_watchdog_policy.c
│   └── test_reliability_flow.c
├── TASK_SUPERVISOR_REQUIREMENTS.md
├── WATCHDOG_POLICY_REQUIREMENTS.md
└── RTOS_Practice_F103.ioc
```

## RTOS configuration

| Item | Configuration |
|---|---|
| RTOS interface | CMSIS-RTOS2 |
| Kernel tick | 1 kHz |
| HAL timebase | TIM2 |
| ControlTask period | 10 ms |
| DiagnosticTask period | 100 ms |
| Monitor timeout | 200 ms |
| Button debounce | 50 ms one-shot timer |
| FreeRTOS heap | 8192 bytes |
| Stack overflow checking | Option 2 |

## Reliability mechanisms

- Bounded queue operations with overflow diagnostics.
- Mutex-protected shared data.
- Deferred GPIO interrupt processing using a binary semaphore.
- Task heartbeat supervision using event flags.
- Software watchdog decision logic.
- RTOS object allocation checks before scheduler startup.
- Per-task stack high-water monitoring.
- Stack overflow and allocation-failure hooks.
- HardFault status register capture.
- Integer divide-by-zero and unaligned-access traps.
- Saturating diagnostic counters.

## Build firmware

Open `RTOS_Practice_F103.ioc` and generate code for STM32CubeIDE, then build the
`Debug` configuration.

Expected result:

```text
0 errors, 0 warnings
```

## Run host unit tests

From a WSL shell:

```bash
cd /mnt/e/stm32/RTOS_Practice_F103

gcc -std=c11 -Wall -Wextra -Werror \
  -ICore/Inc \
  tests/test_task_supervisor.c \
  Core/Src/task_supervisor.c \
  -o /tmp/task_supervisor_test

/tmp/task_supervisor_test

gcc -std=c11 -Wall -Wextra -Werror \
  -ICore/Inc \
  tests/test_watchdog_policy.c \
  Core/Src/watchdog_policy.c \
  -o /tmp/watchdog_policy_test

/tmp/watchdog_policy_test

gcc -std=c11 -Wall -Wextra -Werror \
  -ICore/Inc \
  tests/test_reliability_flow.c \
  Core/Src/task_supervisor.c \
  Core/Src/watchdog_policy.c \
  -o /tmp/reliability_flow_test

/tmp/reliability_flow_test
```

Expected result:

```text
TASK SUPERVISOR TEST: PASS
WATCHDOG POLICY TEST: PASS
RELIABILITY FLOW TEST: PASS
```


## Requirements and tests

- [Task Supervisor requirements](TASK_SUPERVISOR_REQUIREMENTS.md)
- [Watchdog Policy requirements](WATCHDOG_POLICY_REQUIREMENTS.md)
- Cross-module health, fault, and recovery behavior is verified by [tests/test_reliability_flow.c](tests/test_reliability_flow.c).

## Verification status

- STM32 firmware compilation: passed.
- Host unit and integration tests: passed.
- Cppcheck static analysis: passed.
- Line and branch coverage: 100%.
- Requirement traceability: complete for Task Supervisor and Watchdog Policy.
- On-target execution and timing validation: pending hardware availability.

## Target

- MCU: STM32F103RBTx
- IDE: STM32CubeIDE
- Configuration tool: STM32CubeMX
- RTOS API: CMSIS-RTOS2
