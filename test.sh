#!/bin/bash

assert() {
	expected="$1"
	input="$2"

	./9cc "$input" >tmp.s
	cc -o tmp tmp.s
	./tmp
	actual="$?"

	if [ "$actual" = "$expected" ]; then
		echo "$input => $actual"
	else
		echo "$input => $expected expected, but got $actual"

		exit 1
	fi
}

assert 0 '0;'
assert 42 '42;'
assert 21 '5+20-4;'
assert 41 ' 12 + 34 - 5 ;'
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
assert 10 '-10+20;'
assert 1 '0==0;'
assert 0 '1+2*3<=1+2+3;'
assert 1 '3>2;'
assert 0 '1<1;'
assert 0 '1>1;'
assert 0 '3>2>1;' # 3>2 が 1 なので 1>1 は 0
assert 3 'a = 3;'
assert 14 'a = 3; b = 5 * 6 - 8; a + b / 2;'
assert 5 'return 5;'
assert 8 'return 8;'
assert 14 'a = 3; b = 5 * 6 - 8; return a + b / 2;'
assert 144 'x = 6; y = x + x; return y * y;'
assert 6 'if (1) 6; else 28;'
assert 2 'if (1) 2;'
assert 0 'if (0) 2;'
assert 0 'a = 5; while (a) a = a - 1; a;'
assert 55 'sum; for (i = 1; i <= 10; i = i + 1) sum = sum + i; sum;'
assert 55 'sum; i = 1; while (i <= 10) { sum = sum + i; i = i + 1; } sum;'
assert 0 'foo();'

echo OK
