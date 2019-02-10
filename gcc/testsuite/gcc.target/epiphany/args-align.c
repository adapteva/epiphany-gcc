/* { dg-do run } */
/* { dg-require-effective-target int32plus } */
/* { dg-options -O1 } */

typedef int v4si __attribute__((vector_size(16)));
struct T { v4si i[2]; int j; } __attribute__((packed)) __attribute__((aligned((8))));

static v4si __attribute__((noinline, noclone))
foo (struct T t, int anum)
{
  v4si wrong = { 66, 66, 66, 66 };

  return anum == 0x12345678 ? t.i[0] : wrong;
}

static struct T __attribute__((noinline, noclone))
init (struct T t)
{
  __builtin_memset (&t, 1, sizeof (struct T));
  return t;
}

int main()
{
  volatile int koko = 0x12345678;
  struct T t;
  t = init (t);
  if (foo (t, koko)[1] != 0x01010101)
    __builtin_abort ();
  return 0;
}
