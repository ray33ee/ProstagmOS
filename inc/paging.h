#ifndef ALLOCATOR_PROSTAGMOS_H

#define ALLOCATOR_PROSTAGMOS_H


extern void load_pd_table(void*);
extern void enable_paging();

void map_logical_to_physical(uint32_t, uint32_t, uint16_t);

void paging_initialise();

#endif
