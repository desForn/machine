control 17 : Halt 18 22;
input;
output;
stack empty empty;
stack empty empty;

//This program with two stacks uses a single string to represent x#y.
//First it constructs w = (w_i)_{i < max(|x|, |y|)} where w_i = x_i + 3 * y_i (adding leading zeros
//if necessary).
//w is stored in the first stack with the least significant digit at the top.
//After reading the input, x and y are stored alongside in the first stack but aligned at the most
//significant digit (that is, the zeros are added as trailing zeros). The loop starting at state 6
//moves the shorter number (if |x| ≠ |y|) such that the numbers are aligned at the least significant
//digit when entering state 0.
//Then the digits are added and pushed at the second stack, with the most significant digit at the
//top. Finally, they are witten at the output.

17 to 3;        scan 1;         noop;           push 1;         noop;
17 to 3;        scan 2;         noop;           push 2;         noop;
17 to 18;       scan #;         noop;           noop;           noop;

18 to 18;       scan 1;         write 1;        noop;           noop;
18 to 18;       scan 2;         write 2;        noop;           noop;

3 to 3;         scan 1;         noop;           push 1;         noop;
3 to 3;         scan 2;         noop;           push 2;         noop;
3 to 4;         scan #;         noop;           noop;           noop;

4 to 4;         noop;           noop;           pop 1;          push 1;
4 to 4;         noop;           noop;           pop 2;          push 2;
4 to 19;        noop;           noop;           empty;          noop;

19 to 20;       eof;            noop;           noop;           noop;
19 to 5;        scan 1;         noop;           push 3;         empty;
19 to 5;        scan 1;         noop;           push 4;         pop 1;
19 to 5;        scan 1;         noop;           push 5;         pop 2;
19 to 5;        scan 2;         noop;           push 6;         empty;
19 to 5;        scan 2;         noop;           push 7;         pop 1;
19 to 5;        scan 2;         noop;           push 8;         pop 2;

20 to 21;       noop;           noop;           pop 1;          push 1;
20 to 21;       noop;           noop;           pop 2;          push 2;
20 to 22;       noop;           noop;           empty;          noop;

5 to 6;         eof;            noop;           noop;           empty;
5 to 5;         eof;            noop;           push 1;         pop 1;
5 to 5;         eof;            noop;           push 2;         pop 2;
5 to 5;         scan 1;         noop;           push 3;         empty;
5 to 5;         scan 1;         noop;           push 4;         pop 1;
5 to 5;         scan 1;         noop;           push 5;         pop 2;
5 to 5;         scan 2;         noop;           push 6;         empty;
5 to 5;         scan 2;         noop;           push 7;         pop 1;
5 to 5;         scan 2;         noop;           push 8;         pop 2;

6 to 7;         noop;           noop;           pop 1;          noop;
6 to 8;         noop;           noop;           pop 2;          noop;
6 to 10;        noop;           noop;           pop 3;          noop;
6 to 13;        noop;           noop;           pop 4;          noop;
6 to 14;        noop;           noop;           pop 5;          noop;
6 to 11;        noop;           noop;           pop 6;          noop;
6 to 15;        noop;           noop;           pop 7;          noop;
6 to 16;        noop;           noop;           pop 8;          noop;

7 to 9;         noop;           noop;           empty;          push 1;
7 to 7;         noop;           noop;           pop 1;          push 1;
7 to 8;         noop;           noop;           pop 2;          push 1;
7 to 7;         noop;           noop;           pop 4;          push 4;
7 to 8;         noop;           noop;           pop 5;          push 4;
7 to 7;         noop;           noop;           pop 7;          push 7;
7 to 8;         noop;           noop;           pop 8;          push 7;

8 to 9;         noop;           noop;           empty;          push 2;
8 to 7;         noop;           noop;           pop 1;          push 2;
8 to 8;         noop;           noop;           pop 2;          push 2;
8 to 7;         noop;           noop;           pop 4;          push 5;
8 to 8;         noop;           noop;           pop 5;          push 5;
8 to 7;         noop;           noop;           pop 7;          push 8;
8 to 8;         noop;           noop;           pop 8;          push 8;

9 to 6;         noop;           noop;           noop;           empty;
9 to 9;         noop;           noop;           push 1;         pop 1;
9 to 9;         noop;           noop;           push 2;         pop 2;
9 to 9;         noop;           noop;           push 3;         pop 3;
9 to 9;         noop;           noop;           push 4;         pop 4;
9 to 9;         noop;           noop;           push 5;         pop 5;
9 to 9;         noop;           noop;           push 6;         pop 6;
9 to 9;         noop;           noop;           push 7;         pop 7;
9 to 9;         noop;           noop;           push 8;         pop 8;

10 to 12;       noop;           noop;           empty;          push 3;
10 to 10;       noop;           noop;           pop 3;          push 3;
10 to 10;       noop;           noop;           pop 4;          push 3;
10 to 10;       noop;           noop;           pop 5;          push 4;
10 to 11;       noop;           noop;           pop 6;          push 4;
10 to 11;       noop;           noop;           pop 7;          push 5;
10 to 11;       noop;           noop;           pop 8;          push 5;

12 to 6;        noop;           noop;           noop;           empty;
12 to 12;       noop;           noop;           push 1;         pop 1;
12 to 12;       noop;           noop;           push 2;         pop 2;
12 to 12;       noop;           noop;           push 3;         pop 3;
12 to 12;       noop;           noop;           push 4;         pop 4;
12 to 12;       noop;           noop;           push 5;         pop 5;
12 to 12;       noop;           noop;           push 6;         pop 6;
12 to 12;       noop;           noop;           push 7;         pop 7;
12 to 12;       noop;           noop;           push 8;         pop 8;

13 to 0;        noop;           noop;           push 4;         noop;

14 to 0;        noop;           noop;           push 5;         noop;

15 to 0;        noop;           noop;           push 7;         noop;

16 to 0;        noop;           noop;           push 8;         noop;

0 to 22;        noop;           noop;           empty;          noop;
0 to 0;         noop;           noop;           pop 1;          push 1;
0 to 0;         noop;           noop;           pop 2;          push 2;
0 to 0;         noop;           noop;           pop 3;          push 1;
0 to 0;         noop;           noop;           pop 4;          push 2;
0 to 1;         noop;           noop;           pop 5;          push 1;
0 to 0;         noop;           noop;           pop 6;          push 2;
0 to 1;         noop;           noop;           pop 7;          push 1;
0 to 1;         noop;           noop;           pop 8;          push 2;

1 to 22;        noop;           noop;           empty;          push 1;
1 to 0;         noop;           noop;           pop 1;          push 2;
1 to 1;         noop;           noop;           pop 2;          push 1;
1 to 0;         noop;           noop;           pop 3;          push 2;
1 to 1;         noop;           noop;           pop 4;          push 1;
1 to 1;         noop;           noop;           pop 5;          push 2;
1 to 1;         noop;           noop;           pop 6;          push 1;
1 to 1;         noop;           noop;           pop 7;          push 2;
1 to 2;         noop;           noop;           pop 8;          push 1;

2 to 22;        noop;           noop;           empty;          push 2;
2 to 1;         noop;           noop;           pop 1;          push 1;
2 to 1;         noop;           noop;           pop 2;          push 2;
2 to 1;         noop;           noop;           pop 3;          push 1;
2 to 1;         noop;           noop;           pop 4;          push 2;
2 to 2;         noop;           noop;           pop 5;          push 1;
2 to 1;         noop;           noop;           pop 6;          push 2;
2 to 2;         noop;           noop;           pop 7;          push 1;
2 to 2;         noop;           noop;           pop 8;          push 2;

22 to 22;       noop;           write 1;        noop;           pop 1;
22 to 22;       noop;           write 2;        noop;           pop 2;
