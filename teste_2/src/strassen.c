/*###########################
 *Author: Gabriel B Moro
 *---------------------------
 *Matrices Multiplication
 *###########################
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define HPCELO_DECLARE_TIMER double hpcelo_t1, hpcelo_t2;
#define HPCELO_START_TIMER  hpcelo_t1 = hpcelo_gettime();
#define HPCELO_END_TIMER    hpcelo_t2 = hpcelo_gettime();
#define HPCELO_REPORT_TIMER {printf("HPCELO:%f\n", hpcelo_t2-hpcelo_t1);}

/* 
 * hpcelo_gettime: this function returns the current time in seconds
 * with a microsecond resolution. It uses =gettimeofday= call. 
 */
double hpcelo_gettime (void);

/* 
 * hpcelo_create_matrix: this function dynamically allocates a matrix
 * with size*size, considering the argument size. After this, it
 * initializes the matrix using a pseudo-random number generator known
 * as linear congruential generator (see wikipedia for details).
 */
double *hpcelo_create_matrixC (unsigned long long size);


/*
 * hpcelo_free_matrix: this function frees the allocated matrix.
 */
void hpcelo_free_matrixC (double *matrix);

/* 
 * hpcelo_create_matrix: this function dynamically allocates a matrix
 * with size*size, considering the argument size. After this, it
 * initializes the matrix using a pseudo-random number generator known
 * as linear congruential generator (see wikipedia for details).
 */
double **hpcelo_create_matrix (unsigned long long size);


/*
 * hpcelo_free_matrix: this function frees the allocated matrix.
 */
void hpcelo_free_matrix (double **matrix, unsigned long long size);


//next function has been found here:
//http://stackoverflow.com/questions/26237419/faster-than-rand
//all credits to the authors there
static unsigned int g_seed;

void printerOfMatrix(double *, int);

void strassenMultiply(double *, double *, double *, int);

int main(int argc, char *argv[]) 
{

	int HIDE_TIMER=0;
	/*Inicializacao de variaveis*/
	int size=10;

	double *A, *B, *R;

	if (argc == 2){
	  size = atoi(argv[1]);
	}
	if (argc > 2) {
		size = atoi(argv[1]);
		HIDE_TIMER = atoi(argv[2]);
	}

	A = hpcelo_create_matrixC (size);
	B = hpcelo_create_matrixC (size);
	R = hpcelo_create_matrixC (size);

	HPCELO_DECLARE_TIMER;
	HPCELO_START_TIMER;

	strassenMultiply(A,B,R,size);

	HPCELO_END_TIMER;

	if(HIDE_TIMER!=1)
	{
		HPCELO_REPORT_TIMER;
		/*Temos que imprimir isso se não o compilador 
		não faz a multiplicação por não se justificar essa operação*/
		//printf("The last element is %.2f\n", R[size*size-1]);
	}else
	{
		printerOfMatrix(R,size);
	}
	/*Temos que imprimir isso se não o compilador 
	não faz a multiplicação por não se justificar essa operação*/
	
	//printf("The last element is %.2f\n", R[size-1][size-1]);

	hpcelo_free_matrixC(A);
	hpcelo_free_matrixC(B);
	hpcelo_free_matrixC(R);

	return 0;
}

void strassenMultiply(double *A, double *B, double *R, int size) {
	int i,j,k;
	double mat1,mat2,mat3,mat4,mat5,mat6,mat7;
	int b=0;
	b = size / 2;

	#pragma omp parallel for private(i,j,k,mat1,mat2,mat3,mat4,mat5,mat6,mat7)
	for(i=0; i < b; i++){
		for(j=0; j < b; j++) {
			mat1=0,mat2=0,mat3=0,mat4=0,mat5=0,mat6=0,mat7=0;
			for(k=0; k < b; k++) {
				mat1 = mat1 + (A[(i * size) + k] + A[((i+b) * size) + k + b]) * (B[(k * size) + j] + B[((k+b) * size) + j+b]);
				mat2 = mat2 + (A[((i+b) * size) + k] + A[((i+b) * size) + k + b]) * B[(k * size) + j];
				mat3 = mat3 + A[(i * size) + k] * (B[(k * size) + j + b] - B[((k+b) * size) + j + b]);
				mat4 = mat4 + A[((i+b) * size) + k + b] * (B[((k+b) * size) + j] - B[(k * size) + j]);
				mat5 = mat5 + (A[(i * size) + k] + A[(i * size) + k + b]) * B[((k+b) * size) + j + b];
				mat6 = mat6 + (A[((i+b) * size) + k] - A[(i * size) + k]) * (B[(k * size) + j] + B[(k * size) + j + b]);
				mat7 = mat7 + (A[(i * size) + k + b] - A[((i+b) * size) + k + b]) * (B[((k+b) * size) + j] + B[((k+b) * size) + j + b]);
			}
			R[(i * size) + j] = mat1 + mat4 - mat5 + mat7;
			R[(i * size) + j + b] = mat3 + mat5;
			R[((i+b) * size) + j] = mat2 + mat4;
			R[((i+b) * size) + j + b] = mat1 + mat3 - mat2 + mat6;
		}
	}
}


void printerOfMatrix(double *R, int size)
{
	int i,j;

	for(i=0; i < size; i++)
	{
		for(j=0; j < size; j++)
			 printf("%f\n", R[i * size + j]);
	}
}

static inline int fastrand()
{
  g_seed = (214013*g_seed+2531011);
  return (g_seed>>16)&0x7FFF;
}

double hpcelo_gettime (void)
{
  struct timeval tr;
  gettimeofday(&tr, NULL);
  return (double)tr.tv_sec+(double)tr.tv_usec/1000000;
}

double *hpcelo_create_matrixC (unsigned long long size)
{
  double *matrix = malloc(size*size * sizeof(double));
  /* Error checking */
  unsigned long long i,j;
  for (i=0; i < size; i++){
      for(j=0; j < size; j++){
        matrix[i * size + j] = (double) fastrand();
      }
  }
  return matrix;
}


void hpcelo_free_matrixC (double *matrix)
{
  free(matrix);
  return;
}

double **hpcelo_create_matrix (unsigned long long size)
{
  double **matrix = malloc(size * sizeof(double*));
  /* Error checking */
  unsigned long long i, j;
  for (i=0; i < size; i++){
    matrix[i] = malloc(size * sizeof(double));
    for (j = 0; j < size; j++){
      matrix[i][j] = (double) fastrand();
    }
  }
  return matrix;
}

void hpcelo_free_matrix (double **matrix, unsigned long long size)
{
  unsigned long long i;
  for (i=0; i < size; i++){
    free(matrix[i]);
  }
  free(matrix);
  return;
}
