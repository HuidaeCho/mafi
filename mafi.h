#ifndef	_MAFI_H_
#define	_MAFI_H_

//#define	DEBUG

#define	VERSION	0.3

#undef	GLOBAL
#ifdef	_MAFI_C_
#define	GLOBAL
#else
#define	GLOBAL	extern
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include "_matrix.h"
#ifdef	USE_GD
#include "_3d.h"
#include <gd.h>
#include <gdfontt.h>
#include <gdfonts.h>
#include <gdfontmb.h>
#include <gdfontl.h>
#include <gdfontg.h>
#endif

#define	BUFFERSIZE	(5*1024)
#define	HASHSIZE	100
#define LINESIZE	256
#define	NUMLINES	256
#define	TMPBUFSIZE	256

#define	ZEROLIMIT	1.0e-10
#define	ZERO		0	//ZEROLIMIT
#define	E		2.71828182845904523536

#define	DOIF		0x1
#define	DOVAR		0x2
#define	DOMATRIX	0x4

#define	END		-1
#define	BREAK		2
#define	CONTINUE	3
#define	RETURN		4
#define	RETURN_NULL	5

#define	outbuf		(output+strlen(output))
#define	outlen		(BUFFERSIZE-strlen(output))

#define	tmpout		(tmpbuf+strlen(tmpbuf))
#define	tmplen		(TMPBUFSIZE-strlen(tmpbuf))

#define error()		{fprintf(stderr,"\n> %s\nError: %s\n",		      \
				buffer[dolineno],tmpbuf);		      \
			 err=1;						      \
			 return 0;}

#define	ischar(c)	(isalpha((c))||(char)(c)<0)
#define	ischnum(c)	(isalnum((c))||(char)(c)<0)
#define	isvar(c)	(isalnum((c))||(char)(c)<0||(c)=='_')

#define	matx(A,i)	((A)->n<2?(i):_element((A),(i),1))
#define	maty(A,i)	_element((A),(i),((A)->n<2?1:2))

#define	mat3dx(A,i)	((A)->n<3?(i):_element((A),(i),1))
#define	mat3dy(A,i)	_element((A),(i),((A)->n<3?1:2))
#define	mat3dz(A,i)	((A)->n<2?0:_element((A),(i),((A)->n<3?2:3)))

typedef	struct	_matrix	*_PMATRIX;

typedef	double		(*SYSFUNCTYPE)(double,...);
typedef	double		(*SYSFUNCMTYPE)(_PMATRIX,...);

typedef	_PMATRIX	(*SYSMFUNCTYPE)(double,...);
typedef	_PMATRIX	(*SYSMFUNCMTYPE)(_PMATRIX,...);

typedef	_PMATRIX	(*SYSMXFUNCTYPE)(double,_PMATRIX,...);
typedef	_PMATRIX	(*SYSMXFUNCMTYPE)(_PMATRIX,double,...);

typedef struct	_VAR
{
	char		*name;
	double		value;
	struct	_VAR	*prev;
	struct	_VAR	*next;
}		VAR,*PVAR;

typedef	struct	_MATRIX
{
	char		*name;
	_PMATRIX	matrix;
	struct	_MATRIX	*prev;
	struct	_MATRIX	*next;
}		MATRIX,*PMATRIX;

typedef	struct	_STR
{
	char		*name;
	char		*str;
	struct	_STR	*prev;
	struct	_STR	*next;
}		STR,*PSTR;

typedef struct	_PRSVD
{
	char		*name;
	struct	_PRSVD	*prev;
	struct	_PRSVD	*next;
}		PRSVD,*PPRSVD;

typedef struct	_LABEL
{
	char		*name;
	int		lineno;
	struct	_LABEL	*prev;
	struct	_LABEL	*next;
}		LABEL,*PLABEL;

typedef struct	_FUNC
{
	char		*name;
	char		**parname;
	char		*def;
	int		npars;
	double		(*func)(double,...);
	double		(*mfunc)(_PMATRIX,...);
	struct	_FUNC	*prev;
	struct	_FUNC	*next;
}		FUNC,*PFUNC;

typedef struct	_MFUNC
{
	char		*name;
	char		**parname;
	char		*def;
	int		npars;
	_PMATRIX	(*func)(double,...);
	_PMATRIX	(*mfunc)(_PMATRIX,...);
	struct	_MFUNC	*prev;
	struct	_MFUNC	*next;
}		MFUNC,*PMFUNC;

typedef struct	_MXFUNC
{
	char		*name;
	char		**parname;
	char		*def;
	int		npars;
	_PMATRIX	(*func)(double,_PMATRIX,...);
	_PMATRIX	(*mfunc)(_PMATRIX,double,...);
	struct	_MXFUNC	*prev;
	struct	_MXFUNC	*next;
}		MXFUNC,*PMXFUNC;

typedef	struct	_SYSVAR
{
	char		*name;
	double		value;
}		SYSVAR;

typedef	struct	_SYSFUNC
{
	char		*name;
	int		npars;
	double		(*func)(double,...);
	double		(*mfunc)(_PMATRIX,...);
}		SYSFUNC;

typedef	struct	_SYSMFUNC
{
	char		*name;
	int		npars;
	_PMATRIX	(*func)(double,...);
	_PMATRIX	(*mfunc)(_PMATRIX,...);
}		SYSMFUNC;

typedef	struct	_SYSMXFUNC
{
	char		*name;
	int		npars;
	_PMATRIX	(*func)(double,_PMATRIX,...);
	_PMATRIX	(*mfunc)(_PMATRIX,double,...);
}		SYSMXFUNC;

//GLOBAL	int	inlineno,dolineno,cont[NUMLINES],errpos,err;
GLOBAL	int	inlineno,dolineno,errpos,err,_ret_type,func2;
GLOBAL	double	gvalue;
GLOBAL	char	*output,tmpbuf[TMPBUFSIZE],tmpbuf2[TMPBUFSIZE],*pcur,**buffer,tty;

/* funcs.c */
void		_solve_bracket_in(char *func,double x1,double x2,int n,double *xb1,double *xb2,int *nb);
int		_solve_bracket_out(char *func,double *x1,double *x2,int n,double factor);
double		_solve_brent(char *func,double x1,double x2,int n,double tol);
double		_integ_trapezd(char *func,double x1,double x2,int n);
int		_integ_gaussleg(int n,double **x,double **w);
double		_integ(char *func,double x1,double x2,int n,double *x,double *w,int method);
double		_odeint(char *func,double x0,double y0,double x,int n,int method);
double		_diff(char *func,double x,double h,int order);
double		_interp(_PMATRIX A,double x);
double		_interp2(_PMATRIX A,_PMATRIX B,_PMATRIX C,double x1,double x2);
int		_regr(_PMATRIX A,int n,char *f,char *c);
double		_fix(double x);
double		_frac(double x);
double		_sign(double x);
double		_round(double x,int i);
double		_rand(double max);
double		_m(_PMATRIX A);
double		_n(_PMATRIX A);
_PMATRIX	_M(double m,double n,double element);
_PMATRIX	_R(double m,double n,double element);
_PMATRIX	_I(double n);
#ifdef	USE_GD
double		_px(double mx);
double		_py(double my);
double		_mx(double px);
double		_my(double py);
#endif

#ifdef	_HASH_C_
/*
   //	A[]<r,c>	extract B from A
   //	A[]<r,c>B[]
   //
   //	A[]B[]		combine A and B
   //	A[]B[]<r,c>
_PMATRIX	_extract(_PMATRIX A,_PMATRIX B,double r,double c);
_PMATRIX	_combine(_PMATRIX A,_PMATRIX B,double r,double c);
   //	_Mm(A[])	m <= A(m*n)
   //	_Mn(A[])	n <= A(m*n)
   //	_Md(A[])	|A|
*/
/**/

PVAR		varhash[HASHSIZE];
PMATRIX		matrixhash[HASHSIZE];
PSTR		strhash[HASHSIZE];
PFUNC		funchash[HASHSIZE];
PMFUNC		mfunchash[HASHSIZE];
PMXFUNC		mxfunchash[HASHSIZE];
PLABEL		labelhash[HASHSIZE];
PPRSVD		prsvdhash[HASHSIZE];
char		*prsvds[]={
			/* Control Words */
			"if",
			"for",
			"goto",
			"printf",
			"sprintf",
			"eval",
			"fstr",
			"local",
			"return",
			"pause",
			"call",
			"exit",
			"srand",
			"reset",
#ifdef	USE_GD
			/* Graphing Functions */
			"grfbegin",
			"grfend",
			"grf",
			"grf3d",
			"grfxr",
			"grfyr",
			"grfzr",
			"grfxyr",
			"grfyxr",
			"grfdxy",
			"grfx",
			"grfy",
			"grft",
			"grftl",
			"grfto",
			"grfzoom",
			"grf3dvp",
			"grf3dw",
			"grfclr",
			"grfreset",
			/* Graphing Sub Functions */
			/* grid */
			"_grid",
			"_grid_",
			/* 2d, 3d */
			"_polar",
			"_polar_",
			"_para",
			"_para_",
			/* Drawing Sub Functions */
			/* 2d, 3d */
			"_dplot",
			"_dplot_",
			"_vplot",
			"_vplot_",
			"_dmesh",
			"_dmesh_",
			"_vmesh",
			"_vmesh_",
			"_str",
			"_str_",
			"_mv2",
			"_mv2_",
			"_dot",
			"_dot_",
			"_line",
			"_line_",
			"_rect",
			"_rect_",
			/* 2d */
			"_circ",
			"_circ_",
			"_elps",
			"_elps_",
			"_inv",
			"_inv_",
			/* 3d */
			"_cube",
			"_cube_",
			/* get position */
			"_getpos",
			/* fractal */
			"_julia",
			"_julia_",
			"_mandelbrot",
			"_mandelbrot_",
#endif
		};
SYSVAR		sysvars[]={
			{"_version",	VERSION},
			{"_pi",		M_PI},
			{"_e",		E},
		};
char		*sysvars2[]={
			"_ans",
			"_ans[]",
			"_ret_type",
			"_ret",
			"_ret[]",
		};
SYSFUNC		sysfuncs[]={
			/* Numerical Analysis */
			{"solve",-1,	0,0},
			{"integ",-1,	0,0},
			{"odeint",-1,	0,0},
			{"diff",-1,	0,0},
			{"interp",-1,	0,0},
			{"interp2",-1,	0,0},
			{"regr",-1,	0,0},
			/* Reentrant ANSI C functions */
			{"atan",1,	(SYSFUNCTYPE)atan,0},
			{"cos",1,	(SYSFUNCTYPE)cos,0},
			{"sin",1,	(SYSFUNCTYPE)sin,0},
			{"tan",1,	(SYSFUNCTYPE)tan,0},
			{"tanh",1,	(SYSFUNCTYPE)tanh,0},
//			{"frexp",2,	(SYSFUNCTYPE)frexp,0},	// i/o (d,&i)
//			{"modf",2,	(SYSFUNCTYPE)modf,0},	// i/o (d,&d)
			{"abs",1,	(SYSFUNCTYPE)fabs,0},
			{"ceil",1,	(SYSFUNCTYPE)ceil,0},
			{"floor",1,	(SYSFUNCTYPE)floor,0},
			/* Non reentrant ANSI C functions */
			{"acos",1,	(SYSFUNCTYPE)acos,0},
			{"asin",1,	(SYSFUNCTYPE)asin,0},
			{"atan2",2,	(SYSFUNCTYPE)atan2,0},
			{"cosh",1,	(SYSFUNCTYPE)cosh,0},
			{"sinh",1,	(SYSFUNCTYPE)sinh,0},
			{"exp",1,	(SYSFUNCTYPE)exp,0,},
//			{"ldexp",2,	(SYSFUNCTYPE)ldexp,0},	// (d,i)
			{"ln",1,	(SYSFUNCTYPE)log,0},
			{"log",1,	(SYSFUNCTYPE)log10,0},
			{"pow",2,	(SYSFUNCTYPE)pow,0},
			{"sqrt",1,	(SYSFUNCTYPE)sqrt,0},
			{"mod",-1,	0,0},
			/* double Scalar functions */
			{"fix",1,	(SYSFUNCTYPE)_fix,0},
			{"frac",1,	(SYSFUNCTYPE)_frac,0},
			{"sign",1,	(SYSFUNCTYPE)_sign,0},
			{"round",-1,	0,0},
			{"rand",1,	(SYSFUNCTYPE)_rand,0},
			/* double Matrix functions */
			{"m",1,		0,(SYSFUNCMTYPE)_m},
			{"n",1,		0,(SYSFUNCMTYPE)_n},
#ifdef	USE_GD
			/* double GRF functions */
			{"px",1,	(SYSFUNCTYPE)_px,0},
			{"py",1,	(SYSFUNCTYPE)_py,0},
			{"mx",1,	(SYSFUNCTYPE)_mx,0},
			{"my",1,	(SYSFUNCTYPE)_my,0},
#endif
		};
SYSMFUNC	sysmfuncs[]={
			/* matrix Scalar functions */
			{"M[]",3,	(SYSMFUNCTYPE)_M,0},
			{"R[]",3,	(SYSMFUNCTYPE)_R,0},
			{"I[]",1,	(SYSMFUNCTYPE)_I,0},
		};
SYSMXFUNC	sysmxfuncs[]={
			/* matrix Complex functions */
		/*
			{"extract[]",3,	0,_extract},
			{"combine[]",3,	0,_combine},
		*/
		};
#endif

/* readline.c */
int		scanline(void);
void		freebuffer(void);
/* parse.c */
int		doline(char *line,char domode);
void		printv(char *voutbuf,int voutlen,double value);
double		exprs(char *str);
double		eqneq(void);
double		ltegte(void);
double		expr(void);
double		terms(void);
double		term(void);
double		factor(void);
_PMATRIX	mexprs(char *str);
_PMATRIX	mltegte(void);
_PMATRIX	mexpr(void);
_PMATRIX	mterms(void);
_PMATRIX	mterm(void);
_PMATRIX	mfactor(void);
_PMATRIX	getmatrix(char *str);
char		*getstr(char *str);
char		*string(char *str);
/* Not used */
char		*substerm(char *str,char *parname,char *parstr,int mode);
/**/
char		*isset(char *str);
char		*isfunc(char *str);
char		*ismatrix(char *str);
int		isdelimiter(char ch);
void		freegroupvars(void);
/* hash.c */
void		zerohash(void);
void		zeroprsvd(void);
void		zerovar(void);
void		zeromatrix(void);
void		zerostr(void);
void		zerolabel(void);
void		zerofunc(void);
void		zeromfunc(void);
void		zeromxfunc(void);
int		inithash(void);
void		freehash(void);
void		freeprsvd(void);
void		freevar(void);
void		freematrix(void);
void		freestr(void);
void		freelabel(void);
void		freefunc(void);
void		freemfunc(void);
void		freemxfunc(void);
int		gethash(char *name);
void		setsysprsvds(void);
void		setsysvars(void);
void		setsysfuncs(void);
void		setsysmfuncs(void);
void		setsysmxfuncs(void);
void		setprsvd(char *str);
void		setvar(char *str,double value);
void		setmatrix(char *str,_PMATRIX _pmatrix);
void		setstr(char *str,char *pstr);
void		setlabel(char *str,int lineno);
void		setfunc(char *str,int npars,
			double (*func)(double,...),
			double (*mfunc)(_PMATRIX,...));
void		setmfunc(char *str,int npars,
			_PMATRIX (*func)(double,...),
			_PMATRIX (*mfunc)(_PMATRIX,...));
void		setmxfunc(char *str,int npars,
			_PMATRIX (*func)(double,_PMATRIX,...),
			_PMATRIX (*mfunc)(_PMATRIX,double,...));
double		var(char *str);
char		*fstr(char *str);
_PMATRIX	matrix(char *str);
char		*str(char *str);
int		label(char *str);
double		func(char *str);
_PMATRIX	mfunc(char *str);
_PMATRIX	mxfunc(char *str);
double		matrixelement(char *str);
int		isprsvd(char *str);
int		ismxfunc(char *str);
int		issysvar(char *str);
int		issysvar2(char *str);

#endif

