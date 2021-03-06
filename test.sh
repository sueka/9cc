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
assert 0 'int main() { return 3>2>1; }' # 3>2 ă 1 ăŞăŽă§ 1>1 ăŻ 0
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
assert 4 'int main() { int *p; alloc4(&p, 1, 2, 4, 8); int *q; q = p + 2; return *q; }'
assert 8 'int main() { int *p; alloc4(&p, 1, 2, 4, 8); int *q; q = p + 2; *q; q = p + 3; return *q; }'
assert 2 'int main() { int *p; alloc4(&p, 1, 2, 4, 8); int *q; q = p + 3; *q; q = q - 2; return *q; }'
assert 4 'int main() { int x; return sizeof(x); }'
assert 8 'int main() { int *y; return sizeof(y); }'
assert 4 'int main() { int x; return sizeof(x + 3); }'
assert 8 'int main() { int *y; return sizeof(y + 3); }'
assert 4 'int main() { int *y; return sizeof(*y); }'
assert 4 'int main() { return sizeof(1); }'
assert 4 'int main() { return sizeof(sizeof(1)); }'
assert 0 'int main() { 0; 0; 0; 0; 0; 0; 0; 0; 0; 0; 0; 0; 0; 0; 0; 0; 0; 0; 0; 0; 0; 0; 0; 0; 0; 0; return 0; }' # 9a1a6eb 時点で、文が27個以上あると127で終了する。GitHub 上のコミットページ: https://github.com/sueka/9cc/commit/9a1a6eb460e86d877f5e3b4ddecc0a0f026cf768
assert 0 'int main() { int a; a = 0; int b; b = 0; int c; c = 0; int d; d = 0; int e; e = 0; int f; f = 0; int g; g = 0; int h; h = 0; int i; i = 0; int j; j = 0; int k; k = 0; int l; l = 0; int m; m = 0; int n; n = 0; int o; o = 0; int p; p = 0; int q; q = 0; int r; r = 0; int s; s = 0; int t; t = 0; int u; u = 0; int v; v = 0; int w; w = 0; int x; x = 0; int y; y = 0; int z; z = 0; int aa; aa = 0; int ab; ab = 0; return 0; }'
assert 0 'int main() { int a[10]; return 0; }'
assert 3 'int main() { int a[2]; *a = 1; *(a + 1) = 2; int *p; p = a; return *p + *(p + 1) }'

echo OK
