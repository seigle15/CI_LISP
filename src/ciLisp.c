#include "ciLisp.h"

void yyerror(char *s) {
    fprintf(stderr, "\nERROR: %s\n", s);
    // note stderr that normally defaults to stdout, but can be redirected: ./src 2> src.log
    // CLion will display stderr in a different color from stdin and stdout
}

// Array of string values for operations.
// Must be in sync with funcs in the OPER_TYPE enum in order for resolveFunc to work.
char *funcNames[] = {
        "neg",
        "abs",
        "exp",
        "sqrt",
        "add",
        "sub",
        "mult",
        "div",
        "remainder",
        "log",
        "pow",
        "max",
        "min",
        "exp2",
        "cbrt",
        "hypot",
        "read",
        "rand",
        "print",
        "equal",
        "less",
        "greater",
        ""
};

OPER_TYPE resolveFunc(char *funcName)
{
    int i = 0;
    while (funcNames[i][0] != '\0')
    {
        if (strcmp(funcNames[i], funcName) == 0)
            return i;
        i++;
    }
    return CUSTOM_OPER;
}

// Called when an INT or DOUBLE token is encountered (see ciLisp.l and ciLisp.y).
// Creates an AST_NODE for the number.
// Sets the AST_NODE's type to number.
// Populates the value of the contained NUMBER_AST_NODE with the argument value.
// SEE: AST_NODE, NUM_AST_NODE, AST_NODE_TYPE.
AST_NODE *createNumberNode(double value, NUM_TYPE type)
{
    AST_NODE *node;
    size_t nodeSize;

    // allocate space for the fixed sie and the variable part (union)
    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL)
        yyerror("Memory allocation failed!");

    node->type = NUM_NODE_TYPE;
    switch (type){
        case INT_TYPE:
            node->data.number.type = INT_TYPE;

            node->data.number.val = floor(value);
            break;
        case DOUBLE_TYPE:
            node->data.number.type = DOUBLE_TYPE;
            node->data.number.val = value;
            break;
        default:
            printf("Invalid NUM_TYPE type!");
            break;
    }

    return node;
}

// Called when an f_expr is created (see ciLisp.y).
// Creates an AST_NODE for a function call.
// Sets the created AST_NODE's type to function.
// Populates the contained FUNC_AST_NODE with:
//      - An OPER_TYPE (the enum identifying the specific function being called)
//      - 2 AST_NODEs, the operands
// SEE: AST_NODE, FUNC_AST_NODE, AST_NODE_TYPE.
AST_NODE *createFunctionNode(char *funcName, AST_NODE *op1, AST_NODE *op2)
{
    AST_NODE *node;
    size_t nodeSize;

    // allocate space (or error)
    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL)
        yyerror("Memory allocation failed!");

    // TODO set the AST_NODE's type, populate contained FUNC_AST_NODE
    // NOTE: you do not need to populate the "ident" field unless the function is type CUSTOM_OPER.
    // When you do have a CUSTOM_OPER, you do NOT need to allocate and strcpy here.
    // The funcName will be a string identifier for which space should be allocated in the tokenizer.
    // For CUSTOM_OPER functions, you should simply assign the "ident" pointer to the passed in funcName.
    // For functions other than CUSTOM_OPER, you should free the funcName after you're assigned the OPER_TYPE.
    node->type = FUNC_NODE_TYPE;
    node->data.function.oper = resolveFunc(funcName);
    node->data.function.op1 = op1;
    node->data.function.op2 = op2;
    evalFuncNode(&node->data.function);

    return node;
}

// Called after execution is done on the base of the tree.
// (see the program production in ciLisp.y)
// Recursively frees the whole abstract syntax tree.
// You'll need to update and expand freeNode as the project develops.
void freeNode(AST_NODE *node)
{
    if (!node)
        return;

    if (node->type == FUNC_NODE_TYPE)
    {
        // Recursive calls to free child nodes
        freeNode(node->data.function.op1);
        freeNode(node->data.function.op2);

        // Free up identifier string if necessary
        if (node->data.function.oper == CUSTOM_OPER)
        {
            free(node->data.function.ident);
        }
    }

    free(node);
}

// Evaluates an AST_NODE.
// returns a RET_VAL storing the the resulting value and type.
// You'll need to update and expand eval (and the more specific eval functions below)
// as the project develops.
RET_VAL eval(AST_NODE *node)
{
    if (!node)
        return (RET_VAL){INT_TYPE, NULL};

    RET_VAL result = {INT_TYPE, NULL}; // see NUM_AST_NODE, because RET_VAL is just an alternative name for it.

    // TODO complete the switch.
    // Make calls to other eval functions based on node type.
    // Use the results of those calls to populate result.
    switch (node->type)
    {
        case NUM_NODE_TYPE:
            node->type = NUM_NODE_TYPE;
            result = evalNumNode(&node->data.number);
            break;
        case FUNC_NODE_TYPE:
            node->type = FUNC_NODE_TYPE;
            result = evalFuncNode(&node->data.function);
            break;
        default:
            yyerror("Invalid AST_NODE_TYPE, probably invalid writes somewhere!");
    }

    return result;
}  

// returns a pointer to the NUM_AST_NODE (aka RET_VAL) referenced by node.
// DOES NOT allocate space for a new RET_VAL.
RET_VAL evalNumNode(NUM_AST_NODE *numNode)
{
    if (!numNode)
        return (RET_VAL){INT_TYPE, NAN};

    RET_VAL result = {INT_TYPE, NAN};

    // SEE: AST_NODE, AST_NODE_TYPE, NUM_AST_NODE
    if(numNode->type == INT_TYPE){
        result.type = INT_TYPE;
        result.data.number.val  = floor(numNode->val);
    }
    else{
        result.type = DOUBLE_TYPE;
        result.data.number.val  = numNode->val;
    }

    return result;
}


RET_VAL evalFuncNode(FUNC_AST_NODE *funcNode)
{
    if (!funcNode)
        return (RET_VAL){NUM_NODE_TYPE, NULL};

    RET_VAL result = {NUM_NODE_TYPE, NULL};

    // TODO populate result with the result of running the function on its operands.
    // SEE: AST_NODE, AST_NODE_TYPE, FUNC_AST_NODE

    RET_VAL op1 = eval(funcNode->op1);
    RET_VAL op2 = eval(funcNode->op2);;
    //op1.data.number.type;

    switch (funcNode->oper){
        case NEG_OPER :
            result = resolveOneOp(op1.data.number.type, -op1.data.number.val );
            break;
        case ABS_OPER:
            result = resolveOneOp(op1.data.number.type, fabs(op1.data.number.val ));
            break;
        case EXP_OPER:
            result = resolveOneOp(op1.data.number.type, exp(op1.data.number.val ));
            break;
        case SQRT_OPER:
            result = resolveOneOp(op1.data.number.type, sqrt(op1.data.number.val ));
            break;
        case SUB_OPER:
            result = resolveTwoOp(op1.data.number.type, op2.data.number.type, op1.data.number.val  - op2.data.number.val );
            break;
        case ADD_OPER:
            result = resolveTwoOp(op1.data.number.type, op2.data.number.type, op1.data.number.val  + op2.data.number.val );
            break;
        case MULT_OPER:
            result = resolveTwoOp(op1.data.number.type, op2.data.number.type, op1.data.number.val  * op2.data.number.val );
            break;
        case DIV_OPER:
            if(op2.data.number.val  != 0) {
                result = resolveTwoOp(op1.data.number.type, op2.data.number.type, op1.data.number.val  / op2.data.number.val );
            }
            break;
        case REMAINDER_OPER:
            if(op2.data.number.val  != 0) {
                result = resolveTwoOp(op1.data.number.type, op2.data.number.type, remainder(op1.data.number.val , op2.data.number.val ));
            }
            break;
        case LOG_OPER:
            result = resolveOneOp(op1.data.number.type, log(op1.data.number.val ));
            break;
        case POW_OPER:
            result = resolveTwoOp(op1.data.number.type, op2.data.number.type,  pow(op1.data.number.val , op2.data.number.val ));
            break;
        case MAX_OPER:
            result = resolveTwoOp(op1.data.number.type, op2.data.number.type,  fmax(op1.data.number.val , op2.data.number.val ));
            break;
        case MIN_OPER:
            result = resolveTwoOp(op1.data.number.type, op2.data.number.type,  fmin(op1.data.number.val , op2.data.number.val ));
            break;
        case EXP2_OPER:
            result = resolveOneOp(op1.data.number.type, exp2(op1.data.number.val ));
            break;
        case CBRT_OPER:
            result = resolveOneOp(op1.data.number.type, cbrt(op1.data.number.val ));
            break;
        case HYPOT_OPER:
            result = resolveTwoOp(op1.data.number.type, op2.data.number.type,  hypot(op1.data.number.val , op2.data.number.val ));
            break;
        default:
            printf("Invalid function or not implemented yet...");
            break;
    }

    return result;
}

// prints the type and value of a RET_VAL
void printRetVal(RET_VAL val)
{

//    switch(data.number.val .type){
//        case SYMBOL_NODE_TYPE:
//            printf("SYMBOL: %s", data.number.val .)
//    }
    if (val.type == INT_TYPE){
        //int printVal =
        printf("INT_TYPE: %.f", round(val.data.number.val ));
    }
    else{
        printf("DOUBLE_TYPE: %f", val.data.number.val );
    }

}


/*TASK 2 functions */

AST_NODE *createSymbolNode(char *ident){
    AST_NODE *node;
    size_t nodeSize;

    // allocate space for the fixed sie and the variable part (union)
    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL)
        yyerror("Memory allocation failed!");

    node->type = SYMBOL_NODE_TYPE;
    node->data.symbol.ident = malloc(sizeof(ident) + 1);
    strcmp(node->data.symbol.ident, ident);


    return node;
}

AST_NODE *linkSymbolTable(SYMBOL_TABLE_NODE *symbolNode, AST_NODE *node){


    return node;
}

SYMBOL_TABLE_NODE *createSymbolTableNode(char *ident, AST_NODE *node){
    SYMBOL_TABLE_NODE *symbolTableNode;
    size_t nodeSize;

    nodeSize = sizeof(SYMBOL_TABLE_NODE);
    if ((symbolTableNode = calloc(nodeSize, 1)) == NULL)
        yyerror("Memory allocation failed!");


    return symbolTableNode;
}

SYMBOL_TABLE_NODE *addToSymbolTable(SYMBOL_TABLE_NODE *head, SYMBOL_TABLE_NODE *newNode){

    return newNode;
}

/*  HELPER FUNCTIONS  */

RET_VAL resolveOneOp(NUM_TYPE type, double val){
    RET_VAL retVal = {NUM_NODE_TYPE, NULL};
    retVal.data.number.type = type;
    retVal.data.number.val  = val;

    return retVal;
}

RET_VAL resolveTwoOp(NUM_TYPE type1, NUM_TYPE type2, double val){
    RET_VAL retVal = {NUM_NODE_TYPE, NULL};
    if(type1 == DOUBLE_TYPE || type2 == DOUBLE_TYPE){
        retVal.type = NUM_NODE_TYPE;
        retVal.data.number.type = DOUBLE_TYPE;
        retVal.data.number.val = val;
    }
    else{
        retVal.type = NUM_NODE_TYPE;
        retVal.data.number.type = INT_TYPE;
        retVal.data.number.val = round(val);
    }

    return retVal;
}