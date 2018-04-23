/* vgetmem.c - vgetmem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <proc.h>
#include <paging.h>

extern struct pentry proctab[];
/*------------------------------------------------------------------------
 * vgetmem  --  allocate virtual heap storage, returning lowest WORD address
 *------------------------------------------------------------------------
 */
WORD	*vgetmem(nbytes)
	unsigned nbytes; {

	STATWORD ps;
	struct	mblock	*p, *q, *leftover;
	struct pentry *pptr;

	disable(ps);
	// Sanity check on memory request
	if (nbytes==0 || nbytes > 128*NBPG) {
		restore(ps);
		return( (WORD *)SYSERR);
	}

	// Ensure there is memory left
	pptr = &proctab[currpid];
	if (pptr->vmemlist.mnext == NULL) {
		restore(ps);
		return((WORD *)SYSERR);
	}
	// Follow same approach as create.c
	nbytes = (unsigned int) roundmb(nbytes);
	for (q= pptr->vmemlist,p=pptr->memlist->mnext ;
	     p != (struct mblock *) NULL ;
	     q=p,p=p->mnext)                               {

		if ( p->mlen == nbytes) {
			q->mnext = p->mnext;
			restore(ps);
			return( (WORD *)p );
		} else if (p->mlen > nbytes) {
			leftover = (struct mblock *)( (unsigned)p + nbytes );
			q->mnext = leftover;
			leftover->mnext = p->mnext;
			leftover->mlen = p->mlen - nbytes;
			restore(ps);
			return((WORD *)p);
		}
		restore(ps);
		return((WORD *)SYSERR);
	}
}
