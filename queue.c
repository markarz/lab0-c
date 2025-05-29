#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (head)
        INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    element_t *entry, *safe = NULL;

    list_for_each_entry_safe (entry, safe, l, list)
        q_release_element(entry);
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)  // 確保 head 和 s 不是 NULL
        return false;

    element_t *new_element = malloc(sizeof(element_t));
    if (!new_element)  // 檢查 malloc 是否成功
        return false;

    new_element->value = strdup(s);  // 分配記憶體並複製字串
    if (!new_element->value) {
        free(new_element);
        return false;
    }

    list_add(&new_element->list, head);  // 插入節點

    return true;
}


/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s)  // 確保 head 和 s 不是 NULL
        return false;

    element_t *new_element = malloc(sizeof(element_t));
    if (!new_element)  // 檢查 malloc 是否成功
        return false;

    new_element->value = strdup(s);  // 分配記憶體並複製字串
    if (!new_element->value) {
        free(new_element);
        return false;
    }

    list_add_tail(&new_element->list, head);  // 插入節點

    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    // 1. 檢查串列是否存在且非空
    if (!head || list_empty(head))
        return NULL;

    // 2. 取得頭節點
    element_t *elem = list_first_entry(head, element_t, list);

    // 3. 從串列中移除(時間複雜度 O(1))
    list_del(&elem->list);

    if (sp && bufsize > 0) {
        // 先放個空字串，避免後面任何意外
        sp[0] = '\0';

        // 若 elem->value 不為空就複製字串
        if (elem->value) {
            // 複製最多 bufsize - 1 個字元，最後再補 '\0'
            strncpy(sp, elem->value, bufsize - 1);
            sp[bufsize - 1] = '\0';  // 保證結尾是 '\0'
        }
    }

    // 5. 回傳這個被移除的節點
    //    (呼叫端若需要把 elem->value 或 elem 本身 free 掉再自行決定)
    return elem;
}



/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    // 1. 檢查串列是否存在且非空
    if (!head || list_empty(head))
        return NULL;

    // 2. 取得「尾」節點（O(1)）
    element_t *elem = list_last_entry(head, element_t, list);

    // 3. 從串列中移除（O(1)）
    list_del(&elem->list);

    // 4. 若需要複製字串到 sp，就在這裡複製
    if (sp && bufsize > 0) {
        // 先將緩衝區置空，避免後面狀況導致舊資料殘留
        sp[0] = '\0';

        // 若 elem->value 不為 NULL，就做字串拷貝
        if (elem->value) {
            strncpy(sp, elem->value, bufsize - 1);
            sp[bufsize - 1] = '\0';  // 確保結尾有 '\0'
        }
    }

    // 5. 回傳被移除的節點 (呼叫端若需要釋放記憶體，可自行 free(elem->value) 與
    // free(elem))
    return elem;
}



/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head)
        return NULL;
    if (list_is_singular(head)) {
        list_del(head);
        return true;
    }

    struct list_head *cur = NULL;
    struct list_head *tmp = NULL;
    int mid = 0, current = -1;
    cur = head;
    mid = q_size(head) / 2;
    while (cur != NULL && current != mid) {
        tmp = cur;
        cur = cur->next;

        current++;
    }
    element_t *del_element = list_entry(cur, element_t, list);  // O(1) 取得尾部
    tmp->next = cur->next;
    q_release_element(del_element);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return false;

    struct list_head *cur = head->next;

    while (cur != head) {
        element_t *elem1 = list_entry(cur, element_t, list);
        struct list_head *next = cur->next;

        bool has_duplicate = false;

        // 檢查後面是否有相同的數據
        while (next != head) {
            element_t *elem2 = list_entry(next, element_t, list);
            if (strcmp(elem1->value, elem2->value) == 0) {
                struct list_head *dup = next;
                next = next->next;
                list_del(dup);
                q_release_element(elem2);
                has_duplicate = true;
            } else
                break;
        }

        struct list_head *temp = cur;
        cur = next;

        // 如果 `has_duplicate == true`，代表 `temp` 也應該刪除
        if (has_duplicate) {
            list_del(temp);
            q_release_element(elem1);
        }
    }

    return true;
}


/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
