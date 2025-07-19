ascii;
control 1 : Accept, 3 : Reject, 4;
input;
stack empty empty;

1 to 1;     scan a;     push a;
1 to 1;     scan b;     push b;
1 to 2;     scan #;     noop;
1 to 4;     eof;        noop;

2 to 2;     scan a;     pop a;
2 to 2;     scan b;     pop b;
2 to 3;     eof;        empty;
2 to 4;     scan a;     pop b;
2 to 4;     scan a;     empty;
2 to 4;     scan b;     pop a;
2 to 4;     scan b;     empty;
2 to 4;     eof;        pop a;
2 to 4;     eof;        pop b;
2 to 4;     scan #;     noop;

3 to 3;     scan a;     noop;
3 to 3;     scan b;     noop;
3 to 3;     scan #;     noop;
3 to 3;     eof;        pop a;
3 to 3;     eof;        pop b;

4 to 4;     scan a;     noop;
4 to 4;     scan b;     noop;
4 to 4;     scan #;     noop;
4 to 4;     eof;        pop a;
4 to 4;     eof;        pop b;

