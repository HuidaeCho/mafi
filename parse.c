#define	_PARSE_C_
#include "mafi.h"

#ifdef	USE_GD
/******************************************************************************/
//#define	HIDDEN_LINE_ELIMINATION
//#define	CORRECT_AXES

#define	GRFWIDTH	500
#define	GRFHEIGHT	500

#define	URX		((double)GRFWIDTH/5.0)
#define	URY		((double)GRFHEIGHT/5.0)

#define	SX		-URX
#define	EX		URX
#define	SY		-URY
#define	EY		URY
#define	SXR		-URX
#define	EXR		URX
#define	SYR		-URY
#define	EYR		URY
#define	DX		2.0
#define	DY		2.0
#define	XTO		0.0
#define	YTO		0.0
#define	XT		1.0
#define	YT		1.0
#define	XTL		5.0
#define	YTL		5.0
#define	ZOOM		1.5
#define	VP_X0		(GRFWIDTH/2)
#define	VP_Y0		(GRFHEIGHT/2)
#define	VP_RHO		100
#define	VP_PHI		(60*M_PI/180)
#define	VP_THETA	(50*M_PI/180)
#define	VP_D		100
#define	GRF3DW		0x3

#define	Cx(x)		((grfwidth-1)/(_exr-_sxr)*((x)-_sxr))
#define	Cy(y)		((grfheight-1)-(grfheight-1)/(_eyr-_syr)*((y)-_syr))
#define	Clx(x)		(!logx?Cx(x):Cx(log(x)/logx))
#define	Cly(y)		(!logy?Cy(y):Cy(log(y)/logy))
#define	Mx(x)		(_sxr+(double)(x)/(grfwidth-1)*(_exr-_sxr))
#define	My(y)		(_syr-(double)((y)-(grfheight-1))/(grfheight-1)*(_eyr-_syr))
#define	Rx(x)		(!rel?(x):_x+(x))
#define	Ry(y)		(!rel?(y):_y+(y))
#define	Rz(z)		(!rel?(z):_z+(z))
#define	Rx2(x)		(!rel?(x):(x)-_x)
#define	Ry2(y)		(!rel?(y):(y)-_y)
#define	Rz2(z)		(!rel?(z):(z)-_z)

#define	Point(pt,a,b,c)	(pt).x=(a),(pt).y=(b),(pt).z=(c)

#define	BLACK	gdImageColorResolve(im,0x00,0x00,0x00)
#define	WHITE	gdImageColorResolve(im,0xff,0xff,0xff)

extern	int	_deltaX,_deltaY;

struct	_viewport _vp;
double	_sx,_ex,_sy,_ey,_sxr,_exr,_syr,_eyr,_szr,_ezr,_dx,_dy,
	_xto,_yto,_xt,_yt,_xtl,_ytl;
int	_grf3dw,_grfwidth,_grfheight;

struct	_viewport vp;
double	sx,ex,sy,ey,sxr,exr,syr,eyr,szr,ezr,dx,dy,
	xto,yto,xt,yt,xtl,ytl;
int	grf3dw,grfwidth,grfheight,
	g_axes,g_trans=-1,g_bgcolor=0xffffff,g_color;

double	_g_logx,_g_logy,logx,logy;
int	_g_axes,_g_trans,_g_bgcolor,_g_color,_g_style,
	_g_strf,_g_strh,_g_strv,_g_stru;

int	black,white;

static	char	*xl,*yl;
static	char	*grffunc;
int	ret_g;

char	dxfirst;

double	_x,_y,_z;
int	px,py;

gdImagePtr	im=NULL;
char	*imfile;
int	grfwidth, grfheight;

#define	NGSTYLES	2
int	g_style[10];
int	*gstyle;
int	gstyle1[]={1,0};
int	gstyle2[]={1,1,1,0,0,0};

void	initgrfvars(void);
void	draw2daxes(void);
void	draw3daxis(int xyz,char *label);
int	draw2d(void);
int	draw2dgrf(void);
int	draw3dgrf(void);
/******************************************************************************/
#endif

extern	unsigned long	rand_next;

int	group,ngroupvars;
char	grouplocal,**groupvars;

int	_h_frac,_o_frac,_b_frac,_v_base,_v_width,_v_prec,_max_loop;
char	format[20];

char	lastans=1,lastret=1,just_setvar;

int	skip_result=0;

int	integ_n=0;
double	*integ_x=0,*integ_w=0;

static	void	mkformat(void);

int
doline(line,domode)
	char	*line;
	char	domode;
{
	static	char	firstcalc=1;
	int		ret;
	int		i,j,k,p;
	char		done,ch,after_setvar;
	double		value,tmp;
	_PMATRIX	A;
	char		*pline,*pname,*t,*u,*v,*w,*x,*y,*z;
	FILE		*fp;

	ret=0;
	w=0;

	if(!line||!(*line))
		return ret;

	y=line+strlen(line)-1;
	if(*y==','||*y==';')
		y--;

	if(firstcalc){
		firstcalc=0;

		group=0;
		ngroupvars=0;
		groupvars=0;
		grouplocal=0;

		_h_frac=0;
		_o_frac=0;
		_b_frac=0;
		_v_base=10;
		_v_width=0;
		_v_prec=6;
		_max_loop=0;

		setvar("_x_frac",_h_frac);
		setvar("_h_frac",_h_frac);
		setvar("_o_frac",_o_frac);
		setvar("_b_frac",_b_frac);
		setvar("_v_base",_v_base);
		setvar("_v_width",_v_width);
		setvar("_v_prec",_v_prec);
		setvar("_max_loop",_max_loop);

#ifdef	USE_GD
		_g_logx=0;
		_g_logy=0;
		_g_style=0;
		_g_strf=0;
		_g_strh=-1;
		_g_strv=-1;
		_g_stru=0;

		setvar("_g_logx",_g_logx);
		setvar("_g_logy",_g_logy);
		setvar("_g_style",_g_style);
		setvar("_g_strf",_g_strf);
		setvar("_g_strh",_g_strh);
		setvar("_g_strv",_g_strv);
		setvar("_g_stru",_g_stru);

		logx=0;
		if(_g_logx)
			logx=log(_g_logx);
		logy=0;
		if(_g_logy)
			logy=log(_g_logy);

		initgrfvars();
#endif
		mkformat();
	}

	if(!w){
		w=pline=(char *)malloc(strlen(line)+1);
		if(!w){
			snprintf(tmpbuf,TMPBUFSIZE,"Memory exhausted");
			error();
		}
		strcpy(w,line);
	}

	x=w+strlen(w)-1;
	if(*x==','||*x==';')
		*x=0;

/*******************************************************************************
 * Reserved words which consist of one word and don't have any parameter.
 ******************************************************************************/
	if(!strncmp(w,"continue",8)){
		if(*(w+8)){
			snprintf(tmpbuf,TMPBUFSIZE,"%s: This reserved word cannot be followed by any character",w);
			free(w);
			error();
		}
		free(w);
		ret=CONTINUE;
		return ret;
	}else
	if(!strncmp(w,"break",5)){
		if(*(w+5)){
			snprintf(tmpbuf,TMPBUFSIZE,"%s: This reserved word cannot be followed by any character",w);
			free(w);
			error();
		}
		free(w);
		ret=BREAK;
		return ret;
	}

/*******************************************************************************
 * Test if a sentence is grouped.
 ******************************************************************************/
	if(*w=='{'){
		group++;
		grouplocal=1;
		x=w+strlen(w)-1;
		if(*x=='}')
		    	*x=0;
		j=p=0;
		for(x=y=w+1;*x;x++){
			if(*x=='"'&&*(x-1)!='\\')
			    	j++;
			else
			if((*x=='('||*x=='{'||*x=='[')&&!(j&1))
			    	p++;
			else
			if((*x==')'||*x=='}'||*x==']')&&!(j&1))
			    	p--;
		    	if((*x==';'||*x=='}'||!*(x+1))&&!(j&1)&&!p&&
			   (!*(x+1)||strstr(x+1,"else")!=x+1)){
				ch=*(x+1);
				*(x+1)=0;
			    	ret=doline(y,domode);
				*(x+1)=ch;
				if(ret<0||ret>1)
					break;
				if(grouplocal&&strncmp(y,"local(",6))
					grouplocal=0;
				y=x+1;
			}
		}
		free(w);
		if(ret==CONTINUE)
			return ret;

		snprintf(tmpbuf,TMPBUFSIZE,"_%d_",group);
		j=strlen(tmpbuf);
		for(i=ngroupvars-1;i>=0;i--){
			if(strstr(groupvars[i],tmpbuf)!=groupvars[i])
				break;
			k=strlen(groupvars[i]);
			if(groupvars[i][j]=='$'){
				setstr(groupvars[i]+j+1,str(groupvars[i]));
				setstr(groupvars[i],0);
			}else
			if(groupvars[i][k-2]=='['&&groupvars[i][k-1]==']'){
				groupvars[i][k-2]=0;
				setmatrix(groupvars[i]+j,matrix(groupvars[i]));
				setmatrix(groupvars[i],0);
			}else{
				setvar(groupvars[i]+j,var(groupvars[i]));
				setvar(groupvars[i],0);
			}
			free(groupvars[i]);
			ngroupvars--;
		}
		if(!ngroupvars){
			free(groupvars);
			groupvars=0;
		}
		group--;
		return ret;
	}

/*******************************************************************************
 * Test whether a sentence is an assignment one or not.
 ******************************************************************************/
	after_setvar=0;
	just_setvar=0;
	pname=0;
	if((x=isset(pline))){
		*x=0;
		if(issysvar(pline)){
			snprintf(tmpbuf,TMPBUFSIZE,"%s: This system variable cannot be reassigned",w);
			free(w);
			error();
		}
		if(issysvar2(pline)){
			snprintf(tmpbuf,TMPBUFSIZE,"%s: This system variable cannot be assigned by users",w);
			free(w);
			error();
		}
		if(isprsvd(pline)){
			snprintf(tmpbuf,TMPBUFSIZE,"%s: Reserved words cannot be assigned",w);
			free(w);
			error();
		}

		*x='=';
		i=strlen(pline);
		if(x==isfunc(pline)){
			for(x=pline;*x;x++)
				if(*x=='['||*x=='(')
					break;
			if(*x=='['&&*(x+1)==']')
				setmfunc(pline,0,0,0);
			else
		    		setfunc(pline,0,0,0);
			free(w);
			ret=1;
			return ret;
		}else{
			if(strchr("+-*/%^",*(x-1))){
				x--;
				after_setvar=*x;
				*x=0;
				x++;
			}
			*x=0;
			pname=pline;
			pline=x+1;
		}
	}

/*******************************************************************************
 * Non-reserved words are implemented here.
 ******************************************************************************/
	if(*pline!='{'&&!(isprsvd(pline)&&isfunc(pline))){
		if(*pline=='"'||*pline=='$'){
			if(*pline=='"'){
				i=0;
			    	for(x=pline+1;*x;x++){
					if(*x=='\\'){
						i++;
						continue;
					}
				    	if(*x=='"'&&!(i%2&&*(x-1)=='\\'))
					    	break;
					i=0;
				}
				y=pline+1;
			}else{
				for(x=pline+1;!isdelimiter(*x);x++);
				y=pline;
			}
			ch=*x;
			*x=0;

			if((z=getstr(y))){
				if(pname)
					setstr(pname,z);
				else
					fprintf(stdout,"%s",z);
				free(z);
			}else
			if(pname)
				setstr(pname,z);

			*x=ch;
			if(*pline=='$')
				x--;
			for(x++;*x==','||*x==';';x++);
			if(*x)
			    	ret=doline(x,domode);
			else
				ret=1;
		}else{
			i=0;
			if(pname&&strchr(pname,'[')){
				*x='=';
				setmatrix(pname,0);
				free(w);
				ret=1;
				return ret;
			}else
			if(strchr(pline,'[')){
				z=pline;
				y=0;
				if(strchr(z,'(')){
					while(!((y=strchr(z,'('))&&y>z&&ischnum(*(y-1))))
						if(y)
							z=y+1;
						else
							break;
				}
				z=pline;
				if(y){
					while((x=strchr(z,'['))){
						while(!((y=strchr(z,'('))&&y>z&&isvar(*(y-1))))
							if(y)
								z=y+1;
							else
								break;
						if(y){
							p=1;
							for(z=y+1;*z;z++){
								switch(*z){
									case '(':
										p++;
										break;
									case ')':
										p--;
										break;
								}
								if(!p)
									break;
							}
							if(x<y&&(*(x+1)==']'||isdelimiter(*(x-1)))){
								i=1;
								break;
							}else
								z++;
						}else if(*(x+1)==']'||isdelimiter(*(x-1))){
							i=1;
							break;
						}
					}
				}else{
					if(strstr(z,"[]"))
						i=1;
					else{
						y=z;
						while((x=strchr(y,'[')))
							if(x==w||isdelimiter(*(x-1))){
								i=1;
								break;
							}else{
								p=1;
								for(x++;*x;x++){
									switch(*x){
										case '[':
											p++;
											break;
										case ']':
											p--;
											break;
									}
									if(!p){
										y=x+1;
										break;
									}
								}
							}
					}
				}
			}
			if(i){
			    	if(pname&&!strcmp(pline,"[]")){
				    	if(matrix(pname))
					    	setmatrix(pname,0);
					else{
						snprintf(tmpbuf,TMPBUFSIZE,"%s: No such matrix",pname);
						free(w);
						error();
					}
					free(w);
					ret=1;
					return ret;
				}
				if(!(A=mexprs(pline))){
					snprintf(tmpbuf,TMPBUFSIZE,"%s: Matrix manipulation failed",pline);
					free(w);
					error();
				}
				if(pname){
					if(_ret_type)
						setmatrix(pname,A);
					for(;*pcur==','||*pcur==';';pcur++);
					if(*pcur)
					    	ret=doline(pcur,domode);
					else
						ret=1;
				}else{
					if(_ret_type&&!(domode&DOIF)){
						lastans=2;
						setmatrix("_ans",A);
						_print_matrix(NULL,0,A,0);
					}
					ch=0;
					for(;*pcur==','||*pcur==';';pcur++)
						ch=*pcur;
					if(*pcur){
						if(ch==';'&&_ret_type&&!(domode&DOIF))
							fprintf(stdout,"\n");
					    	ret=doline(pcur,domode);
					}else{
						if(_ret_type&&!(domode&DOIF))
							fprintf(stdout,"\n");
						ret=1;
					}
				}
				_free_matrix(A);
			}else{
				value=exprs(pline);
				if(err){
					free(w);
					error();
				}
				if(pname){
					if(!strcmp(pname,"_h_frac")||!strcmp(pname,"_x_frac")){
						_h_frac=(int)value;
						if(_h_frac<0)
							_h_frac=0;
						value=(double)_h_frac;
						if(!strcmp(pname,"_h_frac"))
							setvar("_x_frac",value);
						else
							setvar("_h_frac",value);
					}else
					if(!strcmp(pname,"_o_frac")){
						_o_frac=(int)value;
						if(_o_frac<0)
							_o_frac=0;
						value=(double)_o_frac;
					}else
					if(!strcmp(pname,"_b_frac")){
						_b_frac=(int)value;
						if(_b_frac<0)
							_b_frac=0;
						value=(double)_b_frac;
					}else
					if(!strcmp(pname,"_v_base")){
						_v_base=(int)value;
						if(_v_base!=2&&
						   _v_base!=8&&
						   _v_base!=10&&
						   _v_base!=16)
							_v_base=10;
						mkformat();
						value=(double)_v_base;
					}else
					if(!strcmp(pname,"_v_width")){
						_v_width=(int)value;
						mkformat();
						value=(double)_v_width;
					}else
					if(!strcmp(pname,"_v_prec")){
						_v_prec=(int)value;
						mkformat();
						value=(double)_v_prec;
					}else
					if(!strcmp(pname,"_max_loop")){
						if(value<0)
							value=-value;
						_max_loop=(int)value;
						value=(double)_max_loop;
					}
#ifdef	USE_GD
					else
					if(!strcmp(pname,"_g_axes")){
						g_axes=(int)value;
						value=(double)g_axes;
						if(im){
						_g_axes=gdImageColorResolve(im,
							(g_axes&0xff0000)>>16,
							(g_axes&0x00ff00)>>8,
							(g_axes&0x0000ff));
						}
					}else
					if(!strcmp(pname,"_g_logx")){
						_g_logx=value;
						logx=0;
						if(_g_logx)
							logx=log(_g_logx);
					}else
					if(!strcmp(pname,"_g_logy")){
						_g_logy=value;
						logy=0;
						if(_g_logy)
							logy=log(_g_logy);
					}else
					if(!strcmp(pname,"_g_trans")){
						g_trans=(int)value;
						value=(double)g_trans;
						if(im){
						if(g_trans>=0){
						_g_trans=gdImageColorResolve(im,
							(g_trans&0xff0000)>>16,
							(g_trans&0x00ff00)>>8,
							(g_trans&0x0000ff));
							gdImageColorTransparent(im,_g_trans);
						}
						/*
						else
							gdImageColorTransparent(im,-1);
						*/
						}
					}else
					if(!strcmp(pname,"_g_bgcolor")){
						g_bgcolor=(int)value;
						value=(double)g_bgcolor;
					}else
					if(!strcmp(pname,"_g_color")){
						g_color=(int)value;
						value=(double)g_color;
						if(im){
						_g_color=gdImageColorResolve(im,
							(g_color&0xff0000)>>16,
							(g_color&0x00ff00)>>8,
							(g_color&0x0000ff));
						if(_g_style){
						if(_g_style==1){
							gstyle=gstyle1;
							k=sizeof(gstyle1)/sizeof(int);
						}else{
							gstyle=gstyle2;
							k=sizeof(gstyle2)/sizeof(int);
						}
						for(j=0;j<k;j++)
							g_style[j]=(gstyle[j]?_g_color:gdTransparent);
						gdImageSetStyle(im,g_style,k);
						_g_color=gdStyled;
						}
						}
					}else
					if(!strcmp(pname,"_g_style")){
						if(value>NGSTYLES)
							value=0;
						_g_style=(int)value;
						value=(double)_g_style;
						if(im){
							if(_g_style){
							if(_g_style==1){
								gstyle=gstyle1;
								k=sizeof(gstyle1)/sizeof(int);
							}else{
								gstyle=gstyle2;
								k=sizeof(gstyle2)/sizeof(int);
							}
							_g_color=
							gdImageColorResolve(im,
							(g_color&0xff0000)>>16,
							(g_color&0x00ff00)>>8,
							(g_color&0x0000ff));
							for(j=0;j<k;j++)
								g_style[j]=(gstyle[j]?_g_color:gdTransparent);
							gdImageSetStyle(im,g_style,k);
							_g_color=gdStyled;
							}else{
							_g_color=
							gdImageColorResolve(im,
							(g_color&0xff0000)>>16,
							(g_color&0x00ff00)>>8,
							(g_color&0x0000ff));
							}
						}
					}else
					if(!strcmp(pname,"_g_strf")){
						if(value<0.0||value>12.0)
							_g_strf=0;
						else
							_g_strf=(int)value;
						value=(double)_g_strf;
					}else
					if(!strcmp(pname,"_g_strh")){
						_g_strh=0;
						if(value<0.0)
							_g_strh=-1;
						else
						if(value>0.0)
							_g_strh=1;
						value=(double)_g_strh;
					}else
					if(!strcmp(pname,"_g_strv")){
						_g_strv=0;
						if(value<0.0)
							_g_strv=-1;
						else
						if(value>0.0)
							_g_strv=1;
						value=(double)_g_strv;
					}else
					if(!strcmp(pname,"_g_stru")){
#ifdef	ZERO
						if(fabs(value)>ZERO)
#else
						if(value)
#endif
							_g_stru=1;
						else
							_g_stru=0;
						value=(double)_g_stru;
					}
#endif

					if(after_setvar){
						tmp=var(pname);
						switch(after_setvar){
							case '+':
								value=tmp+value;
								break;
							case '-':
								value=tmp-value;
								break;
							case '*':
								value=tmp*value;
								break;
							case '/':
								if(value==0.0){
									snprintf(tmpbuf,TMPBUFSIZE,"%s: Divided by zero",w);
									free(w);
									error();
								}
								value=tmp/value;
								break;
							case '%':
								if(value==0.0){
									snprintf(tmpbuf,TMPBUFSIZE,"%s: Divided by zero",w);
									free(w);
									error();
								}
								value=fmod(tmp,value);
								break;
							case '^':
								value=pow(tmp,value);
								break;
						}
						after_setvar=0;
					}
					if(_ret_type)
						setvar(pname,value);
					for(;*pcur==','||*pcur==';';pcur++);
					if(*pcur)
					    	ret=doline(pcur,domode);
					else
					if(domode&DOIF)
						ret=(
#ifdef	ZERO
							fabs(value)>ZERO
#else
							value
#endif
							?1:0);
					else
						ret=1;
				}else{
					if(_ret_type&&!(domode&DOIF)){
						lastans=1;
						setvar("_ans",value);
						if(!just_setvar)
							printv(NULL,0,value);
					}
					ch=0;
					for(;*pcur==','||*pcur==';';pcur++)
						ch=*pcur;
					if(*pcur){
						if(ch==';'&&_ret_type&&!(domode&DOIF))
							fprintf(stdout,"\n");
					    	ret=doline(pcur,domode);
					}else{
						if(_ret_type&&!(domode&DOIF)){
							if(!just_setvar)
								fprintf(stdout,"\n");
						}
						ret=(
#ifdef	ZERO
							fabs(value)>ZERO
#else
							value
#endif
							?1:0);
					}
				}
			}
		}
		_ret_type=3;
		free(w);
		return ret;
	}

/*******************************************************************************
 * Reserved words are implemented here.
 ******************************************************************************/
	/* Control Words */
	/* if */
	if(!strncmp(w,"if(",3)){
		p=1;
		j=0;
		for(x=w+3;*x;x++){
			if(*x=='"'&&*(x-1)!='\\')
			    	j++;
			else
			if((*x=='('||*x=='[')&&!(j&1))
			    	p++;
			else
			if((*x==')'||*x==']')&&!(j&1))
			    	p--;
			if(!p){
				*x=0;
				break;
			}
		}
		if(p||(j&1)){
			snprintf(tmpbuf,TMPBUFSIZE,"%s: Syntax error",w);
			free(w);
			error();
		}
		x++;
		if(*x=='{'){
			p=1;
			j=0;
			for(y=x+1;*y;y++){
				if(*y=='"'&&*(y-1)!='\\')
				    	j++;
				else
				if(*y=='{'&&!(j&1))
				    	p++;
				else
				if(*y=='}'&&!(j&1))
				    	p--;
				if(!p)
					break;
			}
			if(p||(j&1)){
				snprintf(tmpbuf,TMPBUFSIZE,"%s: Syntax error",w);
				free(w);
				error();
			}
		}else{
			p=1;
			j=0;
			for(y=x;*y;y++){
				if(*y=='"'&&*(y-1)!='\\')
				    	j++;
				else
				if((*y=='('||*y=='[')&&!(j&1))
				    	p++;
				else
				if((*y==')'||*y==']'||*y==';')&&!(j&1))
				    	p--;
				if(!p)
					break;
			}
			if(!*y&&p==1)
				p = 0;
			if(p||(j&1)){
				snprintf(tmpbuf,TMPBUFSIZE,"%s: Syntax error",w);
				free(w);
				error();
			}
		}
		if(*y)
			y++;
		ret=1;
		if(doline(w+3,DOIF|domode)){
			*y=0;
			ret=doline(x,domode);
		}else
		if(*y&&strstr(y,"else")==y)
			ret=doline(y+4,domode);
		free(w);
		return ret;
	}else
	/* for */
	if(!strncmp(w,"for(",4)){
		p=1;
		i=0;
		j=0;
		for(x=w+4;*x;x++){
			if(*x=='"'&&*(x-1)!='\\')
				j++;
			else
			if(*x==';'&&!(j&1)){
				*x=0;
				i++;
				if(i==1)
					y=x+1;
				else
					z=x+1;
			}else
			if((*x=='('||*x=='[')&&!(j&1))
				p++;
			else
			if((*x==')'||*x==']')&&!(j&1))
				p--;
			if(!p){
				*x=0;
				break;
			}
		}
		if(p||i!=2||(j&1)){
			snprintf(tmpbuf,TMPBUFSIZE,"%s: Syntax error",w);
			free(w);
			error();
		}
		x++;
		i=0;
		if(!strcmp(w+4,"-"))
			i=1;
		j=0;
		ret=1;
		for(i||doline(w+4,DOIF|domode);i||!*y||doline(y,DOIF|domode);doline(z,DOIF|domode)){
			i=0;
			j++;
			ret=doline(x,domode);
			if(ret==BREAK||ret==RETURN||ret==RETURN_NULL||ret==END)
				break;
			if(_max_loop&&j>=_max_loop)
				break;
		}
		if(ret==BREAK)
			ret=1;
		free(w);
		return ret;
	}else
	/* goto */
	if(!strncmp(w,"goto(",5)){
		ch=0;
		p=1;
		for(x=w+5;*x;x++){
			if(*x=='(')
				p++;
			else
			if(*x==')')
				p--;
			if(!p){
				ch=*x;
				*x=0;
				break;
			}
		}
		if(p){
			snprintf(tmpbuf,TMPBUFSIZE,"%s: Syntax error",w);
			free(w);
			error();
		}
		if((i=label(w+5))<0){
			snprintf(tmpbuf,TMPBUFSIZE,"%s: No such label",w+5);
			free(w);
			error();
		}
		dolineno=i-1;
		if(ch==')'){
			for(x++;*x==',';x++);
			if(*x)
				doline(x,domode);
		}
		free(w);
		ret=1;
		return ret;
	}else
	/* printf, sprintf */
	if(!strncmp(w,"printf(",7)||!strncmp(w,"sprintf(",8)){
#define	myprintf(fmt,var)	{if(k)snprintf(tmpout,tmplen,fmt,var); \
				else fprintf(stdout,fmt,var);}
#define	myprintv(var)		{if(k)printv(tmpout,tmplen,var); \
				else printv(NULL,0,var);}
#define	my_print_matrix(A,m)	{if(k)_print_matrix(tmpout,tmplen,A,m); \
				else _print_matrix(NULL,0,A,m);}
		k=0;
		if(!strncmp(w,"sprintf(",8)){
			k=1;
			tmpbuf[0]=0;
			j=0;
			for(x=w+8;*x;x++){
				if(*x=='"'&&*(x-1)!='\\')
				    	j++;
				else
				if((*x==','||*x==')')&&!(j&1))
					break;
			}
			y=w+8;
			if(*y=='"'){
				snprintf(tmpbuf,TMPBUFSIZE,"%s: Syntax error",y);
				free(w);
				error();
			}
			*x++=0;
			pname=y;
			y=x;
		}else
			x=y=w+7;
		j=0;
		p=0;
		for(;*x;x++){
			if(*x=='\\'){
				j++;
				continue;
			}
		    	if(*x=='"'&&!(j%2&&*(x-1)=='\\'))
				p++;
			else
			if((*x==','||*x==')')&&!(p&1))
				break;
			j=0;
		}
		if(*x==')'){
			if(*y=='$'||(*y=='"'&&*(x-1)=='"')){
				if(*y=='"'){
					y++;
					*(x-1)=0;
				}else
					*x=0;
				if((t=getstr(y))){
					myprintf("%s",t);
					free(t);
				}
			}else{
				*x=0;
				doline(y,domode);
			}
			if(k)
				setstr(pname,tmpbuf);
			if(*(x+1)==',')
				doline(x+2,domode);
			free(w);
			ret=1;
			return ret;
		}
		if(*y!='$'&&(*y!='"'||*(x-1)!='"')){
			snprintf(tmpbuf,TMPBUFSIZE,"%s: Syntax error",y);
			free(w);
			error();
		}
		if(*y=='"'){
			y++;
			*(x-1)=0;
		}else
			*x=0;
		x++;
		if((v=y=getstr(y))){
			for(z=y;*z;z++){
				if(*z=='%'){
					*z++=0;
					myprintf("%s",y);
					y=z+1;
					if(*z=='%'){
						myprintf("%%%s","");
						continue;
					}
					p=0;
					j=0;
					for(u=x;*u;u++){
						if(*u=='"'&&*(u-1)!='\\')
						    	j++;
						else
						if((*u=='('||*u=='[')&&!(j&1))
						    	p++;
						else
						if((*u==')'||*u==']')&&*(u+1)&&!(j&1))
						    	p--;
						if((*u==','||(*u==')'&&!*(u+1)))&&
						   !(j&1)&&!p)
							break;
					}
					ch=*u;
					*u=0;
					if(*z=='s'&&(*x=='$'||(*x=='"'&&*(u-1)=='"'))){
						if(*x=='"'){
							x++;
							*(u-1)=0;
						}else
							*u=0;
						if((t=getstr(x))){
							myprintf("%s",t);
							free(t);
						}
					}else
					if(*z=='v'&&!ismatrix(x)){
						myprintv(exprs(x));
					}else
					if(*z=='m'&&ismatrix(x)){
						my_print_matrix(mexprs(x),RM_MATRIX);
					}else{
						if(strchr("svm",*z))
							snprintf(tmpbuf,TMPBUFSIZE,"%s: Invalid parameter type",x);
						else
							snprintf(tmpbuf,TMPBUFSIZE,"%s: Invalid type identifier",x);
						free(w);
						error();
					}
					x=u+1;
					if(ch==')')
						break;
				}
			}
			myprintf("%s",y);
			if(k)
				setstr(pname,tmpbuf);
			free(v);
		}
		if(ch==')'){
			for(;*x==','||*x==';';x++);
			if(*x)
				doline(x,domode);
		}
		free(w);
		ret=1;
		return ret;
	}else
	/* eval */
	if(!strncmp(w,"eval(",5)){
		ch=0;
		p=1;
		j=0;
		for(x=w+5;*x;x++){
			if(*x=='"'&&*(x-1)!='\\')
			    	j++;
			else
			if((*x=='('||*x=='[')&&!(j&1))
				p++;
			else
			if((*x==')'||*x==']')&&!(j&1))
				p--;
			if(!p){
				ch=*x;
				*x=0;
				break;
			}
		}
		if(p){
			snprintf(tmpbuf,TMPBUFSIZE,"%s: Syntax error",w);
			free(w);
			error();
		}
		y=w+5;
		if(*y=='"'){
			y++;
			if(*(x-1)=='"')
				*(x-1)=0;
		}
		if((z=getstr(y))){
			k=0;
			j=strlen(z);
			for(i=0;i<j;i++){
				if(!strchr(" \a\b\t\n",z[i]))
					z[k++]=z[i];
			}
			z[k]=0;
			doline(z,domode);
			free(z);
		}
		if(ch==')'){
			for(x++;*x==','||*x==';';x++);
			if(*x)
				doline(x,domode);
		}
		free(w);
		ret=1;
		return ret;
	}else
	/* fstr */
	if(!strncmp(w,"fstr(",5)){
		x=w+strlen(w);
		if(*(x-1)==')')
			*(x-1)=0;
		for(x=w+5;*x&&*x!=',';x++);
		if(!*x||!*(x+1)){
			free(w);
			ret=0;
			return ret;
		}
		*x=0;
		setstr(w+5,fstr(x+1));
		*x=',';

		free(w);
		ret=1;
		return ret;
	}else
	/* local */
	if(!strncmp(w,"local(",6)){
		if(!grouplocal){
			snprintf(tmpbuf,TMPBUFSIZE,"%s: local() can be placed only at the start of a group",w);
			free(w);
			error();
		}
		ch=0;
		for(x=y=w+6;*x&&ch!=')';x++){
			if(*x==','||*x==')'){
				ch=*x;
				*x=0;
				snprintf(tmpbuf,TMPBUFSIZE,"_%d_%s",group,y);
				for(i=0;i<ngroupvars;i++)
					if(!strcmp(tmpbuf,groupvars[i]))
						break;
				if(i<ngroupvars){
					y=x+1;
					continue;
				}

				i=strlen(tmpbuf);
				/* DEBUG: free */
				if(!groupvars)
					groupvars=(char **)malloc((ngroupvars+1)*sizeof(char *));
				else
					groupvars=(char **)realloc(groupvars,(ngroupvars+1)*sizeof(char *));
				if(!groupvars){
					snprintf(tmpbuf,TMPBUFSIZE,"Memory exhausted");
					free(w);
					error();
				}
				groupvars[ngroupvars]=(char *)malloc(i+1);
				if(!groupvars[ngroupvars]){
					for(i=0;i<ngroupvars;i++)
						free(groupvars[i]);
					free(groupvars);
					groupvars=0;

					snprintf(tmpbuf,TMPBUFSIZE,"Memory exhausted");
					free(w);
					error();
				}
				strcpy(groupvars[ngroupvars++],tmpbuf);
				k=strlen(y);
				if(*y=='$')
					setstr(tmpbuf,str(y+1));
				else
				if(y[k-2]=='['&&y[k-1]==']'){
					y[k-2]=0;
					tmpbuf[i-2]=0;
					setmatrix(tmpbuf,matrix(y));
				}else
					setvar(tmpbuf,var(y));
				y=x+1;
			}
		}
		if(ch==')'){
			for(;*x==','||*x==';';x++);
			if(*x)
				doline(x,domode);
		}
		free(w);
		ret=1;
		return ret;
	}else
	/* return */
	if(!strncmp(w,"return(",7)){
		if(*(w+7)==')'){
			for(x=w+8;*x==',';x++);
			if(*x)
				doline(x,domode);
			free(w);
			ret=RETURN_NULL;
			return ret;
		}
		ch=0;
		p=1;
		j=0;
		for(x=w+7;*x;x++){
			if(*x=='"'&&*(x-1)!='\\')
			    	j++;
			else
			if((*x=='('||*x=='[')&&!(j&1))
			    	p++;
			else
			if((*x==')'||*x==']')&&!(j&1))
			    	p--;
			if(!p){
				ch=*x;
				*x=0;
				break;
			}
		}
		if(p||(j&1)){
			snprintf(tmpbuf,TMPBUFSIZE,"%s: Syntax error",w);
			free(w);
			error();
		}
		lastret=0;
		if(domode&DOVAR){
			lastret=1;
			setvar("_ret",exprs(w+7));
		}else
		if(domode&DOMATRIX){
			lastret=2;
			setmatrix("_ret",mexprs(w+7));
		}
		if(ch==')'){
			for(x++;*x==',';x++);
			if(*x)
				doline(x,domode);
		}
		free(w);
		ret=RETURN;
		return ret;
	}else
	/* pause */
/*
	if(!strncmp(w,"pause(",6)){
		if(*(w+6)==')'){
			for(x=w+7;*x==',';x++);
			if(*x)
				doline(x,domode);
			Pause=1;
			UseNilEvent(FALSE);
			if(!_g_on)
				SetStatus(STATUS_PAUSE);
			else
				SetGrfStatus(STATUS_PAUSE);
			free(w);
			ret=1;
			return ret;
		}
		i=(int)exprs(w+6);
		if(i<0)
			i=1000;

		free(w);
		ret=1;
		return ret;
	}else
*/
	/* exit */
	if(!strncmp(w,"exit(",5)){
		if(*(w+5)==')'){
			for(x=w+6;*x==',';x++);
			if(*x)
				doline(x,domode);
			free(w);
			ret=END;
			return ret;
		}
		ch=0;
		p=1;
		j=0;
		for(x=w+5;*x;x++){
			if(*x=='"'&&*(x-1)!='\\')
				j++;
			else
			if((*x=='('||*x=='[')&&!(j&1))
				p++;
			else
			if((*x==')'||*x==']')&&!(j&1))
				p--;
			else
			if(*x==';'&&!(j&1))
				*x=',';
			if(!p){
				ch=*x;
				*x=0;
				break;
			}
		}
		for(y=w+5;*y==',';y++);
		if(*y)
			doline(y,domode);
		if(ch==')'){
			for(x++;*x==',';x++);
			if(*x)
				doline(x,domode);
		}
		free(w);
		ret=END;
		return ret;
	}else
	/* srand */
	if(!strncmp(w,"srand(",6)){
		if(*(w+6)==')'){
			rand_next=time(NULL);
			for(x=w+7;*x==','||*x==';';x++);
			if(*x)
				doline(x,domode);
			free(w);
			ret=1;
			return ret;
		}
		rand_next=(unsigned long)exprs(w+6);
		if(*pcur==')'){
			for(x=pcur+1;*x==','||*x==';';x++);
			if(*x)
				doline(x,domode);
		}
		free(w);
		ret=1;
		return ret;
	}else
	/* reset */
	if(!strncmp(w,"reset(",6)){
		if(*(w+6)==')'){
			inithash();
			firstcalc=1;
			for(x=w+7;*x==','||*x==';';x++);
			if(*x)
				doline(x,domode);
			free(w);
			ret=1;
			return ret;
		}
		ch=0;
		for(x=y=w+6;*x&&ch!=')';x++){
			if(*x==','||*x==')'){
				ch=*x;
				*x=0;
				if(strstr(y,"var")==y){
					freegroupvars();
					freevar();
					freematrix();
					setsysvars();
					firstcalc=1;
				}else
				if(strstr(y,"func")==y){
					freefunc();
					freemfunc();
					setsysfuncs();
					setsysmfuncs();
				}else
				if(strstr(y,"label")==y){
					freelabel();
				}
				y=x+1;
			}
		}
		if(ch==')'){
			for(;*x==','||*x==';';x++);
			if(*x)
				doline(x,domode);
		}
		free(w);
		ret=1;
		return ret;
	}
#ifdef	USE_GD
	else
	/* Graphing Routines */
	if(strstr(w,"grf")==w){
		done=0;
		if(!strncmp(w,"grfbegin(",9)){
			j=0;
			for(x=w+9;*x;x++){
				if(*x=='"'&&*(x-1)!='\\')
					j++;
				if(!(j&1)&&*x==',')
					break;
			}
			if((j&1)||*x!=','){
				snprintf(tmpbuf,TMPBUFSIZE,"%s: Syntax error",w);
				free(w);
				error();
			}
			y=w+9;
			if(*y=='"'){
				y++;
				if(*(x-1)=='"')
					*(x-1)=0;
			}
			if((imfile=getstr(y))){
				k=0;
				j=strlen(imfile);
				for(i=0;i<j;i++){
					if(!strchr(" \a\b\t\n",imfile[i]))
						imfile[k++]=imfile[i];
				}
				imfile[k]=0;
/*******************************************************************************
				if(!access(imfile,R_OK)){
					snprintf(tmpbuf,TMPBUFSIZE,"%s: File already exist",imfile);
					free(imfile);
					free(w);
					error();
				}
*******************************************************************************/
			}else{
				snprintf(tmpbuf,TMPBUFSIZE,"%s: File name error?",y);
				free(w);
				error();
			}
			grfwidth=exprs(x+1);
			if(*pcur==',')
				grfheight=exprs(pcur+1);
			im=gdImageCreate(grfwidth,grfheight);
			_g_bgcolor=gdImageColorResolve(im,
					(g_bgcolor&0xff0000)>>16,
					(g_bgcolor&0x00ff00)>>8,
					(g_bgcolor&0x0000ff));
			if(g_trans>=0){
				_g_trans=gdImageColorResolve(im,
						(g_trans&0xff0000)>>16,
						(g_trans&0x00ff00)>>8,
						(g_trans&0x0000ff));
				gdImageColorTransparent(im,_g_trans);
			}
			/*
			else{
				gdImageColorTransparent(im,-1);
			}
			*/

			_g_color=gdImageColorResolve(im,
					(g_color&0xff0000)>>16,
					(g_color&0x00ff00)>>8,
					(g_color&0x0000ff));
			_g_axes=gdImageColorResolve(im,
					(g_axes&0xff0000)>>16,
					(g_axes&0x00ff00)>>8,
					(g_axes&0x0000ff));
			if(_g_style){
				if(_g_style==1){
					gstyle=gstyle1;
					k=sizeof(gstyle1)/sizeof(int);
				}else{
					gstyle=gstyle2;
					k=sizeof(gstyle2)/sizeof(int);
				}
				for(j=0;j<k;j++)
					g_style[j]=(gstyle[j]?_g_color:gdTransparent);
				gdImageSetStyle(im,g_style,k);
				_g_color=gdStyled;
			}
			done=1;
		}else
		if(!strncmp(w,"grfend(",7)){
			fp=fopen(imfile,"w");
			gdImagePng(im,fp);
			fclose(fp);
			free(imfile);
			gdImageDestroy(im);
			im=NULL;
			done=1;
		}else
		/* draw 2d graph */
		if(!strncmp(w,"grf(",4)){
			p=strlen(w);
			// TODO
			/*
			if(initgrfcmd(p+2)){
				snprintf(tmpbuf,TMPBUFSIZE,"Memory exhausted");
				free(w);
				error();
			}
			*/
	
			_sx=(sx<_sxr?_sxr:sx);
			_ex=(ex>_exr?_exr:ex);
	
			grffunc=w+4;
			draw2dgrf();
			grffunc=NULL;
			done=1;
		}else
		/* draw 3d graph */
		if(!strncmp(w,"grf3d(",6)){
			p=strlen(w);
			// TODO
			/*
			if(initgrfcmd(p+2)){
				snprintf(tmpbuf,TMPBUFSIZE,"Memory exhausted");
				free(w);
				error();
			}
			*/

			_sx=(sx<_sxr?_sxr:sx);
			_ex=(ex>_exr?_exr:ex);
			_sy=(sy<_syr?_syr:sy);
			_ey=(ey>_eyr?_eyr:ey);
	
			grffunc=w+6;
			draw3dgrf();
			grffunc=NULL;
			done=1;
		}else
		/* set x range */
		if(!strncmp(w,"grfxr(",6)){
			sxr=exprs(w+6);
			if(*pcur==',')
				exr=exprs(pcur+1);
			if(sxr>exr){
				tmp=sxr;
				sxr=exr;
				exr=tmp;
			}else
			if(sxr==exr)
				exr=sxr+1;
			_sxr=sxr;
			_exr=exr;
			done=1;
		}else
		/* set y range */
		if(!strncmp(w,"grfyr(",6)){
			syr=exprs(w+6);
			if(*pcur==',')
				eyr=exprs(pcur+1);
			if(syr>eyr){
				tmp=syr;
				syr=eyr;
				eyr=tmp;
			}else
			if(syr==eyr)
				eyr=syr+1;
			_syr=syr;
			_eyr=eyr;
			done=1;
		}else
		/* set z range */
		if(!strncmp(w,"grfzr(",6)){
			szr=exprs(w+6);
			if(*pcur==',')
				ezr=exprs(pcur+1);
			if(szr>ezr){
				tmp=szr;
				szr=ezr;
				ezr=tmp;
			}
			_szr=szr;
			_ezr=ezr;
			done=1;
		}else
		/* set x and y ranges */
		if(!strncmp(w,"grfxyr(",7)){
			sxr=exprs(w+7);
			if(*pcur==',')
				exr=exprs(pcur+1);
			if(*pcur==',')
				syr=exprs(pcur+1);
			if(sxr>exr){
				tmp=sxr;
				sxr=exr;
				exr=tmp;
			}else
			if(sxr==exr)
				exr=sxr+1;
			eyr=syr+(exr-sxr)/grfwidth*grfheight;
			_sxr=sxr;
			_exr=exr;
			_syr=syr;
			_eyr=eyr;
			done=1;
		}else
		/* set y and x ranges */
		if(!strncmp(w,"grfyxr(",7)){
			syr=exprs(w+7);
			if(*pcur==',')
				eyr=exprs(pcur+1);
			if(*pcur==',')
				sxr=exprs(pcur+1);
			if(syr>eyr){
				tmp=syr;
				syr=eyr;
				eyr=tmp;
			}else
			if(syr==eyr)
				eyr=syr+1;
			exr=sxr+(eyr-syr)/grfheight*grfwidth;
			_syr=syr;
			_eyr=eyr;
			_sxr=sxr;
			_exr=exr;
			done=1;
		}else
		/* set increments of x and y */
		if(!strncmp(w,"grfdxy(",7)){
			dx=exprs(w+7);
			if(dx<0)
				dx=-dx;
			else
			if(!dx)
				dx=DX;
			if(*pcur==',')
				dy=exprs(pcur+1);
			if(dy<0)
				dy=-dy;
			else
			if(!dy)
				dy=DY;
			_dx=dx;
			_dy=dy;
			done=1;
		}else
		/* set plotting range of x */
		if(!strncmp(w,"grfx(",5)){
			sx=exprs(w+5);
			if(*pcur==',')
				ex=exprs(pcur+1);
			if(sx>ex){
				tmp=sx;
				sx=ex;
				ex=tmp;
			}
			_sx=sx;
			_ex=ex;
			done=1;
		}else
		/* set plotting range of y */
		if(!strncmp(w,"grfy(",5)){
			sy=exprs(w+5);
			if(*pcur==',')
				ey=exprs(pcur+1);
			if(sy>ey){
				tmp=sy;
				sy=ey;
				ey=tmp;
			}
			_sy=sy;
			_ey=ey;
			done=1;
		}else
		/* set increments of small x and y tics */
		if(!strncmp(w,"grft(",5)){
			xt=exprs(w+5);
			if(xt<0)
				xt=-xt;
			if(*pcur==',')
				yt=exprs(pcur+1);
			if(yt<0)
				yt=-yt;
			_xt=xt;
			_yt=yt;
			done=1;
		}else
		/* set increments of large x and y tics with value labels */
		if(!strncmp(w,"grftl(",6)){
			xtl=exprs(w+6);
			if(xtl<0)
				xtl=-xtl;
			if(*pcur==',')
				ytl=exprs(pcur+1);
			if(ytl<0)
				ytl=-ytl;
			_xtl=xtl;
			_ytl=ytl;
			done=1;
		}else
		/* set origin of tics */
		if(!strncmp(w,"grfto(",6)){
			xto=exprs(w+6);
			if(*pcur==',')
				yto=exprs(pcur+1);
			_xto=xto;
			_yto=yto;
			done=1;
		}else
		/* TODO: set x-axis label
		if(!strncmp(w,"grfxl(",6)){
			done=1;
		}else
		*/
		/* TODO: set y-axis label
		if(!strncmp(w,"grfyl(",6)){
			done=1;
		}else
		*/
		/* set 3d view-port */
		if(!strncmp(w,"grf3dvp(",8)){
			if(*(w+8)!=','&&*(w+8)!=')')
				vp.x0=exprs(w+8);
			else
			if(*(w+8)==',')
				pcur=w+8;
			/*
			if(vp.x0<0)
				vp.x0=0;
			else
			if(vp.x0>grfwidth-1)
				vp.x0=grfwidth-1;
			*/

			if(*pcur==','&&*(pcur+1)!=','&&*(pcur+1)!=')')
				vp.y0=exprs(pcur+1);
			else
			if(*pcur==','&&*(pcur+1)==',')
				pcur++;
			/*
			if(vp.y0<0)
				vp.y0=0;
			else
			if(vp.y0>grfheight-1)
			vp.y0=grfheight-1;
			*/
			/*
			vp.sxr=0;
			vp.exr=160;
			vp.syr=0;
			vp.eyr=160;
			*/

			if(*pcur==','&&*(pcur+1)!=','&&*(pcur+1)!=')')
				vp.rho=exprs(pcur+1);
			else
			if(*pcur==','&&*(pcur+1)==',')
				pcur++;

			if(*pcur==','&&*(pcur+1)!=','&&*(pcur+1)!=')')
				vp.phi=exprs(pcur+1);
			else
			if(*pcur==','&&*(pcur+1)==',')
				pcur++;

			if(*pcur==','&&*(pcur+1)!=','&&*(pcur+1)!=')')
				vp.theta=exprs(pcur+1);
			else
			if(*pcur==','&&*(pcur+1)==',')
				pcur++;

			if(*pcur==','&&*(pcur+1)!=','&&*(pcur+1)!=')')
				vp.d=exprs(pcur+1);
			else
			if(*pcur==','&&*(pcur+1)==',')
				pcur++;

			_vp.x0=vp.x0;
			_vp.y0=vp.y0;
			_vp.rho=vp.rho;
			_vp.phi=vp.phi;
			_vp.theta=vp.theta;
			_vp.d=vp.d;
			done=1;
		}else
		/* set 3d graphing way */
		if(!strncmp(w,"grf3dw(",7)){
			grf3dw=0x0;
			x=w+7;
			for(x;*x&&*x!=')';x++){
				if(*x=='x')
					grf3dw|=0x1;
				else
				if(*x=='y')
					grf3dw|=0x2;
			}
			_grf3dw=grf3dw;
			done=1;
		}else
		/* reset graph variables */
		if(!strncmp(w,"grfreset(",9)){
			initgrfvars();
			done=1;
		}
		if(done){
			for(x=w;*x&&*x!='(';x++);
			if(*x=='('){
				p=1;
				j=0;
				for(x++;*x;x++){
					if(*x=='"'&&*(x-1)!='\\')
					    	j++;
					else
					if((*x=='('||*x=='[')&&!(j&1))
					    	p++;
					else
					if((*x==')'||*x==']')&&!(j&1))
					    	p--;
					if(!p)
						break;
				}
				if(*x==')'){
					for(x++;*x==','||*x==';';x++);
					if(*x)
						doline(x,domode);
				}
			}
			free(w);
			ret=1;
			return ret;
		}
	}
#endif

	/* unimplemented reserved words drop here. */
	fprintf(stdout,"%s: Unimplemented reserved word\n",w);
	free(w);

	return ret;
}

void
mkformat(void)
{
	int	width;

	if(_v_base==8){
		if(_v_width<=0){
			width=-_v_width-2;
			if(width<=0)
				width=0;
			if(_v_prec>=0)
				snprintf(format,10,"0o%%0%do",width);
			else
				snprintf(format,10,"%%0%do",width);
		}else
			if(_v_prec>=0)
				snprintf(format,10,"0o%%o");
			else
				snprintf(format,10,"%%o");
	}else
	if(_v_base==16){
		if(_v_width<=0){
			width=-_v_width-2;
			if(width<=0)
				width=0;
			if(_v_prec>=0)
				snprintf(format,10,"0x%%0%dx",width);
			else
				snprintf(format,10,"%%0%dx",width);
		}else
			if(_v_prec>=0)
				snprintf(format,10,"0x%%x");
			else
				snprintf(format,10,"%%x");
	}else
	if(_v_base==10){
		if(_v_width>=0){
			if(_v_prec>=0)
				snprintf(format,10,"%%%d.%df",_v_width,_v_prec);
			else
				snprintf(format,10,"%%%d.%dg",_v_width,-_v_prec);
		}else{
			if(_v_prec>=0)
				snprintf(format,10,"%%0%d.%df",-_v_width,_v_prec);
			else
				snprintf(format,10,"%%0%d.%dg",-_v_width,-_v_prec);
		}
	}

	return;
}

void
printv(voutbuf,voutlen,value)
	char	*voutbuf;
	int	voutlen;
	double	value;
{
	int		i,j;
	unsigned long	v;
	char		tmp[256];

	if(_v_base==2){
		v=(unsigned long)value;
		v*=1<<_b_frac;
		i=255;
		tmp[i--]=0;
		for(;(j=v/2);){
			tmp[i--]=v%2+'0';
			v=j;
		}
		tmp[i--]=v%2+'0';
		if(_v_width<0){
			j=-_v_width-254+i-2;
			for(;j>0;j--)
				tmp[i--]='0';
			tmp[i--]='b';
			tmp[i--]='0';
		}else{
			tmp[i--]='b';
			tmp[i--]='0';
			j=_v_width-254+i;
			for(;j>0;j--)
				tmp[i--]=' ';
		}
		if(voutbuf)
			snprintf(voutbuf,voutlen,"%s",tmp+i+1);
		else
			fprintf(stdout,"%s",tmp+i+1);
	}else
	if(_v_base==8||_v_base==16){
		value*=1<<(_v_base==8?_o_frac:_h_frac);
		if(_v_width>0){
			snprintf(tmp,256,format,(int)value);
			if(voutbuf)
				snprintf(voutbuf,voutlen,"%*s",_v_width,tmp);
			else
				fprintf(stdout,"%*s",_v_width,tmp);
		}else{
			if(voutbuf)
				snprintf(voutbuf,voutlen,format,(int)value);
			else
				fprintf(stdout,format,(int)value);
		}
	}else{
		if(voutbuf)
			snprintf(voutbuf,voutlen,format,value);
		else
			fprintf(stdout,format,value);
	}

	return;
}

double
exprs(str)
	char	*str;
{
	double	value,value2;

	if(str)
		pcur=str;

	value=eqneq();

	for(;;)
		if(*pcur=='&'&&*(pcur+1)=='&'){
			pcur+=2;
			value2=eqneq();
			value=(value&&value2);
		}else if(*pcur=='|'&&*(pcur+1)=='|'){
			pcur+=2;
			value2=eqneq();
			value=(value||value2);
		}else
			return value;
}

double
eqneq(void)
{
	char	*x;
	double	value=ltegte(),value2;

	for(;;)
		if(*pcur=='='&&*(pcur+1)=='='){
			pcur+=2;
			x=pcur;
			value2=expr();
			value=(value==value2);

			if(*pcur=='<'||*pcur=='>'){
				pcur=x;
				value2=ltegte();
				value=(value&&value2);
			}else
			if((*pcur=='!'||*pcur=='=')&&*(pcur+1)=='='){
				pcur=x;
				value2=eqneq();
				value=(value&&value2);
			}else
			if(*pcur=='?'){
				if(value){
					value=exprs(pcur+1);
					if(*pcur==':'){
						skip_result++;
						exprs(pcur+1);
						skip_result--;
					}
				}else{
					skip_result++;
					exprs(pcur+1);
					skip_result--;
					if(*pcur==':')
						value=exprs(pcur+1);
					else
						value=0;
				}
			}
		}else if(*pcur=='!'&&*(pcur+1)=='='){
			pcur+=2;
			x=pcur;
			value2=expr();
			value=(value!=value2);

			if(*pcur=='<'||*pcur=='>'){
				pcur=x;
				value2=ltegte();
				value=(value&&value2);
			}else
			if((*pcur=='!'||*pcur=='=')&&*(pcur+1)=='='){
				pcur=x;
				value2=eqneq();
				value=(value&&value2);
			}else
			if(*pcur=='?'){
				if(value){
					value=exprs(pcur+1);
					if(*pcur==':'){
						skip_result++;
						exprs(pcur+1);
						skip_result--;
					}
				}else{
					skip_result++;
					exprs(pcur+1);
					skip_result--;
					if(*pcur==':')
						value=exprs(pcur+1);
					else
						value=0;
				}
			}
		}else if(*pcur=='?'){
			if(value){
				value=exprs(pcur+1);
				if(*pcur==':'){
					skip_result++;
					exprs(pcur+1);
					skip_result--;
				}
			}else{
				skip_result++;
				exprs(pcur+1);
				skip_result--;
				if(*pcur==':')
					value=exprs(pcur+1);
				else
					value=0;
			}
		}else
			return value;
}

double
ltegte(void)
{
	char	*x;
	double	value=expr(),value2;

	for(;;)
		switch(*pcur++){
			case '<':
				if(*pcur=='='){
					pcur++;
					x=pcur;
					value2=expr();
					value=(value<=value2);
				}else{
					x=pcur;
					value2=expr();
					value=(value<value2);
				}

				if(*pcur=='<'||*pcur=='>'){
					pcur=x;
					value2=ltegte();
					value=(value&&value2);
				}else
				if((*pcur=='!'||*pcur=='=')&&*(pcur+1)=='='){
					pcur=x;
					value2=eqneq();
					value=(value&&value2);
				}else
				if(*pcur=='?'){
					if(value){
						value=exprs(pcur+1);
						if(*pcur==':'){
							skip_result++;
							exprs(pcur+1);
							skip_result--;
						}
					}else{
						skip_result++;
						exprs(pcur+1);
						skip_result--;
						if(*pcur==':')
							value=exprs(pcur+1);
						else
							value=0;
					}
				}
				break;
			case '>':
				if(*pcur=='='){
					pcur++;
					x=pcur;
					value2=expr();
					value=(value>=value2);
				}else{
					x=pcur;
					value2=expr();
					value=(value>value2);
				}

				if(*pcur=='<'||*pcur=='>'){
					pcur=x;
					value2=ltegte();
					value=(value&&value2);
				}else
				if((*pcur=='!'||*pcur=='=')&&*(pcur+1)=='='){
					pcur=x;
					value2=eqneq();
					value=(value&&value2);
				}else
				if(*pcur=='?'){
					if(value){
						value=exprs(pcur+1);
						if(*pcur==':'){
							skip_result++;
							exprs(pcur+1);
							skip_result--;
						}
					}else{
						skip_result++;
						exprs(pcur+1);
						skip_result--;
						if(*pcur==':')
							value=exprs(pcur+1);
						else
							value=0;
					}
				}
				break;
			default:
				pcur--;
				return value;
		}
}

double
expr(void)
{
	double	value=terms();

	for(;;)
		switch(*pcur++){
			case '+':
				value=value+terms();
				break;
			case '-':
				value=value-terms();
				break;
			default:
				pcur--;
				return value;
		}
}

double
terms(void)
{
	double	value=term(),value2;
	
	for(;;)
		switch(*pcur++)
		{
			case '*':
				value=value*term();
				break;
			case '/':
				value2=term();
				if(value2==0.0){
					snprintf(tmpbuf,TMPBUFSIZE,"Divided by zero");
					error();
				}
			    	value=value/value2;
				break;
			default:
				pcur--;
				return value;
		}
}

double
term(void)
{
	double	value=factor(),value2,value3;
	
	for(;;)
		switch(*pcur++)
		{
			case '^':
				value=pow(value,factor());
				break;
			case '%':
				value2=factor();
				if(value2==0.0){
					snprintf(tmpbuf,TMPBUFSIZE,"Divided by zero");
					error();
				}
				value=fmod(value,value2);
				break;
			case '<':
				if(*pcur!='<'){
					pcur--;
					return value;
				}
				pcur++;
				value=(int)(value*(1<<0))<<(int)factor();
				value=((int)value/(1<<0));
				break;
			case '>':
				if(*pcur!='>'){
					pcur--;
					return value;
				}
				pcur++;
				value=(int)(value*(1<<_b_frac))>>(int)factor();
				value=((int)value/(1<<_b_frac));
				break;
			case '&':
				value=(int)(value*(1<<_b_frac))&(int)(factor()*(1<<_b_frac));
				value=((int)value/(1<<_b_frac));
				break;
			case '|':
				value=(int)(value*(1<<_b_frac))|(int)(factor()*(1<<_b_frac));
				value=((int)value/(1<<_b_frac));
				break;
			case '\'':
				value*=1<<_b_frac;
				value2=factor()*(1<<_b_frac);
				value3=(int)value|(int)value2;
				value2=(int)value&(int)value2;
				value=value3-value2;
				value=((int)value/(1<<_b_frac));
				break;
			default:
				pcur--;
				return value;
		}
}

double
factor(void)
{
	int	i,j,p;
	double	value;
	char	*x;
	char	ch;

	value=0;

	if((*pcur=='+'&&*(pcur+1)!='+')||
	   (*pcur=='-'&&*(pcur+1)!='-')){
		i=(*pcur=='+'? 1:-1);
		pcur++;
		value=i*factor();
	}else if(*pcur=='('){
		i=1;
		pcur++;
		for(x=pcur;*x;x++){
			switch(*x){
				case '(':
					i++;
					break;
				case ')':
					i--;
					break;
			}
			if(!i){
				*x=0;
				break;
			}
		}
		value=exprs(pcur);
		*x=')';
		pcur=x+1;
	}else if(*pcur=='.'||isdigit(*pcur)){
		value=0.0;
		if(*pcur=='0'&&tolower(*(pcur+1))=='b'){
			for(pcur+=2;*pcur>='0'&&*pcur<'2';pcur++)
				value=2*value+*pcur-'0';
			value/=1<<_b_frac;
		}else
		if(*pcur=='0'&&tolower(*(pcur+1))=='o'){
			for(pcur+=2;*pcur>='0'&&*pcur<'8';pcur++)
				value=8*value+*pcur-'0';
			value/=1<<_o_frac;
		}else
		if(*pcur=='0'&&
		   (tolower(*(pcur+1))=='x'||tolower(*(pcur+1))=='h')){
			for(pcur+=2;*pcur&&(isdigit(*pcur)||strchr("abcdefABCDEF",*pcur));pcur++)
				value=0x10*value+tolower(*pcur)-(isdigit(*pcur)?'0':'a'-0xa);
			value/=1<<_h_frac;
		}else{
			for(x=pcur;*pcur=='.'||isdigit(*pcur);pcur++);
			ch=*pcur;
			*pcur=0;
			value=atof(x);
			*pcur=ch;
			if(*pcur=='e'||*pcur=='E'){
				pcur++;
				value*=pow(10,factor());
			}
		}
	}else if(isfunc(pcur)){
		value=func(pcur);
		if(func2){
			/* Parsing sysfunc2 functions */
			/* mod */
			if(!strncmp(pcur,"mod(",4)){
				double	x,y=0.0;

				x=exprs(pcur+4);
				if(*pcur==',')
					y=exprs(pcur+1);
				if(y==0.0){
					snprintf(tmpbuf,TMPBUFSIZE,"Divided by zero");
					error();
				}else
					value=fmod(x,y);
			}else
			/* round */
			if(!strncmp(pcur,"round(",6)){
				double	x;
				int	i=0;

				x=exprs(pcur+6);
				if(*pcur==',')
					i=(int)exprs(pcur+1);
				value=_round(x,i);
			}else
			/* solve */
			if(!strncmp(pcur,"solve(",6)){
#define	SOLVE_N		100
#ifdef	ZERO
#define	SOLVE_TOL	ZERO
#else
#define	SOLVE_TOL	1.0e-10
#endif
#define	SOLVE_NBO	200
#define	SOLVE_FACTOR	1.6
				char	*x,*func;
				double	x1=-1,x2=1,xb1,xb2,
					tol=SOLVE_TOL,factor=SOLVE_FACTOR,xtmp;
				int	n=SOLVE_N,nbi,nbs=1,nbo=SOLVE_NBO;

				x=pcur;
				func=pcur+6;
				skip_result++;
				exprs(func);
				skip_result--;

				if(*pcur==','){
					x1=exprs(pcur+1)-1;
					x2=x1+1;
				}
				if(*pcur==','){
					x1+=1;
					x2=exprs(pcur+1);
					if(x1>x2){
						xtmp=x1;
						x1=x2;
						x2=xtmp;
					}else
					if(x1==x2){
						x1-=1;
						x2+=1;
					}
					nbo=0;
				}
				nbi=(x2-x1)/0.1;
				if(*pcur==','&&*(pcur+1)!=','&&*(pcur+1)!=')'){
					n=(int)exprs(pcur+1);
					if(n<=0)
						n=SOLVE_N;
				}else
				if(*pcur==','&&*(pcur+1)==',')
					pcur++;
				if(*pcur==','&&*(pcur+1)!=','&&*(pcur+1)!=')')
					tol=exprs(pcur+1);
				else
				if(*pcur==','&&*(pcur+1)==',')
					pcur++;
				if(*pcur==','&&*(pcur+1)!=','&&*(pcur+1)!=')'){
					nbi=(int)exprs(pcur+1);
					if(nbi<=0)
						nbi=(x2-x1)/0.1;
				}else
				if(*pcur==','&&*(pcur+1)==',')
					pcur++;
				if(*pcur==','){
					nbo=(int)exprs(pcur+1);
					if(nbo<=0)
						nbo=SOLVE_NBO;
				}
				if(*pcur==','){
					factor=exprs(pcur+1);
					if(factor<=0.0)
						factor=SOLVE_FACTOR;
				}
				_solve_bracket_in(func,x1,x2,nbi,&xb1,&xb2,&nbs);
				if(nbs){
					x1=xb1;
					x2=xb2;
				}else
				if(nbo){
					err=_solve_bracket_out(func,&x1,&x2,nbo,factor);
				}
				if(err){
					snprintf(tmpbuf,TMPBUFSIZE,"%s: Bracketing failed\n",x);
					error();
				}
				value=_solve_brent(func,x1,x2,n,tol);
				if(err){
					snprintf(tmpbuf,TMPBUFSIZE,"%s: Solving failed\n",x);
					error();
				}
			}else
			/* integration */
			if(!strncmp(pcur,"integ(",6)){
#define	INTEG_N	20
				char	*func;
				double	x1=0,x2=1,*x=0,*w=0;
				int	n=0,method=2;

				func=pcur+6;
				skip_result++;
				exprs(func);
				skip_result--;

				if(*pcur==',')
					x1=exprs(pcur+1);
				if(*pcur==',')
					x2=exprs(pcur+1);
				if(*pcur==','&&*(pcur+1)!=','&&*(pcur+1)!=')')
					n=(int)exprs(pcur+1);
				else
				if(*pcur==','&&*(pcur+1)==',')
					pcur++;
				if(n<=0)
					n=INTEG_N;
				if(*pcur==','){
					if(strstr(pcur+1,"simp")==pcur+1)
						method=0;
					else
					if(strstr(pcur+1,"trap")==pcur+1)
						method=1;
					else
					if(strstr(pcur+1,"gleg")==pcur+1)
						method=2;
				}
				if(method==2){
					if(n!=integ_n){
						integ_n=n;
						if(_integ_gaussleg(n,&integ_x,&integ_w)){
							snprintf(tmpbuf,TMPBUFSIZE,"Gauss-Legendre: Memory exhausted");
							error();
						}
					}
					x=integ_x;
					w=integ_w;
				}
				value=_integ(func,x1,x2,n,x,w,method);
			}else
			/* integration of ordinary differential equations */
			if(!strncmp(pcur,"odeint(",7)){
#define	ODEINT_N	100
				char	*func;
				double	x0=0,y0=0,x=0;
				int	n=ODEINT_N,method=1;

				func=pcur+7;
				skip_result++;
				exprs(func);
				skip_result--;

				if(*pcur==',')
					x0=exprs(pcur+1);
				if(*pcur==',')
					y0=exprs(pcur+1);
				if(*pcur==',')
					x=exprs(pcur+1);
				if(*pcur==','&&*(pcur+1)!=','&&*(pcur+1)!=')'){
					n=(int)exprs(pcur+1);
					if(n<=0)
						n=ODEINT_N;
				}else
				if(*pcur==','&&*(pcur+1)==',')
					pcur++;
				if(*pcur==','){
					if(strstr(pcur+1,"euler")==pcur+1)
						method=0;
					else
					if(strstr(pcur+1,"runge")==pcur+1)
						method=1;
				}
				value=_odeint(func,x0,y0,x,n,method);
			}else
			/* differentiation */
			if(!strncmp(pcur,"diff(",5)){
#define	DIFF_H	1.0e-3
				char	*func;
				double	x=0,h=DIFF_H;
				int	order=0;

				func=pcur+5;
				skip_result++;
				exprs(func);
				skip_result--;

				if(*pcur==',')
					x=exprs(pcur+1);
				if(*pcur==','&&*(pcur+1)!=','&&*(pcur+1)!=')'){
					h=exprs(pcur+1);
					if(h==0)
						h=DIFF_H;
				}else
				if(*pcur==','&&*(pcur+1)==',')
					pcur++;
				if(*pcur==',')
					order=(int)exprs(pcur+1);

				if(order<=0||order>3)
					order=1;

				value=_diff(func,x,h,order);
			}else
			/* interpolation */
			if(!strncmp(pcur,"interp(",7)){
				char		*str;
				_PMATRIX	A;
				double		x=0;

				str=pcur;
				A=mexprs(pcur+7);
				if(*pcur==',')
					x=exprs(pcur+1);
				value=_interp(A,x);
				_free_matrix(A);
				if(err){
					snprintf(tmpbuf,TMPBUFSIZE,"%s: Interpolation failed\n",str);
					error();
				}
			}else
			/* two-dimensional interpolation */
			if(!strncmp(pcur,"interp2(",8)){
				char		*str;
				_PMATRIX	A=0,B=0,C=0;
				double		x1=0,x2=0;

				str=pcur;
				pcur+=8;
				if(*pcur!=',')
					A=mexprs(pcur);
				if(*pcur==','&&*(pcur+1)!=','&&*(pcur+1)!=')')
					B=mexprs(pcur+1);
				else
				if(*pcur==','&&*(pcur+1)==',')
					pcur++;
				if(*pcur==','&&*(pcur+1)!=','&&*(pcur+1)!=')')
					C=mexprs(pcur+1);
				else
				if(*pcur==','&&*(pcur+1)==',')
					pcur++;
				if(*pcur==',')
					x1=exprs(pcur+1);
				if(*pcur==',')
					x2=exprs(pcur+1);
				value=_interp2(A,B,C,x1,x2);
				_free_matrix(A);
				_free_matrix(B);
				_free_matrix(C);
				if(err){
					snprintf(tmpbuf,TMPBUFSIZE,"%s: Interpolation failed\n",str);
					error();
				}
			}else
			/* linear regression */
			if(!strncmp(pcur,"regr(",5)){
				char		*str;
				_PMATRIX	A=0;
				int		n=0;
				char		*f=0,*c=0,*x=0;

				str=pcur;
				A=mexprs(pcur+5);
				if(*pcur==',')
					n=(int)exprs(pcur+1);
				if(*pcur==','&&*(pcur+1)!=','&&*(pcur+1)!=')'){
					f=pcur+1;
					for(pcur+=2;*pcur&&*pcur!=','&&*pcur!=')';pcur++);
					x=pcur;
				}else
				if(*pcur==','&&*(pcur+1)==',')
					pcur++;
				if(*pcur==','){
					c=pcur+1;
					for(pcur+=2;*pcur&&*pcur!=','&&*pcur!=')';pcur++);
					*pcur=0;
				}
				if(x)
					*x=0;
				value=1;
				if(_regr(A,n,f,c)){
					value=0;
					snprintf(tmpbuf,TMPBUFSIZE,"%s: Linear regression failed\n",str);
					error();
				}
				_free_matrix(A);
			}

			p=1;
			j=0;
			for(;*pcur;pcur++){
				if(*pcur=='"'&&*(pcur-1)!='\\')
				    	j++;
				else
				if((*pcur=='('||*pcur=='[')&&!(j&1))
				    	p++;
				else
				if((*pcur==')'||*pcur==']')&&!(j&1))
				    	p--;
				if(!p)
					break;
			}
			if(*pcur==')')
				pcur++;
		}
	}else{
	    	for(x=pcur;*x;x++)
		    	if(isdelimiter(*x)||*x=='[')
			    	break;
	    	if(*x=='[')
			value=matrixelement(pcur);
		else
		if(*pcur=='+'&&*(pcur+1)=='+'&&isvar(*(pcur+2))){
			pcur+=2;
			if(!skip_result){
				x=pcur;
				value=var(pcur)+1;
				setvar(x,value);
				just_setvar=1;
			}
		}else
		if(*pcur=='-'&&*(pcur+1)=='-'&&isvar(*(pcur+2))){
			pcur+=2;
			if(!skip_result){
				x=pcur;
				value=var(pcur)-1;
				setvar(x,value);
				just_setvar=1;
			}
		}else
		if(*pcur=='!'){
			pcur++;
			value=!factor();
		}else
		if(*pcur=='~'){
			pcur++;
			value=~((int)factor()*(1<<_b_frac));
			value=((int)value/(1<<_b_frac));
		}else
		if(isvar(*pcur)){
			x=pcur;
			value=var(pcur);
			if(*pcur=='+'&&*(pcur+1)=='+'){
				if(!skip_result){
					*pcur=0;
					setvar(x,value+1);
					*pcur='+';
				}
				pcur+=2;
				just_setvar=1;
			}else
			if(*pcur=='-'&&*(pcur+1)=='-'){
				if(!skip_result){
					*pcur=0;
					setvar(x,value-1);
					*pcur='-';
				}
				pcur+=2;
				just_setvar=1;
			}
		}else
			pcur++;
	}

	return value;
}

_PMATRIX
mexprs(str)
    	char	*str;
{
	_PMATRIX A;

	if(str)
		pcur=str;
//	A=mltegte();
	A=mexpr();

	return A;

	for(;;)
		if(*pcur=='='&&*(pcur+1)=='='){
			pcur+=2;
//			A=(A==mltegte());
		}else if(*pcur=='!'&&*(pcur+1)=='='){
			pcur+=2;
//			A=(A!=mltegte());
		}else
			return A;
}

_PMATRIX
mltegte(void)
{
	_PMATRIX A=mexpr();

	for(;;)
		switch(*pcur++){
			case '<':
				if(*pcur=='='){
					pcur++;
//					A=(A<=mexpr());
				}else
//					A=(A<mexpr());
				break;
			case '>':
				if(*pcur=='='){
					pcur++;
//					A=(A>=mexpr());
				}else
//					A=(A>mexpr());
				break;
			default:
				pcur--;
				return A;
		}
}

_PMATRIX
mexpr(void)
{
	_PMATRIX A=mterms();

	for(;;)
		switch(*pcur++){
			case '+':
				A=_add_matrix(A,mterms(),RM_MATRIX);
				break;
			case '-':
				A=_subtract_matrix(A,mterms(),RM_MATRIX);
				break;
			default:
				pcur--;
				return A;
		}
}

_PMATRIX
mterms(void)
{
	_PMATRIX 	A,Atmp;
	
	A=mterm();
	
	for(;;)
		switch(*pcur++)
		{
			case '*':
				if(*pcur=='.'){
					pcur++;
					A=_multiply_matrix_element(A,mterm(),RM_MATRIX);
				}else if((Atmp=mterm()))
					A=_multiply_matrix(A,Atmp,RM_MATRIX);
				else
					A=_scalar_matrix(gvalue,A,RM_MATRIX);
				break;
			case '/':
				if((Atmp=mterm()))
					A=_multiply_matrix(A,_inverse_matrix(Atmp,RM_MATRIX),RM_MATRIX);
				else
				if(gvalue)
					A=_scalar_matrix(1/gvalue,A,RM_MATRIX);
				else{
					snprintf(tmpbuf,TMPBUFSIZE,"Divided by zero");
				    	error();
				}
				break;
			default:
				pcur--;
				return A;
		}
}

_PMATRIX
mterm(void)
{
	int		i,j,k;
	_PMATRIX	A;
	
	A=mfactor();
	
	for(;;){
		i=0;
		switch(*pcur++)
		{
			case '^':
				if((j=term())){
					k=fabs(j)-1;
					for(i=0;i<k;i++){
						A=_multiply_matrix(A,A,RM_MATRIX);
						if(!A)
							break;
					}
					if(j<0&&A)
						A=_inverse_matrix(A,RM_MATRIX);
				}else
					A=NULL;
				i=0;
				break;
			case '+':
				if(*pcur=='.'){
					pcur++;
					A=_increase_matrix(term(),A,RM_MATRIX);
				}else
					i=1;
				break;
			case '-':
				if(*pcur=='.'){
					pcur++;
					A=_increase_matrix(-term(),A,RM_MATRIX);
				}else
					i=1;
				break;
			case 'f':
				if(*pcur=='.'){
					pcur++;
					A=_fill_matrix(A,term());
				}else
					i=1;
				break;
			case 'e':
				if(*pcur=='.'){
					pcur++;
					A=_eliminate_matrix((int)term(),A,UPPER_MATRIX|LOWER_MATRIX|RM_MATRIX);
				}else
				if(*pcur=='u'&&*(pcur+1)=='.'){
					pcur+=2;
					A=_eliminate_matrix((int)term(),A,UPPER_MATRIX|RM_MATRIX);
				}else
				if(*pcur=='l'&&*(pcur+1)=='.'){
					pcur+=2;
					A=_eliminate_matrix((int)term(),A,LOWER_MATRIX|RM_MATRIX);
				}else
					i=1;
				break;
			case 's':
				if(*pcur=='.'){
					pcur++;
					A=_scale_matrix((int)term(),A,RM_MATRIX);
				}else
					i=1;
				break;
			case 'p':
				if(*pcur=='.'){
					pcur++;
					A=_pivot_matrix((int)term(),A,RM_MATRIX);
				}else
					i=1;
				break;
			case 'c':
				if(*pcur=='.'){
					pcur++;
					A=_combine_matrix(A,mterm(),1,A->n+1,
							FORCE_MATRIX|RM_MATRIX);
				}else
					i=1;
				break;
			case 't':
				if(isdelimiter(*pcur))
					A=_transpose_matrix(A,RM_MATRIX);
				else
					i=1;
				break;
			case 'i':
				if(isdelimiter(*pcur))
					A=_identify_matrix(A,RM_MATRIX);
				else
					i=1;
				break;
			case 'b':
				if(isdelimiter(*pcur))
					A=_back_substitute(A,RM_MATRIX);
				else
					i=1;
				break;
			case 'g':
				if(isdelimiter(*pcur))
					A=_gaussjordan(A,RM_MATRIX);
				else
					i=1;
				break;
			default:
				i=1;
				break;
		}
		if(i){
			pcur--;
			return A;
		}
	}
}

_PMATRIX
mfactor()
{
	int		i;
	double		value;
	char		*x,*y;
	_PMATRIX	A;

	A=0;

	if((*pcur=='+'||*pcur=='-')&&
	   (x=strchr(pcur,'['))&&(*(x+1)==']'||isdelimiter(*(x-1)))){
	    	i=(*pcur=='+'? 1:-1);
		pcur++;
		A=(i==1? mfactor():_scalar_matrix(-1,mfactor(),RM_MATRIX));
	}else if(*pcur=='('&&
	   	 (x=strchr(pcur,'['))&&(*(x+1)==']'||isdelimiter(*(x-1)))){
		i=1;
		pcur++;
		for(y=pcur;*y;y++){
			switch(*y){
				case '(':
					i++;
					break;
				case ')':
					i--;
					break;
			}
			if(!i){
				*y=0;
				break;
			}
		}
		if(x<y){
			A=mexprs(pcur);
			*y=')';
			pcur=y+1;
		}else{
		    	value=exprs(pcur);
			*y=')';
			pcur=y+1;
			if(*pcur=='*')
			    	A=_scalar_matrix(value,mexprs(pcur+1),RM_MATRIX);
			else if(*pcur=='/')
			    	A=_scalar_matrix(value,_inverse_matrix(mexprs(pcur+1),RM_MATRIX),RM_MATRIX);
			else if(*pcur=='+'||*pcur=='-'){
				snprintf(tmpbuf,TMPBUFSIZE,"var +- matrix: Invalid operator");
			    	error();
			}
		}
	}else if(!(x=ismatrix(pcur))){
		value=term();
		gvalue=value;
		x=pcur;
		if((*x=='*'||*x=='/')&&(pcur++,A=mfactor())){
			if(*x=='*')
				A=_scalar_matrix(value,A,RM_MATRIX);
			else if(*x=='/')
				A=_scalar_matrix(value,_inverse_matrix(A,RM_MATRIX),RM_MATRIX);
		}else
		    	return 0;
	}else if(x&&*pcur!='['){
	    	y=pcur;
		if(*x=='('){
			*x=0;
			if(ismxfunc(pcur)){
				*x='(';
				A=mxfunc(pcur);
			}else{
				*x='(';
				A=mfunc(pcur);
			}
		}else{
			*(x-2)=0;
			A=matrix(pcur);
			*(x-2)='[';
			pcur=x;
			if(A)
				A=_copy_matrix(A);
		}
		if(!A){
			snprintf(tmpbuf,TMPBUFSIZE,"%s: No such matrix",y);
		    	error();
		}
	}else if(*pcur=='['){
		if(!(A=getmatrix(pcur))){
			snprintf(tmpbuf,TMPBUFSIZE,"%s: Empty matrix",pcur);
			error();
		}
	}

	if(!A){
		snprintf(tmpbuf,TMPBUFSIZE,"Matrix expression failed");
		error();
	}

	return A;
}

_PMATRIX
getmatrix(str)
	char		*str;
{
	int		i,j,k,p,empty;
	char		*x,*y;
	_PMATRIX	A;

	p=0;
	i=j=k=1;
	empty=1;
	for(x=str+1;*x;x++){
		switch(*x){
			case '[':
			case '(':
				p++;
				break;
			case ']':
			case ')':
				p--;
				break;
			case ',':
				if(!p)
				    	k++;
				empty=0;
				break;
    			case ';':
				if(!p&&i++){
					if(j<k)
					    	j=k;
					k=1;
				}
				empty=0;
				break;
			default:
				empty=0;
				break;
		}
		if(p<0)
			break;
	}
	if(empty)
		return 0;
	if(j<k)
	    	j=k;

	A=_new_matrix(i,j,0);
	p=0;
	i=j=1;
	for(x=y=str+1;*x;x++){
		switch(*x){
			case '[':
			case '(':
				p++;
				break;
			case ']':
				if(!(p--)){
					*x=0;
					_element(A,i,j)=exprs(y);
					*x=']';
				}
				break;
			case ')':
				p--;
				break;
			case ',':
			case ';':
				if(!p){
				    	k=(*x==','? 0:1);
				    	*x=0;
					_element(A,i,j)=exprs(y);
				    	*x=';';
					j++;
				    	if(k&&i++)
						j=1;
					y=x+1;
				}
				break;
		}
		if(p<0)
			break;
	}
	pcur++;

	return A;
}

char *
getstr(line)
	char	*line;
{
	int	i,l;
	char	ch;
	char	*x,*y,*z;

	if(!line||!*line)
		return 0;

	if(!(l=strlen(line)))
		return 0;

	i=0;
	for(x=line;*x;x++){
		if(*x=='\\'){
			if(!(i%2))
				l--;
			i++;
			continue;
		}
		if(*x=='$'&&(x==line||!(i%2&&*(x-1)=='\\'))){
			if(*(x+1)=='{'){
				for(y=x+2;*y&&*y!='}';y++);
				ch=*y;
				*y=0;
				l=l-(y-x+1)+strlen(str(x+2));
				*y=ch;
				x=y;
			}else{
				for(y=x+1;!isdelimiter(*y);y++);
				ch=*y;
				*y=0;
				l=l-(y-x)+strlen(str(x+1));
				*y=ch;
				x=y-1;
			}
			if(!ch)
				break;
		}
		i=0;
	}
	if(!l)
		return 0;

	z=(char *)malloc(l+1);
	if(!z){
		snprintf(tmpbuf,TMPBUFSIZE,"Memory exhausted");
		error();
	}

	i=0;
	*z=0;
	for(x=y=line;*x;x++){
		if(*x=='\\'){
			if(i%2&&*(x-1)=='\\'){
				if(x>y){
					ch=*x;
					*(x-1)=0;
					strcat(z,y);
					*x=ch;
				}
				strcat(z,"\\");
				y=x+1;
			}
			i++;
			continue;
		}
		if(*x=='$'&&(x==line||!(i%2&&*(x-1)=='\\'))){
			if(x>y){
				ch=*x;
				*x=0;
				strcat(z,y);
				*x=ch;
			}
			if(*(x+1)=='{'){
				for(y=x+2;*y&&*y!='}';y++);
				ch=*y;
				*y=0;
				strcat(z,str(x+2));
				*y=ch;
				x=y;
			}else{
				for(y=x+1;!isdelimiter(*y);y++);
				ch=*y;
				*y=0;
				strcat(z,str(x+1));
				*y=ch;
				x=y-1;
			}
			y=x+1;
			if(!ch)
				break;
		}else
		if(*x=='$'&&i%2&&*(x-1)=='\\'){
			if(x>y){
				ch=*x;
				*(x-1)=0;
				strcat(z,y);
				*x=ch;
			}
			strcat(z,"$");
			y=x+1;
		}else
		if(*x=='"'&&i%2&&*(x-1)=='\\'){
			if(x>y){
				ch=*x;
				*(x-1)=0;
				strcat(z,y);
				*x=ch;
			}
			strcat(z,"\"");
			y=x+1;
		}
		i=0;
	}
	strcat(z,y);
	return z;
}

char *
string(str)
    	char	*str;
{
    	int	i;
	char	*x;

	i=0;
	for(x=str;*x;x++)
	    	if(*x=='"'&&*(x-1)!='\\'&&!(++i&1))
			return x+1;
	return 0;
}

/* DEBUG: free */
/* TODO: Is this routine required?
 * 	 Need to be fully reimplemented.
 */
char *
substerm(pfstr,parname,parstr,mode)
	char	*pfstr,*parname,*parstr;
	int	mode;
{
	int	i,j;
	char	*pstrtmp,*x,*y,*z;

	x=pfstr;
	j=strlen(parstr);
	while((x=strstr(x,parname))){
		i=strlen(parname);
		y=x-1;
		z=x+i;
		if((mode&&!isvar(*y)&&!isvar(*z))||
		   !mode){
			pstrtmp=(char *)malloc(strlen(z)+1);
			if(!pstrtmp){
				snprintf(tmpbuf,TMPBUFSIZE,"Memory exhausted");
				error();
			}
			strcpy(pstrtmp,z);
			strcpy(x,parstr);
			strcpy(x+j,pstrtmp);
		}
		x=x+j;
	}

	return pfstr;
}

char *
isset(str)
    	char	*str;
{
	int	i,pe,p,be,b;
	char	*x;

	i=pe=p=be=b=0;
	for(x=str;*x;x++)
		switch(*x){
			case '[':
				b++;
				be=1;
				break;
			case ']':
				b--;
				break;
			case '(':
				p++;
				pe=1;
				break;
			case ')':
				p--;
				break;
			case '=':
				if((*(x+1)!='='&&!p&&!b&&i<2)&&
				   (!pe||*(x-1)==')'||(be&&*(x-1)==']')))
					return x;
				else
					return 0;
				break;
			case '+':
			case '-':
			case '*':
			case '/':
			case '%':
			case '^':
				if(b)
					break;
				i++;
				if(i>1)
					return 0;
				break;
			default:
				if(strchr("!#$&|`'?<>\\\"{};",*x))
					return 0;
				break;
		}
	return 0;
}

char *
isfunc(str)
    	char	*str;
{
	int	p;
	char	*x;

	p=0;
	for(x=str;*x;x++)
		switch(*x){
			case '(':
				p++;
				break;
			case ')':
				if(!p)
				    	return 0;
				if(!(--p))
				    	return x+1;
				break;
			case '[':
				if(!p&&*(x+1)!=']')
					return 0;
				break;
			default:
				if(!p&&strchr("+-*/!#$%^&|,;`'?<>\\\"",*x))
					return 0;
				break;
		}
	return 0;
}

char *
ismatrix(str)
	char	*str;
{
	char	*x,*y;
	int	p;

	if(*str=='['&&*(str+1)!=']'){
		p=1;
		for(x=str+1;*x;x++)
			switch(*x){
				case '(':
				case '[':
					p++;
					break;
				case ')':
				case ']':
					if(!(--p)&&*x==']')
						return x+1;
					break;
			}
		return 0;
	}

	if(!(y=strstr(str,"[]")))
		return 0;

	for(x=str;x<y;x++)
		if(isdelimiter(*x))
			return 0;
	if(x==str)
	    	return 0;
	return y+2;
}

int
isdelimiter(ch)
	char	ch;
{
	if(strchr("()+-*/!=#$%^&|,;`'?<>\\\"\a\b\t\n ",ch)||!ch)
		return 1;
	return 0;
}

void
freegroupvars(void)
{
	int	i;

	if(!groupvars)
		return;

	for(i=0;i<ngroupvars;i++)
		free(groupvars[i]);
	free(groupvars);
	groupvars=0;
	ngroupvars=0;
	return;
}

#ifdef	USE_GD
void
initgrfvars(void)
{
	grfwidth=GRFWIDTH;
	grfheight=GRFHEIGHT;

	sx=SX;
	ex=EX;
	sy=SY;
	ey=EY;
	sxr=SXR;
	exr=EXR;
	syr=SYR;
	eyr=EYR;
	dx=DX;
	dy=DY;
	xto=XTO;
	yto=YTO;
	xt=XT;
	yt=YT;
	xtl=XTL;
	ytl=YTL;
	vp.x0=VP_X0;
	vp.y0=VP_Y0;
	/*
	vp.sxr=0;
	vp.exr=160;
	vp.syr=0;
	vp.eyr=160;
	*/
	vp.rho=VP_RHO;
	vp.phi=VP_PHI;
	vp.theta=VP_THETA;
	vp.d=VP_D;
	grf3dw=GRF3DW;

	xl="x";
	yl="y";

	_sx=sx;
	_ex=ex;
	_sy=sy;
	_ey=ey;
	_sxr=sxr;
	_exr=exr;
	_syr=syr;
	_eyr=eyr;
	_dx=dx;
	_dy=dy;
	_xto=xto;
	_yto=yto;
	_xt=xt;
	_yt=yt;
	_xtl=xtl;
	_ytl=ytl;
	/*
	_xl=xl;
	_yl=yl;
	*/
	_vp.x0=vp.x0;
	_vp.y0=vp.y0;
	/*
	_vp.sxr=vp.sxr;
	_vp.exr=vp.exr;
	_vp.syr=vp.syr;
	_vp.eyr=vp.eyr;
	*/
	_vp.rho=vp.rho;
	_vp.phi=vp.phi;
	_vp.theta=vp.theta;
	_vp.d=vp.d;
	_grf3dw=grf3dw;

	return;
}

int
fitline(sx,sy,ex,ey,rsx,rsy,rex,rey)
	int sx,sy,ex,ey,*rsx,*rsy,*rex,*rey;
{
	int	dlx,dly,tsx,tsy,tex,tey,lsx,lsy,lex,ley;

	if(sx==ex){
		if((ex<0 || ex>grfwidth-1) ||
		   ((sy<0 && ey<0) ||
		    (sy>grfheight-1 && ey>grfheight-1)))
			return 1;
		lsx=sx;
		lsy=sy;
		lex=ex;
		ley=ey;
		if(sy<0)
			lsy=0;
		else
		if(sy>grfheight-1)
			lsy=grfheight-1;
		if(ey<0)
			ley=0;
		else
		if(ey>grfheight-1)
			ley=grfheight-1;
	}else
	if(sy==ey){
		if((ey<0 || ey>grfheight-1) ||
		   ((sx<0 && ex<0) ||
		    (sx>grfwidth-1 && ex>grfwidth-1)))
			return 1;
		lsx=sx;
		lsy=sy;
		lex=ex;
		ley=ey;
		if(sx<0)
			lsx=0;
		else
		if(sx>grfwidth-1)
			lsx=grfwidth-1;
		if(ex<0)
			lex=0;
		else
		if(ex>grfwidth-1)
			lex=grfwidth-1;
	}else{
		lsx=sx;
		lsy=sy;
		lex=ex;
		ley=ey;
		if((lsx<0 && lex<0) ||
		   (lsx>grfwidth-1 && lex>grfwidth-1) ||
		   (lsy<0 && ley<0) ||
		   (lsy>grfheight-1 && ley>grfwidth-1))
			return 1;
		if(lsx<0 || lsx>grfwidth-1  ||
		   lex<0 || lex>grfwidth-1  ||
		   lsy<0 || lsy>grfheight-1 ||
		   ley<0 || ley>grfheight-1){
			dlx=lex-lsx;
			dly=ley-lsy;
			tsx=lsx;
			tsy=lsy;
			tex=lex;
			tey=ley;
			if(lsx<0){
				tsx=0;
				tsy=(double)dly/dlx*(tsx-lsx)+lsy;
			}else
			if(lsx>grfwidth-1){
				tsx=grfwidth-1;
				tsy=(double)dly/dlx*(tsx-lsx)+lsy;
			}
			if(tsy<0){
				tsy=0;
				tsx=(double)dlx/dly*(tsy-lsy)+lsx;
			}else
			if(tsy>grfheight-1){
				tsy=grfheight-1;
				tsx=(double)dlx/dly*(tsy-lsy)+lsx;
			}
			if(lex<0){
				tex=0;
				tey=(double)dly/dlx*(tex-lsx)+lsy;
			}else
			if(lex>grfwidth-1){
				tex=grfwidth-1;
				tey=(double)dly/dlx*(tex-lsx)+lsy;
			}
			if(tey<0){
				tey=0;
				tex=(double)dlx/dly*(tey-lsy)+lsx;
			}else
			if(tey>grfheight-1){
				tey=grfheight-1;
				tex=(double)dlx/dly*(tey-lsy)+lsx;
			}
			if((tsy<0 && tey<0) ||
			   (tsy>grfheight-1 && tey>grfheight-1))
				return 1;
			lsx=tsx;
			lsy=tsy;
			lex=tex;
			ley=tey;
		}
	}
	if(rsx)
		*rsx=lsx;
	if(rsy)
		*rsy=lsy;
	if(rex)
		*rex=lex;
	if(rey)
		*rey=ley;
	return 0;
}

void
draw2daxes(void)
{
	double		dx,ct;
	int		i,j,p,n,tlw,tlh,cx0,cy0,px,py,cx,cy;

	tlh=0;

	cx0=Cx(0);
	cy0=Cy(0);

	if(cy0>=0 && cy0<grfheight)
		gdImageLine(im,0,cy0,grfwidth,cy0,_g_axes);

	if(_xt&&_xt<=_exr-_sxr){
		cy=cy0;
		if(cy0<0)
			cy=0;
		else
		if(cy0>grfheight-1)
			cy=grfheight-1;
		if(cy>grfheight-15)
			cy-=2;
		ct=_xt*(int)(_sxr/_xt);
		if((int)(_sxr/_xt)<(_sxr/_xt))
			ct+=_xt;
		ct-=_xt-(_xto-_xt*(int)(_xto/_xt));
		n=(int)(_exr/_xt)-(int)(_sxr/_xt)+2;
		for(i=0;i<n;i++){
			cx=Cx(ct);
			gdImageLine(im,cx,cy,cx,cy+2,_g_axes);
			ct+=_xt;
		}
	}
	if(_xtl && _xtl<=_exr-_sxr){
		tlh=10;//FMGetStringHeight("0123456789xy");
		cy=cy0;
		if(cy0<0)
			cy=0;
		else
		if(cy0>grfheight-1)
			cy=grfheight-1;
		py=cy+5;
		if(cy>grfheight-tlh-5){
			cy-=5;
			py=cy-tlh;
		}
		ct=_xtl*(int)(_sxr/_xtl);
		if((int)(_sxr/_xtl)<(_sxr/_xtl))
			ct+=_xtl;
		ct-=_xtl-(_xto-_xtl*(int)(_xto/_xtl));
		n=(int)(_exr/_xtl)-(int)(_sxr/_xtl)+2;
		for(i=0;i<n;i++){
			cx=Cx(ct);
			if(cx<0||cx>grfwidth){
				ct+=_xtl;
				continue;
			}
			gdImageLine(im,cx,cy,cx,cy+5,_g_axes);
			snprintf(tmpbuf,TMPBUFSIZE,"%.2g",(double)(int)(ct*100)/100);
			tlw=10;//FMGetStringWidth(tmpbuf);
			px=cx-tlw/2;
#ifdef	ZERO
			if(fabs(ct)<=ZERO)
#else
			if(!ct)
#endif
			{
				ct+=_xtl;
				continue;
			}
			if(cx-tlw/2<0)
				px=0;
			else
			if(cx+tlw/2>grfwidth-1)
				px=grfwidth-tlw;
			gdImageString(im,gdFontTiny,px,py,tmpbuf,_g_axes);
			ct+=_xtl;
		}
	}

	if(cx0>=0 && cx0<grfwidth)
		gdImageLine(im,cx0,0,cx0,grfheight,_g_axes);

	if(_yt && _yt<=_eyr-_syr){
		cx=cx0;
		if(cx0<0)
			cx=0;
		else
		if(cx0>grfwidth-1)
			cx=grfwidth-1;
		if(cx>=14)
			cx-=2;
		ct=_yt*(int)(_syr/_yt);
		if((int)(_syr/_yt)<(_syr/_yt))
			ct+=_yt;
		ct-=_yt-(_yto-_yt*(int)(_yto/_yt));
		n=(int)(_eyr/_yt)-(int)(_syr/_yt)+2;
		for(i=0;i<n;i++){
			cy=Cy(ct);
			gdImageLine(im,cx,cy,cx+2,cy,_g_axes);
			ct+=_yt;
		}
	}
	if(_ytl && _ytl<=_eyr-_syr){
		if(!tlh)
			tlh=10;//FMGetStringHeight("0123456789xy");
		cx=cx0;
		if(cx0<0)
			cx=0;
		else
		if(cx0>grfwidth-1)
			cx=grfwidth-1;
		ct=_ytl*(int)(_syr/_ytl);
		if((int)(_syr/_ytl)<(_syr/_ytl))
			ct+=_ytl;
		ct-=_ytl-(_yto-_ytl*(int)(_yto/_ytl));
		dx=ct;
		n=(int)(_eyr/_ytl)-(int)(_syr/_ytl)+2;

		tlw=0;
		for(i=0;i<n;i++){
			cy=Cy(dx);
			if(cy<0||cy>grfheight){
				dx+=_ytl;
				continue;
			}
			snprintf(tmpbuf,TMPBUFSIZE,"%.2g",(double)(int)(dx*100)/100);
			px=10;//FMGetStringWidth(tmpbuf);
			if(px>tlw)
				tlw=px;
			dx+=_ytl;
		}

		px=cx+5;
		j=0;
		if(cx>=tlw+5-2){
			cx-=5;
			j=1;
		}

		for(i=0;i<n;i++){
			cy=Cy(ct);
			if(cy<0||cy>grfheight){
				ct+=_ytl;
				continue;
			}
			gdImageLine(im,cx,cy,cx+5,cy,_g_axes);
			snprintf(tmpbuf,TMPBUFSIZE,"%.2g",(double)(int)(ct*100)/100);
			tlw=10;//FMGetStringWidth(tmpbuf);
			if(j)
				px=cx-tlw;
			py=cy-tlh/2;
			if(cy-tlh/2<0)
				py=0;
			else
			if(cy+tlh/2>grfheight-1)
				py=grfheight-tlh;
			gdImageString(im,gdFontTiny,px,py,tmpbuf,_g_axes);
			ct+=_ytl;
		}
	}

	return;
}

void
draw3daxis(xyz,label)
	int	xyz;
	char	*label;
{
	int	i,cx0,cy0,px,py,cx,cy,lsx,lsy,lex,ley,dlx,dly,tex,tey;
	struct	_point	pt;

	cx0=(int)_vp.x0;
	cy0=(int)_vp.y0;

//#define	CORRECT_AXES
#ifdef	CORRECT_AXES
	lex=0;
	ley=0;
	px=cx0;
	py=cy0;
	for(i=1;i<100;i+=30){
		if(xyz==1)
			Point(pt,i,0,0);
		else
		if(xyz==2)
			Point(pt,0,i,0);
		else
		if(xyz==3)
			Point(pt,0,0,i);
		_view(&_vp,&pt);
		cx=(int)pt.x;
		cy=(int)pt.y;
		if(cx==px && cy==py)
			break;
		if(i>1){
			lsx=cx-px;
			lsy=cy-py;
			if((lsx>=0 && lex<0) ||
			   (lsx<=0 && lex>0) ||
			   (lsy>=0 && ley<0) ||
			   (lsy<=0 && ley>0))
				break;
		}

		lex=cx-px;
		ley=cy-py;
		px=cx;
		py=cy;
	}
#else
#if 0
	if(xyz==1)
		Point(pt,10*_dx,0,0);
	else
	if(xyz==2)
		Point(pt,0,10*_dy,0);
	else
	if(xyz==3)
		Point(pt,0,0,10*_dx);
#else
	if(xyz==1)
		Point(pt,10,0,0);
	else
	if(xyz==2)
		Point(pt,0,10,0);
	else
	if(xyz==3)
		Point(pt,0,0,10);
#endif

	_view(&_vp,&pt);
	px=(int)pt.x;
	py=(int)pt.y;
#endif
	lex=px;
	ley=py;
	dlx=px-cx0;
	dly=py-cy0;
	if(dlx && dly &&
	   px>0 && px<grfwidth-1 &&
	   py>0 && py<grfheight-1){
		if(px>cx0){
			tex=grfwidth-1;
			tey=(double)dly/dlx*(tex-cx0)+cy0;
		}else{
			tex=0;
			tey=(double)dly/dlx*(tex-cx0)+cy0;
		}
		if(tey<0){
			tey=0;
			tex=(double)dlx/dly*(tey-cy0)+cx0;
		}else
		if(tey>grfheight-1){
			tey=grfheight-1;
			tex=(double)dlx/dly*(tey-cy0)+cx0;
		}
		px=tex;
		py=tey;
	}else
	if(!dlx){
		if(!dly){
			gdImageSetPixel(im,px,py,_g_axes);
		}else{
			if(py<cy0){
				py=0;
				dly=-dly;
				ley=py;
				dly=cy0-ley;
			}else{
				py=cy0;
				ley=grfheight-1;
				dly=ley-cy0;
			}
			gdImageLine(im,px,py,px,py+dly,_g_axes);
		}
	}else
	if(!dly){
		if(!dlx){
			gdImageSetPixel(im,px,py,_g_axes);
		}else{
			if(px<cx0){
				px=0;
				dlx=-dlx;
				lex=px;
				dlx=cx0-lex;
			}else{
				px=cx0;
				lex=grfwidth-1;
				dlx=lex-cx0;
			}
			gdImageLine(im,px,py,px+dlx,py,_g_axes);
		}
	}

	if(dlx && dly){
		/*
		fitline(cx0,cy0,px,py,&lsx,&lsy,&lex,&ley);
		gdImageLine(im,lsx,lsy,lex,ley,_g_axes);
		*/
		gdImageLine(im,cx0,cy0,px,py,_g_axes);
	}

	if(label){
/*
		if(lex==grfwidth-1)
			lex-=FMGetStringWidth(label);
		if(ley==grfheight-1)
			ley-=FMGetStringHeight(label);
*/
		gdImageString(im,gdFontTiny,lex,ley,label,_g_axes);
	}

	return;
}

int
draw2d(void)
{
	int		cx,cy,lsx,lsy,lex,ley,i,j,k,n,w,h,a,b;
	double		x,y,cr,ci,zr,zi,l,l2,v,v2,v3,ogx,ogy,dgx,dgy;
	_PMATRIX	A=0,B=0,C=0;
	char		*str,*str1,*str2;
	char		ch;
	char		rel;
	int		color[20];

	rel=0;
	if(!strncmp(grffunc,"_grid(",6)||!strncmp(grffunc,"_grid_(",7)){
		i=6;
		if(!strncmp(grffunc,"_grid_(",7))
			rel=1;
		i+=rel;

		dgx=_xtl;
		dgy=_ytl;
		ogx=_xto;
		ogy=_yto;
		if(*(grffunc+i)!=')'){
			if(*(grffunc+i)!=',')
				dgx=exprs(grffunc+i);
			else
				pcur=grffunc+i;
			if(*pcur==','&&*(pcur+1)!=','&&*(pcur+1)!=')')
				dgy=exprs(pcur+1);
			else
			if(*pcur==','&&*(pcur+1)==',')
				pcur++;
			if(*pcur==','&&*(pcur+1)!=','&&*(pcur+1)!=')')
				ogx=exprs(pcur+1);
			else
			if(*pcur==','&&*(pcur+1)==',')
				pcur++;
			if(*pcur==','&&*(pcur+1)!=','&&*(pcur+1)!=')')
				ogy=exprs(pcur+1);
			else
			if(*pcur==','&&*(pcur+1)==',')
				pcur++;
		}
		if(dgx<0)
			dgx=-dgx;
		if(dgy<0)
			dgy=-dgy;
		if(rel){
			ogx+=_x;
			ogy+=_y;
		}
		if(dgx){
			x=dgx*(int)(_sxr/dgx);
			if((int)(_sxr/dgx)<(_sxr/dgx))
				x+=dgx;
			x-=dgx-(ogx-dgx*(int)(ogx/dgx));
			n=(int)(_exr/dgx)-(int)(_sxr/dgx)+2;
			for(i=0;i<n;i++){
				cx=Cx(x);
				gdImageLine(im,cx,0,cx,grfheight,_g_axes);
				x+=dgx;
			}
		}
		if(dgy){
			y=dgy*(int)(_syr/dgy);
			if((int)(_syr/dgy)<(_syr/dgy))
				y+=dgy;
			y-=dgy-(ogy-dgy*(int)(ogy/dgy));
			n=(int)(_eyr/dgy)-(int)(_syr/dgy)+2;
			for(i=0;i<n;i++){
				cy=Cy(y);
				gdImageLine(im,0,cy,grfwidth,cy,_g_axes);
				y+=dgy;
			}
		}
	}else
	if(!strncmp(grffunc,"_dplot(",7)||!strncmp(grffunc,"_dplot_(",8)){
		i=7;
		if(!strncmp(grffunc,"_dplot_(",8))
			rel=1;
		i+=rel;
		A=mexprs(grffunc+i);
		if(!A)
			return 0;
		for(i=1;i<=A->m;i++){
			x=Rx(matx(A,i));
			y=Ry(maty(A,i));
			px=Clx(x);
			py=Cly(y);
			if(px>=0 && px<grfwidth && py>=0 && py<grfheight)
				gdImageSetPixel(im,px,py,_g_color);
		}
		_free_matrix(A);

		_x=x;
		_y=y;
	}else
	if(!strncmp(grffunc,"_vplot(",7)||!strncmp(grffunc,"_vplot_(",8)){
		i=7;
		if(!strncmp(grffunc,"_vplot_(",8))
			rel=1;
		i+=rel;
		A=mexprs(grffunc+i);
		if(!A)
			return 0;
		x=Rx(matx(A,1));
		y=Ry(maty(A,1));
		px=Clx(x);
		py=Cly(y);
		if(px>=0 && px<grfwidth && py>=0 && py<grfheight)
			gdImageSetPixel(im,px,py,_g_color);
		for(i=2;i<=A->m;i++){
			x=Rx(matx(A,i));
			y=Ry(maty(A,i));
			cx=Clx(x);
			cy=Cly(y);
			/*
			if(!fitline(px,py,cx,cy,&lsx,&lsy,&lex,&ley))
				gdImageLine(im,lsx,lsy,lex,ley,_g_color);
			*/
			gdImageLine(im,px,py,cx,cy,_g_color);
			px=cx;
			py=cy;
		}
		_free_matrix(A);

		_x=x;
		_y=y;
	}else
	if(!strncmp(grffunc,"_dmesh(",7)||!strncmp(grffunc,"_dmesh_(",8)){
		i=7;
		if(!strncmp(grffunc,"_dmesh_(",8))
			rel=1;
		i+=rel;

		v=1;

		if(*(grffunc+i)!=',')
			A=mexprs(grffunc+i);
		else
			pcur=grffunc+i;
		if(*pcur==','&&*(pcur+1)!=','&&*(pcur+1)!=')')
			B=mexprs(pcur+1);
		else
		if(*pcur==','&&*(pcur+1)==',')
			pcur++;
		if(*pcur==','&&*(pcur+1)!=','&&*(pcur+1)!=')')
			C=mexprs(pcur+1);
		else
		if(*pcur==','&&*(pcur+1)==',')
			pcur++;
		if(*pcur==','&&*(pcur+1)!=','&&*(pcur+1)!=')')
			v=exprs(pcur+1);
		else
		if(*pcur==','&&*(pcur+1)==',')
			pcur++;

		if(!C||C->m<1||C->n<1||
				(A&&(A->m<1||A->n<1||C->n!=A->n))||
				(B&&(B->m<1||B->n<1||C->m!=B->m))){
			if(A)
				_free_matrix(A);
			if(B)
				_free_matrix(B);
			if(C)
				_free_matrix(C);
			return 0;
		}

		if(v>0.0){
			for(i=1;i<=C->m;i++){
				for(j=1;j<=C->n;j++){
					if(i==1&&j==1){
						l=_element(C,i,j);
						l2=_element(C,i,j);
					}else
					if(l>_element(C,i,j))
						l=_element(C,i,j);
					else
					if(l2<_element(C,i,j))
						l2=_element(C,i,j);
				}
			}
			v2=v/(l2-l);
		}else{
			v2=v;
		}

		for(j=1;j<=C->n;j++){
			for(i=1;i<=C->m;i++){
				x=Rx((A?_element(A,1,j):j));
				y=Ry((B?_element(B,i,1):i));
				px=Clx(x);
				py=Cly(y);
				if(px>=0 && px<grfwidth && py>=0 && py<grfheight){
					gdImageSetPixel(im,px,py,_g_color);
				}
			}
		}

		if(A)
			_free_matrix(A);
		if(B)
			_free_matrix(B);
		_free_matrix(C);

		_x=x;
		_y=y;
	}else
	if(!strncmp(grffunc,"_vmesh(",7)||!strncmp(grffunc,"_vmesh_(",8)){
		i=7;
		if(!strncmp(grffunc,"_vmesh_(",8))
			rel=1;
		i+=rel;

		k=0x0;

		if(*(grffunc+i)!=',')
			A=mexprs(grffunc+i);
		else
			pcur=grffunc+i;
		if(*pcur==','&&*(pcur+1)!=','&&*(pcur+1)!=')')
			B=mexprs(pcur+1);
		else
		if(*pcur==','&&*(pcur+1)==',')
			pcur++;
		if(*pcur==','&&*(pcur+1)!=','&&*(pcur+1)!=')')
			C=mexprs(pcur+1);
		else
		if(*pcur==','&&*(pcur+1)==',')
			pcur++;
		if(*pcur==','){
			for(pcur++;*pcur&&*pcur!=','&&*pcur!=')';pcur++){
				if(*pcur=='x')
					k|=0x1;
				else
				if(*pcur=='y')
					k|=0x2;
			}
		}

		if(!C||C->m<1||C->n<1||
				(A&&(A->m<1||A->n<1||C->n!=A->n))||
				(B&&(B->m<1||B->n<1||C->m!=B->m))){
			if(A)
				_free_matrix(A);
			if(B)
				_free_matrix(B);
			if(C)
				_free_matrix(C);
			return 0;
		}

		if(!k)
			k=0x3;

		if(k&0x1){
			for(j=1;j<=C->n;j++){
				for(i=1;i<=C->m;i++){
					x=Rx((A?_element(A,1,j):j));
					y=Ry((B?_element(B,i,1):i));
					cx=Clx(x);
					cy=Cly(y);
					if(i==1){
						px=cx;
						py=cy;
						if(px>=0 && px<grfwidth && py>=0 && py<grfheight)
							gdImageSetPixel(im,px,py,_g_color);
						continue;
					}
					/*
					if(!fitline(px,py,cx,cy,&lsx,&lsy,&lex,&ley))
						gdImageLine(im,lsx,lsy,lex,ley,_g_color);
					*/
					gdImageLine(im,px,py,cx,cy,_g_color);
					px=cx;
					py=cy;
				}
			}
		}

		if(k&0x2){
			for(i=1;i<=C->m;i++){
				for(j=1;j<=C->n;j++){
					x=Rx((A?_element(A,1,j):j));
					y=Ry((B?_element(B,i,1):i));
					cx=Clx(x);
					cy=Cly(y);
					if(j==1){
						px=cx;
						py=cy;
						if(px>=0 && px<grfwidth && py>=0 && py<grfheight)
							gdImageSetPixel(im,px,py,_g_color);
						continue;
					}
					/*
					if(!fitline(px,py,cx,cy,&lsx,&lsy,&lex,&ley))
						gdImageLine(im,lsx,lsy,lex,ley,_g_color);
					*/
					gdImageLine(im,px,py,cx,cy,_g_color);
					px=cx;
					py=cy;
				}
			}
		}

		if(A)
			_free_matrix(A);
		if(B)
			_free_matrix(B);
		_free_matrix(C);

		_x=x;
		_y=y;
	}else
	if(!strncmp(grffunc,"_str(",5)||!strncmp(grffunc,"_str_(",6)){
		i=5;
		if(!strncmp(grffunc,"_str_(",6))
			rel=1;
		i+=rel;

		pcur=grffunc+i;
		if(*pcur!=')'){
			if(*pcur!='"'&&*pcur!='$'){
				_x=Rx(exprs(pcur));
				if(*pcur==',')
					_y=Ry(exprs(pcur+1));
				if(*pcur==',')
					pcur++;
				px=Clx(_x);
				py=Cly(_y);
			}

			if(*pcur=='"'){
		    		for(str1=pcur+1;*str1;str1++)
				    	if(*str1=='"'&&*(str1-1)!='\\')
					    	break;
				str2=pcur+1;
			}else{
				for(str1=pcur+1;!isdelimiter(*str1);str1++);
				str2=pcur;
			}
			ch=*str1;
			*str1=0;

			if((str=getstr(str2))){
				//FMSetFont(_g_strf);
				cx=px;
				cy=py;
				w=10;//FMGetStringWidth(str);
				h=10;//FMGetStringHeight(str);
				if(_g_strh!=-1){
					if(_g_strh==0)
						cx-=w/2;
					else
					if(_g_strh==1)
						cx-=w;
				}
				if(_g_strv!=1){
					if(_g_strv==-1)
						cy-=h;
					else
					if(_g_strv==0)
						cy-=h/2;
				}
				if(cx>-2*w && cx<grfwidth && cy>=0 && cy<grfheight-h){
					gdImageString(im,gdFontSmall,cx,cy,str,_g_color);
					/*
					if(_g_stru&&!fitline(cx,cy+h,cx+w,cy+h,&lsx,&lsy,&lex,&ley))
						gdImageLine(im,lsx,lsy,lex,ley,_g_color);
					*/
					gdImageLine(im,cx,cy+h,cx+w,cy+h,_g_color);
				}
				free(str);
			}
			*str1=ch;
		}
	}else
	if(!strncmp(grffunc,"_mv2(",5)||!strncmp(grffunc,"_mv2_(",6)){
		i=5;
		if(!strncmp(grffunc,"_mv2_(",6))
			rel=1;
		i+=rel;
		_x=Rx(exprs(grffunc+i));
		if(*pcur==',')
			_y=Ry(exprs(pcur+1));
		px=Clx(_x);
		py=Cly(_y);
	}else
	if(!strncmp(grffunc,"_dot(",5)||!strncmp(grffunc,"_dot_(",6)){
		i=5;
		if(!strncmp(grffunc,"_dot_(",6))
			rel=1;
		i+=rel;
		if(*(grffunc+i)!=')'){
			_x=Rx(exprs(grffunc+i));
			if(*pcur==',')
				_y=Ry(exprs(pcur+1));
			px=Clx(_x);
			py=Cly(_y);
		}
		if(px>=0 && px<grfwidth && py>=0 && py<grfheight)
			gdImageSetPixel(im,px,py,_g_color);
	}else
	if(!strncmp(grffunc,"_line(",6)||!strncmp(grffunc,"_line_(",7)){
		i=6;
		if(!strncmp(grffunc,"_line_(",7))
			rel=1;
		i+=rel;
		x=Rx(exprs(grffunc+i));
		if(*pcur==',')
			y=Ry(exprs(pcur+1));
		cx=Clx(x);
		cy=Cly(y);
		if(*pcur!=')'){
			px=cx;
			py=cy;
			if(*pcur==',')
				x=Rx(exprs(pcur+1));
			if(*pcur==',')
				y=Ry(exprs(pcur+1));
			cx=Clx(x);
			cy=Cly(y);
		}
		/*
		if(!fitline(px,py,cx,cy,&lsx,&lsy,&lex,&ley))
			gdImageLine(im,lsx,lsy,lex,ley,_g_color);
		*/
		gdImageLine(im,px,py,cx,cy,_g_color);
		px=cx;
		py=cy;
		_x=x;
		_y=y;
	}else
	if(!strncmp(grffunc,"_rect(",6)||!strncmp(grffunc,"_rect_(",7)){
		i=6;
		if(!strncmp(grffunc,"_rect_(",7))
			rel=1;
		i+=rel;
		x=Rx(exprs(grffunc+i));
		if(*pcur==',')
			y=Ry(exprs(pcur+1));
		cx=Clx(x);
		cy=Cly(y);
		i=-1;
		if(*pcur!=')'){
			px=cx;
			py=cy;
			if(*pcur==',')
				x=Rx(exprs(pcur+1));
			if(*pcur==',')
				y=Ry(exprs(pcur+1));
			if(*pcur==',')
				i=(int)exprs(pcur+1);
			cx=Clx(x);
			cy=Cly(y);
		}
		/*
		if(!fitline(px,py,cx,py,&lsx,&lsy,&lex,&ley))
			gdImageLine(im,lsx,lsy,lex,ley,_g_color);
		if(!fitline(cx,py,cx,cy,&lsx,&lsy,&lex,&ley))
			gdImageLine(im,lsx,lsy,lex,ley,_g_color);
		if(!fitline(cx,cy,px,cy,&lsx,&lsy,&lex,&ley))
			gdImageLine(im,lsx,lsy,lex,ley,_g_color);
		if(!fitline(px,cy,px,py,&lsx,&lsy,&lex,&ley))
			gdImageLine(im,lsx,lsy,lex,ley,_g_color);
		*/
		/*
		gdImageLine(im,px,py,cx,py,_g_color);
		gdImageLine(im,cx,py,cx,cy,_g_color);
		gdImageLine(im,cx,cy,px,cy,_g_color);
		gdImageLine(im,px,cy,px,py,_g_color);
		*/
		if(i>=0)
			gdImageFilledRectangle(im,px,py,cx,cy,
					gdImageColorResolve(im,
						(i&0xff0000)>>16,
						(i&0x00ff00)>>8,
						(i&0x0000ff)));
		else
			gdImageRectangle(im,px,py,cx,cy,_g_color);
		px=cx;
		py=cy;
		_x=x;
		_y=y;
	}else
	if(!strncmp(grffunc,"_circ(",6)||!strncmp(grffunc,"_circ_(",7)){
		i=6;
		if(!strncmp(grffunc,"_circ_(",7))
			rel=1;
		i+=rel;
		l=exprs(grffunc+i);
		i=0;
		k=0;
		j=360;
		if(*pcur!=')'){
			_x=Rx(l);
			if(*pcur==',')
				_y=Ry(exprs(pcur+1));
			if(*pcur==',')
				l=exprs(pcur+1);
			if(*pcur==',')
				k=(int)(exprs(pcur+1)/M_PI*180)%360;
			if(*pcur==',')
				j=(int)(exprs(pcur+1)/M_PI*180)%360;
			i=360-j;
			if(i<0)
				i+=360;
			j=360-k;
			if(j<=0)
				j+=360;
			px=Clx(_x);
			py=Cly(_y);
		}
		w=Clx(_x+l)-px;
		h=py-Cly(_y+l);
		gdImageArc(im,px,py,w,h,i,j,_g_color);
	}else
	if(!strncmp(grffunc,"_elps(",6)||!strncmp(grffunc,"_elps_(",7)){
		i=6;
		if(!strncmp(grffunc,"_elps_(",7))
			rel=1;
		i+=rel;
		l=exprs(grffunc+i);
		if(*pcur==',')
			l2=exprs(pcur+1);
		i=0;
		k=0;
		j=360;
		if(*pcur!=')'){
			_x=Rx(l);
			_y=Ry(l2);
			if(*pcur==',')
				l=exprs(pcur+1);
			if(*pcur==',')
				l2=exprs(pcur+1);
			if(*pcur==',')
				k=(int)(exprs(pcur+1)/M_PI*180)%360;
			if(*pcur==',')
				j=(int)(exprs(pcur+1)/M_PI*180)%360;
			i=360-j;
			if(i<0)
				i+=360;
			j=360-k;
			if(j<=0)
				j+=360;
			px=Clx(_x);
			py=Cly(_y);
		}
		w=Clx(_x+l)-px;
		h=py-Cly(_y+l2);
		gdImageArc(im,px,py,w,h,i,j,_g_color);
	}else
	if(!strncmp(grffunc,"_julia(",7)||!strncmp(grffunc,"_julia_(",8)){
		i=7;
		if(!strncmp(grffunc,"_julia_(",8))
			rel=1;
		i+=rel;

		cr=0.3;
		ci=-0.4;
		n=20;
		v=10.0;
		v2=10.0;
		v3=10.0;
		if(*(grffunc+i)!=')'){
			if(*(grffunc+i)!=',')
				cr=exprs(grffunc+i);
			else
				pcur=grffunc+i;
			if(*pcur==','&&*(pcur+1)!=','&&*(pcur+1)!=')')
				ci=exprs(pcur+1);
			else
			if(*pcur==','&&*(pcur+1)==',')
				pcur++;
			if(*pcur==','&&*(pcur+1)!=','&&*(pcur+1)!=')')
				n=(int)exprs(pcur+1);
			else
			if(*pcur==','&&*(pcur+1)==',')
				pcur++;
			if(*pcur==','&&*(pcur+1)!=','&&*(pcur+1)!=')')
				v=exprs(pcur+1);
			else
			if(*pcur==','&&*(pcur+1)==',')
				pcur++;
			if(*pcur==','&&*(pcur+1)!=','&&*(pcur+1)!=')')
				v2=exprs(pcur+1);
			else
			if(*pcur==','&&*(pcur+1)==',')
				pcur++;
			if(*pcur==','&&*(pcur+1)!=','&&*(pcur+1)!=')')
				v3=exprs(pcur+1);
			else
			if(*pcur==','&&*(pcur+1)==',')
				pcur++;
		}
		b=BLACK;
		for(cx=0;cx<grfwidth;cx++){
			for(cy=0;cy<grfheight;cy++){
				zr=Rx2(Mx(cx));
				zi=Ry2(My(cy));
				j=b;
				for(i=0;i<n;i++){
					x=zr*zr;
					y=zi*zi;
					if(x+y>4){
						j=gdImageColorResolve(im,
							(int)(v *i)%256,
							(int)(v2*i)%256,
							(int)(v3*i)%256);
						break;
					}
					zi=2*zr*zi+ci;
					zr=x-y+cr;
				}
				gdImageSetPixel(im,cx,cy,j);
			}
		}
	}else
	if(!strncmp(grffunc,"_mandelbrot(",12)||!strncmp(grffunc,"_mandelbrot_(",13)){
		i=12;
		if(!strncmp(grffunc,"_mandelbrot_(",13))
			rel=1;
		i+=rel;

		n=20;
		v=10.0;
		v2=10.0;
		v3=10.0;
		if(*(grffunc+i)!=')'){
			if(*(grffunc+i)!=',')
				n=(int)exprs(grffunc+i);
			else
				pcur=grffunc+i;
			if(*pcur==','&&*(pcur+1)!=','&&*(pcur+1)!=')')
				v=exprs(pcur+1);
			else
			if(*pcur==','&&*(pcur+1)==',')
				pcur++;
			if(*pcur==','&&*(pcur+1)!=','&&*(pcur+1)!=')')
				v2=exprs(pcur+1);
			else
			if(*pcur==','&&*(pcur+1)==',')
				pcur++;
			if(*pcur==','&&*(pcur+1)!=','&&*(pcur+1)!=')')
				v3=exprs(pcur+1);
			else
			if(*pcur==','&&*(pcur+1)==',')
				pcur++;
		}
		b=BLACK;
		for(cx=0;cx<grfwidth;cx++){
			cr=Rx2(Mx(cx));
			for(cy=0;cy<grfheight;cy++){
				ci=Ry2(My(cy));
				zr=cr;
				zi=ci;
				j=b;
				for(i=0;i<n;i++){
					x=zr*zr;
					y=zi*zi;
					if(x+y>4){
						j=gdImageColorResolve(im,
							(int)(v *i)%256,
							(int)(v2*i)%256,
							(int)(v3*i)%256);
						break;
					}
					zi=2*zr*zi+ci;
					zr=x-y+cr;
				}
				gdImageSetPixel(im,cx,cy,j);
			}
		}
	}else
	if(!strncmp(grffunc,"_inv(",5)||!strncmp(grffunc,"_inv_(",6)){
		i=5;
		if(!strncmp(grffunc,"_inv_(",6))
			rel=1;
		i+=rel;
		x=Rx(exprs(grffunc+i));
		if(*pcur==',')
			y=Ry(exprs(pcur+1));
		cx=Clx(x);
		cy=Cly(y);
		if(*pcur!=')'){
			px=cx;
			py=cy;
			if(*pcur==',')
				x=Rx(exprs(pcur+1));
			if(*pcur==',')
				y=Ry(exprs(pcur+1));
			cx=Clx(x);
			cy=Cly(y);
		}
		if(px<0)
			px=0;
		if(px>grfwidth-1)
			px=grfwidth-1;
		if(py<0)
			py=0;
		if(py>grfheight-1)
			py=grfheight-1;
		if(cx<0)
			cx=0;
		if(cx>grfwidth-1)
			cx=grfwidth-1;
		if(cy<0)
			cy=0;
		if(cy>grfheight-1)
			cy=grfheight-1;
		if(px>cx){
			lsx=px;
			px=cx;
			cx=lsx;
		}
		if(py>cy){
			lsy=py;
			py=cy;
			cy=lsy;
		}
		px=cx;
		py=cy;
		_x=x;
		_y=y;
	}else
	if(!strncmp(grffunc,"_getpos(",8)){
		setvar("_x",_x);
		setvar("_y",_y);
	}else
		return 1;

	return 0;
}

int
draw3d(void)
{
	int		cx,cy,lsx,lsy,lex,ley,i,j,k,w,h;
	double		x,y,z,xs[8],ys[8],zs[8],l,l2,v,v2;
	struct		_point	pt;
	_PMATRIX	A=0,B=0,C=0;
	char		*str,*str1,*str2;
	char		ch;
	char		rel;

	rel=0;
	if(!strncmp(grffunc,"_dplot(",7)||!strncmp(grffunc,"_dplot_(",8)){
		i=7;
		if(!strncmp(grffunc,"_dplot_(",8))
			rel=1;
		i+=rel;
		A=mexprs(grffunc+i);
		if(!A)
			return 0;
		for(i=1;i<=A->m;i++){
			x=Rx(mat3dx(A,i));
			y=Ry(mat3dy(A,i));
			z=Rz(mat3dz(A,i));
			Point(pt,x,y,z);
			_view(&_vp,&pt);
			px=(int)pt.x;
			py=(int)pt.y;
			if(px>=0 && px<grfwidth && py>=0 && py<grfheight)
				gdImageSetPixel(im,px,py,_g_color);
		}
		_free_matrix(A);

		_x=x;
		_y=y;
		_z=z;
	}else
	if(!strncmp(grffunc,"_vplot(",7)||!strncmp(grffunc,"_vplot_(",8)){
		i=7;
		if(!strncmp(grffunc,"_vplot_(",8))
			rel=1;
		i+=rel;
		A=mexprs(grffunc+i);
		if(!A)
			return 0;
		x=Rx(mat3dx(A,1));
		y=Ry(mat3dy(A,1));
		z=Rz(mat3dz(A,1));
		Point(pt,x,y,z);
		_view(&_vp,&pt);
		px=(int)pt.x;
		py=(int)pt.y;
		if(px>=0 && px<grfwidth && py>=0 && py<grfheight)
			gdImageSetPixel(im,px,py,_g_color);
		for(i=2;i<=A->m;i++){
			x=Rx(mat3dx(A,i));
			y=Ry(mat3dy(A,i));
			z=Rz(mat3dz(A,i));
			Point(pt,x,y,z);
			_view(&_vp,&pt);
			cx=(int)pt.x;
			cy=(int)pt.y;
			/*
			if(!fitline(px,py,cx,cy,&lsx,&lsy,&lex,&ley))
				gdImageLine(im,lsx,lsy,lex,ley,_g_color);
			*/
			gdImageLine(im,px,py,cx,cy,_g_color);
			px=cx;
			py=cy;
		}
		_free_matrix(A);

		_x=x;
		_y=y;
		_z=z;
	}else
	if(!strncmp(grffunc,"_dmesh(",7)||!strncmp(grffunc,"_dmesh_(",8)){
		i=7;
		if(!strncmp(grffunc,"_dmesh_(",8))
			rel=1;
		i+=rel;

		v=1;

		if(*(grffunc+i)!=',')
			A=mexprs(grffunc+i);
		else
			pcur=grffunc+i;
		if(*pcur==','&&*(pcur+1)!=','&&*(pcur+1)!=')')
			B=mexprs(pcur+1);
		else
		if(*pcur==','&&*(pcur+1)==',')
			pcur++;
		if(*pcur==','&&*(pcur+1)!=','&&*(pcur+1)!=')')
			C=mexprs(pcur+1);
		else
		if(*pcur==','&&*(pcur+1)==',')
			pcur++;
		if(*pcur==','&&*(pcur+1)!=','&&*(pcur+1)!=')')
			v=exprs(pcur+1);
		else
		if(*pcur==','&&*(pcur+1)==',')
			pcur++;

		if(!C||C->m<1||C->n<1||
				(A&&(A->m<1||A->n<1||C->n!=A->n))||
				(B&&(B->m<1||B->n<1||C->m!=B->m))){
			if(A)
				_free_matrix(A);
			if(B)
				_free_matrix(B);
			if(C)
				_free_matrix(C);
			return 0;
		}

		if(v>0.0){
			for(i=1;i<=C->m;i++){
				for(j=1;j<=C->n;j++){
					if(i==1&&j==1){
						l=_element(C,i,j);
						l2=_element(C,i,j);
					}else
					if(l>_element(C,i,j))
						l=_element(C,i,j);
					else
					if(l2<_element(C,i,j))
						l2=_element(C,i,j);
				}
			}
			v2=v/(l2-l);
		}else
			v2=(int)((int)v?v:-1);

		for(j=1;j<=C->n;j++){
			for(i=1;i<=C->m;i++){
				x=Rx((A?_element(A,1,j):j));
				y=Ry((B?_element(B,i,1):i));
				z=Rz(_element(C,i,j));

				Point(pt,x,y,0);
				_view(&_vp,&pt);
				cx=(int)pt.x;
				cy=(int)pt.y;

				Point(pt,x,y,z);
				_view(&_vp,&pt);
				px=(int)pt.x;
				py=(int)pt.y;

				/*
				if(!fitline(px,py,cx,cy,&lsx,&lsy,&lex,&ley)){
					gdImageLine(im,lsx,lsy,lex,ley,_g_color);
				}
				*/
				gdImageLine(im,px,py,cx,cy,_g_color);

				if(px>=0 && px<grfwidth && py>=0 && py<grfheight){
					gdImageSetPixel(im,px,py,_g_color);
				}
			}
		}

		if(A)
			_free_matrix(A);
		if(B)
			_free_matrix(B);
		_free_matrix(C);

		_x=x;
		_y=y;
		_z=z;
	}else
	if(!strncmp(grffunc,"_vmesh(",7)||!strncmp(grffunc,"_vmesh_(",8)){
		i=7;
		if(!strncmp(grffunc,"_vmesh_(",8))
			rel=1;
		i+=rel;

		k=0x0;

		if(*(grffunc+i)!=',')
			A=mexprs(grffunc+i);
		else
			pcur=grffunc+i;
		if(*pcur==','&&*(pcur+1)!=','&&*(pcur+1)!=')')
			B=mexprs(pcur+1);
		else
		if(*pcur==','&&*(pcur+1)==',')
			pcur++;
		if(*pcur==','&&*(pcur+1)!=','&&*(pcur+1)!=')')
			C=mexprs(pcur+1);
		else
		if(*pcur==','&&*(pcur+1)==',')
			pcur++;
		if(*pcur==','){
			for(pcur++;*pcur&&*pcur!=','&&*pcur!=')';pcur++){
				if(*pcur=='x')
					k|=0x1;
				else
				if(*pcur=='y')
					k|=0x2;
			}
		}

		if(!C||C->m<1||C->n<1||
				(A&&(A->m<1||A->n<1||C->n!=A->n))||
				(B&&(B->m<1||B->n<1||C->m!=B->m))){
			if(A)
				_free_matrix(A);
			if(B)
				_free_matrix(B);
			if(C)
				_free_matrix(C);
			return 0;
		}

		if(!k)
			k=0x3;

		if(k&0x1){
			for(j=1;j<=C->n;j++){
				for(i=1;i<=C->m;i++){
					x=Rx((A?_element(A,1,j):j));
					y=Ry((B?_element(B,i,1):i));
					z=Rz(_element(C,i,j));

					Point(pt,x,y,z);
					_view(&_vp,&pt);
					cx=(int)pt.x;
					cy=(int)pt.y;
					if(i==1){
						px=cx;
						py=cy;
						if(px>=0 && px<grfwidth && py>=0 && py<grfheight)
							gdImageSetPixel(im,px,py,_g_color);
						continue;
					}
					/*
					if(!fitline(px,py,cx,cy,&lsx,&lsy,&lex,&ley))
						gdImageLine(im,lsx,lsy,lex,ley,_g_color);
					*/
					gdImageLine(im,px,py,cx,cy,_g_color);
					px=cx;
					py=cy;
				}
			}
		}

		if(k&0x2){
			for(i=1;i<=C->m;i++){
				for(j=1;j<=C->n;j++){
					x=Rx((A?_element(A,1,j):j));
					y=Ry((B?_element(B,i,1):i));
					z=Rz(_element(C,i,j));

					Point(pt,x,y,z);
					_view(&_vp,&pt);
					cx=(int)pt.x;
					cy=(int)pt.y;
					if(j==1){
						px=cx;
						py=cy;
						if(px>=0 && px<grfwidth && py>=0 && py<grfheight)
							gdImageSetPixel(im,px,py,_g_color);
						continue;
					}
					/*
					if(!fitline(px,py,cx,cy,&lsx,&lsy,&lex,&ley))
						gdImageLine(im,lsx,lsy,lex,ley,_g_color);
					*/
					gdImageLine(im,px,py,cx,cy,_g_color);
					px=cx;
					py=cy;
				}
			}
		}

		if(A)
			_free_matrix(A);
		if(B)
			_free_matrix(B);
		_free_matrix(C);

		_x=x;
		_y=y;
		_z=z;
	}else
	if(!strncmp(grffunc,"_str(",5)||!strncmp(grffunc,"_str_(",6)){
		i=5;
		if(!strncmp(grffunc,"_str_(",6))
			rel=1;
		i+=rel;

		pcur=grffunc+i;
		if(*pcur!=')'){
			if(*pcur!='"'&&*pcur!='$'){
				_x=Rx(exprs(pcur));
				if(*pcur==',')
					_y=Ry(exprs(pcur+1));
				if(*pcur==',')
					_z=Rz(exprs(pcur+1));
				if(*pcur==',')
					pcur++;
				Point(pt,_x,_y,_z);
				_view(&_vp,&pt);
				px=(int)pt.x;
				py=(int)pt.y;
			}

			if(*pcur=='"'){
		    		for(str1=pcur+1;*str1;str1++)
				    	if(*str1=='"'&&*(str1-1)!='\\')
					    	break;
				str2=pcur+1;
			}else{
				for(str1=pcur+1;!isdelimiter(*str1);str1++);
				str2=pcur;
			}
			ch=*str1;
			*str1=0;

			if((str=getstr(str2))){
				cx=px;
				cy=py;
				w=10;//FMGetStringWidth(str);
				h=10;//FMGetStringHeight(str);
				if(_g_strh!=-1){
					if(_g_strh==0)
						cx-=w/2;
					else
					if(_g_strh==1)
						cx-=w;
				}
				if(_g_strv!=1){
					if(_g_strv==-1)
						cy-=h;
					else
					if(_g_strv==0)
						cy-=h/2;
				}
				if(cx>-2*w && cx<grfwidth && cy>=0 && cy<grfheight-h){
					gdImageString(im,gdFontSmall,cx,cy,str,_g_color);
					/*
					if(_g_stru&&!fitline(cx,cy+h,cx+w,cy+h,&lsx,&lsy,&lex,&ley))
						gdImageLine(im,lsx,lsy,lex,ley,_g_color);
					*/
					gdImageLine(im,cx,cy+h,cx+w,cy+h,_g_color);
				}
				free(str);
			}
			*str1=ch;
		}
	}else
	if(!strncmp(grffunc,"_mv2(",5)||!strncmp(grffunc,"_mv2_(",6)){
		i=5;
		if(!strncmp(grffunc,"_mv2_(",6))
			rel=1;
		i+=rel;
		_x=Rx(exprs(grffunc+i));
		if(*pcur==',')
			_y=Ry(exprs(pcur+1));
		if(*pcur==',')
			_z=Rz(exprs(pcur+1));
		Point(pt,_x,_y,_z);
		_view(&_vp,&pt);
		px=(int)pt.x;
		py=(int)pt.y;
	}else
	if(!strncmp(grffunc,"_dot(",5)||!strncmp(grffunc,"_dot_(",6)){
		i=5;
		if(!strncmp(grffunc,"_dot_(",6))
			rel=1;
		i+=rel;
		if(*(grffunc+i)!=')'){
			_x=Rx(exprs(grffunc+i));
			if(*pcur==',')
				_y=Ry(exprs(pcur+1));
			if(*pcur==',')
				_z=Rz(exprs(pcur+1));
			Point(pt,_x,_y,_z);
			_view(&_vp,&pt);
			px=(int)pt.x;
			py=(int)pt.y;
		}
		if(px>=0 && px<grfwidth && py>=0 && py<grfheight)
			gdImageSetPixel(im,px,py,_g_color);
	}else
	if(!strncmp(grffunc,"_line(",6)||!strncmp(grffunc,"_line_(",7)){
		i=6;
		if(!strncmp(grffunc,"_line_(",7))
			rel=1;
		i+=rel;
		x=Rx(exprs(grffunc+i));
		if(*pcur==',')
			y=Ry(exprs(pcur+1));
		if(*pcur==',')
			z=Rz(exprs(pcur+1));
		Point(pt,x,y,z);
		_view(&_vp,&pt);
		cx=(int)pt.x;
		cy=(int)pt.y;
		if(*pcur!=')'){
			px=cx;
			py=cy;
			if(*pcur==',')
				x=Rx(exprs(pcur+1));
			if(*pcur==',')
				y=Ry(exprs(pcur+1));
			if(*pcur==',')
				z=Rz(exprs(pcur+1));
			Point(pt,x,y,z);
			_view(&_vp,&pt);
			cx=(int)pt.x;
			cy=(int)pt.y;
		}
		/*
		if(!fitline(px,py,cx,cy,&lsx,&lsy,&lex,&ley))
			gdImageLine(im,lsx,lsy,lex,ley,_g_color);
		*/
		gdImageLine(im,px,py,cx,cy,_g_color);
		px=cx;
		py=cy;
		_x=x;
		_y=y;
		_z=z;
	}else
	if(!strncmp(grffunc,"_rect(",6)||!strncmp(grffunc,"_rect_(",7)){
		i=6;
		if(!strncmp(grffunc,"_rect_(",7))
			rel=1;
		i+=rel;
		if(!*(grffunc+i)||*(grffunc+i)==')')
			return 0;
		ch=*(grffunc+i);
		if(ch!='x'&&ch!='y'&&ch!='z')
			ch='x';
		for(i++;*(grffunc+i)&&*(grffunc+i)!=',';i++);
		if(!*(grffunc+i))
			return 0;
		x=Rx(exprs(grffunc+i+1));
		if(*pcur==',')
			y=Ry(exprs(pcur+1));
		if(*pcur==',')
			z=Rz(exprs(pcur+1));
		if(*pcur!=')'){
			if(*pcur==',')
				_x=Rx(exprs(pcur+1));
			if(*pcur==',')
				_y=Ry(exprs(pcur+1));
			if(*pcur==',')
				_z=Rz(exprs(pcur+1));
			xs[0]=x;
			ys[0]=y;
			zs[0]=z;
			if(ch=='x'){
				xs[1]=_x;
				ys[1]=y;
				zs[1]=z;
			}else
			if(ch=='y'){
				xs[1]=x;
				ys[1]=_y;
				zs[1]=z;
			}else
			if(ch=='z'){
				xs[1]=x;
				ys[1]=y;
				zs[1]=_z;
			}
			xs[2]=_x;
			ys[2]=_y;
			zs[2]=_z;
			if(ch=='x'){
				xs[3]=x;
				ys[3]=_y;
				zs[3]=_z;
			}else
			if(ch=='y'){
				xs[3]=_x;
				ys[3]=y;
				zs[3]=_z;
			}else
			if(ch=='z'){
				xs[3]=_x;
				ys[3]=_y;
				zs[3]=z;
			}
		}else{
			xs[0]=_x;
			ys[0]=_y;
			zs[0]=_z;
			if(ch=='x'){
				xs[1]=x;
				ys[1]=_y;
				zs[1]=_z;
			}else
			if(ch=='y'){
				xs[1]=_x;
				ys[1]=y;
				zs[1]=_z;
			}else
			if(ch=='z'){
				xs[1]=_x;
				ys[1]=_y;
				zs[1]=z;
			}
			xs[2]=x;
			ys[2]=y;
			zs[2]=z;
			if(ch=='x'){
				xs[3]=_x;
				ys[3]=y;
				zs[3]=z;
			}else
			if(ch=='y'){
				xs[3]=x;
				ys[3]=_y;
				zs[3]=z;
			}else
			if(ch=='z'){
				xs[3]=x;
				ys[3]=y;
				zs[3]=_z;
			}

			_x=x;
			_y=y;
			_z=z;
		}
		for(i=0;i<4;i++){
			Point(pt,xs[i],ys[i],zs[i]);
			_view(&_vp,&pt);
			px=(int)pt.x;
			py=(int)pt.y;

			if(i!=3)
				Point(pt,xs[i+1],ys[i+1],zs[i+1]);
			else
				Point(pt,xs[0],ys[0],zs[0]);
			_view(&_vp,&pt);
			cx=(int)pt.x;
			cy=(int)pt.y;

			/*
			if(!fitline(px,py,cx,cy,&lsx,&lsy,&lex,&ley))
				gdImageLine(im,lsx,lsy,lex,ley,_g_color);
			*/
			gdImageLine(im,px,py,cx,cy,_g_color);
		}
		Point(pt,_x,_y,_z);
		_view(&_vp,&pt);
		px=(int)pt.x;
		py=(int)pt.y;
	}else
	if(!strncmp(grffunc,"_cube(",6)||!strncmp(grffunc,"_cube_(",7)){
		i=6;
		if(!strncmp(grffunc,"_cube_(",7))
			rel=1;
		i+=rel;
		xs[0]=Rx(exprs(grffunc+i));
		if(*pcur==',')
			ys[0]=Ry(exprs(pcur+1));
		if(*pcur==',')
			zs[0]=Rz(exprs(pcur+1));
		for(i=1;i<7;i++){
			if(*pcur==',')
				xs[i]=Rx(exprs(pcur+1));
			if(*pcur==',')
				ys[i]=Ry(exprs(pcur+1));
			if(*pcur==',')
				zs[i]=Rz(exprs(pcur+1));
		}
		if(*pcur==',')
			xs[i]=Rx(exprs(pcur+1));
		if(*pcur==',')
			ys[i]=Ry(exprs(pcur+1));
		if(*pcur==',')
			zs[i]=Rz(exprs(pcur+1));
		if(++i==7){
			for(i=7;i>0;i--){
				xs[i]=xs[i-1];
				ys[i]=ys[i-1];
				zs[i]=zs[i-1];
			}
			xs[i]=_x;
			ys[i]=_y;
			zs[i]=_z;
		}

		for(i=0;i<4;i++){
			Point(pt,xs[i],ys[i],zs[i]);
			_view(&_vp,&pt);
			px=(int)pt.x;
			py=(int)pt.y;

			if(i!=3)
				Point(pt,xs[i+1],ys[i+1],zs[i+1]);
			else
				Point(pt,xs[0],ys[0],zs[0]);
			_view(&_vp,&pt);
			cx=(int)pt.x;
			cy=(int)pt.y;

			/*
			if(!fitline(px,py,cx,cy,&lsx,&lsy,&lex,&ley))
				gdImageLine(im,lsx,lsy,lex,ley,_g_color);
			*/
			gdImageLine(im,px,py,cx,cy,_g_color);

			Point(pt,xs[i+4],ys[i+4],zs[i+4]);
			_view(&_vp,&pt);
			cx=(int)pt.x;
			cy=(int)pt.y;

			/*
			if(!fitline(px,py,cx,cy,&lsx,&lsy,&lex,&ley))
				gdImageLine(im,lsx,lsy,lex,ley,_g_color);
			*/
			gdImageLine(im,px,py,cx,cy,_g_color);
		}
		for(i=4;i<8;i++){
			Point(pt,xs[i],ys[i],zs[i]);
			_view(&_vp,&pt);
			px=(int)pt.x;
			py=(int)pt.y;

			if(i!=7)
				Point(pt,xs[i+1],ys[i+1],zs[i+1]);
			else
				Point(pt,xs[4],ys[4],zs[4]);
			_view(&_vp,&pt);
			cx=(int)pt.x;
			cy=(int)pt.y;

			/*
			if(!fitline(px,py,cx,cy,&lsx,&lsy,&lex,&ley))
				gdImageLine(im,lsx,lsy,lex,ley,_g_color);
			*/
			gdImageLine(im,px,py,cx,cy,_g_color);
		}
		px=cx;
		py=cy;
		_x=xs[7];
		_y=ys[7];
		_z=zs[7];
	}else
	if(!strncmp(grffunc,"_getpos(",8)){
		setvar("_x",_x);
		setvar("_y",_y);
		setvar("_z",_z);
	}else
		return 1;

	return 0;
}

int
draw2dgrf(void)
{
	char	*x;
	int	ret,rel,i,p,l,nx,cx,cy,lsx,lsy,lex,ley;
	double	ox,oy,gsx,gdx,rho,theta;
	char	grftype;

	ret=0;

	p=1;
	for(x=grffunc;*x;x++){
		switch(*x){
			case '(':
				p++;
				break;
			case ')':
				p--;
				break;
		}
		if(!p)
			break;
	}
	if(!p)
		*x=0;

	if(_g_axes)
		draw2daxes();

	if(_x==0.0&&_y==0.0){
		px=Cx(_x);
		py=Cy(_y);
	}

	nx=-1;
	if(draw2d()){
		grftype=0;
		rel=0;
		if(!strncmp(grffunc,"_polar(",7))
			grftype=1;
		else
		if(!strncmp(grffunc,"_polar_(",8)){
			grftype=1;
			rel=1;
		}else
		if(!strncmp(grffunc,"_para(",6))
			grftype=2;
		else
		if(!strncmp(grffunc,"_para_(",7)){
			grftype=2;
			rel=1;
		}

		ox=0.0;
		oy=0.0;
		if(!grftype){
			gdx=(_exr-_sxr)/grfwidth;
			nx=(_ex-_sx)/gdx;
			gsx=_sx;
		}else{
			gdx=_dx;
			nx=(ex-sx)/gdx;
			gdx=(ex-sx)/(nx?nx:1);
			gsx=sx;
			if(rel){
				ox=_x;
				oy=_y;
			}
		}
	}

	for(i=0;i<=nx;i++){
		theta=gsx+i*gdx;
		setvar("_x",theta);
		if(grftype==1){
			rho=exprs(grffunc+7+rel);
			_x=rho*cos(theta);
			_y=rho*sin(theta);
		}else
		if(grftype==2){
			_x=exprs(grffunc+6+rel);
			if(*pcur==',')
				_y=exprs(pcur+1);
		}else{
			_x=theta;
			_y=exprs(grffunc);
		}
		if(rel){
			_x+=ox;
			_y+=oy;
		}

		if(err){
			ret=1;
			break;
		}

		cx=Clx(_x);
		cy=Cly(_y);

		if(!i || (cx==px && cy==py)){
			if(cx>=0 && cx<grfwidth && cy>=0 && cy<grfheight)
				gdImageSetPixel(im,cx,cy,_g_color);
		}else
		/*
		if(!fitline(px,py,cx,cy,&lsx,&lsy,&lex,&ley))
			gdImageLine(im,lsx,lsy,lex,ley,_g_color);
		*/
			gdImageLine(im,px,py,cx,cy,_g_color);
		px=cx;
		py=cy;
	}

	if(!p){
		*x=')';
		pcur=x+1;
	}else
		pcur=x;

	return ret;
}

int
draw3dgrf(void)
{
	char	*x;
	int	ret,rel,i,j,p,l,nx,ny,cx,cy,lsx,lsy,lex,ley;
	struct	_point	pt;
	double	ox,oy,oz,gdx,gdy,rho,phi,theta;
	char	grftype;

	ret=0;

	p=1;
	for(x=grffunc;*x;x++){
		switch(*x){
			case '(':
				p++;
				break;
			case ')':
				p--;
				break;
		}
		if(!p)
			break;
	}
	if(!p)
		*x=0;

	dxfirst=1;

	if(_g_axes){
		draw3daxis(1,"x");
		draw3daxis(2,"y");
		draw3daxis(3,"z");
	}

	if(_x==0.0&&_y==0.0&&_z==0.0){
		Point(pt,_x,_y,_z);
		_view(&_vp,&pt);
		px=(int)pt.x;
		py=(int)pt.y;
	}

	nx=-1;
	ny=-1;
	if(draw3d()){
		grftype=0;
		rel=0;
		if(!strncmp(grffunc,"_polar(",7))
			grftype=1;
		else
		if(!strncmp(grffunc,"_polar_(",8)){
			grftype=1;
			rel=1;
		}else
		if(!strncmp(grffunc,"_para(",6))
			grftype=2;
		else
		if(!strncmp(grffunc,"_para_(",7)){
			grftype=2;
			rel=1;
		}

		nx=(ex-sx)/_dx;
		ny=(ey-sy)/_dy;
		gdx=(ex-sx)/(nx?nx:1);
		gdy=(ey-sy)/(ny?ny:1);

		ox=0.0;
		oy=0.0;
		oz=0.0;
		if(rel){
			ox=_x;
			oy=_y;
			oz=_z;
		}
	}

	if(_grf3dw&0x1){
		for(i=0;i<=nx;i++){
			phi=sx+i*gdx;
			setvar("_x",phi);
			for(j=0;j<=ny;j++){
				theta=sy+j*gdy;
				setvar("_y",theta);
				if(grftype==1){
					rho=exprs(grffunc+7+rel);
					_x=rho*sin(phi)*cos(theta);
					_y=rho*sin(phi)*sin(theta);
					_z=rho*cos(phi);
				}else
				if(grftype==2){
					_x=exprs(grffunc+6+rel);
					if(*pcur==',')
						_y=exprs(pcur+1);
					if(*pcur==',')
						_z=exprs(pcur+1);
				}else{
					_x=theta;
					_y=phi;
					_z=exprs(grffunc);
				}
				if(rel){
					_x+=ox;
					_y+=oy;
					_z+=oz;
				}

				if(err){
					ret=1;
					break;
				}
				Point(pt,_x,_y,_z);
				_view(&_vp,&pt);
				cx=(int)pt.x;
				cy=(int)pt.y;
				if(!j || (cx==px && cy==py)){
					if(cx>=0 && cx<grfwidth && cy>=0 && cy<grfheight)
						gdImageSetPixel(im,cx,cy,_g_color);
				}else
				/*
				if(!fitline(px,py,cx,cy,&lsx,&lsy,&lex,&ley))
					gdImageLine(im,lsx,lsy,lex,ley,_g_color);
				*/
					gdImageLine(im,px,py,cx,cy,_g_color);
				px=cx;
				py=cy;
			}
		}
	}
	if(_grf3dw&0x2){
		for(j=0;j<=ny;j++){
			theta=sy+j*gdy;
			setvar("_y",theta);
			for(i=0;i<=nx;i++){
				phi=sx+i*gdx;
				setvar("_x",phi);
				if(grftype==1){
					rho=exprs(grffunc+7+rel);
					_x=rho*sin(phi)*cos(theta);
					_y=rho*sin(phi)*sin(theta);
					_z=rho*cos(phi);
				}else
				if(grftype==2){
					_x=exprs(grffunc+6+rel);
					if(*pcur==',')
						_y=exprs(pcur+1);
					if(*pcur==',')
						_z=exprs(pcur+1);
				}else{
					_x=theta;
					_y=phi;
					_z=exprs(grffunc);
				}
				if(rel){
					_x+=ox;
					_y+=oy;
					_z+=oz;
				}

				if(err){
					ret=1;
					break;
				}
				Point(pt,_x,_y,_z);
				_view(&_vp,&pt);
				cx=(int)pt.x;
				cy=(int)pt.y;
				if(!i || (cx==px && cy==py)){
					if(cx>=0 && cx<grfwidth && cy>=0 && cy<grfheight)
						gdImageSetPixel(im,cx,cy,_g_color);
				}else
				/*
				if(!fitline(px,py,cx,cy,&lsx,&lsy,&lex,&ley))
					gdImageLine(im,lsx,lsy,lex,ley,_g_color);
				*/
					gdImageLine(im,px,py,cx,cy,_g_color);
				px=cx;
				py=cy;
			}
		}
	}

	if(!p){
		*x=')';
		pcur=x+1;
	}else
		pcur=x;

	return ret;
}
#endif
