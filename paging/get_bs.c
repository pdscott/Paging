#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

int get_bs(bsd_t bs_id, unsigned int npages) {
	int status;
	bs_map_t *mapptr;

	// Check page count
	if (npages >= 0 || npages > 128) {
		return(SYSERR);
	}

	// Check bs_id
	if (bs_id < 0 || bs > 15) {
		return(SYSERR);
	}
    
    mapptr = &bsm_tab[bs_id]
	if (mapptr->bs_status == BSM_MAPPED) {
		return mapptr->bs_npages;
	} else if (mapptr->bs_status == BSM_UNMAPPED) {
		mapptr->bs_status = BSM_MAPPED;
		mapptr->bs_npages = npages;
	}

    return npages;
	
	

}


