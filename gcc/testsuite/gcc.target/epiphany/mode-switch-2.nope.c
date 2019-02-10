/* { dg-do run } */
/* { dg-options "-O0" } */

#define FUNNY_NUMBER1  268440163

/* Presumes simulator will exit with error code on raised exception */

/*
 * When compiled with -Os, -Og or -O0 and sz==0, I will set the ARITHMODE flags
 * of the config register from an unitilialized gp register.
 */

/*
 *
 * foo:
 *      ...
 *      sub r0,r0,0
 *      be .L3           <-- branch to exit if sz == 0
 * .L1:
 *      movfs r3,config
 *      ...
 * .L2:
 *      sub r0,r0,1
 *      bne .L1
 *
 * .L3:
 *
 *      mov r0,0xfffe
 *      movt r0,0xfff1
 *      gid
 *      movfs ip,config
 *      eor r5,r5,r3     <-- r3 uninitialized if r0 == 0
 *      and r5,r5,r0
 *      eor r5,r5,r3     <-- likewise
 *
 *      ...
 *
 *
 */

unsigned
hash (char *ptr, unsigned sz, unsigned hash)
{
  while (sz--)
    hash = hash * FUNNY_NUMBER1 + *ptr++;

  return hash;
}

char a[] = "123";

int
main ()
{
  int val;

  val = hash (a, 0, 123);

  return val % 123;
}
