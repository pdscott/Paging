/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

/*-------------------------------------------------------------------------
 * init_frm - initialize frm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_frm() {
	int num;
	fr_map_t *fptr;

	for (num=0; num < NFRAMES; num++) {
		fptr = frm_tab[num];
		clear_frm(fptr);
	}

	return(OK);
}

/*-------------------------------------------------------------------------
 * get_frm - get a free frame according page replacement policy
 *-------------------------------------------------------------------------
 */
SYSCALL get_frm(int* avail) {
	int num;

	for (num=0; num < NFRAMES; num++) {
		if (frm_tab[num].fr_status == FRM_UNMAPPED) {
			*avail = num;
			return(OK);
		}
	}
	if (page_replace_policy == SC) {

		return(OK);
	} else if (page_replace_policy == LFU) {

		return(OK);
	}
	return(SYSERR);
}

/*-------------------------------------------------------------------------
 * free_frm - free a frame 
 *-------------------------------------------------------------------------
 */
SYSCALL free_frm(int i) {
	int store, pageth, status, pid;
	long vaddr;
	fr_map_t *fptr;
	pd_t *pdentry;
	pt_t *ptentry;
	unsigned int pdoffset;
	unsigned int ptoffset;
	
	if (i<0 || i>NFRAMES) {
		return(SYSERR);
	} 
	fptr = frm_tab[i];
	if (fptr->fr_type != FR_PAGE) {
		return(SYSERR);
	}
	pid = fptr->fr_pid; 
	vaddr = fptr->fr_vpno * NBPG;
	status = bsm_lookup(fptr->fr_pid, vaddr, &store, &pageth);
	if (status == SYSERR) {
		return(SYSERR);
	}
	//write page if dirty?

	// Find the page table entry
	pdoffset = (unsigned int) vaddr >> 22;
	ptoffset = (unsinged int) ((vaddr >> 12) & 0x000003FF);
	pdentry = proctab[pid].pdbr + (pdoffset * sizeof(pd_t));
	ptentry = (pdentry->pdbase) * NBPG + (ptoffset * sizeof(pt_t));

	// Reset all the values in the page table entry
	ptentry->pt_pres = 0;
	ptentry->pt_write = 1;
	ptentry->pt_user = 0;
	ptentry->pt_pwt = 0;
	ptentry->pt_pcd = 0;
	ptentry->pt_acc = 0;
	ptentry->pt_dirty = 0;
	ptentry->pt_mbz = 0;
	ptentry->pt_global = 0;
	ptentry->pt_avail = 0;
	ptentry->pt_base = 0;

	// Reset all the values in the inverted page table 
	clear_frm(fptr);
}

LOCAL clear_frm(fr_map_t *fptr) {
	fptr->fr_status = FRM_UNMAPPED;
	fptr->fr_pid = -1;
	fptr->fr_vpno = -1;
	fptr->fr_refcnt = 0;
	fptr->fr_type = FR_PAGE;
	fptr->fr_dirty = 0;
}



