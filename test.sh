#!/bin/bash

assert() {
	expected="$1"
	input="$2"

	./9cc "$input" >tmp.s
	cc -c foo.c -o foo.o
	cc -o tmp tmp.s foo.o
	./tmp
	actual="$?"

	if [ "$actual" = "$expected" ]; then
		echo "$input => $actual"
	else
		echo "$input => $expected expected, but got $actual"

		exit 1
	fi
}

assert 0 'main() { return 0; }'
assert 42 'main() { return 42; }'
assert 21 'main() { return 5+20-4; }'
assert 41 'main() { return  12 + 34 - 5 ; }'
assert 47 'main() { return 5+6*7; }'
assert 15 'main() { return 5*(9-6); }'
assert 4 'main() { return (3+5)/2; }'
assert 10 'main() { return -10+20; }'
assert 1 'main() { return 0==0; }'
assert 0 'main() { return 1+2*3<=1+2+3; }'
assert 1 'main() { return 3>2; }'
assert 0 'main() { return 1<1; }'
assert 0 'main() { return 1>1; }'
assert 0 'main() { return 3>2>1; }' # 3>2 が 1 なので 1>1 は 0
assert 3 'main() { return a = 3; }'
assert 14 'main() { a = 3; b = 5 * 6 - 8; return a + b / 2; }'
assert 5 'main() { return 5; }'
assert 8 'main() { return 8; }'
assert 14 'main() { a = 3; b = 5 * 6 - 8; return a + b / 2; }'
assert 144 'main() { x = 6; y = x + x; return y * y; }'
assert 6 'main() { if (1) return 6; else return 28; }'
assert 2 'main() { if (1) return 2; }'
assert 0 'main() { if (0) return 2; }'
assert 0 'main() { a = 5; while (a) a = a - 1; return a; }'
assert 55 'main() { sum; for (i = 1; i <= 10; i = i + 1) sum = sum + i; return sum; }'
assert 55 'main() { sum; i = 1; while (i <= 10) { sum = sum + i; i = i + 1; } return sum; }'
assert 0 'main() { return foo(); }'
assert 0 'main() { return bar(3, 4); }'
assert 21 'main() { return baz(1, 2, 3, 4, 5, 6); }'
assert 12 'main() { return mul(3, 4); } mul(x, y) { return x * y; }'

echo OK
