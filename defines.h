#ifndef _DEFINES_H_
#define _DEFINES_H_

#define SUCCESS 0
#define ERROR_IO 1
#define ERROR_GRAPHICS 2
#define ERROR_CLOCK 3

#define ERROR_HANDLE(H) \
	do{int c = H; if(c) return c; } while(0)

#define CENTER(X, XMAX) \
	(((XMAX) - (X)) >> 1)

#endif /* _DEFINES_H_ */
