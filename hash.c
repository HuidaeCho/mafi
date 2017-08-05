#define	_HASH_C_
#include "mafi.h"

extern	int	skip_result;
extern	int	ngroupvars;
extern	char	**groupvars;

void
zerohash(void)
{

	ngroupvars=0;
	groupvars=0;
	zeroprsvd();
	zerovar();
	zeromatrix();
	zerostr();
	zerolabel();
	zerofunc();
	zeromfunc();
	zeromxfunc();
}

#define	definezero(zerow);						\
void									\
zero##zerow(void)							\
{									\
	int		i;						\
									\
	for(i=0;i<HASHSIZE;i++)						\
		zerow##hash[i]=0;					\
	return;								\
}

definezero(prsvd);
definezero(var);
definezero(matrix);
definezero(str);
definezero(label);
definezero(func);
definezero(mfunc);
definezero(mxfunc);

int
inithash(void)
{
	_ret_type=3;
	err=0;
//	buffer=0;
//	inlineno=dolineno=0;
	freehash();
	setsysprsvds();
	setsysvars();
	setsysfuncs();
	setsysmfuncs();
	setsysmxfuncs();
	return err;
}

void
freehash(void)
{
	freegroupvars();
	freeprsvd();
	freevar();
	freematrix();
	freestr();
	freelabel();
	freefunc();
	freemfunc();
	freemxfunc();
	return;
}

#define	definefree(freew,freet);					\
void									\
free##freew(void)							\
{									\
	int		i;						\
	P##freet	pvl,pvltmp;					\
									\
	for(i=0;i<HASHSIZE;i++){					\
		pvl=freew##hash[i];					\
		while(pvl){						\
			pvltmp=pvl->next;				\
			free(pvl->name);				\
			free(pvl);					\
			pvl=pvltmp;					\
		}							\
		freew##hash[i]=0;					\
	}								\
	return;								\
}

definefree(prsvd,PRSVD);
definefree(var,VAR);
definefree(label,LABEL);

#define definefreefunc(freew,freet);					\
void									\
free##freew(void)							\
{									\
	int		i,j;						\
	P##freet	pfunc,pfunctmp;					\
									\
	for(i=0;i<HASHSIZE;i++){					\
		pfunc=freew##hash[i];					\
		while(pfunc){						\
			pfunctmp=pfunc->next;				\
			free(pfunc->name);				\
			if(pfunc->def){					\
				for(j=0;j<pfunc->npars;j++)		\
					free(pfunc->parname[j]);	\
				free(pfunc->parname);			\
				free(pfunc->def);			\
			}						\
			free(pfunc);					\
			pfunc=pfunctmp;					\
		}							\
		freew##hash[i]=0;					\
	}								\
	return;								\
}

definefreefunc(func,FUNC);
definefreefunc(mfunc,MFUNC);
definefreefunc(mxfunc,MXFUNC);

void
freematrix(void)
{
	int	i;
	PMATRIX	pmatrix,pmatrixtmp;

	for(i=0;i<HASHSIZE;i++){
		pmatrix=matrixhash[i];
		while(pmatrix){
			pmatrixtmp=pmatrix->next;
			free(pmatrix->name);
			_free_matrix(pmatrix->matrix);
			free(pmatrix);
			pmatrix=pmatrixtmp;
		}
		matrixhash[i]=0;
	}
	return;
}

void
freestr(void)
{
	int	i;
	PSTR	pstr,pstrtmp;

	for(i=0;i<HASHSIZE;i++){
		pstr=strhash[i];
		while(pstr){
			pstrtmp=pstr->next;
			free(pstr->name);
			free(pstr->str);
			free(pstr);
			pstr=pstrtmp;
		}
		strhash[i]=0;
	}
	return;
}

int
gethash(name)
	char	*name;
{
	int	h=0;

	while(*name)
		h+=*name++;

	return abs(h%HASHSIZE);
}

void
setsysprsvds(void)
{
	int	i,n;

	n=sizeof(prsvds)/sizeof(char *);
	for(i=0;i<n;i++)
		setprsvd(prsvds[i]);
	return;
}

void
setsysvars(void)
{
	int	i,n;

	n=sizeof(sysvars)/sizeof(SYSVAR);
	for(i=0;i<n;i++)
		setvar(sysvars[i].name,sysvars[i].value);
	return;
}

void
setsysfuncs(void)
{
	int	i,n;

	n=sizeof(sysfuncs)/sizeof(SYSFUNC);
	for(i=0;i<n;i++)
		setfunc(sysfuncs[i].name,sysfuncs[i].npars,
				sysfuncs[i].func,
				sysfuncs[i].mfunc);
	return;
}

void
setsysmfuncs(void)
{
	int	i,n;

	n=sizeof(sysmfuncs)/sizeof(SYSMFUNC);
	for(i=0;i<n;i++)
		setmfunc(sysmfuncs[i].name,sysmfuncs[i].npars,
				sysmfuncs[i].func,
				sysmfuncs[i].mfunc);
	return;
}

void
setsysmxfuncs(void)
{
	int	i,n;

	n=sizeof(sysmxfuncs)/sizeof(SYSMXFUNC);
	for(i=0;i<n;i++)
		setmxfunc(sysmxfuncs[i].name,sysmxfuncs[i].npars,
				sysmxfuncs[i].func,
				sysmxfuncs[i].mfunc);
	return;
}

void
setprsvd(str)
    	char	*str;
{
    	int	i;
	PPRSVD	pprsvd,pprsvdtmp;

	pprsvd=(PPRSVD)malloc(sizeof(PRSVD));
	if(!pprsvd){
		err=1;
		return;
	}
	pprsvd->name=(char *)malloc(strlen(str)+1);
	if(!pprsvd->name){
		free(pprsvd);
		err=1;
		return;
	}
	strcpy(pprsvd->name,str);

	i=gethash(str);
	pprsvdtmp=prsvdhash[i];
	while(pprsvdtmp){
		if(!strcmp(pprsvdtmp->name,str)){
			if(pprsvdtmp->prev)
				pprsvdtmp->prev->next=pprsvdtmp->next;
			if(pprsvdtmp->next)
				pprsvdtmp->next->prev=pprsvdtmp->prev;
			if(pprsvdtmp==prsvdhash[i])
			    	prsvdhash[i]=pprsvdtmp->next;
			free(pprsvdtmp->name);
			free(pprsvdtmp);
			break;
		}
		pprsvdtmp=pprsvdtmp->next;
	}
	if(prsvdhash[i])
		prsvdhash[i]->prev=pprsvd;
	pprsvd->prev=NULL;
	pprsvd->next=prsvdhash[i];
	prsvdhash[i]=pprsvd;

	return;
}

#define	defineset(type,setw,sett,setv,setzero);				\
void									\
set##setw(str,setv)							\
	char	*str;							\
	type	setv;							\
{									\
	int	i;							\
	char	*x;							\
	P##sett	pvl,pvltmp;						\
									\
	if((x=isset(str)))						\
		*x=0;							\
									\
	i=gethash(str);							\
	pvltmp=setw##hash[i];						\
	while(pvltmp){							\
		if(!strcmp(pvltmp->name,str)){				\
			if(pvltmp->prev)				\
				pvltmp->prev->next=pvltmp->next;	\
			if(pvltmp->next)				\
				pvltmp->next->prev=pvltmp->prev;	\
			if(pvltmp==setw##hash[i])			\
			    	setw##hash[i]=pvltmp->next;		\
			free(pvltmp->name);				\
			free(pvltmp);					\
			break;						\
		}							\
		pvltmp=pvltmp->next;					\
	}								\
	if(!setv&&!setzero){						\
		if(x)							\
			*x='=';						\
	    	return;							\
	}								\
									\
	pvl=(P##sett)malloc(sizeof(sett));				\
	if(!pvl){							\
		if(x)							\
			*x='=';						\
		err=1;							\
		return;							\
	}								\
	pvl->name=(char *)malloc(strlen(str)+1);			\
	if(!pvl->name){							\
		if(x)							\
			*x='=';						\
		free(pvl);						\
		err=1;							\
		return;							\
	}								\
	strcpy(pvl->name,str);						\
	if(x)								\
		*x='=';							\
	if(setw##hash[i])						\
		setw##hash[i]->prev=pvl;				\
	pvl->prev=NULL;							\
	pvl->next=setw##hash[i];					\
	setw##hash[i]=pvl;						\
	pvl->setv=setv;							\
	return;								\
}

defineset(double,var,VAR,value,0);
defineset(int,label,LABEL,lineno,1);

#define	definesetfunc(setw,rett,funct,part,mpart)			\
void									\
set##setw(str,npars,func,mfunc)						\
	char	*str;							\
	int	npars;							\
	rett	(*func)part,(*mfunc)mpart;				\
{									\
	int	i,j,p;							\
	char	*x,*y,*z;						\
	P##funct	pfunc,pfunctmp;					\
	char	ch;							\
									\
	z=0;								\
	if(!func&&!mfunc&&(z=strchr(str,'(')))				\
		*z=0;							\
									\
	i=gethash(str);							\
	pfunctmp=setw##hash[i];						\
	while(pfunctmp){						\
		if(!strcmp(pfunctmp->name,str)){			\
			if(pfunctmp->prev)				\
				pfunctmp->prev->next=pfunctmp->next;	\
			if(pfunctmp->next)				\
				pfunctmp->next->prev=pfunctmp->prev;	\
			if(pfunctmp==setw##hash[i])			\
			    	setw##hash[i]=pfunctmp->next;		\
			free(pfunctmp->name);				\
			free(pfunctmp);					\
			break;						\
		}							\
		pfunctmp=pfunctmp->next;				\
	}								\
	if(z&&*(z+strlen(z+1))=='='){					\
		*z='(';							\
		return;							\
	}								\
									\
	pfunc=(P##funct)malloc(sizeof(funct));				\
	if(!pfunc){							\
		if(z)							\
			*z='(';						\
		err=1;							\
		return;							\
	}								\
	pfunc->name=(char *)malloc(strlen(str)+1);			\
	if(!pfunc->name){						\
		free(pfunc);						\
		if(z)							\
			*z='(';						\
		err=1;							\
		return;							\
	}								\
	strcpy(pfunc->name,str);					\
	if(z)								\
		*z='(';							\
	if(setw##hash[i])						\
		setw##hash[i]->prev=pfunc;				\
	pfunc->prev=NULL;						\
	pfunc->next=setw##hash[i];					\
	setw##hash[i]=pfunc;						\
									\
	pfunc->npars=npars;						\
	pfunc->func=0;							\
	pfunc->mfunc=0;							\
	pfunc->def=0;							\
									\
	if(func)							\
		pfunc->func=func;					\
	else if(mfunc)							\
		pfunc->mfunc=mfunc;					\
									\
	if(func||mfunc||npars<0)					\
		return;							\
									\
	i=0;								\
	for(x=y=z+1;*x;x++)						\
		if(x!=y&&(*x==','||*x==')')){				\
			y=x+1;						\
			i++;						\
		}else if(*x=='=')					\
			break;						\
									\
	pfunc->npars=i;							\
	if(i>0){							\
		pfunc->parname=(char **)malloc(i*sizeof(char *));	\
		if(!pfunc->parname){					\
			free(pfunc->name);				\
			free(pfunc);					\
			err=1;						\
			return;						\
		}							\
	}								\
									\
	i=0;								\
	z++;								\
	for(x=z;*x;x++){						\
		if(*x==','||*x==')'){					\
			if(*x==')'&&x==z)				\
				continue;				\
			ch=*x;						\
			*x=0;						\
			pfunc->parname[i]=(char *)malloc(strlen(z)+1);	\
			if(!pfunc->parname[i]){				\
				for(;--i>=0;)				\
					free(pfunc->parname[i]);	\
				free(pfunc->parname);			\
				free(pfunc->name);			\
				free(pfunc);				\
				*x=ch;					\
				err=1;					\
				return;					\
			}						\
			strcpy(pfunc->parname[i++],z);			\
			*x=ch;						\
			z=x+1;						\
		}else if(*x=='='){					\
			p=0;						\
			j=0;						\
			for(y=x+1;*y;y++){				\
				if(*y=='"'&&*(y-1)!='\\')		\
					j++;				\
				else					\
				if((*y=='('||*y=='['||*y=='{')&&!(j&1))	\
					p++;				\
				else					\
				if((*y==')'||*y==']'||*y=='}')&&!(j&1)){\
					p--;				\
					if(*y=='}'&&*(x+1)=='{'&&!p)	\
						break;			\
				}else					\
				if(*y==';'&&*(x+1)!='{'&&!(j&1)&&!p)	\
					break;				\
			}						\
			ch=*(y+1);					\
			*(y+1)=0;					\
		    	pfunc->def=(char *)malloc(strlen(x+1)+1);	\
			if(!pfunc->def){				\
				for(;--i>=0;)				\
					free(pfunc->parname[i]);	\
				free(pfunc->parname);			\
				free(pfunc->name);			\
				free(pfunc);				\
				*x=ch;					\
				*(y+1)=ch;				\
				err=1;					\
				return;					\
			}						\
			strcpy(pfunc->def,x+1);				\
			*(y+1)=ch;					\
			break;						\
		}							\
	}								\
	return;								\
}

definesetfunc(func,double,FUNC,(double,...),(_PMATRIX,...));
definesetfunc(mfunc,_PMATRIX,MFUNC,(double,...),(_PMATRIX,...));
definesetfunc(mxfunc,_PMATRIX,MXFUNC,(double,_PMATRIX,...),(_PMATRIX,double,...));

void
setmatrix(str,_pmatrix)
	char		*str;
	_PMATRIX	_pmatrix;
{
	int		i,j,k,p;
	char		*x,*y,*mstr;
	PMATRIX		pmatrix,pmatrixtmp;

	x=0;
	if((mstr=isset(str))){
		*mstr++=0;
		if((x=strchr(str,'[')))
		    	*x=0;
	}

	i=gethash(str);
	pmatrixtmp=matrixhash[i];
	while(pmatrixtmp){
		if(!strcmp(pmatrixtmp->name,str)){
		    	if(!x){
				if(pmatrixtmp->prev)
					pmatrixtmp->prev->next=pmatrixtmp->next;
				if(pmatrixtmp->next)
					pmatrixtmp->next->prev=pmatrixtmp->prev;
				if(pmatrixtmp==matrixhash[i])
				    	matrixhash[i]=pmatrixtmp->next;
				free(pmatrixtmp->name);
				_free_matrix(pmatrixtmp->matrix);
				free(pmatrixtmp);
				if(!mstr&&!_pmatrix)
				    	return;
			}
			break;
		}
		pmatrixtmp=pmatrixtmp->next;
	}

	if(x){
		*x='[';
		*(mstr-1)='=';
	    	if(!pmatrixtmp)
		    	return;
		p=0;
		for(y=x+1;*y;y++){
		    	switch(*y){
			    	case '[':
				case '(':
				    	p++;
					break;
				case ']':
					if(!(p--)){
						*y=0;
						j=exprs(x);
						*y=']';
					}
					break;
				case ')':
					p--;
					break;
				case ',':
				    	if(!p){
						*y=0;
					    	i=exprs(x+1);
						*y=',';
						x=y+1;
					}
					break;
			}
			if(p<0)
				break;
		}
	   	_element(pmatrixtmp->matrix,i,j)=exprs(mstr);
		return;
	}
	pmatrix=(PMATRIX)malloc(sizeof(MATRIX));
	if(!pmatrix){
		if(x)
			*x='[';
		if(mstr)
			*(mstr-1)='=';
		err=1;
		return;
	}
	pmatrix->name=(char *)malloc(strlen(str)+1);
	if(!pmatrix->name){
		free(pmatrix);
		if(x)
			*x='[';
		if(mstr)
			*(mstr-1)='=';
		err=1;
		return;
	}
	strcpy(pmatrix->name,str);
	if(x)
		*x='[';
	if(mstr)
		*(mstr-1)='=';

	k=gethash(pmatrix->name);
	if(matrixhash[k])
		matrixhash[k]->prev=pmatrix;
	pmatrix->prev=NULL;
	pmatrix->next=matrixhash[k];
	matrixhash[k]=pmatrix;

	if(_pmatrix)
		pmatrix->matrix=_copy_matrix(_pmatrix);
	else
		pmatrix->matrix=mexprs(mstr);
	return;
}

void
setstr(str,_pstr)
	char	*str,*_pstr;
{
	int	i,j;
	char	*x,*mstr;
	PSTR	pstr,pstrtmp;

	x=0;
	if((mstr=isset(str)))
		*mstr++=0;
	if((x=strchr(str,'[')))
	    	*x=0;

	i=gethash(str);
	pstrtmp=strhash[i];
	while(pstrtmp){
		if(!strcmp(pstrtmp->name,str)){
		    	if(!x){
				if(pstrtmp->prev)
					pstrtmp->prev->next=pstrtmp->next;
				if(pstrtmp->next)
					pstrtmp->next->prev=pstrtmp->prev;
				if(pstrtmp==strhash[i])
				    	strhash[i]=pstrtmp->next;
				free(pstrtmp->name);
				free(pstrtmp->str);
				free(pstrtmp);
			}
			break;
		}
		pstrtmp=pstrtmp->next;
	}
	if(!((_pstr&&*_pstr)||(mstr&&*mstr)))
	    	return;

	if(x){
		*x='[';
		if(mstr)
			*(mstr-1)='=';
	    	if(!pstrtmp)
		    	return;
		i=exprs(x+1);
		if(strlen(pstrtmp->str)>i)
			pstrtmp->str[i]=*(mstr?mstr:_pstr);
		return;
	}
	pstr=(PSTR)malloc(sizeof(STR));
	if(!pstr){
		if(x)
			*x='[';
		if(mstr)
			*(mstr-1)='=';
		err=1;
		return;
	}
	pstr->name=(char *)malloc(strlen(str)+1);
	if(!pstr->name){
		free(pstr);
		if(x)
			*x='[';
		if(mstr)
			*(mstr-1)='=';
		err=1;
		return;
	}
	strcpy(pstr->name,str);
	if(x)
		*x='[';
	if(mstr)
		*(mstr-1)='=';

	j=gethash(pstr->name);
	if(strhash[j])
		strhash[j]->prev=pstr;
	pstr->prev=NULL;
	pstr->next=strhash[j];
	strhash[j]=pstr;

	pstr->str=(char *)malloc(strlen(_pstr?_pstr:mstr)+1);
	if(!pstr->str){
		free(pstr->name);
		free(pstr);
		if(x)
			*x='[';
		if(mstr)
			*(mstr-1)='=';
		err=1;
		return;
	}
	strcpy(pstr->str,(_pstr?_pstr:mstr));
	return;
}

#define	defineget(type,getw,pgett,getn,getzero);			\
type									\
getw(str)								\
    	char	*str;							\
{									\
	int	i;							\
	pgett	pvl;							\
	char	ch;							\
									\
	pcur=str;							\
	for(;*pcur;pcur++)						\
		if(!isvar(*pcur))					\
			break;						\
	if(pcur==str)							\
		return 0;						\
	ch=*pcur;							\
	if(ch)								\
		*pcur=0;						\
	i=gethash(str);							\
	pvl=getw##hash[i];						\
	while(pvl){							\
		if(!strcmp(pvl->name,str))				\
			break;						\
		pvl=pvl->next;						\
	}								\
	if(ch)								\
		*pcur=ch;						\
	if(!pvl){							\
		return (!getzero?0:-1);					\
	}								\
	return pvl->getn;						\
}

defineget(double,var,PVAR,value,0);
defineget(int,label,PLABEL,lineno,1);

#define	definegetfunc(type,getw,pfunct,pexprs,domode,pgetfunc,pcopy);	\
type									\
getw(str)								\
    	char		*str;						\
{									\
	int		i,j;						\
	pfunct		pfunc;						\
	type		ret;						\
	char		*pfstr,*x,*y;					\
	_PMATRIX	mpar[10],mpartmp;				\
	double		par[10],partmp;					\
									\
	for(i=0;i<10;i++){						\
		mpar[i]=0;						\
		par[i]=0;						\
	}								\
									\
	*(x=strchr(str,'('))=0;						\
	i=gethash(str);							\
	pfunc=getw##hash[i];						\
	while(pfunc){							\
		if(!strcmp(pfunc->name,str))				\
			break;						\
		pfunc=pfunc->next;					\
	}								\
	*x='(';								\
									\
	func2=0;							\
	if(pfunc&&pfunc->npars<0&&!skip_result){			\
		func2=1;						\
		return 0;						\
	}								\
									\
	i=j=0;								\
	for(x=y=x+1;*x&&i<10;x++){					\
		switch(*x){						\
			case '(':					\
			case '[':					\
				i++;					\
				break;					\
			case ')':					\
				if(!(i--)){				\
					if(!skip_result){		\
						*x=0;			\
						if(ismatrix(y))		\
							mpar[j++]=mexprs(y);\
						else if(strlen(y))	\
							par[j++]=exprs(y);\
						*x=')';			\
					}				\
					pcur=x+1;			\
				}					\
				break;					\
			case ']':					\
				i--;					\
				break;					\
			case ',':					\
				if(!i){					\
					if(!skip_result){		\
						*x=0;			\
						if(ismatrix(y))		\
							mpar[j++]=mexprs(y);\
						else if(strlen(y))	\
							par[j++]=exprs(y);\
						*x=',';			\
					}				\
					y=x+1;				\
				}					\
				break;					\
		}							\
		if(pcur==x+1)						\
			break;						\
	}								\
									\
	if(skip_result)							\
		return 0;						\
									\
	if(!pfunc){							\
		for(i=0;i<10;i++){					\
			if(mpar[i])					\
				_free_matrix(mpar[i]);			\
		}							\
		if(strstr(str,"[]"))					\
			snprintf(tmpbuf,TMPBUFSIZE,			\
				"\n%s: No such matrix function\n",str);	\
		else							\
	    		snprintf(tmpbuf,TMPBUFSIZE,			\
				"\n%s: No such function\n",str);	\
		error();						\
	}								\
	if(pfunc->npars!=j){						\
		for(i=0;i<10;i++){					\
			if(mpar[i])					\
				_free_matrix(mpar[i]);			\
		}							\
		snprintf(tmpbuf,TMPBUFSIZE,				\
			       "\n%d: Invalid number of parameters, %d expected\n",j,pfunc->npars);\
		error();						\
	}								\
	if(pfunc->func){							\
		if(mpar[0]){						\
			for(i=0;i<10;i++){				\
				if(mpar[i])				\
					_free_matrix(mpar[i]);		\
			}						\
			snprintf(tmpbuf,TMPBUFSIZE,			\
				"\nmatrix: Invalid type of parameters, value expected\n");\
			error();					\
		}							\
		for(i=0;i<10;i++){					\
			if(mpar[i])					\
				_free_matrix(mpar[i]);			\
		}							\
		return (*pfunc->func)(par[0],par[1],par[2],par[3],par[4],\
			        par[5],par[6],par[7],par[8],par[9]);	\
	}else if(pfunc->mfunc){						\
		if(!mpar[0]){						\
			for(i=0;i<10;i++){				\
				if(mpar[i])				\
					_free_matrix(mpar[i]);		\
			}						\
			snprintf(tmpbuf,TMPBUFSIZE,			\
				"\nvalue: Invalid type of parameters, matrix expected\n");\
			error();					\
		}							\
		ret=(*pfunc->mfunc)(mpar[0],mpar[1],mpar[2],mpar[3],mpar[4],	\
			        mpar[5],mpar[6],mpar[7],mpar[8],mpar[9]);	\
		for(i=0;i<10;i++)					\
			if(mpar[i])					\
				_free_matrix(mpar[i]);			\
		return ret;						\
	}								\
	pfstr=(char *)malloc(strlen(pfunc->def)+1);			\
	if(!pfstr){							\
		err=1;							\
		return 0;						\
	}								\
	strcpy(pfstr,pfunc->def);					\
	for(i=0;i<pfunc->npars;i++)					\
		if((y=strstr(pfunc->parname[i],"[]"))){			\
		    	*y=0;						\
			if((mpartmp=matrix(pfunc->parname[i])))		\
				mpartmp=_copy_matrix(mpartmp);		\
			setmatrix(pfunc->parname[i],mpar[i]);		\
			*y='[';						\
			if(mpar[i])					\
				_free_matrix(mpar[i]);			\
			mpar[i]=mpartmp;				\
		}else{							\
			partmp=var(pfunc->parname[i]);			\
			setvar(pfunc->parname[i],par[i]);		\
			par[i]=partmp;					\
		}							\
	if(*pfstr=='{'){						\
		if(doline(pfstr,domode)==RETURN){			\
			if(domode==DOVAR)				\
				_ret_type=1;				\
			else						\
				_ret_type=2;				\
			setvar("_ret_type",_ret_type);			\
			ret=pcopy(pgetfunc("_ret"));			\
		}else{							\
			_ret_type=0;					\
			setvar("_ret_type",_ret_type);			\
		}							\
	}else								\
		ret=pexprs(pfstr);					\
	free(pfstr);							\
	for(i=0;i<pfunc->npars;i++)					\
		if((y=strstr(pfunc->parname[i],"[]"))){			\
		    	*y=0;						\
			setmatrix(pfunc->parname[i],mpar[i]);		\
			*y='[';						\
			if(mpar[i])					\
				_free_matrix(mpar[i]);			\
		}else							\
			setvar(pfunc->parname[i],par[i]);		\
	pcur=x+1;							\
	return ret;							\
}

definegetfunc(double,func,PFUNC,exprs,DOVAR,var,);
definegetfunc(_PMATRIX,mfunc,PMFUNC,mexprs,DOMATRIX,matrix,_copy_matrix);

_PMATRIX
mxfunc(str)
    	char		*str;
{
	int		i,j;
	PMXFUNC		pfunc;
	_PMATRIX	ret;
	char		*pfstr,*x,*y;
	_PMATRIX	mpar[10],mpartmp;
	double		par[10],partmp;	

	for(i=0;i<10;i++){
		mpar[i]=0;
		par[i]=0;
	}

	*(x=strchr(str,'('))=0;
	i=gethash(str);	
	pfunc=mxfunchash[i];
	while(pfunc){
		if(!strcmp(pfunc->name,pcur))
			break;
		pfunc=pfunc->next;
	}
	*x='(';

	func2=0;
	if(pfunc&&pfunc->npars<0){
		func2=1;
		return 0;
	}

	i=j=0;
	for(x=y=x+1;*x;x++){
		switch(*x){
			case '(':
			case '[':
				i++;
				break;
			case ')':
				if(!(i--)){
					*x=0;
					if(ismatrix(y))
						mpar[j++]=mexprs(y);
					else if(strlen(y))
						par[j++]=exprs(y);
					*x=')';
					pcur=x+1;
				}
				break;
			case ']':
				i--;
				break;
			case ',':
				if(!i){
					*x=0;
					if(ismatrix(y))
						mpar[j++]=mexprs(y);
					else if(strlen(y))
						par[j++]=exprs(y);
					*x=',';
					y=x+1;
				}
				break;
		}
		if(pcur==x+1)
			break;
	}
	if(!pfunc){
		for(i=0;i<10;i++){
			if(mpar[i])
				_free_matrix(mpar[i]);
		}
		snprintf(tmpbuf,TMPBUFSIZE,
				"\n%s: No such matrix function\n",str);
		error();
	}
	if(pfunc->npars!=j){
		for(i=0;i<10;i++){
			if(mpar[i])
				_free_matrix(mpar[i]);
		}
		snprintf(tmpbuf,TMPBUFSIZE,
			       "\n%d: Invalid number of parameters, %d expected\n",j,pfunc->npars);
		error();
	}
	if(pfunc->func){	
		if(mpar[0]){
			for(i=0;i<10;i++){
				if(mpar[i])
					_free_matrix(mpar[i]);
			}
			snprintf(tmpbuf,TMPBUFSIZE,
				"\nmatrix: Invalid type of parameters, value expected\n");
			error();
		}
		ret=(*pfunc->func)(par[0],mpar[1],mpar[2],mpar[3],mpar[4],
			        mpar[5],mpar[6],mpar[7],mpar[8],mpar[9]);
		for(i=0;i<10;i++){
			if(mpar[i])
				_free_matrix(mpar[i]);
		}
		return ret;
	}else if(pfunc->mfunc){
		if(!mpar[0]){
			for(i=0;i<10;i++){
				if(mpar[i])
					_free_matrix(mpar[i]);
			}
			snprintf(tmpbuf,TMPBUFSIZE,
				"\nmatrix: Invalid type of parameters, value expected\n");
			error();
		}
		ret=(*pfunc->mfunc)(mpar[0],par[1],par[2],par[3],par[4],
			        par[5],par[6],par[7],par[8],par[9]);
		for(i=0;i<10;i++){
			if(mpar[i])
				_free_matrix(mpar[i]);
		}
		return ret;
	}
	pfstr=(char *)malloc(strlen(pfunc->def)+1);
	if(!pfstr){
		err=1;
		return 0;
	}
	strcpy(pfstr,pfunc->def);
	for(i=0;i<pfunc->npars;i++)
		if((y=strstr(pfunc->parname[i],"[]"))){
			*y=0;
			if((mpartmp=matrix(pfunc->parname[i])))
				mpartmp=_copy_matrix(mpartmp);	
			setmatrix(pfunc->parname[i],mpar[i]);
			*y='[';
			if(mpar[i])
				_free_matrix(mpar[i]);
			mpar[i]=mpartmp;
		}else{
			partmp=var(pfunc->parname[i]);
			setvar(pfunc->parname[i],par[i]);
			par[i]=partmp;
		}
	ret=mexprs(pfstr);	
	free(pfstr);
	for(i=0;i<pfunc->npars;i++)
		if((y=strstr(pfunc->parname[i],"[]"))){
			*y=0;
			setmatrix(pfunc->parname[i],mpar[i]);
			*y='[';
			if(mpar[i])
				_free_matrix(mpar[i]);
		}else
			setvar(pfunc->parname[i],par[i]);
	pcur=x+1;
	return ret;
}

char *
fstr(str)
    	char		*str;
{
	int		i;
	PFUNC		pfunc;
	PMFUNC		pmfunc;
	PMXFUNC		pmxfunc;
	char		*x;

	if((x=strchr(str,'(')))
		*x=0;
	i=gethash(str);

	if(strstr(str,"[]")){
		if(ismxfunc(str)){
			pmxfunc=mxfunchash[i];
			while(pmxfunc){
				if(!strcmp(pmxfunc->name,str))
					break;
				pmxfunc=pmxfunc->next;
			}
			if(x)
				*x='(';
			if(pmxfunc&&pmxfunc->def)
				return pmxfunc->def;
		}else{
			pmfunc=mfunchash[i];
			while(pmfunc){
				if(!strcmp(pmfunc->name,str))
					break;
				pmfunc=pmfunc->next;
			}
			if(x)
				*x='(';
			if(pmfunc&&pmfunc->def)
				return pmfunc->def;
		}
	}else{
		pfunc=funchash[i];
		while(pfunc){
			if(!strcmp(pfunc->name,str))
				break;
			pfunc=pfunc->next;
		}
		if(x)
			*x='(';
		if(pfunc&&pfunc->def)
			return pfunc->def;
	}

	return NULL;
}

_PMATRIX
matrix(str)
	char	*str;
{
	int	i;
	PMATRIX	pmatrix;

	i=gethash(str);
	pmatrix=matrixhash[i];
	while(pmatrix){
		if(!strcmp(pmatrix->name,str))
			return pmatrix->matrix;
		pmatrix=pmatrix->next;
	}

	return 0;
}

double
matrixelement(str)
	char	*str;
{
	int		i,j,p;
	char		*x,*y,*z;
	_PMATRIX	_pmatrix;

	*(x=z=strchr(str,'['))=0;

	p=0;
	for(y=x+1;*y;y++){
	    	switch(*y){
		    	case '[':
			case '(':
			    	p++;
				break;
			case ']':
				if(!(p--)){
					*y=0;
					j=exprs(x);
					*y=']';
				}
				break;
			case ')':
				p--;
				break;
			case ',':
			    	if(!p){
					*y=0;
				    	i=exprs(x+1);
					*y=',';
					x=y+1;
				}
				break;
		}
		if(p<0)
			break;
	}
	pcur=y+1;

	if(skip_result)
		return 0;

	if(!(_pmatrix=matrix(str))){
		snprintf(tmpbuf,TMPBUFSIZE,"\n%s: No such matrix\n",str);
		*z='[';
		error();
	}

	if(!(i>0&&i<_pmatrix->m+1&&j>0&&j<_pmatrix->n+1)){
		snprintf(tmpbuf,TMPBUFSIZE,"\n%s[%d,%d]: No such element\n",
				str,i,j);
		*z='[';
		error();
	}

	*z='[';
	return _element(_pmatrix,i,j);
}

char *
str(str)
	char	*str;
{
	int	i;
	PSTR	pstr;

	i=gethash(str);
	pstr=strhash[i];
	while(pstr){
		if(!strcmp(pstr->name,str))
			return pstr->str;
		pstr=pstr->next;
	}

	return 0;
}

int
isprsvd(str)
    	char	*str;
{
	int	i;
	PPRSVD	pprsvd;
	char	*p;

	if((p=strchr(str,'(')))
		*p=0;

	i=gethash(str);
	pprsvd=prsvdhash[i];

	while(pprsvd){
		if(!strcmp(pprsvd->name,str)){
			if(p)
				*p='(';
		    	return 1;
		}
		pprsvd=pprsvd->next;
	}
	if(p)
		*p='(';
	return 0;
}

int
ismxfunc(str)
	char	*str;
{
	int	i;
	PMXFUNC	pmxfunc;

	i=gethash(str);
	pmxfunc=mxfunchash[i];
	while(pmxfunc){
		if(!strcmp(pmxfunc->name,str))
			return 1;
		pmxfunc=pmxfunc->next;
	}
	return 0;
}

int
issysvar(str)
	char	*str;
{
	int	i,n;

	n=sizeof(sysvars)/sizeof(SYSVAR);
	for(i=0;i<n;i++){
		if(!strcmp(str,sysvars[i].name))
			return 1;
	}

	return 0;
}

int
issysvar2(str)
	char	*str;
{
	int	i,n;

	n=sizeof(sysvars2)/sizeof(char *);
	for(i=0;i<n;i++){
		if(!strcmp(str,sysvars2[i]))
			return 1;
	}

	return 0;
}

