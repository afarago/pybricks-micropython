// SPDX-License-Identifier: MIT
// Copyright (c) 2026 The Pybricks Authors

#include <pbio/os.h>

#include "py/runtime.h"
#include "py/mphal.h"

#if defined(__EMSCRIPTEN__)
#include <emscripten.h>

pbio_os_irq_flags_t pbio_os_hook_disable_irq(void) {
    return 0;
}

void pbio_os_hook_enable_irq(pbio_os_irq_flags_t flags) {
    (void)flags;
}

void pbio_os_hook_wait_for_interrupt(pbio_os_irq_flags_t flags) {
    (void)flags;
    MP_THREAD_GIL_EXIT();
    emscripten_sleep(1);
    MP_THREAD_GIL_ENTER();
    pbio_os_request_poll();
}

#elif defined(_WIN32)

pbio_os_irq_flags_t pbio_os_hook_disable_irq(void) {
    return 0;
}

void pbio_os_hook_enable_irq(pbio_os_irq_flags_t flags) {
    (void)flags;
}

void pbio_os_hook_wait_for_interrupt(pbio_os_irq_flags_t flags) {
    (void)flags;
    MP_THREAD_GIL_EXIT();
    Sleep(1);
    MP_THREAD_GIL_ENTER();
    pbio_os_request_poll();
}

#else

#include <signal.h>
#include <sys/select.h>

pbio_os_irq_flags_t pbio_os_hook_disable_irq(void) {
    sigset_t sigmask;
    sigfillset(&sigmask);
    sigset_t origmask;
    pthread_sigmask(SIG_SETMASK, &sigmask, &origmask);
    return origmask;
}

void pbio_os_hook_enable_irq(pbio_os_irq_flags_t flags) {
    sigset_t origmask = (sigset_t)flags;
    pthread_sigmask(SIG_SETMASK, &origmask, NULL);
}

void pbio_os_hook_wait_for_interrupt(pbio_os_irq_flags_t flags) {
    struct timespec timeout = {
        .tv_sec = 0,
        .tv_nsec = 1000000,
    };
    sigset_t origmask = flags;
    MP_THREAD_GIL_EXIT();
    pselect(0, NULL, NULL, NULL, &timeout, &origmask);
    MP_THREAD_GIL_ENTER();
    pbio_os_request_poll();
}

#endif
