
#include <stdlib.h>
#include <stdio.h>
#include <assert.h> /* assert */
#include <string.h> /* strncpy */
#include "table.h"

struct entry {
  char *word;
  int value;
  struct entry *next;
};


struct table {
  unsigned long size;
  struct entry **buckets;
};


char *
entry_word(struct entry *e) {
  assert(e);
  return e->word;
}


signed int
entry_value_get(struct entry *e) {
  assert(e);
  return e->value;
}


void
entry_value_set(struct entry *e, int value) {
  assert(e);
  e->value = value;
}


tablestatus
entry_create(char *word, unsigned char len, int value, struct entry **e) {

  if((*e = malloc(sizeof(struct entry))) == NULL) {
    return FAIL_ENTRY_ALLOC;
  }
  
  if(((*e)->word = malloc(sizeof(unsigned char)*len + 1)) == NULL) {    
    free(*e);
    return FAIL_STR_ALLOC;
  }

  (*e)->value = value;
  (*e)->next = NULL;
  strncpy((*e)->word, word, len);
  (*e)->word[len] = '\0';

  return SUCCESS;
}


static void
entry_free(struct entry *e) {
  assert(e);
  free(e->word);
  free(e);
}


static unsigned long
djb2_hash(const char *str)
{
  unsigned long hash = 5381;
  int c;

  while ((c = *str++) != '\0') {
    hash = ((hash << 5) + hash) + c;
  }

  return hash;
}


tablestatus
table_create(struct table **t, unsigned long size) {

  if((*t = malloc(sizeof(struct table))) == NULL) {
    return FAIL_TABLE_ALLOC;
  }

  if(((*t)->buckets = malloc(sizeof(struct entry*) * size)) == NULL) {
    free(*t);
    return FAIL_BUCKET_ALLOC;
  }

  (*t)->size = size;
  while(size) {
    (*t)->buckets[--size] = NULL;
  }

  return SUCCESS;
}


tablestatus
table_add(struct table *t, char *word, int value) {
  assert(t);

  tablestatus s;
  unsigned long hash;
  struct entry *new = NULL, *current = NULL;

    
  hash = djb2_hash(word) % t->size;
  current = t->buckets[hash];

  if(current == NULL) {                  /* bucket empty */
    if((s=entry_create(word, strlen(word), value, &new) == SUCCESS)) {
      t->buckets[hash] = new;
    }
    return SUCCESS;
  }
  else {                                 /* bucket already has entries */
    while(current && current->next && (strcmp(word, current->word) != 0)) {
      current = current->next;
    }
   
    if(strcmp(word, current->word) == 0) { /* entry already entered */
      //current->occurrences++;
      current->value = value;
      return SUCCESS;
    }                                    /* entry is new */
    else if((s=entry_create(word, strlen(word), value, &new)) == SUCCESS) {
      current->next = new;
      return SUCCESS;
    }
    else {                               /* allocation failed */
      return s;
    }
  }  
}
  

tablestatus
table_get(struct table *t, const char *word, struct entry **e) {
  assert(t);
  
  unsigned char diff = 0;
  
  *e = t->buckets[djb2_hash(word) % t->size];
  while(*e && (*e)->next) {
    if ((diff = strcmp((*e)->word, word)) == 0) {
      break;
    }
    *e = (*e)->next;
  }
  return (!(*e) || diff) ? FAIL_MISSING : SUCCESS;
}


void
table_free(struct table *t) {
  assert(t);

  struct entry *e, *next;
  while(t->size) {
    e = t->buckets[--(t->size)];
    while(e) {
      next = e->next;
      entry_free(e);
      e = next;
    }
  }
  free(t->buckets);
  free(t);
}


void
table_each(struct table *t, void (*fn)(struct entry *, void *), void *data) {
  unsigned long i = t->size;
  struct entry *e;
  while(i) {
    e = t->buckets[--(i)];
    while(e) {
      fn(e, data);
      e = e->next;
    }
  }
}
