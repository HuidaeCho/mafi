#define	__MATRIX_C_
#include "mafi.h"

struct _matrix *_empty_matrix(m,n)
int m,n;
{
	struct _matrix *A;
	
	_MATRIX_ERROR=_VALID_MATRIX;
	if(m<1||n<1){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}
	
	A=(struct _matrix *)malloc(sizeof(struct _matrix));
	if(!A){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}

	A->m=m;
	A->n=n;
	A->elements=(double *)malloc(m*n*sizeof(double));
	if(!A->elements){
		_MATRIX_ERROR=_INVALID_MATRIX;
		free(A);
		return(NULL);
	}
	return(A);
}

struct _matrix *_new_matrix(m,n,element)
int m,n;
double element;
{
	int i;
	struct _matrix *A;
	
	_MATRIX_ERROR=_VALID_MATRIX;
	if(m<1||n<1){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}
	
	A=(struct _matrix *)malloc(sizeof(struct _matrix));
	if(!A){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}

	A->m=m;
	A->n=n;
	A->elements=(double *)malloc(m*n*sizeof(double));
	if(!A->elements){
		_MATRIX_ERROR=_INVALID_MATRIX;
		free(A);
		return(NULL);
	}

	for(i=0;i<m*n;i++)
		*(A->elements+i)=element;
	return(A);
}

struct _matrix *_unity_matrix(n)
int n;
{
	int i,j;
	struct _matrix *A;
	
	_MATRIX_ERROR=_VALID_MATRIX;
	if(n<1){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);	
	}
	
	A=(struct _matrix *)malloc(sizeof(struct _matrix));
	if(!A){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}

	A->m=A->n=n;
	A->elements=(double *)malloc(n*n*sizeof(double));
	if(!A->elements){
		_MATRIX_ERROR=_INVALID_MATRIX;
		free(A);
		return(NULL);
	}

	for(i=1;i<=n;i++)
		for(j=1;j<=n;j++)
			_element(A,i,j)=(i==j? 1:0);
	return(A);
}

struct _matrix *_copy_matrix(A)
struct _matrix *A;
{
	int i;
	struct _matrix *C;

	_MATRIX_ERROR=_VALID_MATRIX;
	if(A->m<1||A->n<1){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}

	C=(struct _matrix *)malloc(sizeof(struct _matrix));
	if(!C){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}
	 
	C->m=A->m;
	C->n=A->n;
	C->elements=(double *)malloc(C->m*C->n*sizeof(double));
	if(!C->elements){
		_MATRIX_ERROR=_INVALID_MATRIX;
		free(C);
		return(NULL);
	}

	for(i=0;i<A->m*A->n;i++)
		*(C->elements+i)=*(A->elements+i);
	return(C);
}

struct _matrix *_same_matrix(A,B)
struct _matrix *A,*B;
{
	int i;

	_MATRIX_ERROR=_VALID_MATRIX;
	if((A->m<1||A->n<1)||(A->m!=B->m||A->n!=B->n)){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}

	for(i=0;i<A->m*A->n;i++)
		*(A->elements+i)=*(B->elements+i);
	return(A);
}

struct _matrix *_fill_matrix(A,element)
struct _matrix *A;
double element;
{
	int i;

	_MATRIX_ERROR=_VALID_MATRIX;
	if(A->m<1||A->n<1){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}

	for(i=0;i<A->m*A->n;i++)
		*(A->elements+i)=element;
	return(A);
}

struct _matrix *_multiply_matrix(A,B,mode)
struct _matrix *A,*B;
int mode;
{
	int i,j,k;
	struct _matrix *C;

	_MATRIX_ERROR=_VALID_MATRIX;
	if((A->m<1||A->n<1||B->m<1||B->n<1)||(A->n!=B->m)){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}

	C=(struct _matrix *)malloc(sizeof(struct _matrix));
	if(!C){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}

	C->m=A->m;
	C->n=B->n;
	C->elements=(double *)malloc(C->m*C->n*sizeof(double));
	if(!C->elements){
		_MATRIX_ERROR=_INVALID_MATRIX;
		free(C);
		return(NULL);
	}

	for(i=0;i<C->m*C->n;i++)
		*(C->elements+i)=0;

	for(i=1;i<=C->m;i++)
		for(j=1;j<=C->n;j++)
			for(k=1;k<=A->n;k++)
				_element(C,i,j)+=_element(A,i,k)
						*_element(B,k,j);
	if(mode&RM_MATRIX){
		free(A->elements);
		free(A);
		free(B->elements);
		free(B);
	}
	return(C);		
}

struct _matrix *_add_matrix(A,B,mode)
struct _matrix *A,*B;
int mode;
{
	int i;
	struct _matrix *C;
	
	_MATRIX_ERROR=_VALID_MATRIX;
	if((A->m<1||A->n<1)||(A->m!=B->m||A->n!=B->n)){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}
	
	C=(struct _matrix *)malloc(sizeof(struct _matrix));
	if(!C){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}

	C->m=A->m;
	C->n=A->n;
	C->elements=(double *)malloc(C->m*C->n*sizeof(double));
	if(!C->elements){
		_MATRIX_ERROR=_INVALID_MATRIX;
		free(C);
		return(NULL);
	}

	for(i=0;i<C->m*C->n;i++)
		*(C->elements+i)=*(A->elements+i)+*(B->elements+i);

	if(mode&RM_MATRIX){
		free(A->elements);
		free(A);
		free(B->elements);
		free(B);
	}
	return(C);
}

struct _matrix *_subtract_matrix(A,B,mode)
struct _matrix *A,*B;
int mode;
{
	int i;
	struct _matrix *C;
	
	_MATRIX_ERROR=_VALID_MATRIX;
	if((A->m<1||A->n<1)||(A->m!=B->m||A->n!=B->n)){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}
	
	C=(struct _matrix *)malloc(sizeof(struct _matrix));
	if(!C){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}

	C->m=A->m;
	C->n=A->n;
	C->elements=(double *)malloc(C->m*C->n*sizeof(double));
	if(!C->elements){
		_MATRIX_ERROR=_INVALID_MATRIX;
		free(C);
		return(NULL);
	}

	for(i=0;i<C->m*C->n;i++)
		*(C->elements+i)=*(A->elements+i)-*(B->elements+i);

	if(mode&RM_MATRIX){
		free(A->elements);
		free(A);
		free(B->elements);
		free(B);
	}
	return(C);
}

struct _matrix *_multiply_matrix_element(A,B,mode)
struct _matrix *A,*B;
int mode;
{
	int i;
	struct _matrix *C;
	
	if((A->m<1||A->n<1)||(A->m!=B->m||A->n!=B->n)){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}
	
	C=(struct _matrix *)malloc(sizeof(struct _matrix));
	if(!C){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}

	C->m=A->m;
	C->n=A->n;
	C->elements=(double *)malloc(C->m*C->n*sizeof(double));
	if(!C->elements){
		_MATRIX_ERROR=_INVALID_MATRIX;
		free(C);
		return(NULL);
	}

	for(i=0;i<C->m*C->n;i++)
		*(C->elements+i)=*(A->elements+i)**(B->elements+i);

	if(mode&RM_MATRIX){
		free(A->elements);
		free(A);
		free(B->elements);
		free(B);
	}
	return(C);
}

struct _matrix *_scalar_matrix(k,A,mode)
double k;
struct _matrix *A;
int mode;
{
	int i;
	struct _matrix *C;

	_MATRIX_ERROR=_VALID_MATRIX;
	if(A->m<1||A->n<1){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}

	C=(struct _matrix *)malloc(sizeof(struct _matrix));
	if(!C){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}

	C->m=A->m;
	C->n=A->n;
	C->elements=(double *)malloc(C->m*C->n*sizeof(double));
	if(!C->elements){
		_MATRIX_ERROR=_INVALID_MATRIX;
		free(C);
		return(NULL);
	}
	
	for(i=0;i<A->m*A->n;i++)
		*(C->elements+i)=*(A->elements+i)*k;

	if(mode&RM_MATRIX){
		free(A->elements);
		free(A);
	}
	return(C);
}

struct _matrix *_increase_matrix(k,A,mode)
double k;
struct _matrix *A;
int mode;
{
	int i;
	struct _matrix *C;
	
	_MATRIX_ERROR=_VALID_MATRIX;
	if(A->m<1||A->n<1){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}

	C=(struct _matrix *)malloc(sizeof(struct _matrix));
	if(!C){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}

	C->m=A->m;
	C->n=A->n;
	C->elements=(double *)malloc(C->m*C->n*sizeof(double));
	if(!C->elements){
		_MATRIX_ERROR=_INVALID_MATRIX;
		free(C);
		return(NULL);
	}

	for(i=0;i<A->m*A->n;i++)
		*(C->elements+i)=*(A->elements+i)+k;

	if(mode&RM_MATRIX){
		free(A->elements);
		free(A);
	}
	return(C);
}

struct _matrix *_transpose_matrix(A,mode)
struct _matrix *A;
int mode;
{	
	int i,j;
	struct _matrix *C;	
	
	_MATRIX_ERROR=_VALID_MATRIX;
	if(A->m<1||A->n<1){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}
	
	C=(struct _matrix *)malloc(sizeof(struct _matrix));
	if(!C){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}

	C->m=A->n;
	C->n=A->m;
	C->elements=(double *)malloc(C->m*C->n*sizeof(double));
	if(!C->elements){
		_MATRIX_ERROR=_INVALID_MATRIX;
		free(C);
		return(NULL);
	}
	
	for(i=1;i<=C->m;i++)
		for(j=1;j<=C->n;j++)
			_element(C,i,j)=_element(A,j,i);

	if(mode&RM_MATRIX){
		free(A->elements);
		free(A);
	}
	return(C);
}

struct _matrix *_combine_matrix(A,B,r,c,mode)
struct _matrix *A,*B;
int r,c,mode;
{
	int i,j,k,l;
	struct _matrix *C;

	_MATRIX_ERROR=_VALID_MATRIX;
	if((A->m<1||A->n<1)||(B->m<1||B->n<1)){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}
	if(!(mode&FORCE_MATRIX)&&(A->m<r+B->m-1 || A->n<c+B->n-1)){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}

	if(mode&FORCE_MATRIX){
		k=(A->m > r+B->m-1 ? A->m : r+B->m-1);
		l=(A->n > c+B->n-1 ? A->n : c+B->n-1);
	}else{
		k=A->m;
		l=A->n;
	}
	
	C=(struct _matrix *)malloc(sizeof(struct _matrix));
	if(!C){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}

	C->m=k;
	C->n=l;
	C->elements=(double *)malloc(C->m*C->n*sizeof(double));
	if(!C->elements){
		_MATRIX_ERROR=_INVALID_MATRIX;
		free(C);
		return(NULL);
	}

	for(i=1;i<=k;i++)
		for(j=1;j<=l;j++)
			if((i>=r&&i<=r+B->m-1)&&(j>=c&&j<=c+B->n-1))
				_element(C,i,j)=_element(B,i-r+1,j-c+1);
			else
			if(i<=A->m&&j<=A->n)
				_element(C,i,j)=_element(A,i,j);
			else
				_element(C,i,j)=0;

	if(mode&RM_MATRIX){
		free(A->elements);
		free(A);
		free(B->elements);
		free(B);
	}
	return(C);
}

struct _matrix *_extract_matrix(A,r,c,nr,nc,mode)
struct _matrix *A;
int r,c,nr,nc;
int mode;
{
	int i,j,k,l;
	struct _matrix *C;

	_MATRIX_ERROR=_VALID_MATRIX;
	if((A->m<1||A->n<1)||(r<1||c<1)||(r>A->m||c>A->n)||(nr<1||nc<1)){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}

	k=(A->m > r+nr-1 ? r+nr-1 : A->m);
	l=(A->n > c+nc-1 ? c+nc-1 : A->n);

	C=_new_matrix(nr,nc,0);
	if(!C){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}

	for(i=r;i<=k;i++)
		for(j=c;j<=l;j++)
			_element(C,i-r+1,j-c+1)=_element(A,i,j);

	if(mode&RM_MATRIX){
		free(A->elements);
		free(A);
	}
	return(C);
}


/*** Solve Matrix ***/
struct _matrix *_scale_matrix(s,A,mode)
int s;
struct _matrix *A;
int mode;
{
	int i;
	double scale_factor,tmp;
	struct _matrix *C;

	_MATRIX_ERROR=_VALID_MATRIX;
	if(A->m<1||A->n<1){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}
	
	C=_copy_matrix(A);
	if(!C){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}

	scale_factor=_element(C,s,1);
	for(i=2;i<=C->m;i++){
		tmp=_element(C,s,i);
		if(fabs(tmp)>fabs(scale_factor))
			scale_factor=tmp;
	}
	for(i=1;i<=C->n;i++)
		_element(C,s,i)/=scale_factor;

	if(mode&RM_MATRIX){
		free(A->elements);
		free(A);
	}
	return(C);
}

struct _matrix *_pivot_matrix(p,A,mode)
int p;
struct _matrix *A;
int mode;
{
	int i,j;
	double pivot_factor,tmp;
	struct _matrix *C;

	_MATRIX_ERROR=_VALID_MATRIX;
	if(A->m<1||A->n<1){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}
	
	C=_copy_matrix(A);
	if(!C){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}

	j=p;
	pivot_factor=_element(C,(j=p),p);
	for(i=p+1;i<=C->m;i++){
		tmp=_element(C,i,p);
		if(fabs(tmp)>fabs(pivot_factor)){
			pivot_factor=tmp;
			j=i;
		}
	}
	for(i=p;i<=C->n;i++){
		tmp=_element(C,p,i);
		_element(C,p,i)=_element(C,j,i);
		_element(C,j,i)=tmp;
	}

	if(mode&RM_MATRIX){
		free(A->elements);
		free(A);
	}
	return(C);
}

struct _matrix *_eliminate_matrix(e,A,mode)
int e;
struct _matrix *A;
int mode;
{
	int i,j;
	double tmp;
	struct _matrix *C;

	_MATRIX_ERROR=_VALID_MATRIX;
	if(A->m<1||A->n<1||e<1||e>A->m){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}

	C=_copy_matrix(A);
	if(!C){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}

	if(mode&UPPER_MATRIX)
		for(i=1;i<e;i++){
			tmp=_element(C,i,e)/_element(C,e,e);
			for(j=1;j<=C->n;j++)
				_element(C,i,j)-=_element(C,e,j)*tmp;
		}
	if(mode&LOWER_MATRIX)
		for(i=e+1;i<=C->m;i++){
			tmp=_element(C,i,e)/_element(C,e,e);
			for(j=e;j<=C->n;j++)
				_element(C,i,j)-=_element(C,e,j)*tmp;
		}

	if(mode&RM_MATRIX){
		free(A->elements);
		free(A);
	}
	return(C);
}

struct _matrix *_identify_matrix(A,mode)
struct _matrix *A;
int mode;
{
	int i,j;
	double identify_factor;
	struct _matrix *C;

	_MATRIX_ERROR=_VALID_MATRIX;
	if(A->m<1||A->n<1){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}

	C=_copy_matrix(A);
	if(!C){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}

	for(i=1;i<=C->m;i++){
		identify_factor=_element(C,i,i);
		_element(C,i,i)=1;
		for(j=C->m+1;j<=C->n;j++)
			_element(C,i,j)/=identify_factor;
	}

	if(mode&RM_MATRIX){
		free(A->elements);
		free(A);
	}
	return(C);
}

struct _matrix *_back_substitute(A,mode)
struct _matrix *A;
int mode;
{
	int i,j;
	struct _matrix *C;
	
	_MATRIX_ERROR=_VALID_MATRIX;
	if(A->m<1||A->n<1){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}

	C=_empty_matrix(A->n,1);
	if(!C){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}
	
	for(i=A->n;i>=1;i--){
		_element(C,i,1)=_element(A,i,A->n+1);
		for(j=A->n;j>i;j--)
			_element(C,i,1)-=_element(A,i,j)*_element(C,j,1);
		_element(C,i,1)/=_element(A,i,i);
	}

	if(mode&RM_MATRIX){
		free(A->elements);
		free(A);
	}
	return(C);
}

struct _matrix *_gaussjordan(A,mode)
struct _matrix *A;
int mode;
{
	int i;
	struct _matrix *C;

	_MATRIX_ERROR=_VALID_MATRIX;
	if(A->m<1||A->n<1){
		_MATRIX_ERROR=_INVALID_MATRIX;
		if(mode&RM_MATRIX){
			free(A->elements);
			free(A);
		}
		return(NULL);
	}

	C=_copy_matrix(A);
	if(!C){
		_MATRIX_ERROR=_INVALID_MATRIX;
		if(mode&RM_MATRIX){
			free(A->elements);
			free(A);
		}
		return(NULL);
	}

	for(i=1;i<=C->m;i++){
		C=_scale_matrix(i,C,RM_MATRIX);
		if(!C){
			_MATRIX_ERROR=_INVALID_MATRIX;
			if(mode&RM_MATRIX){
				free(A->elements);
				free(A);
			}
			return(NULL);
		}
	}
	for(i=1;i<=C->m;i++){
		C=_pivot_matrix(i,C,RM_MATRIX);
		if(!C){
			_MATRIX_ERROR=_INVALID_MATRIX;
			if(mode&RM_MATRIX){
				free(A->elements);
				free(A);
			}
			return(NULL);
		}
		C=_eliminate_matrix(i,C,UPPER_MATRIX|LOWER_MATRIX|RM_MATRIX);
		if(!C){
			_MATRIX_ERROR=_INVALID_MATRIX;
			if(mode&RM_MATRIX){
				free(A->elements);
				free(A);
			}
			return(NULL);
		}
	}
	C=_identify_matrix(C,RM_MATRIX);
	if(!C){
		_MATRIX_ERROR=_INVALID_MATRIX;
		if(mode&RM_MATRIX){
			free(A->elements);
			free(A);
		}
		return(NULL);
	}

	if(mode&RM_MATRIX){
		free(A->elements);
		free(A);
	}
	return(C);
}

struct _matrix *_inverse_matrix(A,mode)
struct _matrix *A;
int mode;
{
	struct _matrix *B,*C;
	int	n;

	_MATRIX_ERROR=_VALID_MATRIX;
	if(A->m<1||A->n<1||A->m!=A->n){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}

	n=A->m;
	B=_unity_matrix(n);
	if(!B){
		_MATRIX_ERROR=_INVALID_MATRIX;
		if(mode&RM_MATRIX){
			free(A->elements);
			free(A);
		}
		return(NULL);
	}
	C=_combine_matrix(A,B,1,n+1,FORCE_MATRIX);
	if(!C){
		_MATRIX_ERROR=_INVALID_MATRIX;
		free(B->elements);
		free(B);
		if(mode&RM_MATRIX){
			free(A->elements);
			free(A);
		}
		return(NULL);
	}
	C=_gaussjordan(C,RM_MATRIX);
	if(!C){
		_MATRIX_ERROR=_INVALID_MATRIX;
		free(B->elements);
		free(B);
		if(mode&RM_MATRIX){
			free(A->elements);
			free(A);
		}
		return(NULL);
	}
	B=_extract_matrix(C,1,A->n+1,B->m,B->n,RM_MATRIX);
	if(!B){
		_MATRIX_ERROR=_INVALID_MATRIX;
		if(mode&RM_MATRIX){
			free(A->elements);
			free(A);
		}
		return(NULL);
	}
	if(mode&RM_MATRIX){
		free(A->elements);
		free(A);
	}
	return(B);
}

/*** Clean Matrix ***/
int _free_matrix(A)
struct _matrix *A;
{
	_MATRIX_ERROR=_VALID_MATRIX;
	if(!A||A->m<1||A->n<1){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(1);
	}

	free(A->elements);
	free(A);
	return(0);
}

/*** Matrix I/O ***/
int _print_matrix(out,len,A,mode)
char	*out;
int	len;
struct _matrix *A;
int mode;
{
	int i,j;

	_MATRIX_ERROR=_VALID_MATRIX;
	if(A->m<1||A->n<1){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(1);
	}
	if(out)
		snprintf(out+strlen(out),len-strlen(out),"[ ");
	else
		fprintf(stdout,"[ ");
	for(i=1;i<=A->m;i++)
	{
		if(i>1){
			if(out)
				snprintf(out+strlen(out),len-strlen(out),"  ");
			else
				fprintf(stdout,"  ");
		}
		for(j=1;j<=A->n;j++){
			printv((out?out+strlen(out):NULL),
				(out?len-strlen(out):0),
#ifdef	ZERO
				(fabs(_element(A,i,j))<=ZERO ?
				 	0 : _element(A,i,j))
#else
				 _element(A,i,j)
#endif
				);
			if(j<A->n){
				if(out)
					snprintf(out+strlen(out),len-strlen(out),", ");
				else
					fprintf(stdout,", ");
			}
		}
		if(i<A->m){
			if(out)
				snprintf(out+strlen(out),len-strlen(out),";\n");
			else
				fprintf(stdout,";\n");
		}
	}
	if(out)
		snprintf(out+strlen(out),len-strlen(out)," ]");
	else
		fprintf(stdout," ]");
	if(mode&RM_MATRIX){
		free(A->elements);
		free(A);
	}
	return(0);
}

struct _matrix *_scan_matrix(A)
struct _matrix *A;
{
	int i;

	_MATRIX_ERROR=_VALID_MATRIX;
	if(A->m<1||A->n<1){
		_MATRIX_ERROR=_INVALID_MATRIX;
		return(NULL);
	}
	
	for(i=0;i<A->m*A->n;i++)
		scanf("%lf",A->elements+i);
	return(A);
}
/******************/
