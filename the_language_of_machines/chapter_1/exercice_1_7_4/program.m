ascii;
control 0 : Accept 31;
tape * string always;

// After initialisation the tape is in state x#y
// After state 0 the tape is in state x#y$
0 to 0;         smr 0;
0 to 0;         smr 1;
0 to 0;         smr #;
0 to 1;         spml * $;

// After state 1 the tape is in state x#y$ and athome
1 to 1;         movel;
1 to 2;         athome;

// LOOP BEGIN
// At the beginning of the loop iteration the tape state is x(#*)y($*)z(!+) and athome
// x and y are the remaining summands (maybe empty) where the last n trailing bits have been
// subsituted by # or $. z^R is the sum of the erased bits and the possible ! stands for the carry
// bit.

// If x is empty then the control goes to state 4.
// If the least significant bit of x is 0 then replace it by # and the control goes to state 5.
// If the least significant bit of x is 1 then replace it by # and the control goes to state 6.
2 to 2;         smr 0;
2 to 2;         smr 1;
2 to 4;         as #;
2 to 3;         sml #;

3 to 5;         spmr 0 #;
3 to 6;         spmr 1 #;

// Update z(!+) acording the case. If x is empty then we add a ? at the end.

// If x is empty and there is no carry bit then add after z 0?
// If x is empty and there is carry bit then replace ! by 1?
4 to 4;         smr 0;
4 to 4;         smr 1;
4 to 4;         smr #;
4 to 4;         smr $;
4 to 7;         spmr * 0;
4 to 7;         spmr ! 1;

7 to 9;         pml ?;

// If the least significant bit of x is 0 and there is no carry bit then add 0 to z
// If the least significant bit of x is 0 and there is carry bit then replace ! by 1
5 to 5;         smr 0;
5 to 5;         smr 1;
5 to 5;         smr #;
5 to 5;         smr $;
5 to 9;         sp * 0;
5 to 9;         sp ! 1;

// If the least significant bit of x is 1 and there is no carry bit then add 1 to z
// If the least significant bit of x is 1 and there is carry bit then replace ! by 0!
6 to 6;         smr 0;
6 to 6;         smr 1;
6 to 6;         smr #;
6 to 6;         smr $;
6 to 9;         sp * 1;
6 to 8;         spmr ! 0;

8 to 9;         pml !;

// If y is empty then the control goes to state 11.
// If the least significant bit of y is 0 then replace it by $ and the control goes to state 12.
// If the least significant bit of y is 1 then replace it by $ and the control goes to state 13.
9 to 9;         sml 0;
9 to 9;         sml 1;
9 to 10;        sml $;

10 to 10;       sml $;
10 to 11;       smr #;
10 to 12;       spmr 0 $;
10 to 13;       spmr 1 $;

// If y is empty and z ends with ? then erase it and terminate the loop
// If y is empty and z does not end with ? then restart the loop
11 to 11;       smr 0;
11 to 11;       smr 1;
11 to 11;       smr $;
11 to 11;       smr !;
11 to 19;       sp ? *;
11 to 14;       see *;

14 to 14;       movel;
14 to 2;        athome;

// If the last significant bit of y is 0 then delete ? if present and restart the loop
12 to 12;       smr 0;
12 to 12;       smr 1;
12 to 12;       smr $;
12 to 12;       smr !;
12 to 15;       sp ? *;
12 to 15;       see *;

15 to 15;       movel;
15 to 2;        athome;

// If the last significant bit of y is 1 and z ends with 0 then replace it by 1
// If the last significant bit of y is 1 and z ends with 1 then replace it by 0!
// If the last significant bit of y is 1 and z ends with 0! then replace it by 1!
// In all three cases, erase ? at the end if present
13 to 13;       smr 0;
13 to 13;       smr 1;
13 to 13;       smr $;
13 to 13;       smr !;
13 to 16;       spml ? *;
13 to 16;       sml *;

16 to 16;       sml !;
16 to 18;       sp 0 1;
16 to 17;       spmr 1 0;

17 to 18;       print !;

18 to 18;       movel;
18 to 2;        athome;

// LOOP END
// The string z is stored in reverse at the beginning of the tape replacing the $ as z has at most
// one more significant bit than the larger longest of x or y and there is the separator # there is
// always space

// First look for the last character and remove leading zeroes
19 to 19;       smr 0;
19 to 19;       smr 1;
19 to 19;       smr #;
19 to 19;       smr $;
19 to 20;       sml *;

20 to 20;       spml 0 *;
20 to 21;       sp 1 *;
20 to 30;       see $;

21 to 21;       movel;
21 to 22;       ap 1;

// LOOP BEGIN
// At each iteration of the loop, the last character c of the tape is replaced by the default * and
// the first non # or $ is replaced by c 
22 to 22;       smr 0;
22 to 22;       smr 1;
22 to 22;       smr #;
22 to 22;       smr $;
22 to 23;       sml *;

23 to 24;       sp 0 *;
23 to 25;       sp 1 *;
23 to 28;       see #;
23 to 28;       see $;

24 to 24;       movel;
24 to 26;       athome;

26 to 26;       smr 0;
26 to 26;       smr 1;
26 to 22;       sp # 0;
26 to 22;       sp $ 0;

25 to 25;       movel;
25 to 27;       athome;

27 to 27;       smr 0;
27 to 27;       smr 1;
27 to 22;       sp # 1;
27 to 22;       sp $ 1;
// LOOP END
// Remove trailing $ and # characters
28 to 28;       movel;
28 to 29;       athome;

29 to 29;       smr 0;
29 to 29;       smr 1;
29 to 29;       spmr # *;
29 to 29;       spmr $ *;
29 to 31;       see *;

30 to 30;       movel;
30 to 29;       apmr 0;
