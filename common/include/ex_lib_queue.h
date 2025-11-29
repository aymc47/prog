
#ifndef _EX_LIB_QUEUE_H_
#define _EX_LIB_QUEUE_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

typedef struct _Node {
    struct _Node* m_next_ptr;
    void *m_body_ptr;
} Node;

typedef struct _Queue {
    Node* m_front_ptr;
    Node* m_rear_ptr;
} Queue;

typedef struct {
    Queue m_queue;
    pthread_cond_t m_cond;
    pthread_mutex_t m_mutex;
} WaitQueue;

typedef struct {
    uint8_t m_count;
    pthread_cond_t m_cond;
    pthread_mutex_t m_mutex;
} WaitEvent;

typedef struct {
    Queue m_queue;
    pthread_cond_t m_cond;
    pthread_mutex_t m_mutex;
} WaitQueueISR;

extern int lib_queue_init(Queue* in_ins_ptr);
extern int lib_queue_push(Queue* in_ins_ptr, Node* in_node_ptr);
extern Node* lib_queue_pop(Queue* in_ins_ptr, int* out_rc_ptr);
extern Node* lib_queue_get(Queue* in_ins_ptr, Node* in_node_ptr, int* out_rc_ptr);
extern int* lib_queue_remove(Queue* in_ins_ptr, Node* in_node_ptr);

extern int lib_wait_queue_init(WaitQueue* in_ins_ptr);
extern int lib_wait_queue_push(WaitQueue* in_ins_ptr, Node* in_node_ptr);
extern Node* lib_wait_queue_pop(WaitQueue* in_ins_ptr,int* out_rc_ptr);

extern int lib_event_init(WaitEvent* in_ins_ptr);
extern int lib_event_wait_isr(WaitEvent* in_ins_ptr);
extern int lib_event_wakeup_from_isr(WaitEvent* in_ins_ptr);

#endif