ascii;
control 0 : Accept 0 2;
input;
ucounter zero zero;

0 to 1;         scan a;         noop;
0 to 3;         scan b;         noop;

1 to 0;         scan a;         inc;

2 to 3;         scan b;         noop;

3 to 4;         scan b;         noop;

4 to 2;         scan b;         dec;

