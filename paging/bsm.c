/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

/*-------------------------------------------------------------------------
 * init_bsm- initialize bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_bsm() {
    int i;

    for (i=0; i<16; i++) {
        bsm_tab[i].bs_status = BSM_UNMAPPED;
        // other bsm initialization
    }

}

/*-------------------------------------------------------------------------
 * get_bsm - get a free entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL get_bsm(int* avail) {
    int i;

    for (i=0; i<16; i++) {
        if (bsm_tab[i].bs_status == BSM_UNMAPPED) {
            *avail = i;
            return(OK);
        }
    }
    return(SYSERR);
}


/*-------------------------------------------------------------------------
 * free_bsm - free an entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL free_bsm(int i) {
    // see if bs is shared 
    if (bsm_tab[i].bs_status == BSM_UNMAPPED) {
        return(SYSERR)
    }
    bsm_tab[i].bs_status == BSM_UNMAPPED;
    return(OK);

}

/*-------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_lookup(int pid, long vaddr, int* store, int* pageth) {
// receive process id and virtual address
// set store to store descriptor
// set pageth to the correct page in the store for the virtual address
    int i, vpno;
    
    vpno = vaddr>>12;
    for (i=0; i<16; i++) {
        if (bs_map[i].bs_status != BSM_UNMAPPED) {
            // make sure pid actually owns this bs
            first = bs_map[i].bs_vpno;
            last = bs_map[i].bs_vpno + bs_map[i].bs_npages;
            if (first <= vpno && vpno <= last) {
            	*store = i;
                *pageth = vpno - bs_map[i].bs_vpno
                return(OK)
            }
        }
    } 
    return(SYSERR)    

    
}


/*-------------------------------------------------------------------------
 * bsm_map - add an mapping into bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_map(int pid, int vpno, int source, int npages) {
    // make sure this source backing store is not already mapped
    if (bsm_tab[source].bs_status != BSM_UNMAPPED) {
        return(SYSERR);
    }

    // make sure this process is not already mapped?

    if (bsm_tab[source].bs_status == BSM_UNMAPPED) {
        bsm_tab[source].bs_status == BSM_MAPPED;
        bsm_tab[source].bs_npages = npages;
        bsm_tab[source].bs_ref = pid;
        bsm_tab[source].bs_vpno = vpno;
    }
    return(OK);
}



/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag) {
    int store, pageth;
    if (bsm_lookup(pid, vpno * NPBG, &store, &pageth) == SYSERR) {
        return(SYSERR);
    }  else {
        // write dirty pages 
        // 
    }
}


