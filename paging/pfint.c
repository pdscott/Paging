/* pfint.c - pfint */

#include <conf.h>
#include <kernel.h>
#include <paging.h>


/*-------------------------------------------------------------------------
 * pfint - paging fault ISR
 *-------------------------------------------------------------------------
 */
SYSCALL pfint() {
	int store, pageth, status, frame;
	unsigned long addr, faddr;
	unsigned int pdoffset, ptoffset;
	struct pentry *pptr;
	pd_t *pdentry;
	pt_t *ptentry;

	// 1. get faulted address
	// 2. get virtual page number
	addr = read_cr2();
	pdoffset = (addr && 0xFF600000) >> 22;
	ptoffset = (addr && 0x003FF000) >> 12;
    
	// 3. check if addr is legal
	status = bsm_lookup(currpid, addr, &store, &pageth);
	if (status == SYSERR) { return(SYSERR)};

	// 5,6,7 If page table doesn't exist, make it.
	pptr = &proctab[currpid];
	pdentry = (pd_t *) pptr->pdbr;
	if (pdentry[pdoffset].pd_pres == 0) {
		ptentry = new_page_table(currpid);
		if (ptentry == SYSERR) { kill(currpid); }
		pdentry[pdoffset].pd_pres = 1;
		pdentry[pdoffset].pd_write = 1;
		pdentry[pdoffset].pd_base = ptentry >> 12;
		// frame num = this address - base address / 4096
		frame = (pdentry - (FRAME0* NBPG)) / NBPG
		frm_tab[frame].fr_vpno = addr >> 12;
		if (grpolicy() == SC) {
			frm_tab[frame].scbit == 1;
		}
	} else {
		// just note location of page table
		ptentry = pdentry[pdoffset].pdbase << 12;
	}

	// Page in faulted pages
	if (ptentry[ptoffset].pt_pres != 1) {
		status = get_frm(frame);
		if (ptentry == SYSERR) { kill(currpid); }
		frm_tab[frame].fr_vpno = addr >> 12;
		frm_tab[frame].refcnt++;
		if (grpolicy() == SC) {
			frm_tab[frame].scbit == 1;
		}
		faddr = FRAME0*NBPG + frame*NBPG;
		status = read_bs(faddr, store, pageth);
		if (status == SYSERR) { kill(currpid); } 
		frm_tab[frame].fr_bsid = store;
		ptentry[ptoffset].pt_pres = 1;
		ptentry[ptoffset].pt_write = 1;
		ptentry[ptoffset].pt_base = faddr >> 12;

	} 



	write cr3(pptr->pdbr);












}


