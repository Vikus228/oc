#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int is_prime(int n)
{
    if (n <= 1)
        return 0;
    if (n == 2)
        return 1;
    if (n % 2 == 0)
        return 0;

    int sqrt_n = (int)sqrt(n);
    for (int i = 3; i <= sqrt_n; i += 2)
    {
        if (n % i == 0)
            return 0;
    }
    return 1;
}

int main()
{
    int num;
    while (scanf("%d", &num) == 1)
    {
        if (num < 0 || is_prime(num))
        {
            exit(EXIT_SUCCESS);
        }
        else
        {
            printf("%d\n", num);
            fflush(stdout);
        }
    }

    return 0;
}
