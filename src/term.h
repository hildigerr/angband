/* File: term.h */

#ifndef INCLUDED_TERM_H
#define INCLUDED_TERM_H

#include "h-include.h"

extern errr Term_flush(void);
extern errr Term_fresh(void);
extern errr Term_bell(void);
extern errr Term_gotoxy(int x, int y);
extern errr Term_clear(void);

#endif


