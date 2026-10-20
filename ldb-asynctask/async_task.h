/*
 * SPDX-FileCopyrightText: 2026 LI Daobing
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>

struct ldb_async_task;

struct ldb_async_task_ops {
    void (*ref)(struct ldb_async_task* task);
    void (*unref)(struct ldb_async_task* task);
    bool (*start)(struct ldb_async_task* task);
    void (*cancel)(struct ldb_async_task* task);
};

enum ldb_asynctask_state {
    LDB_ASYNCTASK_STATE_INIT,
    LDB_ASYNCTASK_STATE_RUNNING,
    LDB_ASYNCTASK_STATE_CANCELED,
    LDB_ASYNCTASK_STATE_FINISHED
};

struct ldb_async_task {
    const struct ldb_async_task_ops* ops;
    enum ldb_asynctask_state state;
    void* data;
    void (*cb)(struct ldb_async_task* task);
};

static inline void ldb_asynctask_ref(struct ldb_async_task* task) {
    task->ops->ref(task);
}

static inline void ldb_asynctask_unref(struct ldb_async_task* task) {
    task->ops->unref(task);
}

static inline bool ldb_asynctask_start(struct ldb_async_task* task) {
    if (task->state != LDB_ASYNCTASK_STATE_INIT)
        return false;

    task->state = LDB_ASYNCTASK_STATE_RUNNING;
    return task->ops->start(task);
}

static inline void ldb_asynctask_cancel(struct ldb_async_task* task) {
    if (task->state != LDB_ASYNCTASK_STATE_RUNNING)
        return;
    task->state = LDB_ASYNCTASK_STATE_CANCELED;
    if (task->ops->cancel)
        task->ops->cancel(task);
    task->data = NULL;
    task->cb = NULL;
}