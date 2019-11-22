# CI_LISP
Sean Eigle
## Notes about TASK 1: 
everything up to "hypot" function has been implented and is working propertly including the "dot" error in the ciLisp.l file

TASK #1 test run
 (add 5.6 7)
DOUBLE_TYPE: 12.600000
 (sub 6 8)
INT_TYPE: -2
 (mult (add 6 8) (sub 18.5 36.7) )
DOUBLE_TYPE: -254.800000
 (hypot 4 5)
INT_TYPE: 6
 (div 8 0)
INT_TYPE: nan
 quit

Process finished with exit code 0

## Note about TASK 2:

(add ((let (abc 1)) (sub 3 abc)) 4)
 
(mult ((let (a 1) (b 2)) (add a b)) (sqrt 2))
 
(add ((let (a ((let (b 2)) (mult b (sqrt 10))))) (div a 2)) ((let (c 5)) (sqrt c)))

((let (first (sub 5 1)) (second 2)) (add (pow 2 first) (sqrt second)))
INT_TYPE: 6
 
 INT_TYPE: 6
 
 INT_TYPE: 5
 
 INT_TYPE: 5
 (add 4 5)
INT_TYPE: 9
 
