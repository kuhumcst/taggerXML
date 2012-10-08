#ifndef _SYSDEP_H_
#define _SYSDEP_H_

#define NORET void

/* CONSTVOIDP is for pointers to non-modifyable void objects */

typedef const void * CONSTVOIDP;
typedef void * VOIDP;
#define NOARGS void
#define PROTOTYPE(x) x

#endif /* ifndef _SYSDEP_H_ */
