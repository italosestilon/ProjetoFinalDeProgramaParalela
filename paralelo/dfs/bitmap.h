#ifndef BITMAP_H_INCLUDED
#define BITMAP_H_INCLUDED

#include <stdint.h>
#define INT_SIZE 64
#define LWSIZE  6
#define LOWERBITS  0x000000000000003F

typedef uint64_t word;


/*Macros*/
#define high(x)      ((x) >> LWSIZE)
#define low(x)       ((x) & LOWERBITS)
#define indexbit(a,b)   (((a) << LWSIZE)|(b))
#define test(B, x)   ((B[high((x))])&( 1LL << low((x))))
#define biton(S, x)  (S |=  (1LL << (x)))
#define bitoff(S, x) (S &= ~(1LL << (x)))
#define setbit(B, x)    biton(  B[ high((x)) ] , low((x)) )
#define resetbit(B, x)  bitoff( B[ high((x)) ] , low((x)) )
#define is_edge(a,b) (bit[a][b/INT_SIZE]&(mask[b%INT_SIZE]))
#define NWORDS(N) (N/INT_SIZE+1)



static inline int _ffs(word x) { return (x) ? __builtin_ctzll(x) : -1; }
static inline int _fls(word x) { return (x) ? INT_SIZE - 1 - __builtin_clzll(x) : -1; }
static inline int _cbs(word x) { return (x) ? __builtin_popcountll(x) : 0; }


inline int LSB(word * B, int & i, int W){
	while(i <= W){
    		int r = _ffs( B[i] );
    		if( r >= 0 ) return r;
    		i++;
  	}
  	return -1;
}

inline int MSB(word * B, int & i){
	while(i >= 0){
    		int r = _fls( B[i] );
    		if( r >= 0 ) return r;
    		i--;
  	}
  	return -1;
}

inline int COUNT(word * B, int W){
	int SUM;
  	SUM = 0;
  	for(int i = 0; i <= W; i++){
    		SUM += _cbs( B[i] );
  	}
  	return SUM;
}

#endif // CLAUSE_H
