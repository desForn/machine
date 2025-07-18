ascii;
control 0 : Accept, 6;
input;
output;
counter zero zero;

0 to 0;         scan 1;         noop;           inc;
0 to 0;         scan +;         noop;           noop;
0 to 1;         scan -;         noop;           noop;
0 to 2;         eof;            noop;           noop;

1 to 0;         scan +;         noop;           noop;
1 to 1;         scan -;         noop;           noop;
1 to 1;         scan 1;         noop;           dec;
1 to 2;         eof;            noop;           noop;

2 to 3;         noop;           write -;        neg;
2 to 3;         noop;           noop;           pos;
2 to 3;         noop;           noop;           zero;

3 to 4;         noop;           noop;           pos;
3 to 5;         noop;           noop;           neg;
3 to 6;         noop;           noop;           zero;

4 to 3;         noop;           write 1;        dec;

5 to 3;         noop;           write 1;        inc;

