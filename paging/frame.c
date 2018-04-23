/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>


void clear_frm(fr_map_t *fptr);

/*-------------------------------------------------------------------------
 * init_frm - initialize frm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_frm() {
	int num;
	fr_map_t *fptr;

	for (num=0; num < NFRAMES; num++) {
		fptr = &frm_tab[num];
		clear_frm(fptr);
	}

	return(OK);
}

/*-------------------------------------------------------------------------
 * get_frm - get a free frame according page replacement policy
 *-------------------------------------------------------------------------
 */
SYSCALL get_frm(int* avail) {
	int num, lowcount, evicted, frame_id;
	// Try to get a free frame that is unmapped
	for (num=0; num < NFRAMES; num++) {
		if (frm_tab[num].fr_status == FRM_UNMAPPED) {
			*avail = num;
			return(OK);
		}
	}
	// No frames available - need to evict a frame
	if (grpolicy() == SC) {
		// cycle through the list two times, if needed
		for(frame_id=0; frame_id<NFRAMES*2; frame_id++) {
			frame_id = num % NFRAMES;
			if ( frm_tab[frame_id].fr_type = FR_TBL ||
				 frm_tab[frame_id].fr_type = FR_TBL) {
				continue;
			}
			// Found an unreferenced frame - evict it
			if (frm_tab[frame_id].fr_scbit == 0) {
				evicted = frame_id;
				break;
			// Found a recently referenced frame - decrement
			} else if (frm_tab[frame_id].fr_scbit == 1) {
				frm_tab[frame_id].fr_scbit == 0;
			}
		}
		// Write frame to backing store
		free_frm(evicted);
		// Return the evicted frame
		*avail = evicted;
		return(OK);
	} else if (grpolicy() == LFU) {
		lowcount = MAXINT;
		evicted = -1;
		for(frame_id=0; frame_id<NFRAMES; frame_id++) {
			if ( frm_tab[frame_id].fr_type = FR_PAGE &&
				 frm_tab[frame_id].fr_refcnt < lowcount) {
				lowcount = frm_tab[frame_id].fr_refcnt;
				evicted = frame_id;
			}
		}
		if (evicted == -1) { return(SYSERR); }
		// Write frame to backing store
		free_frm(evicted);
		// Return the evicted frame
		*avail = evicted;
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
	unsigned long fr_addr;
	
	if (i<0 || i>NFRAMES) { return(SYSERR); } 
	fptr = &frm_tab[i];
	if (fptr->fr_type != FR_PAGE) { return(SYSERR); }
	pid = fptr->fr_pid; 
	vaddr = fptr->fr_vpno * NBPG;
	status = bsm_lookup(fptr->fr_pid, vaddr, &store, &pageth);
	if (status == SYSERR) { return(SYSERR); }
	fr_addr = FRAME0 * NBPG + i * NBPG;
	write_bs(fr_addr, store, pageth);

	// Find the page table entry
	pdoffset = (unsigned int) vaddr >> 22;
	ptoffset = (unsigned int) ((vaddr >> 12) & 0x000003FF);
	pdentry = proctab[pid].pdbr + (pdoffset * sizeof(pd_t));
	ptentry = (pdentry->pd_base) * NBPG + (ptoffset * sizeof(pt_t));

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

	return(OK);
}

void clear_frm(fr_map_t *fptr) {
	fptr->fr_status = FRM_UNMAPPED;
	fptr->fr_pid = -1;
	fptr->fr_vpno = -1;
	fptr->fr_refcnt = 0;
	fptr->fr_type = FR_PAGE;
	fptr->fr_dirty = 0;
	fptr->fr_scbit = 0;
}



