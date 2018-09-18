
#ifndef SCANH_INCLUDED
#define SCANH_INCLUDED

#include "table.h"

void
parsedic(FILE *fp,
	 struct table *dic);

void
parsetxt(FILE *fp,
	 unsigned int *sents,
	 unsigned int *words,
	 unsigned int *syls,
	 struct table *dic);

#endif
