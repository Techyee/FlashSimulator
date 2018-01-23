/* BM_HEAP */

#include "BM_Heap.h"

// Heap ��� �� �����ϱ�. ������ ���ݹۿ� ����
// ������ �밳 int list�� �������� �������������, ���߿� Blocklist�� �ٲ� ��

/* Return and Extract(Erase) Maximum value in Max-heap */
uint32_t heap_maximum_extract(int* list, int size)
{
	uint32_t max;
	if (size < 1) {
		ERR(eHEAPUNDERFLOW_BM);
		printf("heap underflow\n");
	}
	max = list[0];
	list[0] = list[size - 1];
	list = (int*)realloc(list, sizeof(int) * --size);
	max_heapify(list, size, 0);
	return max;
}

/* Return Maximum value in Max-heap */
uint32_t heap_maximum(int* list) // int*�� �ƴ϶� Block*�̾�� �Ѵ�. 
{
	return list[0]; // return type�� ����� uint32_t�� �ƴ϶� Block
}

void build_max_heap(int* list, int size)
{
	int i;
	for (i = size / 2; i >= 0; --i)
		max_heapify(list, size, i);
}
void max_heapify(int* list, int count, int i) // max heap���� fix�ϱ�
{
	int l = 2 * i + 1;
	int r = 2 * i + 2;
	int largest;
	if (l <= count - 1 && list[l] > list[i])	largest = l;
	else largest = i;
	if (r <= count - 1 && list[r] > list[largest])	largest = r; // if, else, if�� ������ ���� ���� ū value�� ������ index�� ã�Ƴ� ��,
	if (largest != i) {                                          // ���� �θ� ����� ���� i�� largest�� �ƴ϶�� largest�� i�� �ٲ۴�.
		SWAP(list[i], list[largest]);
		max_heapify(list, count, largest); // �� �Ŀ��� ��������� �ݺ�
	}
}

void heapSort(int* list, int count)
{
	//�ϴ� build_max_heap�� ����
	int i;
	int size = count;
	for (i = count / 2; i >= 0; i--)	max_heapify(list, count, i);  // build_max_heap
																	  //
	for (i = count - 1; i > 0; i--)
	{
		SWAP(list[0], list[i]); // ���� ū ���� ���� ���� �� ��ü
		size--;					// ������ ���� heap tree���� ����
		max_heapify(list, size, 0); // �ٽ� max heap���� fix
	}
}

