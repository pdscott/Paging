#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

SYSCALL release_bs(bsd_t bs_id) {
	/* release the backing store with ID bs_id */
	if (bs_id<0 || bs_id>15) { return(SYSERR); }
	
	return free_bsm(bs_id);
}

