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
        bsptr = bsm_tab[i];
        bsptr->bs_status = BSM_UNMAPPED;
        bsptr->pid = -1;
        bsptr->vpno = -1;
        bsptr->npages = 0;
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
    bs_map_t *bsptr;

    bsptr = bsm_tab[i];
    if (bsptr->bs_status == BSM_UNMAPPED) { return(SYSERR); }
    bsptr->bs_status = BSM_UNMAPPED;
    bsptr->pid = -1;
    bsptr->vpno = -1;
    bsptr->npages = 0;
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
        bsptr = bsmap[bs_id];
        if (bsptr->bs_status != BSM_UNMAPPED) {
            if (bsptr->bsvpno <= vpno && vpno <= (bsptr->bs_vpno + bsptr->bsnpages)) {
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
    if (bsm_tab[source].bs_status != BSM_UNMAPPED) { return(SYSERR); }

    if (bsm_tab[source].bs_status == BSM_UNMAPPED) {
        bsm_tab[source].bs_status == BSM_MAPPED;
        bsm_tab[source].bs_pid = pid;
        bsm_tab[source].bs_vpno = vpno;
        bsm_tab[source].bs_npages = npages;
    }
    return(OK);
}



/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag) {
    int store, pageth, status;

    status = bsm_lookup(pid, vpno * NPBG, &store, &pageth);
    if (status == SYSERR) { return(SYSERR); }
    

        // write dirty pages 
        // 
    
}


