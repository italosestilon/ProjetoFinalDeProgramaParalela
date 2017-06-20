#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <iostream>
#include "bitmap.h"

#define MAX_VERTEX 20000

using namespace std;

int Vnbr,Enbr;
word bit[MAX_VERTEX][NWORDS(MAX_VERTEX)];
word pbit[MAX_VERTEX][NWORDS(MAX_VERTEX)];
int degree[MAX_VERTEX];
int pos[MAX_VERTEX];
int rec[MAX_VERTEX];
int record;
int set[MAX_VERTEX];
word mask[INT_SIZE];
int level;
long long int subp;
clock_t clk;
int timeout;
double elapsed;
int s = 2;
int cnt = 0;

void make_saturated_list(int * set, int size, word * U, int W, int * nncnt, int * saturated_vertex, int & saturated_size){
	int u = set[size-1];
	saturated_size = 0;

	word R[NWORDS(Vnbr)];

	for(int k=0; k <= W; k++){
		R[k] = U[k];
	}

	// Complexity O(|P'|)
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


void geracao(word * U, int W, int level, word * newU, int * nncnt){
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


bool isPlex3(int level){
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

	int contador = 0;

	for(int i = 0; i < n; i++){
		for(int j = 0; j < n; j++){
			if( test(bit[i],j) ) contador++;
		}
	}

	contador = contador / 2;


	if( contador != m ){
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

void branching(word * U, int W, int limite){
	word R[NWORDS(Vnbr)];
	word S[NWORDS(Vnbr)];
	int UB = 0;
	int estavel[Vnbr];
	int count = 0;

	for(int i=0; i<=W; i++){
		R[i] = U[i];
	}

	UB = limite;

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

			estavel[count++] = v;

			for(int k = 0; k <= W; k++){
				S[k] = S[k] & ~pbit[v][k];
			}

			resetbit(S, v);

			jv = LSB(S, iv, W);
		}

		for(int k = 0; k < count && k < UB; k++){
			int v = estavel[k];
			resetbit(U, v);
			resetbit(R, v);
		}

		UB -= count > s ? s : count;
	}

	return;
}


void basicPlexBranching(word * U, int W, int level, int * nncnt){
	int iv, jv;
	int iu, ju;
	word newU [NWORDS(Vnbr)];
	word R[NWORDS(Vnbr)];

	if(COUNT(U, W) == 0 && level > record){
		record = level;
		if(!isPlex3(level)){
			printf("Não é um k-plex\n");
			exit(0);
		}
		for(int i = 0; i < level; i++) rec[i] = set[i];
		printf("improving solution size %d\n", record);
	}

	subp++;

	if(++cnt > 1000) {
		cnt = 0;
		elapsed = ((double) (clock() - clk)) / CLOCKS_PER_SEC;
		if(elapsed >= timeout) {
			printf("TIMEOUT\n");
			printf("best %d-plex: ", s);
			for(int i = 0; i < record; i++){
				printf("%d ", pos[rec[i]] + 1);
			}

			printf("\n");
		  	printf("record          =  %10d\n", record );
		  	//printf("subp            =  %10lld\n", subp );
		  	printf("time            =  %10.5f\n", elapsed );
			exit(0);
		}

	}

	for(int k = 0; k <= W; k++){
		R[k] = U[k];
	}
	branching(R, W, record - level);
	iv = 0;
	jv = LSB(R, iv, W);
	iu = 0;
	ju = LSB(U, iu, W);
	int newnncnt[Vnbr];

	while(jv >= 0){
		int v = indexbit(iv, jv);

		for(int i=0; i<Vnbr; i++){
			newnncnt[i] = nncnt[i];
		}

		set[level] = v;
		resetbit(R, v);
		resetbit(U, v);
		jv = LSB(R, iv, W);
		ju = LSB(U, iu, W);


		geracao(U, W, level+1, newU, newnncnt);
		basicPlexBranching(newU, W, level+1, newnncnt);
	}
}



int main(int argc, char *argv[]){
	FILE *infile;

  	/* read input */
  	if(argc < 3) {
		printf("Usage: BB [timeout] k infile\n");
		printf("k: k-plex size\n");
		printf("infile: input file path\n");
		exit(1);
  	}

  	timeout = atoi( argv[1] );

  	printf("timeout %d s\n", timeout );

  	if((infile=fopen(argv[3],"r"))==NULL){
		printf("Error in graph file\n %s\n", argv[2]);
		exit(0);
  	}


	if(argv[3])
		s = atoi(argv[2]);

  	/* read graph */
  	readFile(infile, Vnbr, Enbr);

  	/* "start clock" */
  	clk = clock();

	mask[0] = 1LL;
	for(int i=1;i<INT_SIZE;i++)
		mask[i] = mask[i-1]<<1;

  	//ordenaçao
  	degree_order();

	//Reconstrução da matriz adjacência
	int contador = 0;
	for(int i = 0; i < Vnbr; i++){
		for(int k = 0; k < NWORDS(Vnbr); k++){
			pbit[i][k] = 0LL;
		}

		for(int j = 0; j < Vnbr; j++){
			if( test( bit[pos[i]], pos[j] ) ){
				setbit( pbit[i], j);
				contador++;
			}
		}
	}

  	word U[ NWORDS(Vnbr) ];
  	for(int k = 0; k < NWORDS(Vnbr); k++){
  		U[k] = 0LL;
  	}

  	for(int i = 0; i < Vnbr; i++){
		setbit(U, i);
  	}


  	subp  = 0LL;
  	printf("start search\n");


	int nncnt[Vnbr];

	for(int i = 0; i < Vnbr; i++){
		nncnt[i] = 0;
	}

	basicPlexBranching(U, high(Vnbr), 0, nncnt);

  	elapsed = ((double) (clock() - clk)) / CLOCKS_PER_SEC;

	printf("best %d-plex: ", s);
	for(int i = 0; i < record; i++){
		printf("%d ", pos[rec[i]] + 1);
	}

	printf("\n");
  	printf("record          =  %10d\n", record );
  	//printf("subp            =  %10lld\n", subp );
  	printf("time            =  %10.5f\n", elapsed );

	return 0;
}
