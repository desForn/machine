ascii;
control 0 : Accept, 9;
input;
output;
stack empty empty;

0 to 0;         scan +;         noop;           noop;
0 to 0;         scan 1;         noop;           pop -;
0 to 1;         scan -;         noop;           noop;
0 to 2;         scan 1;         noop;           empty;
0 to 3;         scan 1;         noop;           pop +;
0 to 6;         eof;            noop;           noop;

1 to 1;         scan -;         noop;           noop;
1 to 1;         scan 1;         noop;           pop +;
1 to 0;         scan +;         noop;           noop;
1 to 4;         scan 1;         noop;           empty;
1 to 5;         scan 1;         noop;           pop -;
1 to 6;         eof;            noop;           noop;

2 to 0;         noop;           noop;           push +;

3 to 2;         noop;           noop;           push +;

4 to 1;         noop;           noop;           push -;

5 to 4;         noop;           noop;           push -;

6 to 6;         noop;           write 1;        pop +;
6 to 7;         noop;           write -;        pop -;
6 to 9;         noop;           noop;           empty;

7 to 8;         noop;           write 1;        noop;

8 to 8;         noop;           write 1;        pop -;
8 to 9;         noop;           noop;           empty;

