#ifndef	__3D_H_
#define	__3D_H_

#define _view(v,p)	_dx_view(v,p)
//#define _view(v,p)	_pm_view(PM,v,p)

struct _point{
	double x,y,z;
};

struct _viewport{
	double x0,y0,/*sxr,exr,syr,eyr,*/rho,theta,phi,d;
};

struct _transform{
	struct _matrix *Rx,*Ry,*Rz,*M,*S;
};

/* complete */
struct _transform *_empty_transform(void);
void _free_transform(struct _transform *transform);
struct _point *_empty_point(void);
struct _point *_new_point(double x,double y,double z);
struct _point *_equal_point(struct _point *point);
struct _point *_matrix2point(struct _matrix *matrix);
struct _matrix *_point2matrix(struct _point *point);
struct _point *_rotate_x(struct _point *point,double theta);
struct _point *_rotate_y(struct _point *point,double theta);
struct _point *_rotate_z(struct _point *point,double theta);
struct _point *_move(struct _point *point,double dx,double dy,double dz);
struct _point *_scalar(struct _point *point,double k);
void _free_point(struct _point *point);
void _Rx(struct _transform *transform,double theta);
void _Ry(struct _transform *transform,double theta);
void _Rz(struct _transform *transform,double theta);
void _Mi(struct _transform *transform,double dx,double dy,double dz);
void _Si(struct _transform *transform,double k);
/***********/

struct _matrix *_new_project_matrix(struct _viewport *viewport);
struct _point *_pm_view(struct _matrix *project_matrix,
			struct _viewport *viewport,struct _point *point);
struct _point *_dx_view(struct _viewport *viewport,struct _point *point);

#endif
