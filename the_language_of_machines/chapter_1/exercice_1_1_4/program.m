control 0 : Accept 9;
input;

0 to 1;         scan a; //
0 to 5;         scan b;

1 to 1;         scan a; //a
1 to 2;         scan b;

2 to 3;         scan a; //ab
2 to 5;         scan b;

3 to 7;         scan a; //aba
3 to 4;         scan b;

4 to 3;         scan a; //abab
4 to 8;         scan b;

5 to 6;         scan a; //b
5 to 5;         scan b;

6 to 7;         scan a; //ba
6 to 2;         scan b;

7 to 8;         scan a; //baa
7 to 2;         scan b;

8 to 8;         scan a; //ababb|baaa
8 to 8;         scan b;
8 to 9;         eof;
