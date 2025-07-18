ascii;
control 1 : Accept, 3 : Reject, 4;
input;
stack empty string;

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

