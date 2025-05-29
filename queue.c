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
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *cur = head->next;

    while (cur != head && cur->next != head) {
        struct list_head *next = cur->next;

        // 取得 element_t 結構體
        element_t *elem1 = list_entry(cur, element_t, list);
        element_t *elem2 = list_entry(next, element_t, list);

        // 交換 `value`（這裡只交換指標，因為 `value` 是字串）
        char *temp = elem1->value;
        elem1->value = elem2->value;
        elem2->value = temp;

        // 移動 cur 到下一對
        cur = next->next;
    }
}


/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *cur = head->next, *last = head->prev;

    while (cur != last && last->next != cur) {
        element_t *elem1 = list_entry(cur, element_t, list);
        element_t *elem2 = list_entry(last, element_t, list);

        // 交換 `value`
        char *temp = elem1->value;
        elem1->value = elem2->value;
        elem2->value = temp;

        cur = cur->next;
        last = last->prev;
    }
}


/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head) || list_is_singular(head) || k == 1)
        return;

    struct list_head *cur = head->next;
    while (cur != head) {
        struct list_head *start = cur, *end = cur;
        int count = 1;

        // 找到 k 個節點
        while (count < k && end->next != head) {
            end = end->next;
            count++;
        }

        if (count < k)  // 剩餘節點不足 k 個，停止
            break;

        struct list_head *left = start, *right = end;
        while (left != right && right->next != left) {
            element_t *elem1 = list_entry(left, element_t, list);
            element_t *elem2 = list_entry(right, element_t, list);

            // 交換 `value`
            char *temp = elem1->value;
            elem1->value = elem2->value;
            elem2->value = temp;

            left = left->next;
            right = right->prev;
        }

        cur = end->next;  // 移動到下一組
    }
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head less, equal, greater;
    INIT_LIST_HEAD(&less);
    INIT_LIST_HEAD(&equal);
    INIT_LIST_HEAD(&greater);

    // **改進 pivot 選擇**
    const element_t *first = list_first_entry(head, element_t, list);
    const element_t *mid = list_entry(head->next, element_t, list);
    const element_t *last = list_entry(head->prev, element_t, list);

    const char *pivot_value = NULL;
    if ((strcmp(first->value, mid->value) > 0) ^
        (strcmp(first->value, last->value) > 0))
        pivot_value = first->value;
    else if ((strcmp(mid->value, first->value) > 0) ^
             (strcmp(mid->value, last->value) > 0))
        pivot_value = mid->value;
    else
        pivot_value = last->value;

    struct list_head *cur, *safe;
    list_for_each_safe (cur, safe, head) {
        const element_t *elem = list_entry(cur, element_t, list);

        int cmp = strcmp(elem->value, pivot_value);
        if (descend)
            cmp = -cmp;

        if (cmp < 0) {
            list_move_tail(cur, &less);
        } else if (cmp == 0) {
            list_move_tail(cur, &equal);
        } else {
            list_move_tail(cur, &greater);
        }
    }

    // **遞歸 QuickSort**
    q_sort(&less, descend);
    q_sort(&greater, descend);

    // **合併 sorted less -> equal -> sorted greater**
    list_splice_tail(&less, head);
    list_splice_tail(&equal, head);
    list_splice_tail(&greater, head);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    if (list_is_singular(head))
        return 1;

    struct list_head *cur = head->prev, *prev;
    const element_t *max_elem = list_entry(cur, element_t, list);
    const char *max_value = max_elem->value;  // 目前的最大值

    while (cur != head) {
        prev = cur->prev;
        element_t *elem = list_entry(cur, element_t, list);

        if (strcmp(elem->value, max_value) > 0) {
            max_value = elem->value;  // 更新最大值
        } else {
            list_del(cur);
            free(elem->value);
            free(elem);
        }
        cur = prev;  // 向左移動
    }

    // 計算剩餘節點數
    int total_size = 0;
    list_for_each (cur, head)
        total_size++;
    return total_size;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    if (list_is_singular(head))
        return 1;

    struct list_head *cur = head->prev, *prev;
    const element_t *max_elem = list_entry(cur, element_t, list);
    const char *max_value = max_elem->value;  // 目前的最大值

    while (cur != head) {
        prev = cur->prev;
        element_t *elem = list_entry(cur, element_t, list);

        if (strcmp(elem->value, max_value) < 0) {
            list_del(cur);
            free(elem->value);
            free(elem);
        } else {
            max_value = elem->value;  // 更新最大值
        }

        cur = prev;  // 向左移動
    }

    // 計算剩餘節點數
    int count = 0;
    list_for_each (cur, head)
        count++;
    return count;
}


/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */


/**
 * q_merge - 將 k 個已排序 queue 合併到第一個 queue
 * @head:    queue_contex_t 鏈結串列的表頭
 * @descend: 若為 true，則降序合併；否則升序合併
 *
 * 假設所有 queue 事先已經排序過，此函式以 **O(n log k)** 時間內完成合併。
 * 不會新分配記憶體，所有 queue 只進行指標調整，不影響記憶體配置。
 * 合併後，其他 queue 變為空 queue，所有元素存入第一個 queue。
 *
 * 回傳值:
 *   - 合併後 queue 的總元素數量
 */
int q_merge(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return 0;

    // 取得第一個 queue，作為合併的結果
    queue_contex_t *first_q = list_first_entry(head, queue_contex_t, chain);
    struct list_head *first_list = first_q->q;

    queue_contex_t *cur;
    list_for_each_entry (cur, head, chain) {
        if (cur == first_q || list_empty(cur->q))
            continue;

        // 直接將當前 queue 併入第一個 queue
        list_splice_tail_init(cur->q, first_list);
    }

    // 對合併後的 queue 進行排序
    q_sort(first_list, descend);

    // 計算 queue 長度
    int total_size = 0;
    struct list_head *node;
    list_for_each (node, first_list)
        total_size++;

    first_q->size = total_size;
    return total_size;
}