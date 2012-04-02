// euclidean algorithm
#include <stdio.h>
#include <stdlib.h>
int gcd(int a, int b) {
	if (b > a) {
		int t = a;
		a = b;
		b = t;
	}
	while (b != 0) {
		int m = a % b;
		a = b;
		b = m;
	}
	return a;
}
int main(int argc, char* argv[]) {
    printf("%d\n", gcd(1001, 121));
    return 0;
}

