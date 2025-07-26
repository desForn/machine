ascii;
control 0 : Accept 11;
input;
tape * empty always;

0 to 1;         noop;           pmr $;

1 to 1;         scan a;         pmr a;
1 to 1;         scan b;         pmr b;
1 to 2;         eof;            pml $;

2 to 3;         noop;           spml a $;
2 to 4;         noop;           spml b $;
2 to 11;        noop;           see $;

3 to 5;         noop;           sml a;
3 to 5;         noop;           sml b;
3 to 11;        noop;           see $;

4 to 6;         noop;           sml a;
4 to 6;         noop;           sml b;
4 to 11;        noop;           see $;

5 to 5;         noop;           sml a;
5 to 5;         noop;           sml b;
5 to 7;         noop;           smr $;

6 to 6;         noop;           sml a;
6 to 6;         noop;           sml b;
6 to 8;         noop;           smr $;

7 to 9;         noop;           spmr a $;

8 to 9;         noop;           spmr b $;

9 to 10;        noop;           see a;
9 to 10;        noop;           see b;
9 to 11;        noop;           see $;

10 to 10;       noop;           smr a;
10 to 10;       noop;           smr b;
10 to 2;        noop;           sml $;

