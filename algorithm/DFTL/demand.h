#include "../../include/container.h"
#include "queue.h"

#define EPP (PAGESIZE / sizeof(D_TABLE)) //Number of table entries per page
#define NTP (TOTALSIZE / EPP) //Number of Translation Page
#define	GTDSIZE sizeof(D_TABLE) * NTP
#define CMTSIZE TOTALSIZE - GTDSIZE

typedef struct cached_table{
	int32_t lpa;
	int32_t ppa;
	unsigned char flag;
	LINKED_LIST *queue_ptr;
}C_TABLE;

typedef struct demand_mapping_table{
	int32_t ppa; //Index = lpa
}D_TABLE;

typedef struct demand_OOB{
	int32_t reverse_table;
	unsigned char valid_checker;
}D_OOB;

typedef struct demand_SRAM{
	int32_t lpa_RAM;
	PTR VPTR_RAM;
}D_SRAM;

typedef struct demand_params{
	request *parents;
	int test;
}demand_params;

uint32_t demand_create(lower_info*, algorithm*);
void demand_destroy(lower_info*, algorithm*);
uint32_t demand_get(const request*);
uint32_t demand_set(const request*);
uint32_t demand_remove(const request*);
void *demand_end_req(algo_req*);
int CMT_check(int lpa, int *ppa);
uint32_t demand_eviction(int *CMT_i);
uint32_t demand_GC();
