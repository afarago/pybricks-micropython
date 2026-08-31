// SPDX-License-Identifier: MIT
// Copyright (c) 2026 The Pybricks Authors / lego-ghost-hub

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "py/mpconfig.h"
#include "py/misc.h"
#include "py/qstr.h"
#include "py/obj.h"
#include "py/objfun.h"
#include "py/runtime.h"
#include "py/bc.h"
#include "py/profile.h"
#include "lego_ghost_hub_frames.h"

#if defined(__EMSCRIPTEN__)
#include <emscripten.h>

EM_ASYNC_JS(void, js_bridge_report_frame, (int line, const char *func_name, const int *call_stack, int stack_depth), {
    var funcName = UTF8ToString(func_name);
    var stackArr = [];
    if (call_stack && stack_depth > 0) {
        for (var i = 0; i < stack_depth; i++) {
            stackArr.push(HEAP32[(call_stack >> 2) + i]);
        }
    }
    var frameData = {
        line: line,
        function: funcName,
        callStack: stackArr,
        stackDepth: stack_depth
    };

    if (typeof Module !== 'undefined' && Module.reportFrame) {
        Module.reportFrame(line, funcName, stackArr, stack_depth);
    } else if (typeof window !== 'undefined' && window.wasmHubFrameCallback) {
        window.wasmHubFrameCallback(frameData);
    }

    if (typeof Module !== 'undefined' && Module.beforeLineStep) {
        await Module.beforeLineStep(frameData);
    } else if (typeof window !== 'undefined' && window.wasmHubBeforeLineCallback) {
        await window.wasmHubBeforeLineCallback(frameData);
    }
});
#else
static void js_bridge_report_frame(int line, const char *func_name, const int *call_stack, int stack_depth) {
    (void)line;
    (void)func_name;
    (void)call_stack;
    (void)stack_depth;
}
#endif

void report_frame(int line, const char *function, int *call_stack, int stack_depth) {
    js_bridge_report_frame(line, function, call_stack, stack_depth);
}

#if MICROPY_PY_SYS_SETTRACE
static mp_obj_t lego_ghost_hub_trace_callback(mp_obj_t frame_obj, mp_obj_t event_obj, mp_obj_t arg_obj);
static MP_DEFINE_CONST_FUN_OBJ_3(lego_ghost_hub_trace_callback_obj, lego_ghost_hub_trace_callback);

static mp_obj_t lego_ghost_hub_trace_callback(mp_obj_t frame_obj, mp_obj_t event_obj, mp_obj_t arg_obj) {
    (void)arg_obj;
    mp_obj_frame_t *frame = MP_OBJ_TO_PTR(frame_obj);
    if (!frame) {
        return frame_obj;
    }

    qstr event = mp_obj_str_get_qstr(event_obj);
    if (event == MP_QSTR_line || event == MP_QSTR_call || event == MP_QSTR_return) {
        int line = (int)frame->lineno;
        const char *func_name = "<module>";
        if (frame->code && frame->code->rc) {
            const mp_raw_code_t *rc = frame->code->rc;
            qstr name_qstr = MP_CODE_QSTR_MAP(frame->code->context, rc->prelude.qstr_block_name_idx);
            func_name = qstr_str(name_qstr);
        }

        int temp_stack[64];
        int stack_depth = 0;
        mp_obj_frame_t *f = frame;
        while (f && stack_depth < 64) {
            temp_stack[stack_depth++] = (int)f->lineno;
            f = f->back ? f->back : (f->code_state && f->code_state->prev_state ? f->code_state->prev_state->frame : NULL);
        }

        int call_stack[64];
        for (int i = 0; i < stack_depth; i++) {
            call_stack[i] = temp_stack[stack_depth - 1 - i];
        }

        report_frame(line, func_name, call_stack, stack_depth);
    }

    return MP_OBJ_FROM_PTR(&lego_ghost_hub_trace_callback_obj);
}
#endif

void lego_ghost_hub_frames_reset(void) {
    #if MICROPY_PY_SYS_SETTRACE
    mp_prof_settrace(MP_OBJ_NULL);
    #endif
}

void lego_ghost_hub_frames_init(void) {
    #if MICROPY_PY_SYS_SETTRACE
    mp_prof_settrace(MP_OBJ_FROM_PTR(&lego_ghost_hub_trace_callback_obj));
    #endif
}
