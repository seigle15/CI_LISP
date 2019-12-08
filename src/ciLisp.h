#ifndef __cilisp_h_
#define __cilisp_h_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "ciLispParser.h"

int yyparse(void);

int yylex(void);

void yyerror(char *);

// Enum of all operators.
// must be in sync with funcs in resolveFunc()
typedef enum oper {
    NEG_OPER, // 0
    ABS_OPER,
    EXP_OPER,
    SQRT_OPER,
    ADD_OPER,
    SUB_OPER,
    MULT_OPER,
    DIV_OPER,
    REMAINDER_OPER,
    LOG_OPER,
    POW_OPER,
    MAX_OPER,
    MIN_OPER,
    EXP2_OPER,
    CBRT_OPER,
    HYPOT_OPER,
    READ_OPER,
    RAND_OPER,
    PRINT_OPER,
    EQUAL_OPER,
    LESS_OPER,
    GREATER_OPER,
    CUSTOM_OPER =255
} OPER_TYPE;

OPER_TYPE resolveFunc(char *);

// Types of Abstract Syntax Tree nodes.
// Initially, there are only numbers and functions.
// You will expand this enum as you build the project.
typedef enum {
    NUM_NODE_TYPE,
    FUNC_NODE_TYPE,
    SYMBOL_NODE_TYPE,
    COND_NODE_TYPE
} AST_NODE_TYPE;

// Types of numeric values
typedef enum {
    NO_TYPE,
    INT_TYPE,
    DOUBLE_TYPE
} NUM_TYPE;

// Node to store a number.
typedef struct {
    NUM_TYPE type;
    double val;
} NUM_AST_NODE;

// Node to store a function call with its inputs
typedef struct {
    OPER_TYPE oper;
    char* ident; // only needed for custom functions
    struct ast_node *opList; //now points to a list of operators
} FUNC_AST_NODE;

// Values returned by eval function will be numbers with a type.
// They have the same structure as a NUM_AST_NODE.
// The line below allows us to give this struct another name for readability.
typedef NUM_AST_NODE RET_VAL;


typedef enum { VARIABLE_TYPE, LAMBDA_TYPE, ARG_TYPE } SYMBOL_TYPE;

typedef struct symbol_table_node STACK_NODE;

typedef struct symbol_table_node {
    SYMBOL_TYPE type;
    NUM_TYPE val_type;
    char *ident;
    struct ast_node *val;
    STACK_NODE *stack;
    struct symbol_table_node *next;
} SYMBOL_TABLE_NODE;



typedef struct symbol_ast_node {
    char *ident;
} SYMBOL_AST_NODE;


typedef struct cond_ast_node{
    struct ast_node *cond; // this is the node that checks for non-zero or zero value
    struct ast_node *trueCond; // to eval if cond is nonzero
    struct ast_node *falseCond; // to eval if cond is zero
} COND_AST_NODE;


// Generic Abstract Syntax Tree node. Stores the type of node,
// and reference to the corresponding specific node (initially a number or function call).
typedef struct ast_node {
    AST_NODE_TYPE type;
    SYMBOL_TABLE_NODE  *table;
    struct ast_node *parent;
    struct {
        NUM_AST_NODE number;
        FUNC_AST_NODE function;
        COND_AST_NODE condition;
        SYMBOL_AST_NODE symbol;
    } data;
    struct ast_node *next;
} AST_NODE;

AST_NODE *createNumberNode(double value, NUM_TYPE type);
AST_NODE *createFunctionNode(char *funcName, AST_NODE *opList);
AST_NODE *createSymbolNode(char *ident);
AST_NODE *createFuncList(AST_NODE *node, AST_NODE *next);
AST_NODE *createConditionNode(AST_NODE *condition, AST_NODE *trueExpr, AST_NODE *falseExpr);
SYMBOL_TABLE_NODE *createSymbolTableNode(char *type, AST_NODE *symNode, char *lambda, STACK_NODE *stackNode, AST_NODE *node);
STACK_NODE *createStackNodes(AST_NODE *head, STACK_NODE *next);

RET_VAL eval(AST_NODE *node);
RET_VAL evalNumNode(NUM_AST_NODE *numNode);
RET_VAL evalSymbolNode(AST_NODE *node);
RET_VAL evalFuncNode(FUNC_AST_NODE *funcNode);
RET_VAL evalConditionNode(COND_AST_NODE *condNode);

AST_NODE *linkCustomFunc(AST_NODE *funcName, AST_NODE *funcData);
AST_NODE *linkSymbolTable(SYMBOL_TABLE_NODE *symbolNode, AST_NODE *node);
SYMBOL_TABLE_NODE *addToSymbolTable(SYMBOL_TABLE_NODE *head, SYMBOL_TABLE_NODE *newNode);
SYMBOL_TABLE_NODE *findSymbol(char *ident, AST_NODE *s_expr);
void freeNode(AST_NODE *node);
void printRetVal(RET_VAL val);
NUM_AST_NODE *readVal();
RET_VAL printExpr(AST_NODE *node);


/*  HELPER FUNCTIONS  */
AST_NODE *resolveOneOp(AST_NODE *op);
AST_NODE *resolveTwoOp(OPER_TYPE type, AST_NODE *op);
AST_NODE *resolveMultOp(OPER_TYPE type, AST_NODE *opList);
RET_VAL randVal();
RET_VAL checkType(NUM_TYPE type, RET_VAL val, char *var);
RET_VAL callCustomFunc(FUNC_AST_NODE *func);

#endif
