ascii;
control 0 : Accept 0;
input;
ucounter zero zero;

0 to 1;         scan a;         inc;
0 to 2;         scan b;         inc;

1 to 0;         noop;           zero;
1 to 3;         scan a;         dec;
1 to 1;         scan b;         dec;

2 to 0;         noop;           zero;
2 to 2;         scan a;         dec;
2 to 5;         scan b;         dec;

3 to 4;         noop;           inc;

4 to 1;         noop;           inc;

5 to 6;         noop;           inc;

6 to 2;         noop;           inc;
