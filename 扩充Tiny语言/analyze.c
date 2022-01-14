/****************************************************/
/* File: main.c                                     */
/* Main program for TINY compiler                   */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "parse.h"
#include "util.h"
#include <stdio.h>

/* allocate global variables */
int lineno = 0;
FILE * source;
FILE * listing;
FILE * code;

/* allocate and set tracing flags */
int EchoSource = FALSE;
int TraceScan = FALSE;
int TraceParse = TRUE;

int Error = FALSE;

void AnalyzeCode()
{ TreeNode * syntaxTree;
  source = fopen("SourceCode", "r");
  listing = fopen("Result", "w"); /* send listing to screen */
  fprintf(listing,"\nTINY COMPILATION:\n");
  syntaxTree = parse();
  if (TraceParse) {
    fprintf(listing,"\nSyntax tree:\n");
    printTree(syntaxTree);
  }
  fclose(source);
  fclose(listing);
}

