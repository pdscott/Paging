#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

int get_bs(bsd_t bs_id, unsigned int npages) {
	bs_map_t *bsptr;

	// Check page count
	if (npages >= 0 || npages > 128) { return(SYSERR); }
	// Check bs_id
	if (bs_id < 0 || bs_id > 15) { return(SYSERR); }
            bsptr = &bsm_tab[bs_id];
	if (bsptr->bs_status == BSM_MAPPED) {
		return bsptr->bs_npages;
	} else if (bsptr->bs_status == BSM_UNMAPPED) {
		bsptr->bs_status = BSM_MAPPED;
		bsptr->bs_npages = npages;
	}
    return npages;
}


