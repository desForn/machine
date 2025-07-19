ascii;
control 1 : Accept, 3 : Reject, 4;
input;
queue empty empty;

1 to 1;     scan 0;     enqueue 0;
1 to 1;     scan 1;     enqueue 1;
1 to 2;     scan #;     noop;
1 to 4;     eof;        noop;

2 to 2;     scan 0;     dequeue 0;
2 to 2;     scan 1;     dequeue 1;
2 to 3;     eof;        empty;
2 to 4;     scan 0;     dequeue 1;
2 to 4;     scan 0;     empty;
2 to 4;     scan 1;     dequeue 0;
2 to 4;     scan 1;     empty;
2 to 4;     eof;        dequeue 0;
2 to 4;     eof;        dequeue 1;
2 to 4;     scan #;     noop;

3 to 3;     scan 0;     noop;
3 to 3;     scan 1;     noop;
3 to 3;     scan #;     noop;
3 to 3;     eof;        dequeue 0;
3 to 3;     eof;        dequeue 1;

4 to 4;     scan 0;     noop;
4 to 4;     scan 1;     noop;
4 to 4;     scan #;     noop;
4 to 4;     eof;        dequeue 0;
4 to 4;     eof;        dequeue 1;

