
#ifndef TABLEH_INCLUDED
#define TABLEH_INCLUDED


typedef enum TableStatus {
  SUCCESS,
  FAIL_UNKNOWN,
  FAIL_TABLE_ALLOC,
  FAIL_BUCKET_ALLOC,
  FAIL_ENTRY_ALLOC,
  FAIL_STR_ALLOC,
  FAIL_MISSING
} tablestatus;


struct table;
struct entry;


char *
entry_word(struct entry *e);

signed int
entry_value_get(struct entry *e);

void
entry_value_set(struct entry *e, int value); 

tablestatus
table_create(struct table **t, unsigned long size);

tablestatus
table_add(struct table *t, char *word, int value);

tablestatus
table_get(struct table *t, const char *word, struct entry **e);

void
table_free(struct table *t);

void
table_each(struct table *t, void (*fn)(struct entry *, void *), void *data);


#endif
