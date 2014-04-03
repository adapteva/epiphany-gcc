/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-final { scan-assembler-times "add\[ \t]*sp,sp,#" 2 } } */
/* { dg-final { scan-assembler-not "add\[ \t]*sp,sp,ip" } } */

int f (int i)
{
  volatile j = i;
  return j;
}
