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
    bs_map_t *bsptr;

    for (i=0; i<16; i++) {
        bsptr = &bsm_tab[i];
        bsptr->bs_status = BSM_UNMAPPED;
        bsptr->bs_pid = -1;
        bsptr->bs_vpno = -1;
        bsptr->bs_npages = 0;
    }
    return(OK);
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
    // should not release a shared backing store
    bs_map_t *bsptr;

    bsptr = &bsm_tab[i];
    if (bsptr->bs_status == BSM_UNMAPPED) { return(SYSERR); }
    bsptr->bs_status = BSM_UNMAPPED;
    bsptr->bs_pid = -1;
    bsptr->bs_vpno = -1;
    bsptr->bs_npages = 0;
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
    int bs_id, vpno;
    bs_map_t *bsptr;
    
    vpno = ((unsigned long)vaddr)>>12;
    for (bs_id=0; bs_id<16; bs_id++) {
        bsptr = &bsm_tab[bs_id];
        if (bsptr->bs_status != BSM_UNMAPPED) {
            if (bsptr->bs_vpno <= vpno && vpno <= (bsptr->bs_vpno + bsptr->bs_npages)) {
            	*store = bs_id;
                *pageth = vpno - bsptr->bs_vpno;
                return(OK);
            }
        }
    }
    return(SYSERR);
}


/*-------------------------------------------------------------------------
 * bsm_map - add an mapping into bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_map(int pid, int vpno, int source, int npages) {
    // make sure this source backing store is not already mapped
    if (vpno < 0 || vpno > 4096) { return(SYSERR); }
    if (bsm_tab[source].bs_status != BSM_UNMAPPED) { return(SYSERR); }


    bsm_tab[source].bs_status == BSM_MAPPED;
    bsm_tab[source].bs_pid = pid;
    bsm_tab[source].bs_vpno = vpno;
    bsm_tab[source].bs_npages = npages;

    return(OK);
}



/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag) {
    int bs_id, frame_id, page, status;

    if (vpno < 0 || vpno > 4096) { return(SYSERR); }
    status = bsm_lookup(pid, vpno * NBPG, &bs_id, &page);
    if (status == SYSERR) { return(SYSERR); }
    
    // remove mapping

    return(OK);
    
}


