/* { dg-do compile } */
/* { dg-options "-O2 -fno-common" } */
/* { dg-final { scan-assembler-times "movt" 4 } } */

typedef unsigned int uint32_t;
typedef unsigned int uint16_t;

struct dma_desc {
   uint32_t config;
   uint32_t inner_stride;
   uint32_t count;
   uint32_t outer_stride;
   void *src_addr;
   void *dst_addr;
};
typedef struct dma_desc e_dma_desc_t;

e_dma_desc_t dma;
int a;
int id[8];
#define NULL ((void *)0)

static inline void _ez_dma_set(register e_dma_desc_t  *dma,
                uint32_t config,
                e_dma_desc_t *next,
                uint16_t inner_src, uint16_t inner_dst,
                uint16_t inner_count, uint16_t outer_count,
                uint16_t outer_src, uint16_t outer_dst,
                void *src, void*dst) {
   //register e_dma_desc_t *dmap = dma;

   dma->config = config | (((uint32_t)next)<<16);
   dma->inner_stride = (inner_dst << 16) | inner_src;
   dma->count = (inner_count << 16) | outer_count;
   dma->outer_stride = (outer_dst << 16) | outer_src;
   dma->src_addr = src;
   dma->dst_addr = dst;
}

void __attribute__((section(".text.ds1")))
dmas_inline1(void) {
   register e_dma_desc_t *dmap = &dma;

   _ez_dma_set(dmap, 3, NULL,
          1, 2,
          12, 13,
          5, 1,
          id, &a);
}
