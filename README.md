# CChess
A simple command line chess program.

Here is how the chess board looks like:
```
  ―――――――――――――――――
8 |♖|♘|♗|♕|♔|♗|♘|♖|
7 |♗|♗|♗|♗|♗|♗|♗|♗|
6 | | | | | | | | |
5 | | | | | | | | |
4 | | | | | | | | |
3 | | | | | | | | |
2 |♟|♟|♟|♟|♟|♟|♟|♟|
1 |♜|♞|♝|♛|♚|♝|♞|♜|
  ―――――――――――――――――
   a b c d e f g h
```

## Installation
There are no pre-compiled binaries as the program is small. You have to compile it yourself.
### Compilation
Make sure ```make``` and a C compiler is installed and run ```make``` in the root directory of the project. A ```cchess``` binary should be generated. Then add the execution permission, if necessary, by running ```chmod +x cchess```.

## Notes
- If it looks too small, you can temporarly make your terminal font size bigger. There isn't much can be done by the program about this. You can only do so much with a command line interface.
- Chess pieces' colors may look wrong depending on your terminal color scheme (especially in light themes).