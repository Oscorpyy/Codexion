*This project has been created as part of the 42 curriculum by opernod.*

# Codexion

*Master the race for resources before the deadline masters you.*

## Description
Codexion is a concurrency and multithreading project inspired by the classic Dining Philosophers problem, but adapted into a modern software development environment setup. In this simulation, several coders must share limited resources (dongles) to repeatedly compile, debug, and refactor their code. If a coder spends too much time waiting for resources and fails to compile their code before their deadline, they "burn out" and the simulation ends.

The goal is to master resource race conditions, avoid deadlocks, and implement efficient thread scheduling (such as FIFO and EDF) using POSIX threads.

## Instructions

### Compilation
To compile the project, simply run `make` at the root of the repository. This will compile the source files according to the stringent 42 Norm and create the `codexion` executable.

```bash
make
```

Other available rules:
- `make clean`: Removes object files.
- `make fclean`: Removes object files and the executable.
- `make re`: Recompiles the entire project from scratch.
- `make test` / `make test_val` / `make test_hell`: Runs predefined stress tests, optionally with Valgrind or Helgrind.

### Execution
Run the program with the following mandatory arguments:

```bash
./codexion <number_of_coders> <time_to_burnout> <time_to_compile> <time_to_debug> <time_to_refactor> <number_of_compiles_required> <dongle_cooldown> <scheduler>
```

**Example:**
```bash
./codexion 5 800 200 200 200 5 10 edf
```
*This starts a simulation with 5 coders where the timeframe to avoid burnout is 800ms, compilation takes 200ms, etc. Schedulers supported are `fifo` and `edf`.*

## Blocking cases handled

Our simulation specifically addresses numerous concurrency hazards to maintain stability:
- **Deadlock prevention & Coffman's conditions:** By strictly managing how dongles (shared resources) are acquired and released, we mathematically break the circular wait and hold-and-wait conditions.
- **Starvation prevention:** Implemented precise fair-queueing mechanisms (like EDF - Earliest Deadline First) to ensure high-priority coders get their required dongles before they burn out.
- **Cooldown handling:** Managed through strict timestamp tracking to prevent coders from monopolizing resources instantly after releasing them.
- **Precise burnout detection:** Instead of relying on long blocking `usleep` calls that delay death detection by tens of milliseconds, the project implements a micro-grained sleep routine (`ft_usleep`). It tests the environment's state every 50µs, enabling sub-millisecond death detection even with over 200 active threads.
- **Log serialization:** All standard output printing is piped through a specific text execution mutex, ensuring that timestamps and states are printed chronologically and text buffers never interleave on `stdout`.

## Thread synchronization mechanisms

Effective handling of the coders relies heavily on POSIX threading primitives to coordinate access to shared memory and monitor application bounds.

- **`pthread_mutex_t`:** Extensively used throughout the codebase to shield data pools from race conditions. 
  - `write_mutex` prevents terminal output garbling.
  - `run_mutex` safely tracks the `is_running` boolean to instantly notify all threads of a routine stop (e.g., when a coder burns out).
  - `coder_mutex` defends inner variables like `last_compile_time` and `compiles_done` from being read by the global monitor exactly when the coder thread updates them.
- **Custom Event Implementation:** Rather than relying exclusively on complex `pthread_cond_t` broadcasts to wake sleeping threads, we combined granular mutex locks with a high-frequency polling approach inside our wait cycles (`ft_usleep`). Whenever a resource changes or a burnout triggers, the threads naturally break out of their sleep loops immediately by checking the safely-locked `check_running()` and `check_burnout()` status.
- **Data Races tracking:** Using Helgrind and Valgrind, isolated data transitions between the observer monitor (in the main thread) and the active worker threads (coders) are guaranteed absolutely mutually exclusive.

## Resources

- **Classic References:**
  - *POSIX Threads Programming* (Livermore Computing Center)
  - *Operating System Concepts* (Silberschatz, Galvin, Gagne) – Chapters on Synchronization and Deadlocks.
  - The Dining Philosophers Problem – Edsger W. Dijkstra.
- **Usage of AI:** Generative AI (GitHub Copilot) was utilized during the development of this project for the following targeted tasks:
  1. Assisting with the understanding of the project's scope and requirements.
  2. Formatting the custom colored output in the `Makefile` and proposing various testing scenarios.
  3. Aiding in the comprehension and implementation of specific programmatic logic and algorithms.
  4. Generating the structural skeleton and initial draft of this `README.md` file.
  5. Helping with pre-evaluation checks and verifications, such as ensuring code compliance with the strict 42 Norm.
  