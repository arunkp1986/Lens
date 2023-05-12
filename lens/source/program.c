#include <stdio.h>
int main() {
    int n, i;
    unsigned long long fact = 1;
    n = 10;
    if (n < 0){
        printf("n is negative");
    }
    else{
        for (i = 1; i <= n; ++i) {
            fact *= i;
        }
        printf("Factorial of %d = %llu", n, fact);
    }
    return 0;
}
