#define	_READLINE_C_
#include "mafi.h"

int
scanline(void)
{
    	int	i,j,k,l,cont,in,n;
	char	ch,ch2,ch3,ifelse,tmp[8],*v,*w,*x,*y,*z,**lbuffer,*str;

	str=0;
	ifelse=0;
	do{
		if(str)
			free(str);
		lbuffer=0;
		in=n=0;
		i=LINESIZE;
		do{
			j=k=cont=0;
			while((ch=getchar())!=';'||(k&1)||cont||ifelse){
				ifelse=0;
		    		if(i>=LINESIZE){
				    	if(!n)
						i=0;
					else
				    		z[i]=0;
					in+=i;
					i=0;
					if(!lbuffer)
						lbuffer=(char **)malloc((n+1)*sizeof(char *));
					else
						lbuffer=(char **)realloc(lbuffer,(n+1)*sizeof(char *));
					if(!lbuffer)
						return -2;

					z=lbuffer[n]=(char *)malloc(LINESIZE);
					if(!z){
						if(lbuffer){
							for(i=0;i<n;i++)
								free(lbuffer[i]);
							free(lbuffer);
						}
						return -2;
					}
					n++;
				}
				if(!ch||ch==EOF){
					if(lbuffer){
					    	for(i=0;i<n;i++)
						    	free(lbuffer[i]);
						free(lbuffer);
					}
					return -1;
				}else if(ch=='"')
					k++;
				else if(ch=='\\'&&(k&1)){
					l=i;
					ch=getchar();
					switch(ch){
						case '\\':
						case '"':
						case '$':
							z[i++]='\\';
							z[i++]=ch;
							break;
						case 't':
							z[i++]='\t';
							break;
						case 'n':
						case 'r':
							z[i++]='\n';
							break;
						case 'a':
							z[i++]='\a';
							break;
						case 'b':
							z[i++]='\b';
							break;
						default:
							z[i++]=ch;
							break;
					}
					if(l!=i)
						continue;
					i++;
				}else if(ch=='/'&&!(k&1)){
					ch=getchar();
					switch(ch){
						case '/':
							while(getchar()!='\n');
							continue;
							break;
						case '*':
							j++;
							ch=getchar();
							for(;;){
								switch(ch){
									case 0:
										if(lbuffer){
											for(i=0;i<n;i++)
												free(lbuffer[i]);
											free(lbuffer);
										}
										return -1;
										break;
									case '/':
										if((ch=getchar())=='*'){
											ch=getchar();
											j++;
										}
										break;
									case '*':
										if((ch=getchar())=='/'){
											ch=getchar();
											j--;
										}
										break;
									default:
										ch=getchar();
										continue;
										break;
								}
								if(!j){
									ungetc(ch,stdin);
									break;
								}
							}
							continue;
							break;
						default:
							z[i++]='/';
							break;
					}
				}

				if((ch=='('||ch=='{'||ch=='[')&&!(k&1))
				    	cont++;
				else if((ch==')'||ch=='}'||ch==']')&&!(k&1))
				    	cont--;
		
				if(!strchr("\t\n ",ch)||(k&1))
					z[i++]=ch;
				if(ch=='}'&&!(k&1)&&!cont)
					break;
			}
			if(!tty){
				for(;(ch2=getchar())&&strchr(";\t\n ",ch2););
				ungetc(ch2,stdin);
				for(j=0;j<4;j++){
					for(;(ch3=getchar())&&strchr("\t\n ",ch3););
					if(!ch3)
						break;
					tmp[j]=ch3;
				}
				tmp[j]=0;
				for(;--j>=0;)
					ungetc(tmp[j],stdin);
				if(!strcmp(tmp,"else"))
					ifelse=1;
				if(ifelse&&ch==';')
					ungetc(ch,stdin);
			}
		}while(ifelse);
		z[i]=0;
		in+=i;

		if(!in || !lbuffer){
			if(lbuffer){
				for(i=0;i<n;i++)
					free(lbuffer[i]);
				free(lbuffer);
			}
			y=z+i;
			continue;
		}

		w=y=str=(char *)malloc(in+1);
		if(!w){
			if(lbuffer){
				for(i=0;i<n;i++)
					free(lbuffer[i]);
				free(lbuffer);
			}
			return -2;
		}
		for(i=0;i<n;i++){
		    	strcpy(str+i*LINESIZE,lbuffer[i]);
			free(lbuffer[i]);
		}
		free(lbuffer);

		if(!(*str==':'&&!inlineno)){
			for(;;){
				x=strchr(str,':');
				y=strchr(str,'"');
				if(x&&(!y||y>x)){
					if(x==str)
						str++;
					else{
					    	*x=0;
						for(y=str;*y;y++){
							if(isdelimiter(*y))
								break;
						}
						if(!*y){
							setlabel(str,inlineno);
							str=y=x+1;
							*x=':';
						}else{
							*x=':';
							break;
						}
					}
				}else{
					y=str;
					break;
				}
			}
			if(str!=w){
				v=(char *)malloc(strlen(str)+1);
				if(!v){
					free(w);
					return -2;
				}
				strcpy(v,str);
				y=str=v;
				free(w);
			}
		}
	}while(!*str);

	if(!buffer)
		buffer=(char **)malloc((inlineno+1)*sizeof(char *));
	else
		buffer=(char **)realloc(buffer,(inlineno+1)*sizeof(char *));
	if(!buffer){
		free(str);
		return -2;
	}
	buffer[inlineno]=str;

	return strlen(buffer[inlineno++]);
}

void
freebuffer(void)
{
    	int	i;

	if(buffer){
		for(i=0;i<inlineno;i++)
			free(buffer[i]);
		free(buffer);
		buffer=0;
	}
	inlineno=dolineno=0;

	return;
}
