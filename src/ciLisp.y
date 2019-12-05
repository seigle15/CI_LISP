%{
    #include "ciLisp.h"
%}

%union {
    double dval;
    char *sval;
    struct ast_node *astNode;
    struct symbol_table_node *symNode;
};

%token <sval> FUNC SYMBOL TYPE
%token <dval> INT DOUBLE
%token LPAREN RPAREN EOL QUIT LET COND


%type <astNode> s_expr_list s_expr symbol f_expr number
%type <symNode> let_elem let_list let_section

%%

program:
    s_expr EOL {
        fprintf(stderr, "yacc: program ::= s_expr EOL\n");
        if ($1) {
            printRetVal(eval($1));
            freeNode($1);
        }
    };

s_expr:
    number {
        fprintf(stderr, "yacc: s_expr ::= number\n");
        $$ = $1;
    }
    | symbol {
        fprintf(stderr, "yacc: symbol ::= SYMBOL\n");
        $$ = $1;
    }
    | LPAREN let_section s_expr RPAREN{
        fprintf(stderr, "yacc: s_expr ::= LET\n");
        $$ = linkSymbolTable($2, $3);
    }
    | f_expr {
        fprintf(stderr, "yacc: f_expr ::= FUNC\n");
        $$ = $1;
    }
    | LPAREN COND s_expr s_expr s_expr RPAREN {

    }
    | QUIT {
        fprintf(stderr, "yacc: s_expr ::= QUIT\n");
        exit(EXIT_SUCCESS);
    }
    | error {
        fprintf(stderr, "yacc: s_expr ::= error\n");
        yyerror("unexpected token");
        $$ = NULL;
    };

let_section:
    /* EMPTY */ {
    fprintf(stderr, "yacc: let_section ::= empty\n");
        $$ = NULL;
    }
    | LPAREN let_list RPAREN {
    fprintf(stderr, "yacc: let_section ::= let_list\n");
        $$ = $2;
    };

let_list:
        LET let_elem {
        fprintf(stderr, "yacc: let_list ::= LET let_elem\n");
          $$ = $2
        }
        | let_list let_elem{
        fprintf(stderr, "yacc: let_elem ::= let_list let_elem\n");
            $$ = addToSymbolTable($1, $2);
        };

let_elem:
        LPAREN SYMBOL s_expr RPAREN{
        fprintf(stderr, "yacc: let_elem ::= SYMBOL s_expr\n");
        $$ = createSymbolTableNode($2, $3, NULL);
        }
        | LPAREN TYPE SYMBOL s_expr RPAREN{
        fprintf(stderr, "yacc: let_elem ::= TYPE SYMBOL s_expr\n");
        $$ = createSymbolTableNode($3, $4, $2);
      };

f_expr:
  LPAREN FUNC s_expr_list RPAREN {
      fprintf(stderr, "yacc: f_expr ::= LPAREN FUNC expr_list RPAREN\n");
      $$ = createFunctionNode($2, $3);
  };


s_expr_list:
    /* EMPTY */ {
    fprintf(stderr, "yacc: s_expr_list ::= s_expr s_expr_list\n");
      $$ = NULL;
    }
    |
    s_expr s_expr_list {
    fprintf(stderr, "yacc: s_expr_list ::= s_expr s_expr_list\n");
    $$ = createFuncList($1, $2);
    }
    |
    s_expr {
    fprintf(stderr, "yacc: s_expr_list ::= s_expr_list\n");
    $$ = createFuncList($1, NULL);
    };

number:
    INT {
        fprintf(stderr, "yacc: number ::= INT\n");
        $$ = createNumberNode($1, INT_TYPE);
    }
    | DOUBLE {
        fprintf(stderr, "yacc: number ::= DOUBLE\n");
        $$ = createNumberNode($1, DOUBLE_TYPE);
    };

symbol:
    SYMBOL {
        fprintf(stderr, "yacc: symbol ::= SYMBOL\n");
        $$ = createSymbolNode($1);
    }


%%

