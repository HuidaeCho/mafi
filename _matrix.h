#ifndef __MATRIX_H_
#define __MATRIX_H_

#undef	GLOBAL
#ifdef	_MAFI_C_
#define	GLOBAL
#else
#define	GLOBAL	extern
#endif

#define	_VALID_MATRIX	0
#define	_INVALID_MATRIX	1

#define UPPER_MATRIX	0x1
#define LOWER_MATRIX	0x2

#define	RM_MATRIX	0x4

#define FORCE_MATRIX	0x8

struct _matrix{
	int m,n;
	double *elements;
};

GLOBAL	int _MATRIX_ERROR;

#define	_element(A,i,j)	*((A)->elements+(A)->n*((i)-1)+((j)-1))

struct _matrix *_empty_matrix(int m,int n);
struct _matrix *_new_matrix(int m,int n,double element);
struct _matrix *_unity_matrix(int n);
struct _matrix *_copy_matrix(struct _matrix *A);
struct _matrix *_same_matrix(struct _matrix *A,struct _matrix *B);
struct _matrix *_fill_matrix(struct _matrix *A,double element);

struct _matrix *_multiply_matrix(struct _matrix *A,struct _matrix *B,int mode);
struct _matrix *_add_matrix(struct _matrix *A,struct _matrix *B,int mode);
struct _matrix *_subtract_matrix(struct _matrix *A,struct _matrix *B,int mode);
struct _matrix *_multiply_matrix_element(struct _matrix *A,struct _matrix *B,int mode);
struct _matrix *_scalar_matrix(double k,struct _matrix *A,int mode);
struct _matrix *_increase_matrix(double k,struct _matrix *A,int mode);
struct _matrix *_transpose_matrix(struct _matrix *A,int mode);
struct _matrix *_combine_matrix(struct _matrix *A,struct _matrix *B,int r,int c,int mode);
struct _matrix *_extract_matrix(struct _matrix *A,int r,int c,int nr,int nc,int mode);

/*** Solve Matrix ***/
struct _matrix *_scale_matrix(int s,struct _matrix *A,int mode);
struct _matrix *_pivot_matrix(int p,struct _matrix *A,int mode);
struct _matrix *_eliminate_matrix(int e,struct _matrix *A,int mode);
struct _matrix *_identify_matrix(struct _matrix *A,int mode);
struct _matrix *_back_substitute(struct _matrix *A,int mode);
struct _matrix *_gaussjordan(struct _matrix *A,int mode);
struct _matrix *_inverse_matrix(struct _matrix *A,int mode);

/*** Clean Matrix ***/
int _free_matrix(struct _matrix *A);

/*** Matrix I/O ***/
int _print_matrix(char *out,int len,struct _matrix *A,int mode);
struct _matrix *_scan_matrix(struct _matrix *A);
/******************/

#endif
