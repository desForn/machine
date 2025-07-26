ascii;
control 0 : Accept 2 : Reject 3;
input;
stack empty empty;
stack empty empty;

0 to 0;         scan a;         push a;         noop; 
0 to 0;         scan b;         push b;         noop;
0 to 1;         scan #;         noop;           noop;
0 to 3;         eof;            noop;           noop;

1 to 1;         noop;           pop a;          push a;
1 to 1;         noop;           pop b;          push b;
1 to 2;         noop;           empty;          noop;

2 to 2;         scan a;         noop;           pop a;
2 to 2;         scan b;         noop;           pop b;
2 to 3;         scan a;         noop;           pop b;
2 to 3;         scan a;         noop;           empty;
2 to 3;         scan b;         noop;           pop a;
2 to 3;         scan b;         noop;           empty;
2 to 3;         eof;            noop;           pop a;
2 to 3;         eof;            noop;           pop b;

3 to 3;         scan a;         noop;           noop;
3 to 3;         scan b;         noop;           noop;
3 to 3;         eof;            pop a;          noop;
3 to 3;         eof;            pop b;          noop;
3 to 3;         eof;            empty;          pop a;
3 to 3;         eof;            empty;          pop b;
