/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-optimized" } */

int *a_p;
int a[1000];

void
f(int k)
{
        long long i;

        for (i=k; i<1000; i+=k) {
                a_p = &a[i];
                *a_p = 100;
        }
}

/* { dg-final { scan-tree-dump-times "&a" 1 "optimized" } } */
