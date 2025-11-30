
#ifndef _EX_LIB_TASK_H_
#define _EX_LIB_TASK_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef void (TaskHandler)(void*);
extern int lib_task_create(TaskHandler* in_func_ptr, void* in_arg_ptr);
extern int lib_task_wait();
extern int lib_task_wakeup();

#endif  // _EX_LIB_TASK_H_