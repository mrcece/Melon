
/*
 * Copyright (C) Niklaus F.Schen.
 */
#ifndef __MLN_FORK_H
#define __MLN_FORK_H
#include <sys/types.h>
#include "mln_types.h"
#include "mln_connection.h"
#include "mln_ipc.h"
#include "mln_event.h"

#define STATE_IDLE    0
#define STATE_LENGTH  1
#define M_F_TYPELEN   sizeof(mln_u32_t)
#define M_F_LENLEN    sizeof(mln_u32_t)

typedef void (*clr_handler)(void *);

/*ipc handler*/
typedef void (*ipc_handler)(mln_event_t *, \
                            void *, /*mln_fork_t or mln_tcp_connection_t*/\
                            void *,/*buffer*/\
                            mln_u32_t,/*buffer length*/\
                            void **);/*user data pointer*/
typedef struct {
    ipc_handler              handler;
    void                    *data;
    mln_u32_t                type;
} mln_ipc_handler_t;

enum proc_state_type {
    M_PST_DFL,
    M_PST_SUP /*supervise*/
};

enum proc_exec_type {
    M_PET_DFL,
    M_PET_EXE /*call exec*/
};

struct mln_fork_attr {
    mln_s8ptr_t             *args;
    mln_u32_t                n_args;
    int                      fd;
    pid_t                    pid;
    enum proc_exec_type      etype;
    enum proc_state_type     stype;
};

typedef struct mln_fork_s {
    struct mln_fork_s       *prev;
    struct mln_fork_s       *next;
    mln_s8ptr_t             *args;
    mln_tcp_connection_t     conn;
    pid_t                    pid;
    mln_u32_t                n_args;
    mln_u32_t                state;
    mln_u32_t                msg_len;
    mln_u32_t                msg_type;
    enum proc_exec_type      etype;
    enum proc_state_type     stype;
} mln_fork_t;

extern int mln_pre_fork(void);
extern void
mln_set_master_ipc_handler(mln_ipc_handler_t *ih) __NONNULL1(1);
extern void
mln_set_worker_ipc_handler(mln_ipc_handler_t *ih) __NONNULL1(1);
extern int
mln_fork_scan_all(mln_event_t *ev, \
                  mln_u32_t flag, \
                  int timeout_ms, \
                  ev_fd_handler fd_handler) __NONNULL1(1);
extern int do_fork(void);
/*
 * Only master process can call 'mln_fork_spawn'.
 * argument 'handler()' is used to
 * clear process resources.
 */
extern int
mln_fork_spawn(enum proc_state_type stype, \
               mln_s8ptr_t *args, \
               mln_u32_t n_args, \
               mln_event_t *master_ev);
extern int
mln_fork_restart(mln_event_t *master_ev);
extern void
mln_fork_master_set_events(mln_event_t *ev) __NONNULL1(1);
extern void
mln_fork_worker_set_events(mln_event_t *ev) __NONNULL1(1);
extern void
mln_set_resource_clear_handler(clr_handler handler, void *data);
extern void
mln_ipc_fd_handler_master(mln_event_t *ev, int fd, void *data);
extern void
mln_ipc_fd_handler_worker(mln_event_t *ev, int fd, void *data);
extern void
mln_socketpair_close_handler(mln_event_t *ev, mln_fork_t *f, int fd) __NONNULL2(1,2);

#endif
