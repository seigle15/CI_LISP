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


    if(node->data.function.opList != NULL){
        node->data.function.opList->parent = node;
    }

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
                result = resolveOneOp(funcNode->opList);
                result.val = -funcNode->opList->data.number.val;
                break;
            case ABS_OPER:
                result = resolveOneOp(funcNode->opList );
                result.val = fabs(funcNode->opList->data.number.val);
                break;
            case EXP_OPER:
                result = resolveOneOp(funcNode->opList);
                result.val = exp(funcNode->opList->data.number.val);
                break;
            case SQRT_OPER:
                result = resolveOneOp(funcNode->opList );
                result.val = sqrt(funcNode->opList->data.number.val);
                break;
            case SUB_OPER:
                result = resolveTwoOp(funcNode->oper, funcNode->opList );
                result.val = funcNode->opList->data.number.val -
                             funcNode->opList->next->data.number.val;
                break;
            case ADD_OPER:
                result = resolveMultOp(funcNode->oper, funcNode->opList);
                break;
            case MULT_OPER:
                result = resolveMultOp(funcNode->oper, funcNode->opList);
                break;
            case DIV_OPER:
                if(funcNode->opList->next != NULL && funcNode->opList->next->data.number.val != 0) {
                    result = resolveTwoOp(funcNode->oper, funcNode->opList);
                    result.val = funcNode->opList->data.number.val /
                                 funcNode->opList->next->data.number.val;
                }
                break;
            case REMAINDER_OPER:
                if(funcNode->opList->next != NULL && funcNode->opList->next->data.number.val != 0) {
                    result = resolveTwoOp(funcNode->oper, funcNode->opList);
                    result.val = remainder(funcNode->opList->data.number.val ,
                                           funcNode->opList->next->data.number.val);
                }
                break;
            case LOG_OPER:
                result = resolveOneOp(funcNode->opList);
                result.val = log(funcNode->opList->data.number.val);
                break;
            case POW_OPER:
                result = resolveTwoOp(funcNode->oper, funcNode->opList);
                result.val = pow(funcNode->opList->data.number.val ,
                                 funcNode->opList->next->data.number.val);
                break;
            case MAX_OPER:
                result = resolveTwoOp(funcNode->oper, funcNode->opList);
                result.val = fmax(funcNode->opList->data.number.val ,
                                  funcNode->opList->next->data.number.val);
                break;
            case MIN_OPER:
                result = resolveTwoOp(funcNode->oper, funcNode->opList);
                result.val = fmin(funcNode->opList->data.number.val ,
                                  funcNode->opList->next->data.number.val);
                break;
            case EXP2_OPER:
                result = resolveOneOp(funcNode->opList);
                result.val = exp2(funcNode->opList->data.number.val);
                break;
            case CBRT_OPER:
                result = resolveOneOp(funcNode->opList);
                result.val = cbrt(funcNode->opList->data.number.val);
                break;
            case HYPOT_OPER:
                result = resolveTwoOp(funcNode->oper, funcNode->opList);
                result.val = hypot(funcNode->opList->data.number.val ,
                                   funcNode->opList->next->data.number.val);
                break;
            case PRINT_OPER:
                result = printExpr(funcNode->opList);
                break;
            default:
                printf("Invalid function or not implemented yet...");
                break;
        }

    return result;
}


AST_NODE *addToList(AST_NODE *list){
    RET_VAL retVal = {INT_TYPE, NAN};
    retVal = evalFuncNode(&list->data.function);
    AST_NODE *node = createNumberNode(retVal.val, retVal.type);
    node->next = list->next;
 //   freeNode(list);
    (*list) = *node;
    return list;

}

RET_VAL resolveOneOp(AST_NODE *op){
    RET_VAL retVal = {INT_TYPE, NAN};
    if(op->type == FUNC_NODE_TYPE){
        (*op) = *(addToList(op));
    }
    retVal.type = op->data.number.type;
    return retVal;
}

RET_VAL resolveTwoOp(OPER_TYPE type, AST_NODE *op){
    RET_VAL retVal = {INT_TYPE, NAN};
    if(op->next == NULL){
        printf("ERROR: too few parameters for the function %s\n", funcNames[type]);
        return retVal;
    }
    if(op->type == FUNC_NODE_TYPE){
        (*op) = *(addToList(op));
    }
    if(op->next->type == FUNC_NODE_TYPE){
        (*op->next) = *(addToList(op->next));
    }
    if(op->data.number.type == DOUBLE_TYPE || op->next->data.number.type == DOUBLE_TYPE){
        retVal.type = DOUBLE_TYPE;
    }
    else{
        retVal.type = INT_TYPE;
    }
    return retVal;
}

RET_VAL resolveMultOp(OPER_TYPE type, AST_NODE *opList){
    RET_VAL retVal = {INT_TYPE, NAN};
    if(opList->next == NULL ){
        printf("ERROR: too few parameters for the function %s\n", funcNames[type]);
        return retVal;
    }
    if(type == MULT_OPER){
        retVal.val = 1;
    }
    else{
        retVal.val = 0;
    }
    AST_NODE *iterator = opList;

    while (iterator != NULL){

        if(iterator->type == FUNC_NODE_TYPE){
            (*iterator) = *(addToList(iterator));
        }
        switch (type){
            case ADD_OPER:
                retVal.val += iterator->data.number.val;
                break;
            case MULT_OPER:
                if(retVal.val == NAN){
                    retVal.val = iterator->data.number.val;
                }
                else{
                    retVal.val *= iterator->data.number.val;
                }
                break;
            default:
                printf("should not be here\n");

        }
        iterator = iterator->next;
    }


    return retVal;
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

SYMBOL_TABLE_NODE *findSymbol(char *ident, AST_NODE *s_expr){
    if(s_expr == NULL){
        printf("Symbol Not Declared: %s\n", ident);
        return NULL;
    }
    SYMBOL_TABLE_NODE *node = s_expr->table;

    //if(s_expr.)

    while (node != NULL && strcmp(ident, node->ident) != 0){
        node = node->next;
    }
    if(node == NULL){
        node = findSymbol(ident, s_expr->parent);
    }
    return node;
}

RET_VAL evalSymbolNode(AST_NODE *node){
    if(node == NULL){
        return (RET_VAL){INT_TYPE, NAN};
    }
    SYMBOL_TABLE_NODE *val = findSymbol(node->data.symbol.ident, node);

    if(val == NULL){
        return (RET_VAL){INT_TYPE, NAN};
    }

    return eval(val->val);
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

NUM_TYPE checkType(char *type, AST_NODE *data, char *var){
    if(type != NULL && strcmp(type, "double") == 0){
        data->data.number.type = DOUBLE_TYPE;
        return DOUBLE_TYPE;
    }
    else if (type != NULL && strcmp(type, "int") == 0){
        data->data.number.type = INT_TYPE;
        if(data->data.number.val != (long)data->data.number.val){
            printf("WARNING: precision loss in the assignment for variable %s\n", var);
            data->data.number.val = round(data->data.number.val);
        }
        return INT_TYPE;
    }
    return NO_TYPE;
}

SYMBOL_TABLE_NODE *createSymbolTableNode(char *ident, AST_NODE *node, char *type){
    SYMBOL_TABLE_NODE *symbolTableNode;
    size_t nodeSize;

    nodeSize = sizeof(SYMBOL_TABLE_NODE);
    if ((symbolTableNode = calloc(nodeSize, 1)) == NULL)
        yyerror("Memory allocation failed!");

    symbolTableNode->ident = ident;
    symbolTableNode->val_type = node->data.number.type;
    symbolTableNode->val = node;

    symbolTableNode->val_type = checkType(type, symbolTableNode->val, symbolTableNode->ident);
    return symbolTableNode;
}

SYMBOL_TABLE_NODE *addToSymbolTable(SYMBOL_TABLE_NODE *head, SYMBOL_TABLE_NODE *newNode){

    if(newNode == NULL){
        return head;
    }
    newNode->next = head;
    return newNode;
}


RET_VAL printSymbolTable(SYMBOL_TABLE_NODE *table){
    RET_VAL result = {INT_TYPE, NAN};

    while(table != NULL){
        printf("Symbol: %s = %f ", table->ident, table->val->data.number.val);
        result.type = table->val_type;
        result.val = table->val->data.number.val;
        table = table->next;
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
        switch (iterator->type){
            case NUM_NODE_TYPE:
                printf("Number: %f ", iterator->data.number.val);
                result.type = iterator->data.number.type;
                result.val = iterator->data.number.val;
                break;
            case FUNC_NODE_TYPE:
                (*iterator) = (*addToList(iterator));
                printf("Function %s Evaluation: %f", funcNames[node->data.function.oper], iterator->data.number.val);
                result.type = iterator->data.number.type;
                result.val = iterator->data.number.val;
                break;
            case SYMBOL_NODE_TYPE:
                printSymbolTable(iterator->table);
                break;
        }

        iterator = iterator->next;
    }
    printf("\n");

    return result;
}


/*TASK #5 FUNCTIONS */
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

/*  HELPER FUNCTIONS  */

