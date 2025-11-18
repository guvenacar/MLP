#include <stdio.h>

long fibonacci(long n) {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

int main() {
    printf("Computing fibonacci(35)...\n");
    long result = fibonacci(35);
    printf("Result: %ld\n", result);
    return 0;
}
