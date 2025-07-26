ascii;

control 1 : Accept 4;
input;
tape * empty always;

1 to 1;         scan a;         pmr a;
1 to 1;         scan b;         pmr b;
1 to 2;         scan #;         pml #;

2 to 2;         noop;           movel;
2 to 3;         noop;           athome;

3 to 3;         scan a;         smr a;
3 to 3;         scan b;         smr b;
3 to 4;         eof;            see #;
