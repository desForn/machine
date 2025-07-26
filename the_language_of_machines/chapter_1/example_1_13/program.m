ascii;

control 1 : Accept 8;
input;
tape * empty always;
tape * empty always;

1 to 2;         scan a;         pmr a;          noop;
1 to 2;         scan b;         pmr b;          noop;
1 to 3;         eof;            noop;           print $;

2 to 1;         scan a;         pmr a;          pmr #;
2 to 1;         scan b;         pmr b;          pmr #;

3 to 3;         noop;           movel;          noop;
3 to 4;         noop;           athome;         noop;

4 to 4;         noop;           noop;           movel;
4 to 5;         noop;           noop;           athome;

5 to 5;         noop;           smr a;          spmr # a;
5 to 5;         noop;           smr b;          spmr # b;
5 to 6;         noop;           noop;           see $;

6 to 6;         noop;           noop;           movel;
6 to 7;         noop;           noop;           athome;

7 to 7;         noop;           smr a;          smr a;
7 to 7;         noop;           smr b;          smr b;
7 to 8;         noop;           noop;           see $;

