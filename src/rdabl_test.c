
#include <stdio.h>
#include "CuTest.h"





extern CuSuite *test_table_create(void);
extern CuSuite *test_scan_create(void);

void testall(void) {
  CuString *output = CuStringNew();
  CuSuite* suite = CuSuiteNew();

  CuSuiteAddSuite(suite, test_table_create());
  CuSuiteAddSuite(suite, test_scan_create());
  
  CuSuiteRun(suite);
  CuSuiteSummary(suite, output);
  CuSuiteDetails(suite, output);

  printf("%s\n", output->buffer);

  CuStringDelete(output);
  CuSuiteDelete(suite);
}


int
main(int argc, char *argv[]) {
  testall();
}
