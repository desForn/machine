ascii;
control 0 : Accept 33;
input;
ucounter zero zero;
ucounter zero zero;
ucounter zero zero;

//This program is obtained by simulating ../exercice_1_6_2/program_a.m with 3 unsigned counters.
//The first counter holds the state of the first stack in dyadic notation and the second one holds
//similarly the state of the second one. The third counter is an auxiliary one.
//This program is an acceptor while the simulated one is a recogniser. For simplicity, we ignore the
//rejecting state of the original program.

//push a is simulated by doubling the counter and then adding 1.
//push b is simulated by doubling the counter and then adding 2.
//pop a  is simulated by subtracting 1, dividing by 2 and asserting that the remainder is 0.
//pop b  is simulated by subtracting 1, dividing by 2 and asserting that the remainder is 1.
//empty  is simulated by the zero operation.

0 to 3;         scan a;         noop;           noop;           zero;   //Call subroutine 0:
                                                                        //  push a;
                                                                        //  return to 0;
0 to 6;         scan b;         noop;           noop;           zero;   //Call subroutine 1 (push b)
                                                                        //  push b;
                                                                        //  return to 0;
0 to 1;         scan #;         noop;           noop;           zero;

1 to 10;        noop;           noop;           noop;           zero;   //Call subroutine 2:
                                                                        //  if (pop a) return to 15;
                                                                        //  if (pop b) return to 16;
                                                                        //  if (empty) return to 17;

15 to 18;       noop;           noop;           noop;           zero;   //Call subroutine 3:
                                                                        //  push a;
                                                                        //  return to 1;
16 to 21;       noop;           noop;           noop;           zero;   //Call subroutine 4:
                                                                        //  push b;
                                                                        //  return to 1;
17 to 2;        noop;           noop;           noop;           zero;

2 to 25;        noop;           noop;           noop;           zero;   //Call subroutine 5:
                                                                        //  if (pop a) return to 30;
                                                                        //  if (pop b) return to 31;
                                                                        //  if (empty) return to 32;
30 to 2;        scan a;         noop;           noop;           zero;

31 to 2;        scan b;         noop;           noop;           zero;

32 to 33;       eof;            noop;           noop;           zero;

33 to 33;       eof;            dec;            noop;           zero;
33 to 33;       eof;            zero;           dec;            zero;

//BEGIN: subroutine 0
3 to 3;         noop;           dec;            noop;           inc;
3 to 4;         noop;           zero;           noop;           noop;

4 to 5;         noop;           inc;            noop;           dec;
4 to 0;         noop;           inc;            noop;           zero; //return

5 to 4;         noop;           inc;            noop;           noop;
//END: subroutine 0
//BEGIN: subroutine 1
6 to 6;         noop;           dec;            noop;           inc;
6 to 7;         noop;           zero;           noop;           noop;

7 to 8;         noop;           inc;            noop;           dec;
7 to 9;         noop;           inc;            noop;           zero;

8 to 7;         noop;           inc;            noop;           noop;

9 to 0;         noop;           inc;            noop;           zero; //return
//END: subroutine 1
//BEGIN: subroutine 2
10 to 11;       noop;           dec;            noop;           noop;
10 to 17;       noop;           zero;           noop;           noop; // return if (empty)

11 to 12;       noop;           dec;            noop;           noop;
11 to 13;       noop;           zero;           noop;           noop;

12 to 11;       noop;           dec;            noop;           inc;
12 to 14;       noop;           zero;           noop;           noop;

13 to 13;       noop;           inc;            noop;           dec;
13 to 15;       noop;           noop;           noop;           zero; //return if (pop a)

14 to 14;       noop;           inc;            noop;           dec;
14 to 16;       noop;           noop;           noop;           zero; //return if (pop b)
//END: subroutine 2
//BEGIN: subroutine 3
18 to 18;       noop;           noop;           dec;            inc;
18 to 19;       noop;           noop;           zero;           noop;

19 to 20;       noop;           noop;           inc;            dec;
19 to 1;        noop;           noop;           inc;            zero; //return

20 to 19;       noop;           noop;           inc;            noop;
//END: subroutine 3
//BEGIN: subroutine 4
21 to 21;       noop;           noop;           dec;            inc;
21 to 22;       noop;           noop;           zero;           noop;

22 to 23;       noop;           noop;           inc;            dec;
22 to 24;       noop;           noop;           inc;            zero;

23 to 22;       noop;           noop;           inc;            noop;

24 to 1;        noop;           noop;           inc;            zero; //return
//END: subroutine 4
//BEGIN: subroutine 5
25 to 26;       noop;           noop;           dec;            noop;
25 to 32;       noop;           noop;           zero;           noop; // return if (empty)

26 to 27;       noop;           noop;           dec;            noop;
26 to 28;       noop;           noop;           zero;           noop;

27 to 26;       noop;           noop;           dec;            inc;      
27 to 29;       noop;           noop;           zero;           noop;

28 to 28;       noop;           noop;           inc;            dec;
28 to 30;       noop;           noop;           noop;           zero; //return if (pop a)

29 to 29;       noop;           noop;           inc;            dec;
29 to 31;       noop;           noop;           noop;           zero; //return if (pop b)
//END: subroutine 5
