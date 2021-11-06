#!/bin/bash

assert() {
	expected="$1"
	input="$2"

	./9cc "$input" >tmp.s
	cc -c test_helpers.c -o test_helpers.o
	cc -o tmp tmp.s test_helpers.o
	./tmp
	actual="$?"

	if [ "$actual" = "$expected" ]; then
		echo "$input => $actual"
	else
		echo "$input => $expected expected, but got $actual"

		exit 1
	fi
}

assert 0 'int main() { return 0; }'
assert 42 'int main() { return 42; }'
assert 21 'int main() { return 5+20-4; }'
assert 41 'int main() { return  12 + 34 - 5 ; }'
assert 47 'int main() { return 5+6*7; }'
assert 15 'int main() { return 5*(9-6); }'
assert 4 'int main() { return (3+5)/2; }'
assert 10 'int main() { return -10+20; }'
assert 1 'int main() { return 0==0; }'
assert 0 'int main() { return 1+2*3<=1+2+3; }'
assert 1 'int main() { return 3>2; }'
assert 0 'int main() { return 1<1; }'
assert 0 'int main() { return 1>1; }'
assert 0 'int main() { return 3>2>1; }' # 3>2 が 1 なので 1>1 は 0
assert 3 'int main() { int a; return a = 3; }'
assert 14 'int main() { int a; int b; a = 3; b = 5 * 6 - 8; return a + b / 2; }'
assert 5 'int main() { return 5; }'
assert 8 'int main() { return 8; }'
assert 14 'int main() { int a; int b; a = 3; b = 5 * 6 - 8; return a + b / 2; }'
assert 144 'int main() { int x; int y; x = 6; y = x + x; return y * y; }'
assert 6 'int main() { if (1) return 6; else return 28; }'
assert 2 'int main() { if (1) return 2; }'
assert 0 'int main() { if (0) return 2; }'
assert 0 'int main() { int a; a = 5; while (a) a = a - 1; return a; }'
assert 55 'int main() { int sum; int i; for (i = 1; i <= 10; i = i + 1) sum = sum + i; return sum; }'
assert 55 'int main() { int sum; int i; i = 1; while (i <= 10) { sum = sum + i; i = i + 1; } return sum; }'
assert 0 'int main() { return foo(); }'
assert 0 'int main() { return bar(3, 4); }'
assert 21 'int main() { return baz(1, 2, 3, 4, 5, 6); }'
assert 12 'int main() { return mul(3, 4); } int mul(int x, int y) { return x * y; }'
assert 0 'int main() { return fib(0); } int fib(int n) { if (n == 0) return 0; if (n == 1) return 1; return fib(n - 2) + fib(n - 1); }'
assert 1 'int main() { return fib(1); } int fib(int n) { if (n == 0) return 0; if (n == 1) return 1; return fib(n - 2) + fib(n - 1); }'
assert 1 'int main() { return fib(2); } int fib(int n) { if (n == 0) return 0; if (n == 1) return 1; return fib(n - 2) + fib(n - 1); }'
assert 2 'int main() { return fib(3); } int fib(int n) { if (n == 0) return 0; if (n == 1) return 1; return fib(n - 2) + fib(n - 1); }'
assert 3 'int main() { return fib(4); } int fib(int n) { if (n == 0) return 0; if (n == 1) return 1; return fib(n - 2) + fib(n - 1); }'
assert 5 'int main() { return fib(5); } int fib(int n) { if (n == 0) return 0; if (n == 1) return 1; return fib(n - 2) + fib(n - 1); }'
assert 8 'int main() { return fib(6); } int fib(int n) { if (n == 0) return 0; if (n == 1) return 1; return fib(n - 2) + fib(n - 1); }'
assert 3 'int main() { int x; int y; x = 3; y = &x; return *y; }'
assert 3 'int main() { int x; int y; int z; x = 3; y = 5; z = &y + 8; return *z; }'
assert 3 'int main() { int x; int *y; y = &x; *y = 3; return x; }'

echo OK
