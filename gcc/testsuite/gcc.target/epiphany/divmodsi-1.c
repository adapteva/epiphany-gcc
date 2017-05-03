/* { dg-do run } */
/* { dg-options "-O3 -funroll-loops" } */

extern void abort (void);

struct my_args {
  float *ga;
  int s;
  int N;
  int d;
  unsigned s2;
  unsigned N2;
  unsigned d2;
};

__attribute__((noinline)) void
blockage (float *s, int a, int b)
{
  __asm__ __volatile__ ("" ::: "memory");
}

__attribute__((noinline)) void
ublockage (float *s, unsigned a, unsigned b)
{
  __asm__ __volatile__ ("" ::: "memory");
}

__attribute__((noinline)) int
divsi (struct my_args *theptr)
{
  float *ga = theptr->ga;

  int d = theptr->d;
  int N = theptr->N;
  int s = theptr->s;

  /* Compiler sets up args for __divsi like this:
     mov r0, N
     mov r1, d
     But if d was ever to be assigned to r0 we get:
     mov r0, N   <-- d overwritten
     mov r1, r0  */
  int n = N / d;

  int i, k;
  for (i = 0; i < s; i++) {
    float *r;
    for (k = 0; k < s; k++)
      {
	r = ga + k * N;
	blockage (r, n, s * N + d);
      }
    blockage (r, n, d);
  }

  return n;
}

__attribute__((noinline)) int
modsi (struct my_args *theptr)
{
  float *ga = theptr->ga;

  int d = theptr->d;
  int N = theptr->N;
  int s = theptr->s;

  int n = N % d;

  int i, k;
  for (i = 0; i < s; i++) {
    float *r;
    for (k = 0; k < s; k++)
      {
	r = ga + k * N;
	blockage (r, n, s * N + d);
      }
    blockage (r, n, d);
  }

  return n;
}

__attribute__((noinline)) int
udivsi (struct my_args *theptr)
{
  float *ga = theptr->ga;

  unsigned d = theptr->d2;
  unsigned N = theptr->N2;
  unsigned s = theptr->s2;

  unsigned n = N / d;

  unsigned i, k;
  for (i = 0; i < s; i++) {
    float *r;
    for (k = 0; k < s; k++)
      {
	r = ga + k * N;
	ublockage (r, n, s * N + d);
      }
    ublockage (r, n, d);
  }

  return n;
}

__attribute__((noinline)) int
umodsi (struct my_args *theptr)
{
  float *ga = theptr->ga;

  unsigned d = theptr->d2;
  unsigned N = theptr->N2;
  unsigned s = theptr->s2;

  unsigned n = N % d;

  unsigned i, k;
  for (i = 0; i < s; i++) {
    float *r;
    for (k = 0; k < s; k++)
      {
	r = ga + k * N;
	ublockage (r, n, s * N + d);
      }
    ublockage (r, n, d);
  }

  return n;
}

int main ()
{
  struct my_args a = { 0, 0, 5, 2, 0, 5, 2 };

  if (divsi (&a) != 2)
    abort ();

  if (modsi (&a) != 1)
    abort ();

  if (udivsi (&a) != 2)
    abort ();

  if (umodsi (&a) != 1)
    abort ();

  return 0;
}
