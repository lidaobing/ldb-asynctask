/*
 * SPDX-FileCopyrightText: 2026 LI Daobing
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>

struct ldb_asynctask;

struct ldb_asynctask_ops {
    void (*ref)(struct ldb_asynctask* task);
    void (*unref)(struct ldb_asynctask* task);
    bool (*start)(struct ldb_asynctask* task);
    void (*cancel)(struct ldb_asynctask* task);
};

enum ldb_asynctask_state {
    LDB_ASYNCTASK_STATE_INIT,
    LDB_ASYNCTASK_STATE_RUNNING,
    LDB_ASYNCTASK_STATE_CANCELED,
    LDB_ASYNCTASK_STATE_FINISHED
};

struct ldb_asynctask {
    const struct ldb_asynctask_ops* ops;
    enum ldb_asynctask_state state;
    void* data;
    void (*cb)(struct ldb_asynctask* task);
};

static inline void ldb_asynctask_ref(struct ldb_asynctask* task) {
    task->ops->ref(task);
}

static inline void ldb_asynctask_unref(struct ldb_asynctask* task) {
    task->ops->unref(task);
}

static inline bool ldb_asynctask_start(struct ldb_asynctask* task) {
    if (task->state != LDB_ASYNCTASK_STATE_INIT)
        return false;

    task->state = LDB_ASYNCTASK_STATE_RUNNING;
    return task->ops->start(task);
}

static inline void ldb_asynctask_cancel(struct ldb_asynctask* task) {
    if (task->state != LDB_ASYNCTASK_STATE_RUNNING)
        return;
    task->state = LDB_ASYNCTASK_STATE_CANCELED;
    if (task->ops->cancel)
        task->ops->cancel(task);
    task->data = NULL;
    task->cb = NULL;
}