
#include <stdio.h> /* CuTest macros use NULL */

#include "table.h"
#include "CuTest.h"

/* ensure SUCCESS status on found word */
static void
test_table_add_status(CuTest *tc) {  
  tablestatus s;
  struct table *t;
  struct entry *e;

  char *word = "barbledegook";
  table_create(&t, 32);
  table_add(t, word, 0);
  s = table_get(t, word, &e);
  CuAssertIntEquals(tc, s, SUCCESS);
  table_free(t);

}

/* ensure FAIL_MISSING status on missing word */
static void
test_table_missing_status(CuTest *tc) {
  tablestatus s;
  struct table *t;
  struct entry *e;
  char *in;

  in= "barbledegook";
  table_create(&t, 32);
  s = table_get(t, in, &e);
  CuAssertIntEquals(tc, FAIL_MISSING, s);
  table_free(t);
}

/* ensure added entry has correct fields */
static void
test_table_add(CuTest *tc) {
  tablestatus s;
  struct table *t;
  struct entry *e;
  char *in;
  int value;

  in = "entrye";
  value = 1024UL;  
  table_create(&t, 32);
  s = table_add(t, in, value);
  s = table_get(t, in, &e);
  
  CuAssertIntEquals(tc, SUCCESS, s);
  CuAssertStrEquals(tc, in, entry_word(e));
  CuAssertIntEquals(tc, value, entry_value_get(e));

  table_free(t);
}

/* ensure multiple adds increments occurrence count */
static void
test_table_occurrences(CuTest *tc) {
  tablestatus s;
  struct table *t;
  struct entry *e;
  char *in;
  int i, count, value;
  
  in = "entrye";
  count = 8, value = 100;
  
  table_create(&t, 32);
  for(i = 0; i < count; i++) {
      s = table_add(t, in, value);
  }  
  s = table_get(t, in, &e);
  
  CuAssertIntEquals(tc, SUCCESS, s);
  CuAssertStrEquals(tc, in, entry_word(e));
  CuAssertIntEquals(tc, value, entry_value_get(e));

  table_free(t);

}
 

CuSuite *test_table_create() {
  CuSuite *suite = CuSuiteNew();
  SUITE_ADD_TEST(suite, test_table_add_status);
  SUITE_ADD_TEST(suite, test_table_missing_status);
  SUITE_ADD_TEST(suite, test_table_add);
  SUITE_ADD_TEST(suite, test_table_occurrences);
  return suite;
}
