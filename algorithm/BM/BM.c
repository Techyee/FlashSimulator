/* Badblock Manager */
#include "BM.h"

// �⺻ 4���� �������̽�

void		BM_invalidate_ppa(uint32_t PPA)
{
	// array�� ���� invalid ���θ� ǥ��
	// parameter�� ���� PPA�� INVALID���� VALID�� �ٲٴ� �Լ��ε�

}
int8_t		BM_is_invalid_ppa(uint32_t PPA)
{
	// parameter�� ���� PPA�� VALID���� INVALID���� ��ȯ�ϴ� �Լ��ε�
	// 1bit�� �ϰ� �ص� �ǰ����� status�� VALID INVALID �ܿ� �� �������� �����Ƿ� �ϴ� char�� ��ȯ
}
uint32_t	BM_get_gc_victim(Block *Block_list)
{
	/* victim block�� PPA�� ��ȯ�ϴ� �Լ� */
	/* 
	 * Parameter: Array(Heap) of Block structure
	 * Parameter�� Heap���� �־����Ƿ�, Heap ������ �̿��Ͽ� cnt(P/E)�� max�� node��(max heap�� root) ã�Ƽ� �� PPA(PBA?)�� ��ȯ�Ѵ�.
	 */
}
uint32_t	BM_get_weared_block(Block *Block_list)
{
	// �̰� ����
}