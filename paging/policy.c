/* policy.c = srpolicy*/

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

extern pt_t* global_page_tables[4];
extern int page_replace_policy;
fr_map_t frm_tab[];
/*-------------------------------------------------------------------------
 * srpolicy - set page replace policy 
 *-------------------------------------------------------------------------
 */
SYSCALL srpolicy(int policy) {
	if(policy != SC && policy != LFU) { return(SYSERR); }
	page_replace_policy = policy;
	return(OK);
}

/*-------------------------------------------------------------------------
 * grpolicy - get page replace policy 
 *-------------------------------------------------------------------------
 */
SYSCALL grpolicy() {
  return page_replace_policy;
}

int new_page_dir(int pid) {
	int frame_id, status, i;
	fr_map_t *fptr;
	struct pentry *pptr;
	pd_t *pdentry;

	// Get a new frame to house the directory
	status = get_frm(&frame_id);
	if (status == SYSERR) { return(SYSERR); }

	// Initialize the new frame in frame table
	fptr = &frm_tab[frame_id];
	fptr->fr_status = FRM_MAPPED;
	fptr->fr_pid = pid;
	fptr->fr_vpno = -1;
	fptr->fr_refcnt = 0;
	fptr->fr_type = FR_DIR;
	fptr->fr_dirty = 0;
	pptr = &proctab[pid];

	// Document location of directory in process table
	pptr->pdbr = (unsigned long) (FRAME0 * NBPG) + (frame_id * NBPG);

	// Populate the new page directory
	int numEntries = NBPG/sizeof(pd_t);
	for (i=0; i< numEntries; i++) {
		pdentry = pptr->pdbr + i * sizeof(pd_t);
		// First four entries point to global tables
		if (i<4) {
			pdentry->pd_pres = 1;
			pdentry->pd_base = (unsigned long) global_page_tables[i] >> 12;
		} else {
			pdentry->pd_pres = 0;
			pdentry->pd_base = 0;
		}
		pdentry->pd_write = 1;
		pdentry->pd_user = 0;
		pdentry->pd_pwt = 0;
		pdentry->pd_pcd = 0;
		pdentry->pd_acc = 0;
		pdentry->pd_mbz = 0;
		pdentry->pd_fmb = 0;
		pdentry->pd_global = 0;
		pdentry->pd_avail = 0;
	}

	return frame_id;
}

pt_t* new_page_table(int pid) {
	int frame_id, status, i;
	unsigned long frm_addr;
	pt_t *ptentry;
	fr_map_t *fptr;

    // Get a new frame to house the directory
	status = get_frm(&frame_id);
	if (status == SYSERR) { return(SYSERR); }

	// Initialize the new frame in frame table
	fptr = &frm_tab[frame_id];
	fptr->fr_status = FRM_MAPPED;
	fptr->fr_pid = pid;
	fptr->fr_vpno = -1;
	fptr->fr_refcnt = 0;
	fptr->fr_type = FR_TBL;
	fptr->fr_dirty = 0;
	frm_addr = (unsigned long) (FRAME0 * NBPG) + (frame_id * NBPG);

	// Populate the new page table
	int numEntries = NBPG/sizeof(pt_t);
	for (i=0; i< numEntries; i++) {
		ptentry = frm_addr + i * sizeof(pt_t);
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
	}

	return (pt_t *) frm_addr;
}


