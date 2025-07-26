ascii;
control 3 : Halt 5;
input;
output;
stack empty empty;
stack empty empty;

3 to 3;         scan 1;         noop;           push 1;         noop;
3 to 3;         scan 2;         noop;           push 2;         noop;
3 to 4;         scan #;         noop;           noop;           noop;

4 to 4;         scan 1;         noop;           noop;           push 1;
4 to 4;         scan 2;         noop;           noop;           push 2;
4 to 0;         eof;            noop;           noop;           noop;

0 to 5;         noop;           noop;           empty;          empty;
0 to 0;         noop;           write 1;        empty;          pop 1;
0 to 0;         noop;           write 2;        empty;          pop 2;
0 to 0;         noop;           write 1;        pop 1;          empty;
0 to 0;         noop;           write 2;        pop 1;          pop 1;
0 to 1;         noop;           write 1;        pop 1;          pop 2;
0 to 0;         noop;           write 2;        pop 2;          empty;
0 to 1;         noop;           write 1;        pop 2;          pop 1;
0 to 1;         noop;           write 2;        pop 2;          pop 2;

1 to 5;         noop;           write 1;        empty;          empty;
1 to 0;         noop;           write 2;        empty;          pop 1;
1 to 1;         noop;           write 1;        empty;          pop 2;
1 to 0;         noop;           write 2;        pop 1;          empty;
1 to 1;         noop;           write 1;        pop 1;          pop 1;
1 to 1;         noop;           write 2;        pop 1;          pop 2;
1 to 1;         noop;           write 1;        pop 2;          empty;
1 to 1;         noop;           write 2;        pop 2;          pop 1;
1 to 2;         noop;           write 1;        pop 2;          pop 2;

2 to 5;         noop;           write 2;        empty;          empty;
2 to 1;         noop;           write 1;        empty;          pop 1;
2 to 1;         noop;           write 2;        empty;          pop 2;
2 to 1;         noop;           write 1;        pop 1;          empty;
2 to 1;         noop;           write 2;        pop 1;          pop 1;
2 to 2;         noop;           write 1;        pop 1;          pop 2;
2 to 1;         noop;           write 2;        pop 2;          empty;
2 to 2;         noop;           write 1;        pop 2;          pop 1;
2 to 2;         noop;           write 2;        pop 2;          pop 2;
