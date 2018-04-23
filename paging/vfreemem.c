/* vfreemem.c - vfreemem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <proc.h>

extern struct pentry proctab[];
/*------------------------------------------------------------------------
 *  vfreemem  --  free a virtual memory block, returning it to vmemlist
 *------------------------------------------------------------------------
 */
SYSCALL	vfreemem(block, size)
	struct	mblock	*block;
	unsigned size;                        {

	STATWORD ps;    
	struct	mblock	*p, *q;
	unsigned top;
	struct pentry * pptr;

	// Sanity check
	if (size==0) { return(SYSERR); }

	size = (unsigned)roundmb(size);
	disable(ps);

	// Find the correct block
	for( p=pptr->vmemlist->mnext,q= pptr->memlist;
	     p != (struct mblock *) NULL && p < block ;
	     q=p,p=p->mnext )                             {
		; // do nothing
	}
	
	//Couldn't find it
	if (((top=q->mlen+(unsigned)q)>(unsigned)block && q!= &memlist) ||
	    (p!=NULL && (size+(unsigned)block) > (unsigned)p )) {
		restore(ps);
		return(SYSERR);
	}

	//Free the block or coalesce with adjacent free space
	if ( q!= &memlist && top == (unsigned)block )
			q->mlen += size;
	else {
		block->mlen = size;
		block->mnext = p;
		q->mnext = block;
		q = block;
	}

	if ( (unsigned)( q->mlen + (unsigned)q ) == (unsigned)p) {
		q->mlen += p->mlen;
		q->mnext = p->mnext;
	}
	restore(ps);
	return(OK);
}
