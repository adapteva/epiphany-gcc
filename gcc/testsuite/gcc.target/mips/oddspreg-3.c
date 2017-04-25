/* Check that we disable odd-numbered single precision registers.  */
/* { dg-skip-if "needs asm output" { *-*-* } { "-fno-fat-lto-objects" } { "" } } */
/* { dg-options "-mabi=32 -mfp32 -march=loongson3a -mhard-float" } */

NOMIPS16 void
foo ()
{
  register float foo asm ("$f1"); /* { dg-error "isn't suitable for" } */
  asm volatile ("" : "=f" (foo));
}
