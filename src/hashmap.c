#include <assert.h>

#include "cboxes/hash.h"
#include "cboxes/hashmap.h"
#include "cboxes/list.h"
#include "cboxes/pair.h"
#include "cboxes/shallow.h"
#include "cboxes/type.h"

cs_Hashmap *cs_Hashmap_New(const cs_Type *type, u64 capacity) {
    cs_Hashmap *hashmap = malloc(sizeof(cs_Hashmap));

    *hashmap = (cs_Hashmap){
        .slots = cs_List_New(CS_LIST_TYPE),
        .type = type,
        .capacity = capacity,
        .count = 0,
    };

    for (u64 i = 0; i < capacity; i++) {
        cs_List_PushBack(hashmap->slots, cs_List_New(CS_PAIR_TYPE));
    }
    return hashmap;
}

cs_Hashmap *cs_Hashmap_NewD(size_t size, u64 capacity) {
    return cs_Hashmap_New(
        cs_Type_New(size, false, cs_ShallowCopy, cs_ShallowFree), capacity);
}

u64 cs_Hashmap_Hash(cs_Hashmap *hashmap, cstr key) {
    return cs_lose_lose_hash((const unsigned char *)key) % hashmap->capacity;
}

#define CS_EXIT_ON_ERR(C)                                                      \
    do {                                                                       \
        cs_Status __s;                                                         \
        if ((__s = C) != cs_OK)                                                \
            return __s;                                                        \
    } while (0)

cs_Status cs_Hashmap_Set(cs_Hashmap *hashmap, cstr key, void *value) {
    u64 index = cs_Hashmap_Hash(hashmap, key);

    cs_List *slot = NULL;
    CS_EXIT_ON_ERR(CS_LIST_GET(hashmap->slots, index, slot));

    if (cs_List_IsEmpty(slot)) {
        cs_List_PushBack(slot, cs_Pair_New(key, value, hashmap->type));
    } else {
        cs_Pair *targetPair = NULL;
        CS_LIST_FOREACHV(slot, cs_Pair, pair, {
            if (pair->key == key) {
                targetPair = pair;
            }
        });
        if (targetPair == NULL) {
            cs_List_PushBack(slot, cs_Pair_New(key, value, hashmap->type));
        } else {
            targetPair->type->free(targetPair->value);
            targetPair->value = cs_Type_StoreValue(targetPair->type, value);
        }
    }

    return cs_OK;
}

void cs_Hashmap_Free(void *ptr) {
    cs_Hashmap *hashmap = (cs_Hashmap *)ptr;

    for (u64 i = 0; i < hashmap->capacity; i++) {
        cs_List *slot = NULL;
        assert(CS_LIST_GET(hashmap->slots, i, slot) == cs_OK);
        cs_List_Free(slot);
    }
    cs_List_Free(hashmap->slots);

    free(ptr);
}
