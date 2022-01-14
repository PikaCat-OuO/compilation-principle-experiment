/****************************************************/
/* File: parse.c                                    */
/* The parser implementation for the TINY compiler  */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

static TokenType token; /* holds current token */

/* function prototypes for recursive calls */
static TreeNode * stmt_sequence(void);
static TreeNode * statement(void);
static TreeNode * if_stmt(void);
static TreeNode * repeat_stmt(void);
static TreeNode * dowhile_stmt(void);
static TreeNode * for_stmt(void);
static TreeNode * assign_stmt(void);
static TreeNode * read_stmt(void);
static TreeNode * write_stmt(void);
static TreeNode * exp(void);
static TreeNode * simple_exp(void);
static TreeNode * term(void);
static TreeNode * power(void);
static TreeNode * factor(void);
static TreeNode * condition(void);
static TreeNode * orterm(void);
static TreeNode * andterm(void);
static TreeNode * notterm(void);
static TreeNode * regex(void);
static TreeNode * reorterm(void);
static TreeNode * reandterm(void);
static TreeNode * recloterm(void);

static void syntaxError(char * message)
{ fprintf(listing,"\n>>> ");
  fprintf(listing,"Syntax error at line %d: %s",lineno,message);
  Error = TRUE;
}

static void match(TokenType expected)
{ if (token == expected) token = getToken();
  else {
    syntaxError("unexpected token -> ");
    printToken(token,tokenString);
    fprintf(listing,"      ");
  }
}

TreeNode * stmt_sequence(void)
{ TreeNode * t = statement();
  TreeNode * p = t;
  // statement sequence's follow set
  while ((token!=ENDFILE) && (token!=END) &&
         (token!=ELSE) && (token!=UNTIL) &&
         (token!=WHILE) && (token!=ENDDO))
  { TreeNode * q;
    match(SEMI);
    q = statement();
    if (q!=NULL) {
      if (t==NULL) t = p = q;
      else /* now p cannot be NULL either */
      { p->sibling = q;
        p = q;
      }
    }
  }
  return t;
}

//P394 
//lineno: 961
TreeNode * statement(void)
{ TreeNode * t = NULL;
  switch (token) {
    case IF : t = if_stmt(); break;
    case REPEAT : t = repeat_stmt(); break;
    case DO: t = dowhile_stmt(); break;
    case FOR: t = for_stmt(); break;
    case ID : t = assign_stmt(); break;
    case READ : t = read_stmt(); break;
    case WRITE : t = write_stmt(); break;
    default : syntaxError("unexpected token -> ");
              printToken(token,tokenString);
              token = getToken();
              break;
  } /* end case */
  return t;
}


//P394 
//lineno: 977
TreeNode * if_stmt(void)
{ TreeNode * t = newStmtNode(IfK);
  match(IF);
  if (t!=NULL) t->child[0] = condition();
  match(THEN);
  if (t!=NULL) t->child[1] = stmt_sequence();
  if (token==ELSE) {
    match(ELSE);
    if (t!=NULL) t->child[2] = stmt_sequence();
  }
  match(END);
  return t;
}

//P394 
//lineno:991
TreeNode * repeat_stmt(void)
{ TreeNode * t = newStmtNode(RepeatK);
  match(REPEAT);
  if (t!=NULL) t->child[0] = stmt_sequence();
  match(UNTIL);
  if (t!=NULL) t->child[1] = condition();
  return t;
}

TreeNode *dowhile_stmt(void)
{ TreeNode * t = newStmtNode(DoWhileK);
  match(DO);
  if (t!=NULL) t->child[0] = stmt_sequence();
  match(WHILE);
  if (t!=NULL) t->child[1] = condition();
  return t;
}

TreeNode *for_stmt(void)
{ TreeNode * p = newStmtNode(AssignK);
  match(FOR);
  if (p!=NULL && token==ID) p->attr.name = copyString(tokenString);
  match(ID);
  match(ASSIGN);
  if (p!=NULL) p->child[0] = simple_exp();
  TreeNode * t = NULL;
  if (token==TO) {
    t = newStmtNode(ForToK);
    t->child[0] = p;
    match(TO);
    t->child[1] = simple_exp();
    match(DO);
    t->child[2] = stmt_sequence();
    match(ENDDO);
  } else {
    t = newStmtNode(ForDowntoK);
    t->child[0] = p;
    match(DOWNTO);
    t->child[1] = simple_exp();
    match(DO);
    t->child[2] = stmt_sequence();
    match(ENDDO);
  }
  return t;
}

TreeNode * assign_stmt(void)
{ TreeNode * t = newStmtNode(AssignK);
  if (t!=NULL && token==ID) t->attr.name = copyString(tokenString);
  match(ID);
  TreeNode * p = newExpNode(OpK);
  if (t!=NULL) t->child[0] = p;
  if (t!=NULL && p!=NULL && (token==ASSIGN || token==MINUSASSIGN)) {
    p->attr.op = token;
    match(token);
    t->child[1] = exp();
  } else if (t!=NULL && p!=NULL) {
    p->attr.op = token;
    match(REASSIGN);
    t->child[1] = regex();
  }
  return t;
}

TreeNode * read_stmt(void)
{ TreeNode * t = newStmtNode(ReadK);
  match(READ);
  if ((t!=NULL) && (token==ID)) t->attr.name = copyString(tokenString);
  match(ID);
  return t;
}

TreeNode * write_stmt(void)
{ TreeNode * t = newStmtNode(WriteK);
  match(WRITE);
  if (t!=NULL) t->child[0] = exp();
  return t;
}

TreeNode * exp(void)
{ TreeNode * t = simple_exp();
  if (token==LTEQ || token==GTEQ || token==LT || token==GT || token==NOTEQ || token==EQ) {
    TreeNode * p = newExpNode(OpK);
    if (p!=NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
    }
    match(token);
    if (t!=NULL) t->child[1] = simple_exp();
  }
  return t;
}

TreeNode * simple_exp(void)
{ TreeNode * t = term();
  while ((token==PLUS)||(token==MINUS))
  { TreeNode * p = newExpNode(OpK);
    if (p!=NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token);
      t->child[1] = term();
    }
  }
  return t;
}

TreeNode * term(void)
{ TreeNode * t = power();
  while (token==TIMES || token==OVER || token==MOD)
  { TreeNode * p = newExpNode(OpK);
    if (p!=NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token);
      p->child[1] = power();
    }
  }
  return t;
}

TreeNode * power(void)
{ TreeNode * t = factor();
  while (token==POWER)
  { TreeNode * p = newExpNode(OpK);
    if (p!=NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token);
      p->child[1] = factor();
    }
  }
  return t;
}

TreeNode * factor(void)
{ TreeNode * t = NULL;
  switch (token) {
    case NUM :
      t = newExpNode(ConstK);
      if ((t!=NULL) && (token==NUM))
        t->attr.val = atoi(tokenString);
      match(NUM);
      break;
    case ID :
      t = newExpNode(IdK);
      if ((t!=NULL) && (token==ID))
        t->attr.name = copyString(tokenString);
      match(ID);
      break;
    case LPAREN :
      match(LPAREN);
      t = exp();
      match(RPAREN);
      break;
    default:
      syntaxError("unexpected token -> ");
      printToken(token,tokenString);
      token = getToken();
      break;
    }
  return t;
}

TreeNode *condition(void) {
  TreeNode *t = orterm();
  while (token==OR) {
    TreeNode *p = newExpNode(OpK);
    if (p!=NULL) {
      p->child[0] = t;
      p->attr.op = token;
      match(token);
      p->child[1] = orterm();
      t = p;
    }
  }
  return t;
}

TreeNode *orterm(void) {
  TreeNode *t = andterm();
  while (token==AND) {
    TreeNode *p = newExpNode(OpK);
    if (p!=NULL) {
      p->child[0] = t;
      p->attr.op = token;
      match(token);
      p->child[1] = andterm();
      t = p;
    }
  }
  return t;
}

TreeNode *andterm(void) {
  TreeNode *t = NULL;
  if (token==NOT) {
    t = newExpNode(OpK);
    t->attr.op = token;
    match(NOT);
    t->child[0] = notterm();
  } else t = notterm();
  return t;
}

TreeNode *notterm(void)
{ TreeNode * t = NULL;
  if (token==LPAREN) {
    match(LPAREN);
    t = condition();
    match(RPAREN);
  } else t = exp();
  return t;
}

TreeNode *regex(void) {
  TreeNode *t = reorterm();
  while (token==REOR) {
    TreeNode *p = newExpNode(OpK);
    if (p!=NULL) {
      p->child[0] = t;
      p->attr.op = token;
      match(token);
      p->child[1] = reorterm();
      t = p;
    }
  }
  return t;
}

TreeNode *reorterm(void) {
  TreeNode *t = reandterm();
  while (token==REAND) {
    TreeNode *p = newExpNode(OpK);
    if (p!=NULL) {
      p->child[0] = t;
      p->attr.op = token;
      match(token);
      p->child[1] = reandterm();
      t = p;
    }
  }
  return t;
}

TreeNode *reandterm(void) {
  TreeNode *t = recloterm();
  if (t!=NULL && token==RECLOSURE) {
    TreeNode *p = newExpNode(OpK);
    p->child[0] = t;
    p->attr.op = token;
    match(RECLOSURE);
    t = p;
  }
  return t;
}

TreeNode *recloterm(void)
{ TreeNode * t = NULL;
  switch (token) {
  case ID :
    t = newExpNode(IdK);
    if ((t!=NULL) && (token==ID))
      t->attr.name = copyString(tokenString);
    match(ID);
    break;
  case LPAREN :
    match(LPAREN);
    t = regex();
    match(RPAREN);
    break;
  default:
    syntaxError("unexpected token -> ");
    printToken(token,tokenString);
    token = getToken();
    break;
  }
  return t;
}

/****************************************/
/* the primary function of the parser   */
/****************************************/
/* Function parse returns the newly 
 * constructed syntax tree
 */
TreeNode * parse(void)
{ TreeNode * t;
  token = getToken();
  t = stmt_sequence();
  if (token!=ENDFILE)
    syntaxError("Code ends before file\n");
  return t;
}
