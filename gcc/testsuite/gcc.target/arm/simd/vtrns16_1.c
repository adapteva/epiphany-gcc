/* Test the `vtrns16' ARM Neon intrinsic.  */

/* { dg-options "-save-temps -O1 -fno-inline" } */
/* { dg-add-options arm_neon } */

#include "arm_neon.h"
#include "../../aarch64/simd/vtrns16.x"

/* { dg-final { scan-assembler-times "vtrn\.16\[ \t\]+\[dD\]\[0-9\]+, ?\[dD\]\[0-9\]+!?\(?:\[ \t\]+@\[a-zA-Z0-9 \]+\)?\n" 1 } } */
/* { dg-final { cleanup-saved-temps } } */
