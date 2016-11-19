echo A #&& test rshell.cpp 
test rshell.cpp && echo A #|| echo B
test abc.cpp #; test rshell.cpp
(echo A && echo B) #&& echo C
test -d rshell && (echo A && echo B) #|| test a.out
#test rshell.cpp && test a.out