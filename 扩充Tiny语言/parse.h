/****************************************************/
/* File: parse.h                                    */
/* The parser interface for the TINY compiler       */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _PARSE_H_
#define _PARSE_H_

/* Function parse returns the newly 
 * constructed syntax tree
 */
#include "globals.h"
TreeNode * parse(void);

#endif
