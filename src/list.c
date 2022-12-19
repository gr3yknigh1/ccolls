#include "list.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <assert.h>

LNode* LNode_Construct(void* value, LNode* next, size_t size) {
    assert(value != NULL);
    assert(size != 0);

    LNode* node = malloc(sizeof(LNode));
    node->value = malloc(size);
    memcpy(node->value, value, size);
    node->next = next;
    node->size = size;
    return node;
}

void LNode_Free(LNode *node) {
    assert(node != NULL);

    free(node->value);
    if (node->next != NULL) {
        LNode_Free(node->next);
    }
    free(node);
}

bool LNode_Equals(const LNode* node, const LNode* other) {
    return node->value == other->value \
        && node->size == other->size   \
        && node->next == other->next;
}

List* List_Construct() {
    List* list = malloc(sizeof(List));
    list->length = 0;
    list->head = NULL;
    list->tail = NULL;
    return list;
}

int List_Get(const List *list, u64 index, void **outNode) {
    LNode* node;
    int exitCode = List_GetNode(list, index, &node);
    *outNode = node->value;
    return exitCode;
}

int List_GetNode(const List* list, u64 index, LNode** outNode) {
    if (List_IsEmpty(list)) {
        printf("Error: No items in list\n");
        return LIST_INDEX_ERROR;
    } else if (!List_InRange(list, index)) {
        printf("IndexError: List[%lu] Index(%lu)\n", list->length, index);
        return LIST_INDEX_ERROR;
    }

    // NOTE: Handle first and last special cases
    if (List_IsFirst(list, index)) {
        *outNode = list->head;
        return LIST_OK;
    } else if (List_IsLast(list, index)) {
        *outNode = list->tail;
        return LIST_OK;
    } else {
        // NOTE: Staring from second item because first is already handled
        _List_LoopUntil(list, 1, list->length, index, outNode);
    }

    return LIST_OK;
}

u64 List_PushBack(List *list, void *value, size_t size) {
    if (List_IsEmpty(list)) {
        list->head = LNode_Construct(value, NULL, size);
        list->tail = list->head;
    } else if (list->length == 1) {
        list->tail = LNode_Construct(value, NULL, size);
        list->head->next = list->tail;
    } else {
        list->tail->next = LNode_Construct(value, NULL, size);
        list->tail = list->tail->next;
    }

    list->length++;
    return list->length;
}

u64 List_PushFront(List *list, void *value, size_t size) {
    if (List_IsEmpty(list)) {
        list->head = LNode_Construct(value, NULL, size);
        list->tail = list->head;
    } else {
        LNode* newNode = LNode_Construct(value, list->head, size);
        list->head = newNode;
    }

    list->length++;
    return list->length;
}

int List_Insert(List *list, u64 index, void *value, size_t size) {
    if (List_IsEmpty(list) || List_IsLast(list, index)) {
        List_PushBack(list, value, size);
        return LIST_OK;
    } else if (List_IsFirst(list, index)) {
        List_PushFront(list, value, size);
        return LIST_OK;
    } else if (!List_InRange(list, index)) {
        printf("IndexError: List[%lu] Index(%lu)\n", list->length, index);
        return LIST_INDEX_ERROR;
    }

    LNode *before;
    _List_LoopUntil(list, 1, list->length, index - 1, &before);

    before->next = LNode_Construct(value, before->next, size);

    list->length++;
    return LIST_OK;
}

u64 List_ExpandBack(List* list, void* begin, const u64 length);
u64 List_ExpandFront(List* list, void* begin, const u64 length);
u64 List_ExpandInsert(
    List* list, u64 index, void* begin, const u64 length);

int List_Pop(List *list, u64 index, LNode** outNode) {
    if (List_IsEmpty(list)) {
        printf("Error: No items in list\n");
        return LIST_INDEX_ERROR;
    } else if (index >= list->length) {
        printf("Error: Index greater or equal to length: %lu\n", index);
        return LIST_INDEX_ERROR;
    } else if (index < 0) {
        printf("Error: Index less than 0: %lu\n", index);
        return LIST_INDEX_ERROR;
    }

    if (List_IsFirst(list, index)) {
        *outNode = list->head;
        list->head = list->head->next;
    } else {
        LNode *before;
        _List_LoopUntil(list, 1, list->length, index - 1, &before);
        *outNode = before->next;
        before->next = before->next->next;
    }

    list->length--;
    return LIST_OK;
}

int List_PopRange(List* list, u64 start, u64 end, LNode** outNodes);

int List_FreeItem(List* list, u64 index) {
    if (!List_InRange(list, index)) {
        printf("IndexError: List[%lu] Index(%lu)\n", list->length, index);
        return LIST_INDEX_ERROR;
    }

    LNode* node;
    List_Pop(list, index, &node);
    node->next = NULL;
    LNode_Free(node);

    return LIST_OK;
}


int List_FreeRange(List* list, u64 start, u64 end);

void List_Clear(List* list) {
    if (list->length == 0) {
        return;
    }
    LNode_Free(list->head);
    list->length = 0;
    list->head = NULL;
    list->tail = NULL;
}

void List_Free(List *list) {
    if (list->length == 0) {
        return;
    }
    List_Clear(list);
    free(list);
    list = NULL;
}

bool List_IsEmpty(const List* list) {
    return list->length == 0;
}

bool List_IsFirst(const List* list, const u64 index) {
    return index == 0;
}

bool List_IsLast(const List* list, const u64 index) {
    return index == list->length - 1;
}

bool List_InRange(const List* list, const u64 index) {
    return index >= 0 && index < list->length;
}

u64 _List_IncementLength(List* list) {
    list->length++;
    return list->length;
}

void _List_LoopUntil(
    const List* list,
    u64 start,
    u64 end,
    u64 index,
    LNode** outNode
) {

    u64 currentIndex = start;
    LNode* currentNode = list->head->next;
    while(currentNode != NULL) {
        if (currentIndex == index) {
            *outNode = currentNode;
            break;
        }

        currentIndex++;
        currentNode = currentNode->next;
    }
}

