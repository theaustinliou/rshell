exit #just exit
echo A && exit #A
echo A || exit #A, testdone
echo A; exit #A
echo A #exit #A, testdone
(echo A || exit) || exit #A, testdone
(echo A || exit) && exit #A
(echo A && exit) || (echo B && exit) #A
(echo A || exit && echo B) || (echo C && exit) #A B C
echo A || exit && (echo B && exit) #A B