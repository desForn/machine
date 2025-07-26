ascii;

control 0 : Accept 3;
input;
tape * empty always;

0 to 1;         noop;           pmr $;
1 to 1;         scan a;         pmr a;
1 to 1;         scan b;         pmr b;
1 to 2;         scan #;         movel;

2 to 2;         scan a;         sml a;
2 to 2;         scan b;         sml b;
2 to 3;         eof;            see $;
