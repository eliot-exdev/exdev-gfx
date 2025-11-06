/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#include "exdevgfx/helper.h"
#include "exdevgfx/exdev_base.h"

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/timer.h>

#include <devices/timer.h>

static struct timeval a;

exdev_timestamp_t now() {
    GetSysTime(&a);
    return a.tv_secs * 1000 + a.tv_micro / 1000;
}

void sleep_for_ms(const exdev_timestamp_t ms) {
    if (ms <= 0) {
        return;
    }

    struct timerequest request = time_request;
    request.tr_node.io_Message.mn_ReplyPort = &((struct Process *) FindTask(NULL))->pr_MsgPort;
    request.tr_node.io_Command = TR_ADDREQUEST;
    request.tr_time.tv_secs = (ULONG) ms / 1000;
    request.tr_time.tv_micro = (ULONG) (ms % 1000) * 1000;
    DoIO((struct IORequest *) &request);
}