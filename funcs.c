#define _FUNCS_C_
#include "mafi.h"

#define	EPS	3.0e-11

#ifdef	USE_GD
extern	double	_sxr,_exr,_syr,_eyr;
extern	int	grfwidth,grfheight;
#endif
extern	char	format[];

unsigned long	rand_next=1;


/*******************************************************************************
 * Scalar Functions
 ******************************************************************************/
void
_solve_bracket_in(func,x1,x2,n,xb1,xb2,nb)
	char	*func;
	double	x1,x2,*xb1,*xb2;
	int	n,*nb;
{
	int	i,nbb;
	double	x,fp,fc,dx;

	nbb=0;
	dx=(x2-x1)/n;
	setvar("_x",(x=x1));
	fp=exprs(func);
#ifdef	ZERO
	fp=(fabs(fp)<=ZERO?0.0:fp);
#endif

	for(i=0;i<n;i++){
		setvar("_x",(x+=dx));
		fc=exprs(func);
#ifdef	ZERO
		fc=(fabs(fc)<=ZERO?0.0:fc);
#endif
		if(fc*fp<=0.0){
			xb1[nbb]=x-dx;
			xb2[nbb]=x;
			nbb++;
			if(*nb==nbb)
				return;
		}
		fp=fc;
	}
	*nb=nbb;
	return;
}

int
_solve_bracket_out(func,x1,x2,n,factor)
	char	*func;
	double	*x1,*x2,factor;
	int	n;
{
	int	i;
	double	f1,f2;

	if(*x1==*x2)
		return 1;

	setvar("_x",*x1);
	f1=exprs(func);
#ifdef	ZERO
	f1=(fabs(f1)<=ZERO?0.0:f1);
#endif

	setvar("_x",*x2);
	f2=exprs(func);
#ifdef	ZERO
	f2=(fabs(f2)<=ZERO?0.0:f2);
#endif

	for(i=0;i<n;i++){
		if(f1*f2<0.0)
			return 0;
		if(fabs(f1)<fabs(f2)){
			setvar("_x",(*x1+=factor*(*x1-*x2)));
			f1=exprs(func);
#ifdef	ZERO
			f1=(fabs(f1)<=ZERO?0.0:f1);
#endif
		}else{
			setvar("_x",(*x2+=factor*(*x2-*x1)));
			f2=exprs(func);
#ifdef	ZERO
			f2=(fabs(f2)<=ZERO?0.0:f2);
#endif
		}
	}

	return 1;
}

double
_solve_brent(func,x1,x2,n,tol)
	char	*func;
	double	x1,x2,tol;
	int	n;
{
	int	i;
	double	a=x1,b=x2,c=x2,d,e,min1,min2,fa,fb,fc,p,q,r,s,tol1,xm;

	err=0;

	setvar("_x",a);
	fa=exprs(func);
#ifdef	ZERO
	fa=(fabs(fa)<=ZERO?0.0:fa);
#endif

	setvar("_x",b);
	fb=exprs(func);
#ifdef	ZERO
	fb=(fabs(fb)<=ZERO?0.0:fb);
#endif

	if(fa*fb>0.0){
		err=1;
		return 0;
	}

	fc=fb;
	for(i=0;i<n;i++){
		if(fb*fc>0.0){
			c=a;
			fc=fa;
			e=d=b-a;
		}
		if(fabs(fc)<fabs(fb)){
			a=b;
			b=c;
			c=a;
			fa=fb;
			fb=fc;
			fc=fa;
		}
		tol1=2.0*EPS*fabs(b)+0.5*tol;
		xm=0.5*(c-b);
		if(fabs(xm)<=tol1||fb==0.0)
#ifdef	ZERO
			return (fabs(b)<=ZERO?0.0:b);
#else
			return b;
#endif
		if(fabs(e)>=tol1&&fabs(fa)>fabs(fb)){
			s=fb/fa;
			if(a==c){
				p=2.0*xm*s;
				q=1.0-s;
			}else{
				q=fa/fc;
				r=fb/fc;
				p=s*(2.0*xm*q*(q-r)-(b-a)*(r-1.0));
				q=(q-1.0)*(r-1.0)*(s-1.0);
			}
			if(p>0.0)
				q=-q;
			p=fabs(p);
			min1=3.0*xm*q-fabs(tol1*q);
			min2=fabs(e*q);
			if(2.0*p<(min1<min2?min1:min2)){
				e=d;
				d=p/q;
			}else{
				d=xm;
				e=d;
			}
		}else{
			d=xm;
			e=d;
		}
		a=b;
		fa=fb;
		if(fabs(d)>tol1)
			b+=d;
		else
			b+=(xm>0.0?tol1:(xm<0.0?-tol1:0));
//			b+=(xm>0.0?tol1:-tol1);
		setvar("_x",b);
		fb=exprs(func);
#ifdef	ZERO
		fb=(fabs(fb)<=ZERO?0.0:fb);
#endif
	}

	err=1;
	return 0;
}

double
_integ_trapezd(func,x1,x2,n)
	char	*func;
	double	x1,x2;
	int	n;
{
	int	i,j;
	double	f1,f2,x,sum,dx;
	static	double	ret;

	if(!n){
		setvar("_x",x1);
		f1=exprs(func);
		setvar("_x",x2);
		f2=exprs(func);
		ret=0.5*(x2-x1)*(f2+f1);
	}else{
		for(i=1,j=1;j<n;j++)
			i<<=1;
		dx=(x2-x1)/i;
		x=x1+0.5*dx;
		for(sum=0,j=0;j<i;j++,x+=dx){
			setvar("_x",x);
			sum+=exprs(func);
		}
		ret=0.5*(ret+(x2-x1)*sum/i);
	}

	return ret;
}

int
_integ_gaussleg(n,x,w)
	double	**x,**w;
	int	n;
{
	int	i,j,m;
	double	z,z1,xm,xr,p1,p2,p3,pp;

	if(!x||!w)
		return 1;

	if(*x){
		free(*x);
		*x=0;
	}
	if(*w){
		free(*w);
		*w=0;
	}

	*x=(double *)malloc(n*sizeof(double));
	if(!*x)
		return 1;

	*w=(double *)malloc(n*sizeof(double));
	if(!*w){
		free(*x);
		return 1;
	}

	m=(n+1)/2;
	xm=0.5;
	xr=0.5;
	for(i=1;i<=m;i++){
		z=cos(M_PI*(i-0.25)/(n+0.5));
		do{
			p1=1.0;
			p2=0.0;
			for(j=1;j<=n;j++){
				p3=p2;
				p2=p1;
				p1=((2.0*j-1.0)*z*p2-(j-1.0)*p3)/j;
			}
			pp=n*(z*p1-p2)/(z*z-1.0);
			z1=z;
			z=z1-p1/pp;
		}while(fabs(z-z1)>EPS);
		(*x)[i-1]=xm-xr*z;
		(*x)[n-i]=xm+xr*z;
		(*w)[i-1]=2.0*xr/((1.0-z*z)*pp*pp);
		(*w)[n-i]=(*w)[i-1];
	}

	return 0;
}

double
_integ(func,x1,x2,n,x,w,method)
	char	*func;
	double	x1,x2,*x,*w;
	int	n,method;
{
	int	i;
	double	ret;

	ret=0;
	if(method==0){
		double	dx;

		dx=(x2-x1)/n;
		for(i=0;i<n;i++){
			setvar("_x",x1+(i+0.5)*dx);
			ret+=dx*exprs(func);
		}
	}else
	if(method==1){
		double	s,st,os,ost;

		for(i=0;i<n;i++){
			st=_integ_trapezd(func,x1,x2,i);
			s=(4.0*st-ost)/3.0;
			if((fabs(s-os)<EPS*fabs(os)) ||
			   (s==0.0&&os==0.0&&i>5)){
				ret=s;
				break;
			}
			os=s;
			ost=st;
		}
	}else
	if(method==2){
		double	xr,dx;

		xr=x2-x1;
		for(i=0;i<n;i++){
			dx=xr*x[i];
			setvar("_x",x1+dx);
			ret+=w[i]*exprs(func);
		}
		ret*=xr;
	}

	return ret;
}

double
_odeint(func,x0,y0,x,n,method)
	char	*func;
	double	x0,y0,x;
	int	n,method;
{
	int	i;
	double	x1,y1,h;

	x1=x0;
	y1=y0;
	h=(x-x0)/n;

	if(method==0){
		for(i=0;i<n;i++){
			setvar("_x",x1);
			setvar("_y",y1);
			y1+=exprs(func)*h;
			x1=x0+(i+1)*h;
		}
	}else
	if(method==1){
		double	h2,f1,f2,f3,f4;

		h2=h/2;
		for(i=0;i<n;i++){
			setvar("_x",x1);
			setvar("_y",y1);
			f1=h*exprs(func);

			setvar("_x",x1+h2);
			setvar("_y",y1+f1/2);
			f2=h*exprs(func);

			setvar("_x",x1+h2);
			setvar("_y",y1+f2/2);
			f3=h*exprs(func);

			setvar("_x",x1+h);
			setvar("_y",y1+f3);
			f4=h*exprs(func);

			x1=x0+(i+1)*h;
			y1+=(f1+2*f2+2*f3+f4)/6;
		}
	}

	return y1;
}

double
_diff(func,x,h,order)
	char	*func;
	double	x,h;
	int	order;
{
	double	fb2,fb1,f0,ff1,ff2;
	double	ret;

	setvar("_x",x-2*h);
	fb2=exprs(func);

	setvar("_x",x-h);
	fb1=exprs(func);

	setvar("_x",x);
	f0=exprs(func);

	setvar("_x",x+h);
	ff1=exprs(func);

	setvar("_x",x+2*h);
	ff2=exprs(func);

	if(order==1){
		ret=(fb2-8*fb1+8*ff1-ff2)/(12*h);
	}else
	if(order==2){
		ret=(-fb2+16*fb1-30*f0+16*ff1-ff2)/(12*h*h);
	}else
	if(order==3){
		ret=(-fb2+2*fb1-2*ff1+ff2)/(2*h*h*h);
	}

	return ret;
}

double
_interp(A,x)
	_PMATRIX	A;
	double		x;
{
	int	i,j,ns=1;
	double	y,dy,w,t,h,hh,dd,*c,*d;

	err=0;
	if(!A||A->m<2||A->n<1){
		err=1;
		return 0;
	}

	if(A->m==2){
		y=(maty(A,2)-maty(A,1))/(matx(A,2)-matx(A,1))*(x-matx(A,1))+
			maty(A,1);
		setvar("_dy",0.0);
		return y;
	}

	c=(double *)malloc(A->m*sizeof(double));
	if(!c){
		err=1;
		return 0;
	}

	d=(double *)malloc(A->m*sizeof(double));
	if(!d){
		free(c);
		err=1;
		return 0;
	}

	hh=fabs(x-matx(A,1));
	for(i=1;i<=A->m;i++){
		h=fabs(x-matx(A,i));
		if(!h){
			y=maty(A,i);
			setvar("_dy",0.0);
			free(c);
			free(d);
			return y;
		}else
		if(h<hh){
			ns=i;
			hh=h;
		}
		c[i-1]=maty(A,i);
		d[i-1]=maty(A,i)+EPS;
	}
	y=maty(A,ns--);
	for(i=1;i<A->m;i++){
		for(j=1;j<=A->m-i;j++){
			w=c[j]-d[j-1];
			h=matx(A,i+j)-x;
			t=(matx(A,j)-x)*d[j-1]/h;
			dd=t-c[j];
			if(!dd){
				free(c);
				free(d);
				err=1;
				return 0;
			}
			dd=w/dd;
			d[j-1]=c[j]*dd;
			c[j-1]=t*dd;
		}
		y+=(dy=(2*ns<(A->m-i)?c[ns]:d[(ns--)-1]));
	}

	free(c);
	free(d);
	setvar("_dy",dy);

	return y;
}


double
_interp2(A,B,C,x1,x2)
	_PMATRIX	A,B,C;
	double		x1,x2;
{
	int		i,j;
	double		y;
	_PMATRIX	iA,iB;

	err=0;
	if(!C||C->m<2||C->n<2||
			(A&&(A->m<1||A->n<2||C->n!=A->n))||
			(B&&(B->m<2||B->n<1||C->m!=B->m))){
		err=1;
		return 0;
	}

	iA=_new_matrix(C->n,2,0);
	iB=_new_matrix(C->m,2,0);
	for(j=1;j<=C->n;j++){
		for(i=1;i<=C->m;i++){
			_element(iB,i,1)=(B?_element(B,1,i):i);
			_element(iB,i,2)=_element(C,i,j);
		}
		_element(iA,j,1)=(A?_element(A,1,j):j);
		_element(iA,j,2)=_interp(iB,x1);
	}

	y=_interp(iA,x2);

	_free_matrix(iA);
	_free_matrix(iB);

	return y;
}

int
_regr(A,n,f,c)
	_PMATRIX	A;
	int		n;
	char		*f,*c;
{
	int		i,j,k,l;
	double		*a,*x,tmp;
	_PMATRIX	B;
#define	FSTRSIZE	128
	char		fstr[FSTRSIZE];

	if(!A||A->m<1||A->n<1||n<1||(!f&&!c))
		return 1;

	if(A->m<n)
		n=A->m;

	a=(double *)malloc((n+1)*(n+2)*sizeof(double));
	if(!a)
		return 1;

	*a=A->m;
	j=1;
	for(i=0;i<n+1;i++){
		for(;j<n+1;j++){
			x=a+i*(n+2)+j;
			*x=0;
			for(k=1;k<=A->m;k++)
				*x+=pow(matx(A,k),i+j);
		}
		x=a+i*(n+2)+n+1;
		*x=0;
		for(k=1;k<=A->m;k++)
			*x+=pow(matx(A,k),i)*maty(A,k);
		j=0;
	}

	/** Cholesky method **/
	for(k=0;k<n+1;k++){
		for(i=-1;i<k-1;){
			i++;
			tmp=0;
			for(j=-1;j<i-1;){
				j++;
				tmp+=*(a+i*(n+2)+j)*(*(a+k*(n+2)+j));
			}
			*(a+k*(n+2)+i)=(*(a+k*(n+2)+i)-tmp)/(*(a+i*(n+2)+i));
		}
		tmp=0;
		for(j=-1;j<k-1;){
			j++;
			tmp+=*(a+k*(n+2)+j)*(*(a+k*(n+2)+j));
		}
		*(a+k*(n+2)+k)=sqrt(*(a+k*(n+2)+k)-tmp);
	}

	/** forward substitution **/
	*(a+n+1)/=*a;
	for(i=1;i<n+1;i++){
		for(j=-1;j<i-1;){
			j++;
			*(a+i*(n+2)+n+1)-=*(a+i*(n+2)+j)*(*(a+j*(n+2)+n+1));
		}
		*(a+i*(n+2)+n+1)/=*(a+i*(n+2)+i);
	}

	for(i=n;i>=0;i--){
		for(j=i+1;j<n+1;j++)
			*(a+i*(n+2)+n+1)-=*(a+j*(n+2)+i)*(*(a+j*(n+2)+n+1));
		*(a+i*(n+2)+n+1)/=*(a+i*(n+2)+i);
	}

	if(f)
		fprintf(stdout,"%s(x)=",f);
	if(c)
		B=_new_matrix(n+1,1,0);
	j=0;
	for(i=0;i<n+1;i++){
		tmp=*(a+i*(n+2)+n+1);
#ifdef	ZERO
		tmp=(fabs(tmp)<=ZERO?0.0:tmp);
#endif
		if(f){
			if(tmp){
				if(tmp>=0.0&&j)
					strcat(fstr,"+");
				j=1;
				l=strlen(fstr);
				fprintf(stdout,format,tmp);
				if(i){
					l=strlen(fstr);
					fprintf(stdout,"*x");
					if(i>1){
						l=strlen(fstr);
						fprintf(stdout,"^%d",i);
					}
				}
			}
		}
		if(c)
			_element(B,i+1,1)=tmp;
	}

	if(f)
		setfunc(fstr,0,0,0);
	if(c){
		setmatrix(c,B);
		_free_matrix(B);
	}

	free(a);

	return 0;
}

double
_fix(x)
	double	x;
{
	return (x>=0 ? floor(x):ceil(x));
}

double
_frac(x)
	double	x;
{
	return x - (x>=0 ? floor(x):ceil(x));
}

double
_sign(x)
	double	x;
{
	return (x>0 ? 1:(x<0 ? -1:0));
}

double
_round(x,i)
	double	x;
	int	i;
{
	if(x==0)
		return 0.0;

	x=x*pow(10.0,(double)i)+(x>0 ? 0.5:-0.5);

	return (x>=0 ? floor(x):ceil(x))/pow(10.0,(double)i);
}

double
_rand(max)
	double	max;
{
	rand_next=rand_next*1103515245L+12345;
	return (double)(rand_next%((unsigned long)0x7fffffff+1))/0x7fffffff*max;
//	return (double)(rand_next%((unsigned long)max+1));
}

/*******************************************************************************
 * Matrix Functions
 ******************************************************************************/
double
_m(A)
	_PMATRIX	A;
{
	if(!A)
		return 0;

	return A->m;
}

double
_n(A)
	_PMATRIX	A;
{
	if(!A)
		return 0;

	return A->n;
}

_PMATRIX
_M(m,n,element)
	double		m,n,element;
{
	return _new_matrix((int)m,(int)n,element);
}

_PMATRIX
_R(m,n,element)
	double		m,n,element;
{
	int		i,j;
	_PMATRIX	A;

	A=_new_matrix((int)m,(int)n,0);

#ifdef	ZERO
	if(fabs(element)>ZERO)
#else
	if(element)
#endif
	{
		for(i=1;i<=(int)m;i++)
			for(j=1;j<=(int)n;j++)
				if(element>0)
					_element(A,i,j)=_rand(element);
				else
				if(element<0)
					_element(A,i,j)=(_rand(1)>0.5?1:-1)*_rand(element);
	}
	return A;
}

_PMATRIX
_I(n)
	double		n;
{
	return _unity_matrix((int)n);
}

/*
_PMATRIX
_extract(A,B,r,c)
	_PMATRIX	A,B;
	double		r,c;
{
	_PMATRIX	B;

	return _extract_matrix(A,B,(int)r,(int)c);
}

_PMATRIX
_combine(A,B,r,c)
	_PMATRIX	A,B;
	double		r,c;
{
	return _combine_matrix(A,B,(int)r,(int)c,NEW_MATRIX);
}
*/

#ifdef	USE_GD
/*******************************************************************************
 * GRF Functions
 ******************************************************************************/
double
_px(mx)
	double	mx;
{
	return grfwidth/(_exr-_sxr)*(mx-_sxr);
}

double
_py(my)
	double	my;
{
	return grfheight-grfheight/(_eyr-_syr)*(my-_syr);
}

double
_mx(px)
	double	px;
{
	return _sxr+px/grfwidth*(_exr-_sxr);
}

double
_my(py)
	double	py;
{
	return _syr-(py-grfheight)/grfheight*(_eyr-_syr);
}
#endif
