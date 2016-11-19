echo A && echo B && echo C #ABC
echo A && (echo B && echo C) #ABC
(echo A && echo B) || (echo C && echo D) #AB
(echo A || echo B) && (echo C && echo D) #ACD
(echo A && echo B || echo C) || (echo D && echo E) #AB
echo A || (echo B && echo C) #A
test rshell.cpp || (test a.out && echo A) #(True)
test -d rshell.cpp && (echo A && echo B) #(False) AB
