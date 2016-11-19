test rshell.cpp #(True)
test rshell.cpp && test a.out #(True) (True)
test rshell.cpp && test abc.cpp #(True) (False)
echo A && test rshell.cpp #A (True)
test rshell.cpp || echo A #(True)
test rshell.cpp && echo A || test a.out #(True) A
test abc.cpp || test rshell.cpp #(False)
test -e rshell.cpp #(True)
test -d rshell.cpp #(False)
test -f rshell #(True)
[ -e /test/file/path ] && echo A #(True) A
