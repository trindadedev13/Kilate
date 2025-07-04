#include "kilate/hashmap.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kilate/string.h"

klt_hashmap* klt_hash_map_make(size_t itemSize) {
  klt_hashmap* hashMap = malloc(sizeof(klt_hashmap));
  hashMap->itemSize = itemSize;
  hashMap->capacity = 64;
  hashMap->itens = klt_vector_make(sizeof(klt_hashitem*));
  for (size_t i = 0; i < hashMap->capacity; i++) {
    klt_hashitem* null_ptr = NULL;
    klt_vector_push_back(hashMap->itens, &null_ptr);
  }
  return hashMap;
}

void klt_hash_map_delete(klt_hashmap* self) {
  for (size_t i = 0; i < self->itens->size; ++i) {
    klt_hashitem* item = *(klt_hashitem**)klt_vector_get(self->itens, i);
    if (item != NULL) {
      free(item->key);
      free(item->value);
      free(item);
    }
  }
  klt_vector_delete(self->itens);
  free(self);
}

unsigned int klt_hash_map_hash(klt_hashmap* self, klt_str key) {
  assert(self);
  assert(key);
  unsigned int hash = 5381;
  int c;
  while ((c = *key++)) {
    hash = ((hash << 5) + hash) + c;  // hash * 33 + c
  }
  return hash % self->capacity;
}
void* klt_hash_map_get(klt_hashmap* self, klt_str key) {
  assert(self);
  assert(key);
  unsigned int index = klt_hash_map_hash(self, key);

  klt_hashitem** itemPtr = (klt_hashitem**)klt_vector_get(self->itens, index);
  klt_hashitem* item = *itemPtr;

  while (item) {
    if (klt_str_equals(item->key, key)) {
      return item->value;
    }
    item = item->next;
  }
  return NULL;
}

void klt_hash_map_put(klt_hashmap* self, klt_str key, void* value) {
  assert(self);
  assert(key);

  unsigned int index = klt_hash_map_hash(self, key);
  klt_hashitem** headPtr = (klt_hashitem**)klt_vector_get(self->itens, index);
  klt_hashitem* head = *headPtr;

  klt_hashitem* item = head;
  while (item) {
    if (klt_str_equals(item->key, key)) {
      memcpy(item->value, value, self->itemSize);
      return;
    }
    item = item->next;
  }

  klt_hashitem* newItem = malloc(sizeof(klt_hashitem));
  newItem->key = strdup(key);
  newItem->value = malloc(self->itemSize);
  memcpy(newItem->value, value, self->itemSize);
  newItem->next = head;

  *headPtr = newItem;
}
