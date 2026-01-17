#ifndef EX_TASK_H
#define EX_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef void (TaskHandler)(void *);
typedef struct OsalTask OsalTask;   // instance structure

extern int lib_task_create(OsalTask *in_ins_ptr,TaskHandler *in_func_ptr,void *in_arg_ptr);
extern int lib_task_join(OsalTask *in_ins_ptr);
extern int lib_task_detach(OsalTask *in_ins_ptr);

#ifdef __cplusplus
}
#endif

#endif /* EX_TASK_H */
