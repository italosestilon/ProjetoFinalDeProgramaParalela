#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <iostream>
#include <pthread.h>
#include "bitmap.h"
#include "ThreadPool.h"
#include <vector>
#include <random>

#define MAX_VERTEX 20000

using namespace std;

int Vnbr,Enbr;
word bit[MAX_VERTEX][NWORDS(MAX_VERTEX)];
word pbit[MAX_VERTEX][NWORDS(MAX_VERTEX)];
int degree[MAX_VERTEX];
int pos[MAX_VERTEX];
int rec[MAX_VERTEX];
int record;
word mask[INT_SIZE];
int level;
clock_t clk;
int timeout;
int nt;
double elapsed;
int s = 2;
int cnt = 0;
double duracao;
struct timeval start, stop;

ThreadPool *tp;

double rtclock(){
    struct timezone Tzp;
    struct timeval Tp;
    int stat;
    stat = gettimeofday (&Tp, &Tzp);
    if (stat != 0) printf("Error return from gettimeofday: %d",stat);
    return(Tp.tv_sec + Tp.tv_usec*1.0e-6);
}

pthread_mutex_t mutex_update_solution = PTHREAD_MUTEX_INITIALIZER;

void make_saturated_list(int * set, int size, word * U, int W, int * nncnt, int * saturated_vertex, int & saturated_size){
	int u = set[size-1];
	saturated_size = 0;

	word R[NWORDS(Vnbr)];

	for(int k=0; k <= W; k++){
		R[k] = U[k];
	}

	for(int j = 0; j < size-1; j++){
		int v = set[j];
		if( !test(pbit[u], v) ){
			nncnt[v]++;
		}
	}

	int iv = 0, jv = LSB(R, iv, W);

	while(jv >= 0){
		int v = indexbit(iv, jv);

		if(!test(pbit[u], v)){
			nncnt[v]++;
		}

		resetbit(R, v);
		jv = LSB(R, iv, W);
	}

	for(int j = 0; j < size; j++){
		int v = set[j];
		if( nncnt[v] == s-1){
			saturated_vertex[saturated_size++] = v;
		}
	}
}

bool isPlex(int * S, int satured_size, int v){
	for(int i=0; i<satured_size; i++){
		int u = S[i];

		if(!test(pbit[v], u)) return false;
	}

	return true;
}


void generate(word * U, int W, int level, word * newU, int * nncnt, int * set){
	word R[NWORDS(Vnbr)];
	for(int i=0; i<=W; i++){
		R[i] = U[i];
		newU[i] = 0LL;
	}

	int iv, jv;

	int S[Vnbr];
	int satured_size = 0;

	make_saturated_list(set, level, U, W, nncnt, S, satured_size);

	iv = 0;
	jv = LSB(R, iv, W);

	while(jv >= 0){
		int v = indexbit(iv, jv);

		if(nncnt[v] <= s-1 && isPlex(S, satured_size, v)){
			setbit(newU, v);
		}

		resetbit(R, v);
		jv = LSB(R, iv, W);
	}
}


bool isPlex3(int level, int * set){
    word C[NWORDS(Vnbr)];

    int W = high(Vnbr);

    for(int i=0; i<=W; i++) C[i] = 0LL;

    for(int i=0; i<level; i++) setbit(C, set[i]);

    word S[W+1];

    for(int i=0; i<level; i++){
        int v = set[i];

        for(int k = 0; k <= W; k++) S[k] = C[k] & pbit[v][k];

        if( COUNT(S, W)  < level - s) return false;

    }
    return true;
}

int readFile(FILE * graphFile, int & n, int  & m) {
	char type  = ' ';
	char linestr[1024];
	char * datastr;
	long i, j;
	int nedges;

	while (type != 'p') {
		type = fgetc(graphFile);
		if (type != EOF) {
			/* header */
			if (type == 'c') {
				datastr = fgets(linestr, 1024, graphFile);
				if (datastr == NULL)
					return -1;
			}

			/* Vertices */
			if (type == 'p') {
				datastr = fgets(linestr, 1024, graphFile);
				if (datastr == NULL)
					return -1;

				datastr = strtok(linestr," ");

				datastr = strtok(NULL," ");
				n = atoi(datastr);

				datastr = strtok(NULL," ");
				m = atoi(datastr);

			}
		}
	}

	printf("Graph with %d vertices and %d edges density %f\n", n, m, (double)(2*m)/(n*(n-1)) );


  	for(i=0; i<n; i++) degree[i] = 0;
  	for(i=0;i<n;i++)
		for(j=0;j<n/INT_SIZE+1;j++)
  			bit[i][j] = 0;

  	nedges = 0;
	type = fgetc(graphFile);
	while (type != EOF) {
		/* Edges */
		if (type == 'e') {
			datastr = fgets(linestr, 100, graphFile);


			if (datastr == NULL)
				return -1;

			datastr = strtok(linestr," ");
			i = atol(datastr) - 1;

			datastr = strtok(NULL," ");
			j = atol(datastr) - 1;

  			if( i != j ){
				nedges++;
				setbit(bit[i], j);
				setbit(bit[j], i);
				degree[i]++;
				degree[j]++;
  			}

		}
		type = fgetc(graphFile);
	}

	int count = 0;

	for(int i = 0; i < n; i++){
		for(int j = 0; j < n; j++){
			if( test(bit[i],j) ) count++;
		}
	}

	count = count / 2;


	if( count != m ){
		printf("Error bit adjacency graph representation\n");
		exit(1);
	}

	return 0;
}

void degree_order(){
	bool used[MAX_VERTEX];
	int count;
	int delta = 0;
	int t = 0;

	/* order vertices */
	for(int i = 0; i < Vnbr; i++) {
		degree[i] = 0;
		for(int j=0;j<Vnbr;j++){
  			if ( test(bit[i], j) ) {
				degree[i]++;
				t++;
			}
		}
		if ( degree[i] > delta) delta = degree[i];
  	}

	for(int i=0;i<Vnbr;i++)
		used[i] = false;

	count = Vnbr - 1;

	do{
		int min_degree;
		int p;
		min_degree = delta + 1;
		p = -1;

		for(int i=Vnbr-1;i>=0;i--){
			if((!used[i])&&(degree[i]<min_degree)){
				min_degree = degree[i];
				p = i;
			}
		}

		pos[count--] = p;
		used[p] = true;
		for(int j=0;j<Vnbr;j++){
			if((!used[j])&&(j!=p)&&(test(bit[p],j))){
				degree[j]--;
			}
		}
	} while(count >= 0);
}

void branching(word * U, int W, int bound){
	word R[NWORDS(Vnbr)];
	word S[NWORDS(Vnbr)];
	int UB = 0;
	int stable[Vnbr];
	int count = 0;

	for(int i=0; i<=W; i++){
		R[i] = U[i];
	}

	UB = bound;

	while(1){
		int iv, jv;

		if(UB <= 0) break;

		for(int k = 0; k <= W; k++){
			S[k] = R[k];
		}

		iv = 0;
		jv = LSB(S, iv, W);

		if(jv < 0) break;

		count = 0;

		while(jv >= 0){
			int v = indexbit(iv, jv);

			stable[count++] = v;

			for(int k = 0; k <= W; k++){
				S[k] = S[k] & ~pbit[v][k];
			}

			resetbit(S, v);

			jv = LSB(S, iv, W);
		}

		for(int k = 0; k < count && k < UB; k++){
			int v = stable[k];
			resetbit(U, v);
			resetbit(R, v);
		}

		UB -= count > s ? s : count;
	}

	return;
}


void thread_slave(task c){
	int iv, jv;
	int iu, ju;
	word R[NWORDS(Vnbr)];

	int W = c.W;
	int level = c.level;
	int * nncnt = c.nncnt;
	int * set = c.set;
	word * U = c.U;

	if(level + COUNT(U, W) <= record){
		free(c.set);
		free(c.U);
		free(c.nncnt);
		return;
	}

	if(COUNT(U, W) == 0){

		pthread_mutex_lock(&mutex_update_solution);
		if(level > record){
			record = level;
			if(!isPlex3(level, set)){
				printf("Oh, wait! The set isn't a k-plex\n");
				exit(0);
			}
			for(int i = 0; i < level; i++) rec[i] = set[i];
			printf("improving solution size %d\n", record);
		}
		pthread_mutex_unlock(&mutex_update_solution);
	}


	/*gettimeofday(&stop, NULL);

	duracao = ((double) (stop.tv_sec * 1000000 + stop.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)) / 1000000;

	if(duracao >= timeout){
		printf("TIMEOUT\n");
		printf("best %d-plex: ", s);
		for(int i = 0; i < record; i++){
			printf("%d ", pos[rec[i]] + 1);
		}

		printf("\n");
		printf("record          =  %10d\n", record );
	  	//printf("subp            =  %10lld\n", subp );
		printf("time            =  %10.5f\n", duracao );
		//tp.~ThreadPool();
	}*/

	for(int k = 0; k <= W; k++){
		R[k] = U[k];
	}

	branching(R, W, record - level);
	iv = 0;
	jv = LSB(R, iv, W);
	iu = 0;
	ju = LSB(U, iu, W);

	while(jv >= 0){
		int v = indexbit(iv, jv);

		task c2;
		c2.W = W;
		c2.level = level+1;
		c2.set = (int*)malloc(sizeof(int)*Vnbr);
		c2.U = (word*)malloc(sizeof(word)*NWORDS(Vnbr));
		c2.nncnt = (int*)malloc(sizeof(int)*Vnbr);

		for(int i=0; i<Vnbr; i++){
			c2.set[i] = set[i];
			c2.nncnt[i] = nncnt[i];
		}

		c2.set[level] = v;
		resetbit(R, v);
		resetbit(U, v);
		jv = LSB(R, iv, W);
		ju = LSB(U, iu, W);

		generate(U, W, c2.level, c2.U, c2.nncnt, c2.set);

    if(level + COUNT(c2.U, c2.W) >= record){
      int seed = chrono::system_clock::now().time_since_epoch().count();
      default_random_engine generator (seed);
      uniform_int_distribution<int> distribution(1,10);
      int dice_roll = distribution(generator);

      if(dice_roll >= 6)
        thread_slave(c2);
      else{
        tp->enqueue(c2);
      }
    }
	}

	free(c.U);
	free(c.set);
	free(c.nncnt);
}

void thread_master(){
  tp = new ThreadPool(nt, thread_slave);
	task c;
	c.U = (word*)malloc(sizeof(word)*NWORDS(Vnbr));
	c.nncnt = (int*)malloc(sizeof(int)*Vnbr);
	c.set = (int*)malloc(sizeof(int)*Vnbr);
	c.W = high(Vnbr);
	c.level = 0;

	int W = c.W;
	int level = c.level;
	int * nncnt = c.nncnt;
	int * set = c.set;
	word * U = c.U;

  	for(int k = 0; k < NWORDS(Vnbr); k++){
  		c.U[k] = 0LL;
  	}

  	for(int i = 0; i < Vnbr; i++){
		setbit(c.U, i);
  	}

	for(int i = 0; i < Vnbr; i++){
		c.nncnt[i] = 0;
	}

	tp->run();

	int iv, jv;
	int iu, ju;
	word R[NWORDS(Vnbr)];

	for(int k = 0; k <= W; k++){
		R[k] = U[k];
	}

	branching(R, W, record - level);
	iv = 0;
	jv = LSB(R, iv, W);
	iu = 0;
	ju = LSB(U, iu, W);

	while(jv >= 0){
		int v = indexbit(iv, jv);

		task c2;
		c2.W = W;
		c2.level = level+1;
		c2.set = (int*)malloc(sizeof(int)*Vnbr);
		c2.U = (word*)malloc(sizeof(word)*NWORDS(Vnbr));
		c2.nncnt = (int*)malloc(sizeof(int)*Vnbr);

		for(int i=0; i<Vnbr; i++){
			c2.set[i] = set[i];
			c2.nncnt[i] = nncnt[i];
		}

		c2.set[level] = v;
		resetbit(R, v);
		resetbit(U, v);
		jv = LSB(R, iv, W);
		ju = LSB(U, iu, W);

		generate(U, W, c2.level, c2.U, c2.nncnt, c2.set);
		tp->enqueue(c2);
	}
}



int main(int argc, char *argv[]){
	FILE *infile;

  	/* read input */
  	if(argc < 4) {
		printf("Usage: BB timeout nt k infile\n");
		printf("k: k-plex size\n");
		printf("infile: input file path\n");
		exit(1);
  	}

	timeout = atoi( argv[1] );

	printf("timout %d\n", timeout);

  	nt = atoi( argv[2] );

  	printf("nt %d\n", nt );

  	if((infile=fopen(argv[4],"r"))==NULL){
		printf("Error in graph file\n %s\n", argv[4]);
		exit(0);
  	}



	s = atoi(argv[3]);

  	/* read graph */
  	readFile(infile, Vnbr, Enbr);

  	/* "start clock" */
  	gettimeofday(&start, NULL);

	mask[0] = 1LL;
	for(int i=1;i<INT_SIZE;i++){
		mask[i] = mask[i-1]<<1;
	}

  	//ordenaçao
  	degree_order();

	//Reconstrução da matriz adjacência
	for(int i = 0; i < Vnbr; i++){
		for(int k = 0; k < NWORDS(Vnbr); k++){
			pbit[i][k] = 0LL;
		}

		for(int j = 0; j < Vnbr; j++){
			if( test( bit[pos[i]], pos[j] ) ){
				setbit( pbit[i], j);
			}
		}
	}


  	//subp  = 0LL;
  	printf("start search\n");

	thread_master();

	gettimeofday(&stop, NULL);

	duracao = ((double) (stop.tv_sec * 1000000 + stop.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)) / 1000000;

	printf("best %d-plex: ", s);
	for(int i = 0; i < record; i++){
		printf("%d ", pos[rec[i]] + 1);
	}

	printf("\n");
  	printf("record          =  %10d\n", record );
  	printf("time            =  %10.5lf\n", duracao );

	return 0;
}
