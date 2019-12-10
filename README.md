# CI_LISP
Sean Eigle
## Notes about TASK 1... 
Everything up to "hypot" function has been implented and is working propertly 

### _Functions Added or Edited_ 
1. createNumberNode: 
* Takes in a double value and a number type 
* creates a pointer to an ast node and assigns the type of ast node to number 
* sets the num ast node to the values past in
* returns the created ast node

2. createFunctionNode: 
* Takes in a function name and 2 operands which are ast nodes
* creates a pointer to an ast node and assigns the type of ast node to function
* calls revalFuncName to get a symbol type for the function name 
* sets the struct func ast node symbol and operands
* returns returns the created ast node

3. evalNumNode:
4. evalFuncNode:

### _Test Values_

> (neg 0)
INT_TYPE: -0
> (neg 5)
INT_TYPE: -5
> (neg -9)
INT_TYPE: 9
> (neg 5 6)
Too many arguments: Taking first val
INT_TYPE: -5
> (abs 5)
INT_TYPE: 5
> (abs -4)
INT_TYPE: 4
> (abs (neg 5))
INT_TYPE: 5
> (exp 5 6)
Too many arguments: Taking first val
INT_TYPE: 148
> (exp 5)
INT_TYPE: 148
> (exp -5)
INT_TYPE: 0
> (exp (add 4 5))
INT_TYPE: 8103
> (sqrt 64)
INT_TYPE: 8
> (sqrt -64)(
INT_TYPE: nan
> (add 5 6)
INT_TYPE: 11
> (add (add 5 6) 4)
INT_TYPE: 15
> (sub 5 6)
INT_TYPE: -1
> (mult  7 8)
INT_TYPE: 56
> (remainder 6.8 5)
DOUBLE_TYPE: 1.80
> (remainder 10 4)
INT_TYPE: 2
> (log 10)
INT_TYPE: 2
> (log 56)
INT_TYPE: 4
> (log 5.7)
DOUBLE_TYPE: 1.74
> (pow 2 4)
INT_TYPE: 16
> (pow (add 25 17) 3)
INT_TYPE: 74088
> (max -1 5)
INT_TYPE: 5
> (min -17 17)
INT_TYPE: -17
> (exp 4 5)
Too many arguments: Taking first val
INT_TYPE: 55
> (exp2 5)
INT_TYPE: 32
> (exp2 3 4)
Too many arguments: Taking first val
INT_TYPE: 8
>  (cbrt 64)
INT_TYPE: 4
> (cbrt)
No arguments given
INT_TYPE: nan
> (add ((let (abc 1)) (sub 3 abc)) 4)
INT_TYPE: 6
> (mult ((let (a 1) (b 2)) (add a b)) (sqrt 2))
NO_TYPE: 4
> (add ((let (a ((let (b 2)) (mult b (sqrt 10))))) (div a 2)) ((let (c 5)) (sqrt c)))
INT_TYPE: 5
> ((let (first (sub 5 1)) (second 2)) (add (pow 2 first) (sqrt second)))
INT_TYPE: 17
> ((let (a ((let (c 3) (d 4)) (mult c d)))) (sqrt a))
NO_TYPE: 3
> ((let (int a 1.25))(add a 1))
WARNING: precision loss in the assignment for variable "a"
INT_TYPE: 2
> ((let (a 0)) (cond (less (rand) 0.5) (add a 1) (sub a 1)))
INT_TYPE: -1
> ((let (myA (read))(myB (rand)))(cond (less myA myB) (print myA) (print myB)))
read ::= 5
=> Symbol: myB = 0.00 
INT_TYPE: 0
> 
> ((let (myA (read))(myB (rand)))(cond (less myA myB) (print myA) (print myB)))
read ::= 1
=> Symbol: myB = 0.00 
INT_TYPE: 0
> 
> ((let (myA (read))(myB (rand)))(cond (less myA myB) (print myA) (print myB)))
read ::= -1
=> Symbol: myA = -1.00 
INT_TYPE: -1
> ((let (double myFunc lambda (x y) (mult (add x 5) (sub y 2)))) (sub (myFunc 3 5) 2))
INT_TYPE: 22
> ((let (f lambda (x y) (add x y)))(f (sub 5 2) (mult 2 3))) 
NO_TYPE: 9
> ((let (int a 1)(f lambda (x y) (add x y)))(f 2 (f a 3)))
NO_TYPE: 5
> 
