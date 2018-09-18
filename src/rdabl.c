
#include <stdio.h>    /* fprintf */
#include <stdlib.h>   /* exit    */
#include <unistd.h>   /* getopt  */
#include <sys/stat.h> /* stat    */

#include "table.h"
#include "scan.h"



#define MAX(a,b) (((a)>(b))?(a):(b))



static void
usage(const char *);


static void
prscore(FILE *fp,
	unsigned int sents,
	unsigned int words,
	unsigned int syls);



static void
usage(const char *prog) {
  fprintf(stderr, "Usage: %s [-d dic] [text]\n", prog);
}


static void
prscore(FILE *fp,
	unsigned int sents,
	unsigned int words,
	unsigned int syls) {
  
  float score = 206.835 -	   \
    1.015 * (words/(float)MAX(sents,1)) -	\
    84.6 * (syls/(float)MAX(words,1));

  const char *fmt = "%d %d %d %.2f\n%s\n";
      
  if      (score >= 90.0)
    fprintf(fp, fmt, sents, words, syls, score,			\
	    "Very easy to read. Easily understood by an average 11-year-old student.");
  else if (score >= 80.0)
    fprintf(fp, fmt, sents, words, syls, score,			\
	    "Easy to read. Conversational English for consumers.");
  else if (score >= 70.0)
    fprintf(fp, fmt, sents, words, syls, score,			\
	    "Fairly easy to read.");
  else if (score >= 60.0)
    fprintf(fp, fmt, sents, words, syls, score, 		\
	    "Plain English. Easily understood by 13- to 15-year-old students.");
  else if (score >= 50.0)
    fprintf(fp, fmt, sents, words, syls, score,			\
	    "Fairly difficult to read.");
  else if (score >= 30.0)
    fprintf(fp, fmt, sents, words, syls, score,			\
	    "Difficult to read.");
  else if (score >= 0.0)
    fprintf(fp, fmt, sents, words, syls, score,			\
	    "Very difficult to read. Best understood by university graduates.");
  else
    fprintf(fp, fmt, sents, words, syls, score,			\
	    "Score outside classifiable range. I don't even know how to begin reading this.");        
}


int
main(int argc, char *argv[]) {
  int opt;
  FILE *df, *cf;
  struct stat st;
  struct table *dic, *corpus;
  tablestatus ds, cs;
  unsigned int sents, words, syls;


  sents = words = syls = 0;
  df = NULL, cf = stdin;
  dic = NULL, corpus = NULL;
  ds = FAIL_UNKNOWN, cs = FAIL_UNKNOWN;
  
   
  while((opt = getopt(argc, argv, "d:h")) != -1) {
    switch (opt) {
    case 'd':

      if((df = fopen(optarg, "r")) == NULL) {
	fprintf(stderr, "%s: Failed to open dictionary file '%s'.\n", argv[0], optarg);
	goto bye;
      }

      stat(optarg, &st);
      if((ds = table_create(&dic, st.st_size >> 3)) != SUCCESS) {
	fprintf(stderr, "%s: Unable to set up table for dictionary. \
                        This could mean insufficient memory...or a bug.\n", argv[0]);		
	goto bye;
      }

      parsedic(df, dic);
      break;

    case 'h':
      usage(argv[0]);
      goto bye;
      break;

    default:
      goto bye;
    }
  }


  if((optind < argc) && (cf = fopen(argv[optind], "r")) == NULL) {
      fprintf(stderr, "%s: Failed to open corpus file '%s'.\n", argv[0], argv[optind]);
      goto bye;
  }

  stat(argv[optind], &st);
  if((cs = table_create(&corpus, st.st_size >> 3)) != SUCCESS) {
      fprintf(stderr, "%s: Unable to set up table for corpus. \
                       This could mean insufficient memory...or a bug.", argv[0]);
      goto bye;
  }

  
  parsetxt(cf, &sents, &words, &syls, dic);
  
  prscore(stdout, sents, words, syls);
  
 bye:  
  if(df)     { fclose(df); }
  if(cf)     { fclose(cf); }
  if(dic)    { table_free(dic); }
  if(corpus) { table_free(corpus); }
  exit(((ds == SUCCESS)  && (cs == SUCCESS)) ?
       EXIT_SUCCESS : EXIT_FAILURE);
  
}
