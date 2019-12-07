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

NUM_TYPE resolveType(char *type){
    if(type == NULL){
        return NO_TYPE;
    }
    else if(strcmp(type, "double") == 0){
        return DOUBLE_TYPE;
    }
    else if(strcmp(type, "int") == 0){
        return INT_TYPE;
    }
    return NO_TYPE;
}

//*********************************
// Node Creation Functions
//*********************************

// Called when an INT or DOUBLE token is encountered (see ciLisp.l and ciLisp.y).
// Creates an AST_NODE for the .
// Sets the AST_NODE's type to .
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
AST_NODE *createFunctionNode(char *funcName, AST_NODE *opList)
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
    node->data.function.opList = opList;

    AST_NODE *setParentTemp = opList;
    while (setParentTemp != NULL){
        setParentTemp->parent = node;
        setParentTemp = setParentTemp->next;
    }

    return node;
}

AST_NODE *createSymbolNode(char *ident){
    AST_NODE *node;
    size_t nodeSize;

    // allocate space for the fixed sie and the variable part (union)
    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL)
        yyerror("Memory allocation failed!");

    node->type = SYMBOL_NODE_TYPE;
    node->data.symbol.ident = malloc(sizeof(ident) + 1);
    strcpy(node->data.symbol.ident, ident);

    return node;
}

AST_NODE *createFuncList(AST_NODE *node, AST_NODE *next){
    if(node == NULL){
        return NULL;
    }
    if(next == NULL){
        node->next = NULL;
        return node;
    }
    node->next = next;

    return node;
}

AST_NODE *createConditionNode(AST_NODE *condition, AST_NODE *trueExpr, AST_NODE *falseExpr){
    AST_NODE *node;
    size_t nodeSize;

    // allocate space (or error)
    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL)
        yyerror("Memory allocation failed!");


    node->type = COND_NODE_TYPE;
    node->data.condition.cond = condition;
    condition->parent = node;

    node->data.condition.trueCond = trueExpr;
    trueExpr->parent = node;

    node->data.condition.falseCond = falseExpr;
    falseExpr->parent = node;


    return node;
}

SYMBOL_TABLE_NODE *createSymbolTableNode(char *ident, AST_NODE *node, char *type){
    SYMBOL_TABLE_NODE *symbolTableNode;
    size_t nodeSize;

    nodeSize = sizeof(SYMBOL_TABLE_NODE);
    if ((symbolTableNode = calloc(nodeSize, 1)) == NULL)
        yyerror("Memory allocation failed!");

    symbolTableNode->ident = ident;
    symbolTableNode->val_type = resolveType(type);
    symbolTableNode->val = node;

    return symbolTableNode;
}

//*********************************
// Evaluation Functions
//*********************************

// Evaluates an AST_NODE.
// returns a RET_VAL storing the the resulting value and type.
// You'll need to update and expand eval (and the more specific eval functions below)
// as the project develops.
RET_VAL eval(AST_NODE *node)
{
    if (!node)
        return (RET_VAL){INT_TYPE, NAN};

    RET_VAL result = {INT_TYPE, NAN}; // see NUM_AST_NODE, because RET_VAL is just an alternative name for it.

    // TODO complete the switch.
    // Make calls to other eval functions based on node type.
    // Use the results of those calls to populate result.
    switch (node->type)
    {
        case NUM_NODE_TYPE:
            result = evalNumNode(&node->data.number);
            break;
        case FUNC_NODE_TYPE:
            result = evalFuncNode(&node->data.function);
            break;
        case SYMBOL_NODE_TYPE:
            //printf("Resolving Symbol ");
            result = evalSymbolNode(node);
            break;
        case COND_NODE_TYPE:
            result = evalConditionNode(&node->data.condition);
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
        result.val  = floor(numNode->val);
    }
    else{
        result.type = DOUBLE_TYPE;
        result.val  = numNode->val;
    }

    return result;
}

RET_VAL evalFuncNode(FUNC_AST_NODE *funcNode)
{
    if (!funcNode){
        printf("Function not specified \n");
        return (RET_VAL){INT_TYPE, NAN};
    }
    RET_VAL result = {INT_TYPE, NAN};

    switch (funcNode->oper){
        case NEG_OPER :
            funcNode->opList = resolveOneOp(funcNode->opList);
            result.val = -funcNode->opList->data.number.val;
            result.type = funcNode->opList->data.number.type;
            break;
        case ABS_OPER:
            funcNode->opList = resolveOneOp(funcNode->opList );
            result.val = fabs(funcNode->opList->data.number.val);
            result.type = funcNode->opList->data.number.type;
            break;
        case EXP_OPER:
            funcNode->opList = resolveOneOp(funcNode->opList);
            result.val = exp(funcNode->opList->data.number.val);
            result.type = funcNode->opList->data.number.type;
            break;
        case SQRT_OPER:
            funcNode->opList = resolveOneOp(funcNode->opList );
            result.val = sqrt(funcNode->opList->data.number.val);
            result.type = funcNode->opList->data.number.type;
            break;
        case SUB_OPER:
            funcNode->opList = resolveTwoOp(funcNode->oper, funcNode->opList );
            result.val = funcNode->opList->data.number.val -
                         funcNode->opList->next->data.number.val;
            result.type = funcNode->opList->data.number.type;
            break;
        case ADD_OPER:
            funcNode->opList = resolveMultOp(funcNode->oper, funcNode->opList);
            result = funcNode->opList->data.number;
            break;
        case MULT_OPER:
            funcNode->opList = resolveMultOp(funcNode->oper, funcNode->opList);
            result = funcNode->opList->data.number;
            break;
        case DIV_OPER:
            if(funcNode->opList->next != NULL && funcNode->opList->next->data.number.val != 0) {
                funcNode->opList = resolveTwoOp(funcNode->oper, funcNode->opList);
                result.val = funcNode->opList->data.number.val /
                             funcNode->opList->next->data.number.val;
                result.type = funcNode->opList->data.number.type;
            }
            break;
        case REMAINDER_OPER:
            if(funcNode->opList->next != NULL && funcNode->opList->next->data.number.val != 0) {
                funcNode->opList = resolveTwoOp(funcNode->oper, funcNode->opList);
                result.val = remainder(funcNode->opList->data.number.val ,
                                       funcNode->opList->next->data.number.val);
            }
            result.type = funcNode->opList->data.number.type;
            break;
        case LOG_OPER:
            funcNode->opList = resolveOneOp(funcNode->opList);
            result.val = log(funcNode->opList->data.number.val);
            result.type = funcNode->opList->data.number.type;
            break;
        case POW_OPER:
            funcNode->opList = resolveTwoOp(funcNode->oper, funcNode->opList);
            result.val = pow(funcNode->opList->data.number.val ,
                             funcNode->opList->next->data.number.val);
            result.type = funcNode->opList->data.number.type;
            break;
        case MAX_OPER:
            funcNode->opList = resolveTwoOp(funcNode->oper, funcNode->opList);
            result.val = fmax(funcNode->opList->data.number.val ,
                              funcNode->opList->next->data.number.val);
            result.type = funcNode->opList->data.number.type;
            break;
        case MIN_OPER:
            funcNode->opList = resolveTwoOp(funcNode->oper, funcNode->opList);
            result.val = fmin(funcNode->opList->data.number.val ,
                              funcNode->opList->next->data.number.val);
            result.type = funcNode->opList->data.number.type;
            break;
        case EXP2_OPER:
            funcNode->opList = resolveOneOp(funcNode->opList);
            result.val = exp2(funcNode->opList->data.number.val);
            result.type = funcNode->opList->data.number.type;
            break;
        case CBRT_OPER:
            funcNode->opList = resolveOneOp(funcNode->opList);
            result.val = cbrt(funcNode->opList->data.number.val);
            result.type = funcNode->opList->data.number.type;
            break;
        case HYPOT_OPER:
            funcNode->opList = resolveTwoOp(funcNode->oper, funcNode->opList);
            result.val = hypot(funcNode->opList->data.number.val ,
                               funcNode->opList->next->data.number.val);
            result.type = funcNode->opList->data.number.type;
            break;
        case PRINT_OPER:
            result = printExpr(funcNode->opList);
            break;
        case READ_OPER:
            result = *readVal();
            break;
        default:
            printf("Invalid function or not implemented yet...");
            break;
    }

    return result;
}

RET_VAL evalSymbolNode(AST_NODE *node){
    if(node == NULL){
        return (RET_VAL){INT_TYPE, NAN};
    }
    SYMBOL_TABLE_NODE *val = findSymbol(node->data.symbol.ident, node);

    if(val == NULL){
        return (RET_VAL){INT_TYPE, NAN};
    }
    RET_VAL retVal;
    if(val->val->type != NUM_NODE_TYPE){
        retVal  = eval(val->val);
        retVal = checkType(val->val_type, retVal, val->ident);
        (*val).val->data.number.val = retVal.val;
        (*val).val->data.number.type = retVal.type;
    }
    else{
        retVal.type = val->val_type;
        retVal.val = val->val->data.number.val;
    }

    return retVal;
}

RET_VAL evalConditionNode(COND_AST_NODE *condNode){

    if(eval(condNode->cond).val != 0){
        return eval(condNode->trueCond);
    }
    else{
        return eval(condNode->falseCond);
    }

}

//********************************
// Utility Functions
//********************************

AST_NODE *linkSymbolTable(SYMBOL_TABLE_NODE *symbolNode, AST_NODE *node){

    if(node == NULL){
        printf("Error: invalid or no s_expression\n");
        return node;
    }
    if(symbolNode == NULL){
        printf("Invalid Expression or Symbol\n");
        return node;
    }

    node->table = symbolNode;
    SYMBOL_TABLE_NODE *temp = symbolNode;
    while(temp != NULL){
        temp->val->parent = node;
        temp = temp->next;
    }
    return node;
}

SYMBOL_TABLE_NODE *addToSymbolTable(SYMBOL_TABLE_NODE *head, SYMBOL_TABLE_NODE *newNode){

    if(newNode == NULL){
        return head;
    }
    newNode->next = head;
    return newNode;
}

SYMBOL_TABLE_NODE *findSymbol(char *ident, AST_NODE *s_expr){
    if(s_expr == NULL){
        printf("Symbol Not Declared: %s\n", ident);
        return NULL;
    }
    else if(s_expr->table == NULL && s_expr->parent != NULL){
        s_expr->table = findSymbol(ident, s_expr->parent);
    }

    SYMBOL_TABLE_NODE *node = s_expr->table;

    while (node != NULL && strcmp(ident, node->ident) != 0){
        node = node->next;
    }
    if(node == NULL){
        node = findSymbol(ident, s_expr->parent);
    }
    //node.
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
        freeNode(node->data.function.opList->next);

        // Free up identifier string if necessary
        if (node->data.function.oper == CUSTOM_OPER)
        {
            free(node->data.function.ident);
        }
    }

    free(node);
}

// prints the type and value of a RET_VAL
void printRetVal(RET_VAL val)
{
    if (val.type == INT_TYPE){
        //int printVal =
        printf("INT_TYPE: %.f", round(val.val ));
    }
    else if(val.type == DOUBLE_TYPE){
        printf("DOUBLE_TYPE: %f", val.val );
    }
    else{
        printf("INVALID: %f", val.val );
    }

}

RET_VAL printSymbol(AST_NODE *symASTNode){
    RET_VAL result = {INT_TYPE, NAN};

    SYMBOL_TABLE_NODE *symbol  = findSymbol(symASTNode->data.symbol.ident, symASTNode->parent);
    result = eval(symbol->val);
    result = checkType(symbol->val_type, result, symbol->ident);

    if(symbol->val_type == INT_TYPE){
        printf("Symbol: %s = %.f \n", symbol->ident, result.val);
    }
    else{
        printf("Symbol: %s = %.2f \n", symbol->ident, result.val);
    }

    return result;
}

RET_VAL printExpr(AST_NODE *node){
    if (!node)
        return (RET_VAL){INT_TYPE, NAN};

    RET_VAL result = {INT_TYPE, NAN};
    AST_NODE *iterator = node;
    printf("=> ");

    while (iterator != NULL){

        if (iterator->type == SYMBOL_NODE_TYPE){
            result = printSymbol(iterator);
        }
        else{
            result = eval(iterator);
            if(result.type == INT_TYPE){
                printf("Number: %.f ", result.val);
            }
            else{
                printf("Number: %.2f ", result.val);
            }
        }
        iterator = iterator->next;
    }
    printf("\n");

    return result;
}

RET_VAL *readVal(){
    double input;
    printf("read ::= ");
    scanf("%lf", &input);
    RET_VAL *node;

    size_t nodeSize;

    // allocate space for the fixed sie and the variable part (union)
    nodeSize = sizeof(RET_VAL);
    if ((node = calloc(nodeSize, 1)) == NULL)
        yyerror("Memory allocation failed!");

    //if the entered value includes a dot, then the type of the variable should be set to double
    if(input != (long)input){
        node->type = DOUBLE_TYPE;
        node->val = input;
    }
    else{
        node->type = INT_TYPE;
        node->val = floor(input);
    }

    return node;
}

//*********************************
// HELPER FUNCTIONS
//*********************************

AST_NODE *resolveOneOp(AST_NODE *op){
    RET_VAL retVal = {INT_TYPE, NAN};
    if(op == NULL){
        return NULL;
    }
    if(op->type != NUM_NODE_TYPE){
        retVal = eval(op);
    }
    op->data.number = retVal;
    return op;
}

AST_NODE *resolveTwoOp(OPER_TYPE type, AST_NODE *op){
    RET_VAL retVal = {INT_TYPE, NAN};
    if(op->next == NULL){
        printf("ERROR: too few parameters for the function %s\n", funcNames[type]);
        return NULL;
    }
    if(op->type != NUM_NODE_TYPE){
        retVal = eval(op);
        op->data.number.val = retVal.val;
        op->data.number.type = retVal.type;
    }
    if(op->next->type != NUM_NODE_TYPE){
        retVal = eval(op->next);
        op->next->data.number.type = retVal.type;
        op->next->data.number.val = retVal.val;
    }
    if(op->data.number.type == DOUBLE_TYPE || op->next->data.number.type == DOUBLE_TYPE){
        op->data.number.type = DOUBLE_TYPE;
    }
    else{
        op->data.number.type = INT_TYPE;
    }
    return op;
}

AST_NODE *resolveMultOp(OPER_TYPE type, AST_NODE *opList){
    RET_VAL retVal = {INT_TYPE, NAN};
    if(opList->next == NULL ){
        printf("ERROR: too few parameters for the function %s\n", funcNames[type]);
        return NULL;
    }
    if(opList->type != NUM_NODE_TYPE){
        retVal = eval(opList);
        opList->data.number.val = retVal.val;
        opList->data.number.type = retVal.type;
    }
    AST_NODE *iterator = opList->next;

    if(iterator == NULL){
        printf("ERROR: too few parameters for the function %s\n", funcNames[type]);
        return NULL;
    }

    while (iterator != NULL){
        if(iterator->type != NUM_NODE_TYPE){
            retVal = eval(iterator);
            iterator->data.number.val = retVal.val;
            iterator->data.number.type = retVal.type;
        }
        switch (type){
            case ADD_OPER:
                opList->data.number.val += iterator->data.number.val;
                if(iterator->data.number.type == DOUBLE_TYPE){
                    opList->data.number.type = iterator->data.number.type;
                }
                break;
            case MULT_OPER:
                opList->data.number.val *= iterator->data.number.val;
                if(iterator->data.number.type == DOUBLE_TYPE){
                    opList->data.number.type = iterator->data.number.type;
                }
                break;
            default:
                printf("should not be here\n");

        }
        iterator = iterator->next;
    }

    return opList;
}

RET_VAL checkType(NUM_TYPE givenType, RET_VAL val, char *var){
    if(givenType == DOUBLE_TYPE){
        val.type = DOUBLE_TYPE;
        return val;
    }
    if(givenType == INT_TYPE){
        if(val.val != (long)val.val){
            printf("WARNING: precision loss in the assignment for variable \"%s\"\n", var);
            val.val = round(val.val);
        }
        return val;
    }
    else {
        return val;
    }

}



