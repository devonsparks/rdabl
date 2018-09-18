
#include <stdio.h> /* fopen, CuTest macros use NULL */

#include "CuTest.h"

#include "table.h"
#include "scan.h"

extern char sample_txt[];
extern int  sample_txt_len;

extern char txt_sample_dic[];
extern int  txt_sample_dic_len;

extern char txt_dicwords[];
extern char txt_dicwords_len;

static void
table_count(struct entry *e, void *acc) {
  (*((int *)(acc)))++;
}

static void
test_parsedic_expected_words(CuTest *tc) {
  FILE *dic;
  struct table *t;
  struct entry *e;

  dic = NULL;
  unsigned int acc = 0;

  table_create(&t, 1000L);
  dic = fmemopen(txt_sample_dic, txt_sample_dic_len, "r");
  CuAssertPtrNotNullMsg(tc, "Sample dictionary not found", dic);
  parsedic(dic, t);
  table_each(t, table_count, (void *)(&acc));
  CuAssertIntEquals_Msg(tc, "Expected different number of dictionary entries", acc, 123892);

  /* sampling of words */
  table_get(t, "ZOLP", &e);
  CuAssertStrEquals_Msg(tc, "Expected word 'ZOLP'", "ZOLP", entry_word(e));

  table_get(t, "AARON'S", &e);
  CuAssertStrEquals_Msg(tc, "Expected word \"AARON\'", "AARON'S", entry_word(e));
  fclose(dic);
  table_free(t);
}

static void
test_parsetxt_without_dic(CuTest *tc) {
  FILE *txt;
  unsigned int sents, words, syls;

  sents = words = syls = 0;
  txt = fmemopen(sample_txt, sample_txt_len, "r");
  CuAssertPtrNotNullMsg(tc, "Sample text not found", txt);

  parsetxt(txt, &sents, &words, &syls, NULL);
  printf("%d %d %d\n", sents, words, syls);
  fclose(txt);
}


static void
test_parsetxt_with_dic(CuTest *tc) {
  FILE *txt, *dic;
  struct table *t;
  
  unsigned int sents, words, syls;

  txt = dic = NULL;
  sents = words = syls = 0;

  table_create(&t, 10000L);

  
  txt = fmemopen(sample_txt, sample_txt_len, "r");
  CuAssertPtrNotNullMsg(tc, "Sample text not found", txt);

  dic = fmemopen(txt_sample_dic, txt_sample_dic_len, "r");
  CuAssertPtrNotNullMsg(tc, "Sample dicionary not found", txt);

  parsedic(dic, t);
  parsetxt(txt, &sents, &words, &syls, t);
  printf("%d %d %d\n", sents, words, syls);
  fclose(txt);
  fclose(dic);
  table_free(t);
}


CuSuite *
test_scan_create() {
  CuSuite *suite = CuSuiteNew();
  SUITE_ADD_TEST(suite, test_parsedic_expected_words);
  SUITE_ADD_TEST(suite, test_parsetxt_without_dic);
  SUITE_ADD_TEST(suite, test_parsetxt_with_dic);  
  return suite;
}
