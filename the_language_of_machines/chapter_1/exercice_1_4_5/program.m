ascii;
control 0 : Accept 0 : Reject 3;
input;
counter zero zero;

0 to 1;         scan (;         zero;
0 to 1;         scan (;         pos;
0 to 2;         scan );         zero;
0 to 2;         scan );         pos;
0 to 3;         eof;            pos;
0 to 3;         noop;           neg;

1 to 0;         noop;           inc;

2 to 0;         noop;           dec;

3 to 5;         scan (;         neg;
3 to 5;         scan );         neg;
3 to 4;         eof;            pos;

4 to 3;         noop;           dec;

5 to 3;         noop;           inc;
