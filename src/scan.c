
#include <stdio.h>  /* FILE */

#include "scan.h"
#define WORDSIZE 128
#include <ctype.h> /* toupper */

#define LETTER(l)(((l) >= 'A' && ((l) <= 'Z')) || ((l) == '\''))
#define DIGIT(l) ((l) >= '0' && (l) <= '9')
#define NL(l)    ((l) == '\n')
#define SP(l)    ((l) == ' ')
#define AIOUY(l) ((l) == 'A'  || \
		  (l) == 'I'  || \
		  (l) == 'O'  || \
		  (l) == 'U'  || \
		  (l) == 'Y')
#define E(l)     ((l) == 'E')
#define TERM(l)  ((l) == '.'  || \
		  (l) == '!'  || \
		  (l) == '?')

#define SEP(l)   ((l) == ','  || \
		  (l) == '\0' || \
		  SP(l)       || \
		  NL(l)       || \
		  TERM(l))

#define SYLEDGE(c, state, next) ((state == Txt_AIOUY && next == Txt_Cons) || \
				 (state == Txt_AIOUY && next == Txt_Sep)  || \
				 (state == Txt_E     && next == Txt_Cons) || \
				 (state == Txt_Cons && c == 'M'))

#define WORDEDGE(c, state, next) (state != Txt_Start && \
				  state != Txt_Sep   &&	\
				  next == Txt_Sep)

#define SENTEDGE(c, state, next) (TERM(c) && s != Txt_Sep)


typedef enum DicState {
  Dic_Start,
  Dic_Letter,
  Dic_Sep,
  Dic_Digit,
  Dic_NL,
  Dic_Fail,
  Dic_Count
} DicState;


static DicState
dfa[Dic_Count][Dic_Count] = {
    /*               Dic_Start   Dic_Letter   Dic_Sep   Dic_Digit   Dic_NL    Dic_Fail  */   
    /* Dic_Start  */ {Dic_Fail,  Dic_Letter,  Dic_Fail, Dic_Fail,   Dic_NL,   Dic_Fail},
    /* Dic_Letter */ {Dic_Fail,  Dic_Letter,  Dic_Sep,  Dic_Fail,   Dic_Fail, Dic_Fail},
    /* Dic_Sep    */ {Dic_Fail,  Dic_Fail,    Dic_Sep,  Dic_Digit,  Dic_Fail, Dic_Fail},    
    /* Dic_Digit  */ {Dic_Fail,  Dic_Fail,    Dic_Fail, Dic_Digit,  Dic_NL,   Dic_Fail},    
    /* Dic_NL     */ {Dic_Fail,  Dic_Letter,  Dic_Fail, Dic_Fail,   Dic_NL,   Dic_Fail},
    /* Dic_Fail   */ {Dic_Fail,  Dic_Fail,    Dic_Fail, Dic_Fail,   Dic_Fail, Dic_Fail}
  };

typedef enum TxtState {
  Txt_Start,
  Txt_AIOUY,
  Txt_Cons,
  Txt_E,
  Txt_Sep,
  Txt_Count
} TxtState;


void
parsetxt(FILE *fp,
	unsigned int *sents,
	unsigned int *words,
	unsigned int *syls,
	struct table *dic) {
	       
  TxtState s, snext;
  unsigned int wi, sylcnt;
  char c, wordbuf[WORDSIZE] = {'\0'};
  struct entry *e;
  
  s = Txt_Start;
  wi = sylcnt = 0;
  e = NULL;
  
  while(1) {
    c = toupper(fgetc(fp));
    
    if      (AIOUY(c)) { snext = Txt_AIOUY; }
    else if (E(c))     { snext = Txt_E;     }
    else if (SEP(c))   { snext = Txt_Sep;   }
    else               { snext = Txt_Cons;  }

    if(snext != Txt_Sep) wordbuf[wi++] = c;

    /* side effects */

    if(SYLEDGE(c, s, snext)) {
      sylcnt++;
      }

    if(WORDEDGE(c, s, snext)) {
      (*words)++;  
      wordbuf[wi] ='\0';
      wi = 0;

      /* maybe update syllable estimate */
      if(dic != NULL) {
	table_get(dic, wordbuf, &e);
	if(e != NULL) {
	  sylcnt = entry_value_get(e);
	}
      }
      e = NULL;
      
      *syls = *syls + sylcnt;
      sylcnt = 0;
    }
          
    if(SENTEDGE(c, s, next)) {
      (*sents)++;
    }
    
    if(c == '\0' || c == EOF) break;

    s = snext;
  }

}


void
parsedic(FILE *fp, struct table *dic) {
  DicState s, snext;
  unsigned int wi, sylcnt;
  char c, wordbuf[WORDSIZE] = {'\0'};
  
  s = Dic_Start;
  wi = sylcnt = 0;
 
  while(1) {
    c = toupper(fgetc(fp));

    /* terminating conditions */
    if(c == '\0' || c == EOF || s == Dic_Fail) break;
    
    /* map characters to states */
    if      (LETTER(c)) { snext = Dic_Letter;  }
    else if (DIGIT(c))  { snext = Dic_Digit;   }
    else if (NL(c))     { snext = Dic_NL;      }
    else if (SP(c))     { snext = Dic_Sep;     } // changed from SEP
    else                { snext = Dic_Fail;    }

    /* side effects */
    if(snext == Dic_Letter && wi < (WORDSIZE - 1)) {
      wordbuf[wi++] = c;
    }
    else if (s == Dic_Letter && snext == Dic_Sep) {
      wordbuf[wi] = '\0';
      wi = 0;
    }
    else if(snext == Dic_Digit) {
      sylcnt = (sylcnt * 10) + (c - '0');
    }
    else if(s == Dic_Digit && snext == Dic_NL) {
      table_add(dic, wordbuf, sylcnt);
      wordbuf[0] = '\0';
      sylcnt = wi = 0;
    }
    
    /* update state */
    s = dfa[s][snext];
  }
}
