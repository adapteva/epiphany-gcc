/* { dg-lto-do run }  */
/* { dg-require-effective-target fpic } */
/* { dg-suppress-ld-options {-fPIC} }  */
/* { dg-require-effective-target tls_runtime } */
/* { dg-extra-ld-options "-fPIC" { target epiphany*-*-* } } */
int foo (int x)
{
  return x;
}
