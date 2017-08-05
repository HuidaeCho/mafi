#define	__3D_C_
#include "mafi.h"

#ifdef	USE_GD
extern	char	dxfirst;

struct _point *_empty_point(void)
{
	struct _point *point;
	
	point=(struct _point *)malloc(sizeof(struct _point));
	return(point);
}

struct _point *_new_point(x,y,z)
double x,y,z;
{
	struct _point *point;
	
	point=(struct _point *)malloc(sizeof(struct _point));
	point->x=x;
	point->y=y;
	point->z=z;
	return(point);
}

struct _point *_equal_point(point)
struct _point *point;
{
	struct _point *new_point;
	
	new_point=(struct _point *)malloc(sizeof(struct _point));
	new_point->x=point->x;
	new_point->y=point->y;
	new_point->z=point->z;
	return(new_point);
}

struct _point *_matrix2point(matrix)
struct _matrix *matrix;
{
	struct _point *new_point;
	
	new_point=_new_point(_element(matrix,1,1),
			     _element(matrix,2,1),
			     _element(matrix,3,1));
	return(new_point);
}

struct _matrix *_point2matrix(point)
struct _point *point;
{
	struct _matrix *new_matrix;

	new_matrix=_new_matrix(4,1,1);
	_element(new_matrix,1,1)=point->x;
	_element(new_matrix,2,1)=point->y;
	_element(new_matrix,3,1)=point->z;
	return(new_matrix);
}

struct _point *_rotate_x(point,theta)
struct _point *point;
double theta;
{
	struct _point *new_point;
	struct _matrix *A,*B;
	struct _transform *tmp;
	
	tmp=_empty_transform();
	_element(tmp->Rx,2,2)=_element(tmp->Rx,3,3)=cos(theta);
	_element(tmp->Rx,2,3)=-(_element(tmp->Rx,3,2)=sin(theta));
	
	A=_new_matrix(4,1,1);
	_element(A,1,1)=point->x;
	_element(A,2,1)=point->y;	
	_element(A,3,1)=point->z;
	
	B=_multiply_matrix(tmp->Rx,A,0);
	_free_matrix(A);
	new_point=_new_point(_element(B,1,1),
			     _element(B,2,1),
			     _element(B,3,1));
	_free_matrix(B);
	_free_transform(tmp);
	return(new_point);
}

struct _point *_rotate_y(point,theta)
struct _point *point;
double theta;
{
	struct _point *new_point;
	struct _matrix *A,*B;
	struct _transform *tmp;
	
	tmp=_empty_transform();
	
	_element(tmp->Ry,1,1)=_element(tmp->Ry,3,3)=cos(theta);
	_element(tmp->Ry,1,3)=-(_element(tmp->Ry,3,1)=-sin(theta));
	
	A=_new_matrix(4,1,1);
	_element(A,1,1)=point->x;
	_element(A,2,1)=point->y;	
	_element(A,3,1)=point->z;
	
	B=_multiply_matrix(tmp->Ry,A,0);
	_free_matrix(A);
	new_point=_new_point(_element(B,1,1),
			     _element(B,2,1),
			     _element(B,3,1));
	_free_matrix(B);
	_free_transform(tmp);
	return(new_point);
}

struct _point *_rotate_z(point,theta)
struct _point *point;
double theta;
{
	struct _point *new_point;
	struct _matrix *A,*B;
	struct _transform *tmp;
	
	tmp=_empty_transform();
	
	_element(tmp->Rz,1,1)=_element(tmp->Rz,2,2)=cos(theta);
	_element(tmp->Rz,1,2)=-(_element(tmp->Rz,2,1)=sin(theta));
	
	A=_new_matrix(4,1,1);
	_element(A,1,1)=point->x;
	_element(A,2,1)=point->y;	
	_element(A,3,1)=point->z;
	
	B=_multiply_matrix(tmp->Rz,A,0);
	_free_matrix(A);
	new_point=_new_point(_element(B,1,1),
			     _element(B,2,1),
			     _element(B,3,1));
	_free_matrix(B);
	_free_transform(tmp);
	return(new_point);
}

struct _point *_move(point,dx,dy,dz)
struct _point *point;
double dx,dy,dz;
{
	struct _point *new_point;
	struct _transform *tmp;
	
	tmp=_empty_transform();
	
	_element(tmp->M,1,4)=dx;
	_element(tmp->M,2,4)=dy;
	_element(tmp->M,3,4)=dz;
	
	new_point=_new_point(point->x+dx,point->y+dy,point->z+dz);
	_free_transform(tmp);
	return(new_point);
}

struct _point *_scalar(point,k)
struct _point *point;
double k;
{
	struct _point *new_point;
	struct _transform *tmp;
	
	tmp=_empty_transform();
	
	_element(tmp->S,1,1)=
	_element(tmp->S,2,2)=
	_element(tmp->S,3,3)=
	_element(tmp->S,4,4)=k;
	
	new_point=_new_point(k*point->x,k*point->y,k*point->z);
	_free_transform(tmp);
	return(new_point);
}

void _free_point(point)
struct _point *point;
{
	free(point);
}

void _Rx(transform,theta)
struct _transform *transform;
double theta;
{
	_element(transform->Rx,2,2)=_element(transform->Rx,3,3)=cos(theta);
	_element(transform->Rx,2,3)=-(_element(transform->Rx,3,2)=sin(theta));
}

void _Ry(transform,theta)
struct _transform *transform;
double theta;
{
	_element(transform->Ry,1,1)=_element(transform->Ry,3,3)=cos(theta);
	_element(transform->Ry,1,3)=-(_element(transform->Ry,3,1)=-sin(theta));
}

void _Rz(transform,theta)
struct _transform *transform;
double theta;
{
	_element(transform->Rz,1,1)=_element(transform->Rz,2,2)=cos(theta);
	_element(transform->Rz,1,2)=-(_element(transform->Rz,2,1)=sin(theta));
}

void _Mi(transform,dx,dy,dz)
struct _transform *transform;
double dx,dy,dz;
{
	_element(transform->M,1,4)=dx;
	_element(transform->M,2,4)=dy;
	_element(transform->M,3,4)=dz;
}

void _Si(transform,k)
struct _transform *transform;
double k;
{
	_element(transform->S,1,1)=
	_element(transform->S,2,2)=
	_element(transform->S,3,3)=
	_element(transform->S,4,4)=k;
}

/*****************/
struct _point *_dx_view(viewport,point)
struct _viewport *viewport;
struct _point *point;
{
#if 1
#ifndef	SLOW3D
	double	x,y,z;
#else
	struct _point *new_point;
	double z;
#endif
	static double cp,sp,ct,st,ctcp,ctsp,stcp,stsp;

	if(dxfirst){
		cp=cos(viewport->phi);
		sp=sin(viewport->phi);
		ct=cos(viewport->theta);
		st=sin(viewport->theta);
		ctcp=ct*cp;
		ctsp=ct*sp;
		stcp=st*cp;
		stsp=st*sp;
		dxfirst=0;
	}

	z=viewport->d/(-ctsp*point->x-stsp*point->y-cp*point->z+viewport->rho);
#ifndef	SLOW3D
	x=z*(-st*point->x+ct*point->y)+viewport->x0;
	y=-z*(-ctcp*point->x-stcp*point->y+sp*point->z)+viewport->y0;
	z=viewport->d;

	point->x=x;
	point->y=y;
	point->z=z;

	return(point);
#else
	new_point=_new_point(z*(-st*point->x+ct*point->y)+viewport->x0,
			     -z*(-ctcp*point->x-stcp*point->y+sp*point->z)
			     +viewport->y0,
			     viewport->d);
	return(new_point);
#endif
#else
	struct _point *new_point;
	double z;

	z=viewport->d/
		(-cos(viewport->theta)*sin(viewport->phi)*point->x
		 -sin(viewport->theta)*sin(viewport->phi)*point->y
		 -cos(viewport->phi)*point->z+viewport->rho);
	new_point=_new_point(z*(-sin(viewport->theta)*point->x
			+cos(viewport->theta)*point->y)+viewport->x0,
			-z*(-cos(viewport->theta)*cos(viewport->phi)
			*point->x-sin(viewport->theta)*cos(viewport->phi)
			*point->y+sin(viewport->phi)*point->z)+viewport->y0,
			viewport->d);
	return(new_point);
#endif
}
/*****************/
struct _matrix *_new_project_matrix(viewport)
struct _viewport *viewport;
{
	struct _matrix *A,*B;
	struct _transform *tmp;
	
	tmp=_empty_transform();
	_Mi(tmp,-viewport->rho*cos(viewport->theta)*sin(viewport->phi),
	       -viewport->rho*sin(viewport->theta)*sin(viewport->phi),
	       -viewport->rho*cos(viewport->phi));
	_Rz(tmp,-M_PI/2-viewport->theta);
	_Rx(tmp,-viewport->phi);
	A=_multiply_matrix(tmp->Rz,tmp->M,0);
	B=_multiply_matrix(tmp->Rx,A,0);
	_free_matrix(A);
	_free_transform(tmp);
	return(B);
}

struct _transform *_empty_transform(void)
{
	struct _transform *transform;
	
	transform=(struct _transform *)malloc(sizeof(struct _transform));
	
	transform->Rx=_unity_matrix(4);
	transform->Ry=_unity_matrix(4);
	transform->Rz=_unity_matrix(4);
	transform->M=_unity_matrix(4);
	transform->S=_unity_matrix(4);
	return(transform);
}

void _free_transform(transform)
struct _transform *transform;
{
	free(transform->Rx);
	free(transform->Ry);
	free(transform->Rz);
	free(transform->M);
	free(transform->S);
	free(transform);
}

struct _point *_pm_view(project_matrix,viewport,point)
struct _matrix *project_matrix;
struct _viewport *viewport;
struct _point *point;
{
	struct _point *new_point;
	struct _matrix *A,*B;
	
	A=_new_matrix(4,1,1);
	_element(A,1,1)=point->x;
	_element(A,2,1)=point->y;	
	_element(A,3,1)=point->z;
	
	B=_multiply_matrix(project_matrix,A,0);
	_free_matrix(A);
	new_point=_new_point(-_element(B,1,1)*viewport->d/_element(B,3,1)
			     +viewport->x0,
			     _element(B,2,1)*viewport->d/_element(B,3,1)
			     +viewport->y0,
			     viewport->d);
	_free_matrix(B);
	return(new_point);
}
#endif
