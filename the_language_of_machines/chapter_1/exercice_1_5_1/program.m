ascii;
control 0 : Accept 2 3;
input;
stack empty empty;

0 to 0;         scan a;         push a;
0 to 1;         scan b;         push b;

1 to 1;         scan b;         push b;
1 to 2;         scan a;         pop b;

2 to 2;         scan a;         pop b;
2 to 3;         scan b;         pop a;

3 to 3;         scan b;         pop a;
