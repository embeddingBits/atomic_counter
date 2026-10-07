# Kernel Atomic Counter

A rudimentary Linux kernel module implementation of a thread-safe 64-bit atomic counter exposed via `/proc` 

## Overview

This repository (`atomic_counter`) provides an educational and practical example of how to manage shared state safely within the Linux kernel. It utilizes the kernel's `atomic64_t` API to guarantee lock-free, atomic increments and state resets, avoiding race conditions when accessed by multiple concurrent user-space processes.

### Key Features
*   **`/proc` Filesystem Integration:** Creates a node at `/proc/atomic_counter`.
*   **Atomic Operations:** Uses `atomic64_inc_return` to ensure thread-safe increments without needing spinlocks or mutexes.
*   **User-Space Write Access:** Allows authorized users to write specific commands (like `"reset"`) to the `/proc` node to modify the counter's state dynamically.

