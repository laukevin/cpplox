CPPlox is a c++ version of the java tree walk interpreter in https://craftinginterpreters.com/a-tree-walk-interpreter.html

There's some tricks with casting that were done to get the visitor pattern to work neatly with c++.

This will build the interpreter and put it into interpreter mode

```
mkdir build
cd build
cmake ..
./cpplox
```
