/* { dg-do run } */
/* { dg-options "-O2 -fno-omit-frame-pointer -fschedule-insns -fdse" } */

/* When the exit function address gets spilled to a stack slot, and
   reload_combine_recognize_const_pattern moves the load from that stacks
   slot across and adjusts for the frame pointer adjustment in the nonlocal
   goto receiver, dead store elimination can fail to recognize that load
   as keeping the store to the stack slot live.  */

#ifndef NO_TRAMPOLINES

typedef __SIZE_TYPE__ size_t;
extern void abort (void);
extern void exit (int);
extern void qsort(void *, size_t, size_t, int (*)(const void *, const void *));

int main ()
{
  __label__ nonlocal;
  int compare (const void *a, const void *b)
  {
    static int count = 2;

    /* Without this check, the failure mode is a timeout.  */
    if (!--count)
      abort ();
    goto nonlocal;
  }

  char array[3];
  qsort (array, 3, 1, compare);
  abort ();

 nonlocal:
  exit (0);
}

#else
int main() { return 0; }
#endif
