/* vcreate.c - vcreate */
    
#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <paging.h>

/*
static unsigned long esp;
*/

LOCAL	newpid();
/*------------------------------------------------------------------------
 *  create  -  create a process to start running a procedure
 *------------------------------------------------------------------------
 */
SYSCALL vcreate(procaddr,ssize,hsize,priority,name,nargs,args)
	int	*procaddr;		/* procedure address		*/
	int	ssize;			/* stack size in words		*/
	int	hsize;			/* virtual heap size in pages	*/
	int	priority;		/* process priority > 0		*/
	char	*name;			/* name (for debugging)		*/
	int	nargs;			/* number of args that follow	*/
	long	args;			/* arguments (treated like an	*/
					/* array in the code)		*/                    {
	
	int status, pid, pages;
	unsigned long *hpaddr;
	bsd_t bs_id;
	struct pentry *pptr;

	// Get Free BS_ID
	status = get_bsm(&bs_id); 
	if (status == SYSERR) { return(SYSERR); }

	// Reserve Pages in BS
    pages = get_bs(bs_id, hsize);
	if (pages == SYSERR || pages < hsize) { return(SYSERR); }

	// Get a new PID
	pid = create(procaddr, ssize, priority, name, nargs, args);
	if (pid == SYSERR) { return(SYSERR); }

	// Create the BS Mapping
    status = bsm_map(pid, 4096, bs_id, hsize);
	if(status == SYSERR) { return(SYSERR); }

	// Update process table 
	pptr = &proctab[pid];
	pptr->store = bs_id;
	pptr->vhpno = 4096;
	pptr->vhpnpages = hsize;
	// Need to update vmemlist
	//hpaddr = (int *) (BACKING_STORE_BASE + (bs_id * BACKING_STORE_UNIT_SIZE))
	//*hpaddr = (struct mblock *) NULL;
}

/*------------------------------------------------------------------------
 * newpid  --  obtain a new (free) process id
 *------------------------------------------------------------------------
 */
LOCAL	newpid()
{
	int	pid;			/* process id to return		*/
	int	i;

	for (i=0 ; i<NPROC ; i++) {	/* check all NPROC slots	*/
		if ( (pid=nextproc--) <= 0)
			nextproc = NPROC-1;
		if (proctab[pid].pstate == PRFREE)
			return(pid);
	}
	return(SYSERR);
}
