#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head =
        (struct list_head *) malloc(sizeof(struct list_head));
    if (!head)
        return NULL;

    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    struct list_head *safe, *curr;
    element_t *tmp;

    list_for_each_safe (curr, safe, head) {
        tmp = list_entry(curr, typeof(*tmp), list);
        list_del(curr);
        free(tmp->value);
        free(tmp);
    }
    free(head);

    return;
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;

    element_t *new_elem = malloc(sizeof(element_t));
    if (!new_elem)
        return false;

    new_elem->value = strdup(s);
    if (!new_elem->value) {
        free(new_elem);
        return false;
    }

    list_add(&new_elem->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;

    element_t *new_elem = malloc(sizeof(element_t));
    if (!new_elem)
        return false;

    new_elem->value = strdup(s);
    if (!new_elem->value) {
        free(new_elem);
        return false;
    }

    list_add_tail(&new_elem->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *rm_elem = list_first_entry(head, element_t, list);
    list_del(&rm_elem->list);

    if (sp && bufsize > 0) {
        strncpy(sp, rm_elem->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }


    return rm_elem;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *rm_elem = list_last_entry(head, element_t, list);
    list_del(&rm_elem->list);

    if (sp && bufsize > 0) {
        strncpy(sp, rm_elem->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }


    return rm_elem;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    struct list_head *ptr;
    int len = 0;
    list_for_each (ptr, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/

    if (!head || list_empty(head))
        return false;

    struct list_head *slow, *fast;
    slow = head->next;
    fast = head->next;

    while (fast->next != head && fast->next->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }

    element_t *del_elem = list_entry(slow, element_t, list);
    list_del(slow);
    free(del_elem->value);
    free(del_elem);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;

    struct list_head *curr_lh = head->next;
    while (curr_lh != head) {
        bool dup = false;
        struct list_head *next_lh = curr_lh->next, *del;

        while (next_lh != head) {
            // if equal , delete element and move to next elem
            if (next_lh &&
                strcmp(list_entry(next_lh, element_t, list)->value,
                       list_entry(curr_lh, element_t, list)->value) == 0) {
                del = next_lh;
                next_lh = next_lh->next;
                list_del(del);
                free(list_entry(del, element_t, list)->value);
                free(list_entry(del, element_t, list));
                dup = true;
            } else {
                break;
            }
        }

        // delete current element
        if (dup) {
            del = curr_lh;
            curr_lh = next_lh;
            list_del(del);
            free(list_entry(del, element_t, list)->value);
            free(list_entry(del, element_t, list));
        } else {
            curr_lh = next_lh;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head))
        return;

    struct list_head *curr = head->next, *seg_head = head;
    for (struct list_head *tmp; curr != head && curr->next != head;
         curr = curr->next) {
        tmp = curr->next;
        list_del(curr->next);
        list_add(tmp, seg_head);
        seg_head = curr;
    }
    return;
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *ptr = head, *tmp;
    do {
        tmp = ptr->prev;
        ptr->prev = ptr->next;
        ptr->next = tmp;
        ptr = ptr->prev;
    } while (ptr != head);

    return;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    int length = q_size(head);
    struct list_head *seg_head = head, *curr = head->next, *tmp;
    for (int i = 0, bound; i + k <= length;) {
        bound = i + k;
        while (i < bound) {
            tmp = curr->next;
            list_move(curr, seg_head);
            curr = tmp;
            i++;
        }
        seg_head = curr->prev;
    }
    return;
}

/* Merge Two double linked queue */
void mergeSort_merge(struct list_head *q1, struct list_head *q2, bool descend)
{
    struct list_head merged;
    INIT_LIST_HEAD(&merged);

    struct list_head *ptr1 = q1->next, *ptr2 = q2->next;

    while (ptr1 != q1 && ptr2 != q2) {
        if ((descend && strcmp(list_entry(ptr1, element_t, list)->value,
                               list_entry(ptr2, element_t, list)->value) > 0) ||
            (!descend &&
             strcmp(list_entry(ptr1, element_t, list)->value,
                    list_entry(ptr2, element_t, list)->value) < 0)) {
            struct list_head *next_lh = ptr1->next;
            list_move_tail(ptr1, &merged);
            ptr1 = next_lh;
        } else {
            struct list_head *next_lh = ptr2->next;
            list_move_tail(ptr2, &merged);
            ptr2 = next_lh;
        }
    }

    while (ptr1 != q1) {
        struct list_head *next = ptr1->next;
        list_move_tail(ptr1, &merged);
        ptr1 = next;
    }

    while (ptr2 != q2) {
        struct list_head *next = ptr2->next;
        list_move_tail(ptr2, &merged);
        ptr2 = next;
    }

    list_splice(&merged, q1);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *slow, *fast;
    for (slow = fast = head->next;
         fast->next != head && fast->next->next != head;) {
        slow = slow->next;
        fast = fast->next->next;
    }

    struct list_head list_to;
    list_cut_position(&list_to, head, slow);
    q_sort(head, descend);
    q_sort(&list_to, descend);
    mergeSort_merge(head, &list_to, descend);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head) || list_is_singular(head))
        return q_size(head);

    struct list_head *curr = head->prev;
    const char *up_bound = list_entry(curr, element_t, list)->value;
    while (curr->prev != head) {
        if (strcmp(list_entry(curr->prev, element_t, list)->value, up_bound) >
            0) {
            struct list_head *tmp = curr->prev;
            list_del(curr->prev);
            free(list_entry(tmp, element_t, list)->value);
            free(list_entry(tmp, element_t, list));
        } else {
            up_bound = list_entry(curr->prev, element_t, list)->value;
            curr = curr->prev;
        }
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head) || list_is_singular(head))
        return q_size(head);

    struct list_head *curr = head->prev;
    const char *low_bound = list_entry(curr, element_t, list)->value;
    while (curr->prev != head) {
        if (strcmp(list_entry(curr->prev, element_t, list)->value, low_bound) <
            0) {
            struct list_head *tmp = curr->prev;
            list_del(curr->prev);
            free(list_entry(tmp, element_t, list)->value);
            free(list_entry(tmp, element_t, list));
        } else {
            low_bound = list_entry(curr->prev, element_t, list)->value;
            curr = curr->prev;
        }
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
