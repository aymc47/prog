#include <pthread.h>
#include <errno.h>
#include "ex_lib_task.h"

struct OsalTask {
    pthread_t   m_thread;
    TaskHandler *m_entry;
    void        *m_arg;
};

static void *task_entry(void *in_arg_ptr)
{
    OsalTask *task_ptr = (OsalTask *)in_arg_ptr;

    if ((task_ptr != NULL) && (task_ptr->m_entry != NULL)) {
        task_ptr->m_entry(task_ptr->m_arg);
    }
    return (void *)0;
}

int lib_task_create(OsalTask *in_ins_ptr,TaskHandler *in_func_ptr, void *in_arg_ptr)
{
    int rc = 0;

    if ((in_ins_ptr == NULL) || (in_func_ptr == NULL)) {rc =-1;}
    else {
        in_ins_ptr->m_entry = in_func_ptr;
        in_ins_ptr->m_arg   = in_arg_ptr;

        rc = pthread_create(&in_ins_ptr->m_thread, (pthread_attr_t *)0, task_entry, (void *)in_ins_ptr);
    }
    return rc;
}

int lib_task_join(OsalTask *in_ins_ptr)
{
    int rc = 0;
    if (in_ins_ptr == NULL) {rc=-1;}
    else {
        rc = pthread_join(in_ins_ptr->m_thread, (void **)0);
    }
    return rc;
}

int lib_task_detach(OsalTask *in_ins_ptr)
{
    int rc = 0;
    if (in_ins_ptr == NULL) {rc = -1;}
    else {
        rc = pthread_detach(in_ins_ptr->m_thread);
    }
    return rc;
}
