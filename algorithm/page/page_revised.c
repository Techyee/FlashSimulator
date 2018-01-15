#include <string.h>
#include <stdlib.h>
#include "page.h"

struct algorithm algo_pbase=
{
	.create = pbase_create,
	.destroy = pbase_destroy,
	.get = pbase_get,
	.set = pbase_set,
	.remove = pbase_remove
};

uint32_t PPA_status = 0;
int32_t NUMOP = 32*1024*1024;
int32_t NUMOB = NUMOP/256;
int init_done = 0;//check if initial write is done.

//structure below represents the table comp. for mapping
typedef struct mapping_table{
	int32_t lpa_to_ppa;
	unsigned char valid_checker;
}TABLE;//costs 5bytes.

///structure below represents the OOB in SSD pages.
typedef struct virtual_OOB{
	int32_t reverse_table;
}OOB;//"revese_table" would be included.
// this structure costs 4bytes.

typedef struct SRAM{
	int32_t lpa_RAM;
	char* VPTR_RAM;
}SRAM;

TABLE* page_TABLE;
OOB* page_OOB;
SRAM* page_SRAM;
uint16_t* invalid_per_block;

page_OOB =(OOB *)malloc(sizeof(OOB)*NUMOP); //we may use this as virtual OOB.
page_SRAM = (SRAM *)malloc(sizeof(SRAM)*256);
invalid_per_block = (uint16_t*)malloc(sizeof(uint16_t)*NUMOB);
for (int i = 0; i<NUMOB; i++)
	invalid_per_block[i] = 0;

uint32_t pbase_create(lower_info* li, algorithm *algo) //define & initialize mapping table.
{
	 algo->li = li; 	//allocate li parameter to algorithm's li.
	 page_TABLE = (TABLE*)malloc(sizeof(TABLE)*NUMOP);
	 for(int i = 0; i < NUMOP; i++)
	{
        	page_TABLE[i]->lpa_to_ppa = -1;
		page_TABLE[i]->valid_checker = 0;
	}
	//init mapping table.
}	//now we can use page table after pbase_create operation.

void pbase_destroy(lower_info* li, algorithm *algo)
{
        free(page_TABLE);//deallocate table.
        free(page_OOB);//FIXME:may be unnecessary?
        free(invalid_per_block);
	free(SRAM);
	//Question: why normal_destroy need li and algo?
}

void *pbase_end_req(algo_req* input)
{
	pbase_params* params=(pbase_params*)input->params;
	request *res=params->parents;
	res->end_req(res);
	free(params);
	free(input);
}

uint32_t pbase_get(const request *req)
{
	//put request in normal_param first.
	//request has a type, key and value.
	pbase_params* params = (pbase_params*)malloc(sizeof(pbase_params));
	params->parents=req;
	params->test=-1; //default parameter setting.

	algo_req * my_req = (algo_req*)malloc(sizeof(algo_req)); //init reqeust
	my_req->end_req=pbase_end_req;//allocate end_req for request.
	my_req->params=(void*)params;//allocate parameter for request.

	KEYT target = page_TABLE[req->key]->lpa_to_ppa;

	algo_pbase.li->pull_data(target,PAGESIZE,req->value,0,my_req,0);
	//key-value operation.
	//Question: why value type is char*?
}

uint32_t pbase_set(const request *req)
{
	pbase_params* params = (pbase_params*)malloc(sizeof(pbase_params));
	params->parents=req;
	params->test=-1;

	algo_req * my_req = (algo_req*)malloc(sizeof(algo_req));
	my_req->end_req = pbase_end_req;
	my_req->params = (void*)params;
	
	//garbage_collection necessity detection.
	if (PPA_status == NUMOP)
	{
		pbase_garbage_collection();
		init_done = 1;
	}

	else if ((init_done = 1) && (PPA_status % 256 = 255))
	{
		pbase_garbage_collection();
	}
	//!garbage_collection.
	
	if (page_TABLE[req->key]->lpa_to_ppa != -1)
	{
		int temp = page_TABLE[req->key]->lpa_to_ppa; //find old ppa.
		page_TABLE[temp]->valid_checker = 0; //set that ppa validity to 0.
		invalid_per_block[temp/256] += 1;
	}
	
	page_TABLE[req->key]->lpa_to_ppa = PPA_status; //map ppa status to table.
	page_TABLE[PPA_status]->valid_checker |= 1; 
	OOB[PPA_status]->revese_table = req->key;//reverse-mapping.
	KEYT set_target = PPA_status;
	PPA_status++;

	algo_pbase.li->push_data(set_target,PAGESIZE,req->value,0,my_req,0);
}

uint32_t pbase_remove(const request *req)
{
    page_TABLE[req->key]->lpa_to_ppa = -1; //reset to default.
	OOB[req->key]->reverse_table = -1; //reset reverse_table to default.
}

uint32_t SRAM_load(int ppa, int a)
{
	char* value_PTR;
	algo_req * my_req = (algo_req*)malloc(sizeof(algo_req));
	my_req->end_req = pbase_end_req; //request termination.
	algo_pbase.li->pull_data(ppa,PAGESIZE,value_PTR,0,my_req,0);
	SRAM[a]->lpa_RAM = OOB[ppa]->reverse_table;//load reverse-mapped lpa.
	SRAM[a]->VPTR_RAM = value_PTR;
}

uint32_t SRAM_unload(int ppa, int a)
{
	algo_req * my_req = (algo_req*)malloc(sizeof(algo_req));
	my_req->end_req = pbase_end_req;
	algo_pbase.li->push_data(ppa,PAGESIZE,SRAM[a]->VPTR_RAM,0,my_req,0);
	
	page_TABLE[SRAM[a]->lpa_RAM]->lpa_to_ppa = ppa;
	page_TABLE[ppa]->valid_checker |= 1;
	OOB[ppa]->reverse_table = SRAM[a]->lpa_RAM;
	
	SRAM[a]->lpa_RAM = -1;
	SRAM[a]->VPTR_RAM = NULL;
}

uint32_t pbase_garbage_collection()//do pbase_read and pbase_set 
{
	int target_block = 0;
	int invalid_num = 0;
	for (int i = 0; i<256; i++)
	{
		if(invalid_per_block[i] >= invalid_num)
		{
			target_block = i;
			invalid_num = invalid_per_block[i];
		}
	}//found block with the most invalid block.
	
	PPA_status = target_block*256;
	int valid_component = 256 - invalid_num;
	int a = 0;
	for (int i = 0; i<256; i++)
	{
		page_TABLE[PPA_status+i]->valid_checker = 0;
		if (page_TABLE[PPA_status + i]->valid_checker & 1)
		{
			SRAM_load(PPA_status + i, a);
			a++;
		}
	}
	algo_pbase.li->trim_block(PPA_status, false);
	for (int i = 0; i<valid_component; i++)
	{
		SRAM_unload(PPA_status,i);
		PPA_status++;
	}
	invalid_per_block[target_block] = 0;
}

