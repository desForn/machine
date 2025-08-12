control 0 : Accept 4;
input;
tape * empty always;
tape * empty always;

0 to 1;         scan a;         pmr a;          pmr a;
0 to 1;         scan b;         pmr b;          pmr b;
0 to 4;         eof;            noop;           noop;

1 to 1;         scan a;         pmr a;          pmr a;
1 to 1;         scan b;         pmr b;          pmr b;
1 to 2;         eof;            movel;          movel;

2 to 2;         noop;           movel;          noop;
2 to 3;         noop;           athome;         noop;

3 to 3;         noop;           smr a;          sml a;
3 to 4;         noop;           see a;          as  a;
3 to 3;         noop;           smr b;          sml b;
3 to 4;         noop;           see b;          as  b;
