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

int lib_wait_queue_init(WaitQueue* in_ins_ptr)
{
    if (in_ins_ptr == NULL) return -1;

    in_ins_ptr->m_queue.m_front_ptr = NULL;
    in_ins_ptr->m_queue.m_rear_ptr  = NULL;

    pthread_mutex_init(&in_ins_ptr->m_mutex, NULL);
    pthread_cond_init(&in_ins_ptr->m_cond, NULL);

    return 0;
}

int lib_wait_queue_push(WaitQueue* in_ins_ptr, Node* in_node_ptr)
{
    int rc = 0;
    if ((in_ins_ptr == NULL) || (in_node_ptr == NULL)) return -1;

    pthread_mutex_lock(&in_ins_ptr->m_mutex);

    rc = lib_queue_push(&in_ins_ptr->m_queue, in_node_ptr);

    pthread_cond_signal(&in_ins_ptr->m_cond);
    pthread_mutex_unlock(&in_ins_ptr->m_mutex);

    return rc;
}

Node* lib_wait_queue_pop(WaitQueue* in_ins_ptr,int* out_rc_ptr){
    int rc = 0;
    Node* ret_ptr = NULL;

    if (!in_ins_ptr || !out_rc_ptr) rc = -1;
    if (!rc){
        pthread_mutex_lock(&in_ins_ptr->m_mutex);

        while (in_ins_ptr->m_queue.m_front_ptr == NULL) {
            pthread_cond_wait(&in_ins_ptr->m_cond, &in_ins_ptr->m_mutex);
        }

        ret_ptr = lib_queue_pop(&in_ins_ptr->m_queue, &rc);

        pthread_mutex_unlock(&in_ins_ptr->m_mutex);
    }
    if(out_rc_ptr) *out_rc_ptr = rc;

    return ret_ptr;
}

int lib_event_init(WaitEvent* in_ins_ptr)
{
    int rc = 0;
    if (in_ins_ptr == NULL) return -1;

    (void)pthread_mutex_init(&in_ins_ptr->m_mutex, NULL);
    (void)pthread_cond_init(&in_ins_ptr->m_cond, NULL);
    
    in_ins_ptr->m_count = 0;

    return rc;
}

int lib_event_wakeup_from_isr(WaitEvent* in_ins_ptr){
    if (in_ins_ptr == NULL) return -1;
    
    pthread_mutex_lock(&in_ins_ptr->m_mutex);
    in_ins_ptr->m_count++;
    pthread_cond_signal(&in_ins_ptr->m_cond);
    pthread_mutex_unlock(&in_ins_ptr->m_mutex);

    return 0;
}
int lib_event_wait_isr(WaitEvent* in_ins_ptr){
    if (in_ins_ptr == NULL) return -1;

    pthread_mutex_lock(&in_ins_ptr->m_mutex);
    while(in_ins_ptr->m_count) {
        pthread_cond_wait(&in_ins_ptr->m_cond, &in_ins_ptr->m_mutex);
    }
    pthread_mutex_unlock(&in_ins_ptr->m_mutex);
    return 0;
}
