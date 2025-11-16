#include "ex_lib_queue.h"

int lib_queue_init(Queue* in_ins_prt)
{
    if (in_ins_prt == NULL) return -1;
    
    in_ins_prt->m_front_ptr = NULL;
    in_ins_prt->m_rear_ptr  = NULL;

    return 0;
}

int lib_queue_push(Queue* in_ins_ptr, Node* in_node_ptr)
{
    if ((in_ins_ptr == NULL) || (in_node_ptr == NULL)) return -1;

    in_node_ptr->m_next_ptr = NULL;
    if (in_ins_ptr->m_rear_ptr == NULL) {
        in_ins_ptr->m_front_ptr = in_node_ptr;
        in_ins_ptr->m_rear_ptr  = in_node_ptr;
    } else {
        in_ins_ptr->m_rear_ptr->m_next_ptr = in_node_ptr;
        in_ins_ptr->m_rear_ptr = in_node_ptr;
    }

    return 0;
}

Node* lib_queue_pop(Queue* in_ins_ptr, int* out_rc_ptr)
{
    if ((in_ins_ptr == NULL) || (out_rc_ptr == NULL)) {
        if(out_rc_ptr) *out_rc_ptr = -1;
        return NULL;
    }

    Node* node_ptr = NULL;
    node_ptr = in_ins_ptr->m_front_ptr;
    if (in_ins_ptr->m_front_ptr != NULL) {
        in_ins_ptr->m_front_ptr = in_ins_ptr->m_front_ptr->m_next_ptr;
        node_ptr->m_next_ptr = NULL;
    }
    *out_rc_ptr = 0;
    return node_ptr;
}

Node* lib_queue_get(Queue* in_ins_ptr, Node* in_node_ptr, int* out_rc_ptr)
{
    if ((in_ins_ptr == NULL) || (in_node_ptr == NULL) || (out_rc_ptr == NULL)) {
        if(out_rc_ptr) *out_rc_ptr = -1;
        return NULL;
    }

    Node* cur_ptr = in_ins_ptr->m_front_ptr;
    while (cur_ptr != NULL) {
        if (cur_ptr == in_node_ptr) {
            *out_rc_ptr = 0;
            return cur_ptr->m_body_ptr;
        }
        cur_ptr = cur_ptr->m_next_ptr;
    }

    *out_rc_ptr = -1;
    return NULL;
}

int* lib_queue_remove(Queue* in_ins_ptr, Node* in_node_ptr)
{
    if ((in_ins_ptr == NULL) || (in_node_ptr == NULL)) {
        return NULL;
    }

    Node* prev_ptr = NULL;
    Node* cur_ptr  = in_ins_ptr->m_front_ptr;

    while (cur_ptr != NULL) {
        if (cur_ptr == in_node_ptr) {

            if (prev_ptr == NULL) {
                /* remove head */
                in_ins_ptr->m_front_ptr = cur_ptr->m_next_ptr;
            } else {
                prev_ptr->m_next_ptr = cur_ptr->m_next_ptr;
            }

            if (cur_ptr->m_next_ptr == NULL) {
                in_ins_ptr->m_rear_ptr = prev_ptr;
            }

            /* return body pointer as int* (your interface definition) */
            return (int*)cur_ptr->m_body_ptr;
        }

        prev_ptr = cur_ptr;
        cur_ptr = cur_ptr->m_next_ptr;
    }

    return NULL;
}

int lib_wait_queue_init(Queue* in_ins_ptr)
{
    if (in_ins_ptr == NULL) return -1;

    WaitQueue* wq_ptr = (WaitQueue*)in_ins_ptr;

    wq_ptr->m_queue.m_front_ptr = NULL;
    wq_ptr->m_queue.m_rear_ptr  = NULL;

    pthread_mutex_init(&wq_ptr->m_mutex, NULL);
    pthread_cond_init(&wq_ptr->m_cond, NULL);

    return 0;
}

int lib_wait_queue_push(Queue* in_ins_ptr, Node* in_node_ptr)
{
    if ((in_ins_ptr == NULL) || (in_node_ptr == NULL)) return -1;

    WaitQueue* wq_ptr = (WaitQueue*)in_ins_ptr;

    pthread_mutex_lock(&wq_ptr->m_mutex);

    (void)lib_queue_push(&wq_ptr->m_queue, in_node_ptr);

    pthread_cond_signal(&wq_ptr->m_cond);
    pthread_mutex_unlock(&wq_ptr->m_mutex);

    return 0;
}

Node* lib_wait_queue_pop(Queue* in_ins_ptr)
{
    if (in_ins_ptr == NULL) return NULL;

    WaitQueue* wq_ptr = (WaitQueue*)in_ins_ptr;
    Node* ret_ptr = NULL;
    int rc = 0;

    pthread_mutex_lock(&wq_ptr->m_mutex);

    while (wq_ptr->m_queue.m_front_ptr == NULL) {
        pthread_cond_wait(&wq_ptr->m_cond, &wq_ptr->m_mutex);
    }

    ret_ptr = lib_queue_pop(&wq_ptr->m_queue, &rc);

    pthread_mutex_unlock(&wq_ptr->m_mutex);

    return ret_ptr;
}