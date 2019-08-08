#include "math.h"
#include "src/rtos/utils_common/include/utils_mem.h"
//#include "vxlib.h"
//#include "vlib.h"
//#include "vxlib_cn.h"
#include "dsplib.h"
#include "sparse_optical_flow_dsp.h"

/**************************************************  svd part ********************************************************/

#pragma CODE_SECTION(DSPF_sp_svd, ".text:optimized");
#pragma CODE_SECTION(DSPF_sp_convert_to_bidiag_diy, ".text:optimized");
#pragma CODE_SECTION(DSPF_sp_bidiag_to_diag_diy, ".text:optimized");
#pragma CODE_SECTION(DSPF_sp_sort_singular_values_diy, ".text:optimized");

#define ENABLE_NR1  1
#define ENABLE_NR2  1

static int DSPF_sp_convert_to_bidiag_diy(const int Nrows,const int Ncols,float *restrict U,float *restrict V,float *restrict diag,float *restrict superdiag);
static int DSPF_sp_bidiag_to_diag_diy(const int Nrows,const int Ncols,float *restrict U,float *restrict V,float *restrict diag,float *restrict superdiag);
static int DSPF_sp_sort_singular_values_diy(const int Nrows,const int Ncols,float *restrict U,float *restrict V,float *restrict singular_values);


int DSPF_sp_svd_diy(const int Nrows,const int Ncols,float *restrict A,float *restrict U,float *restrict V,float *restrict U1,float *restrict diag,float *restrict superdiag)
{
  int row,col,Nrows1,Ncols1,status;

  _nassert(Nrows>0);
  _nassert(Ncols>0);
  _nassert((int)A % 8 == 0);
  _nassert((int)U % 8 == 0);
  _nassert((int)V % 8 == 0);
  _nassert((int)U1 % 8 == 0);
  _nassert((int)diag % 8 == 0);
  _nassert((int)superdiag % 8 == 0);

  /* ------------------------------------------------------------------- */
  /* copy A matrix to U                                                  */
  /* ------------------------------------------------------------------- */
  if (Nrows>=Ncols) {
	Nrows1=Nrows;
	Ncols1=Ncols;
    memcpy(U,A,sizeof(float)*Nrows*Ncols);
  } else {
	/* transpose matrix */
#pragma MUST_ITERATE(1,,)
	for (row=0;row<Nrows;row++) {
#pragma MUST_ITERATE(1,,)
      for (col=0;col<Ncols;col++) {
    	U[row+col*Nrows]=A[col+row*Ncols];
      }
	}
	Nrows1=Ncols;
	Ncols1=Nrows;

    /*printf("check A after trans\n");
    int32_t i;
    for(i = 0; i < Nrows; i++){
      int32_t j;
      for(j = 0; j < Ncols; j++){
          printf("%f| ", *(A + i*Ncols1 + j));
      }
      printf("\n");
    }
    printf("checked\n");

    printf("check U after trans\n");
    for(i = 0; i < Nrows1; i++){
      int32_t j;
      for(j = 0; j < Ncols1; j++){
          printf("%f| ", *(U + i*Ncols1 + j));
      }
      printf("\n");
    }
    printf("checked\n");*/
  }
      
  /*printf("check A after into func\n");
  int32_t i;
  for(i = 0; i < Nrows; i++){
      int32_t j;
      for(j = 0; j < Ncols; j++){
          printf("%f| ", *(A + i*Ncols + j));
      }
      printf("\n");
  }
  printf("checked\n");*/

  /* ------------------------------------------------------------------- */
  /* convert A to bidiagonal matrix using Householder reflections        */
  /* ------------------------------------------------------------------- */
  DSPF_sp_convert_to_bidiag_diy(Nrows1,Ncols1,U,V,diag,superdiag);
  /*printf("check V after 1st step\n");
      for(i = 0; i < Nrows1; i++){
          int32_t j;
          for(j = 0; j < Ncols1; j++){
              printf("%f| ", *(V + i*Ncols1 + j));
          }
      printf("\n");
  } 
  printf("checked\n");*/

  /* ------------------------------------------------------------------- */
  /* convert bidiagonal to diagonal using Givens rotations               */
  /* ------------------------------------------------------------------- */
  status=DSPF_sp_bidiag_to_diag_diy(Nrows1,Ncols1,U,V,diag,superdiag);
  /*printf("check V after 2nd step\n");
      for(i = 0; i < Nrows1; i++){
          int32_t j;
          for(j = 0; j < Ncols1; j++){
              printf("%f| ", *(V + i*Ncols1 + j));
          }
      printf("\n");
  } 
  printf("checked\n");*/


  /* ------------------------------------------------------------------- */
  /* sort singular values in descending order                            */
  /* ------------------------------------------------------------------- */
  DSPF_sp_sort_singular_values_diy(Nrows1,Ncols1,U,V,diag);
  /*printf("check V after 3rd step\n");
  for(i = 0; i < Nrows; i++){
      int32_t j;
      for(j = 0; j < Nrows; j++){
          printf("%f| ", *(V + i*Nrows + j));
      }
      printf("\n");
  } 
  printf("checked\n");
  printf("check U after 3rd step\n");
      for(i = 0; i < Ncols; i++){
          int32_t j;
          for(j = 0; j < Ncols; j++){
              printf("%f| ", *(U + i*Ncols + j));
          }
      printf("\n");
  } 
  printf("checked\n");*/



  /* ------------------------------------------------------------------- */
  /* switch U and V                                                      */
  /* ------------------------------------------------------------------- */
  if (Ncols>Nrows) {
#ifndef ENABLE_REDUCED_FORM
    //printf("full form\n");
    //printf("nrows, ncols are : %d, %d\n", Nrows, Ncols);
	memcpy(U1,V,sizeof(float)*Nrows*Nrows);
    memset(V, 0.0, sizeof(float)*Ncols*Ncols);

    /*printf("check U after memcp V before memcp U\n");
    for(i = 0; i < Ncols; i++){
        int32_t j;
        for(j = 0; j < Ncols; j++){
            printf("%f| ", *(U + i*Ncols + j));
        }
    printf("\n");
    } 
    printf("checked\n");

    printf("check V after memset\n");
      for(i = 0; i < Ncols; i++){
          int32_t j;
          for(j = 0; j < Ncols; j++){
              printf("%f| ", *(V + i*Ncols + j));
          }
      printf("\n");
    } 
    printf("checked\n");*/

	memcpy(V,U,sizeof(float)*Ncols*Ncols);
    /*for(i = 0; i < Ncols; i++){
          int32_t j;
          for(j = 0; j < Ncols; j++){
              *(V + i*Ncols + j) = *(U + i*Ncols + j);
          }
    } 

    printf("check V after memcp\n");
      for(i = 0; i < Ncols; i++){
          int32_t j;
          for(j = 0; j < Ncols; j++){
              printf("%f| ", *(V + i*Ncols + j));
          }
      printf("\n");
    } 
    printf("checked\n");*/
	memcpy(U,U1,sizeof(float)*Nrows*Nrows);
#else
	memcpy(U1,V,sizeof(float)*Ncols*Ncols);
	memcpy(V,U,sizeof(float)*Nrows*Ncols);
	memcpy(U,U1,sizeof(float)*Nrows*Ncols);
#endif
  }

  return status;
}

static int DSPF_sp_convert_to_bidiag_diy(const int Nrows,const int Ncols,float *restrict U,float *restrict V,float *restrict diag,float *restrict superdiag)
{
  int i,j,k;
  float s,s2,si,scale,half_norm_squared;
  float x,y,inv_s;

  _nassert(Nrows>0);
  _nassert(Ncols>0);
  _nassert((int)U % 8 == 0);
  _nassert((int)V % 8 == 0);
  _nassert((int)diag % 8 == 0);
  _nassert((int)superdiag % 8 == 0);

  /* Householder processing */
  s=0;
  scale=0;
  for (i=0;i<Ncols;i++) {
	superdiag[i]=scale*s;
	/* process columns */
	scale=0;
#pragma MUST_ITERATE(1,,)
	for (j=i;j<Nrows;j++) {
	  scale+=fabs(U[i+j*Ncols]);
	}
	if (scale>0) {
	  s2=0;
#ifndef ENABLE_NR1
#pragma MUST_ITERATE(1,,)
	  for (j=i;j<Nrows;j++) {
		U[i+j*Ncols]=U[i+j*Ncols]/scale;
#else
	  x=scale;
      y=_rcpsp(x);
      y=y*(2.0-x*y);
      y=y*(2.0-x*y);
#pragma MUST_ITERATE(1,,)
  	  for (j=i;j<Nrows;j++) {
  		U[i+j*Ncols]=U[i+j*Ncols]*y;
#endif
		s2+=U[i+j*Ncols]*U[i+j*Ncols];
	  }
	  if (U[i+i*Ncols]<0) {
		s=sqrt(s2);
	  } else {
		s=-sqrt(s2);
	  }
	  half_norm_squared=U[i+i*Ncols]*s-s2;
#ifdef ENABLE_NR1
	  x=half_norm_squared;
      y=_rcpsp(x);
      y=y*(2.0-x*y);
      y=y*(2.0-x*y);
#endif
	  U[i+i*Ncols]-=s;
	  for (j=i+1;j<Ncols;j++) {
		si=0;
#pragma MUST_ITERATE(1,,)
		for (k=i;k<Nrows;k++) {
		  si+=U[i+k*Ncols]*U[j+k*Ncols];
		}
#ifndef ENABLE_NR1
		si=si/half_norm_squared;
#else
		si=si*y;
#endif
#pragma MUST_ITERATE(1,,)
		for (k=i;k<Nrows;k++) {
		  U[j+k*Ncols]+=si*U[i+k*Ncols];
		}
	  }
	} /* if (scale>0) */
#pragma MUST_ITERATE(1,,)
	for (j=i;j<Nrows;j++) {
	  U[i+j*Ncols]*=scale;
	}
	diag[i]=s*scale;
	/* process rows */
    s=0;
    scale=0;
    if ((i<Nrows)&&(i!=Ncols-1)) {
      for (j=i+1;j<Ncols;j++) {
    	scale+=fabs(U[j+i*Ncols]);
      }
      if (scale>0) {
        s2=0;
#ifdef ENABLE_NR1
        x=scale;
        y=_rcpsp(x);
        y=y*(2.0-x*y);
        y=y*(2.0-x*y);
#endif
        for (j=i+1;j<Ncols;j++) {
#ifndef ENABLE_NR1
          U[j+i*Ncols]=U[j+i*Ncols]/scale;
#else
          U[j+i*Ncols]=U[j+i*Ncols]*y;
#endif
          s2+=U[j+i*Ncols]*U[j+i*Ncols];
        }
        j--;
        if (U[j+i*Ncols]<0) {
          s=sqrt(s2);
        } else {
          s=-sqrt(s2);
        }
        half_norm_squared=U[i+1+i*Ncols]*s-s2;
#ifdef ENABLE_NR1
        x=half_norm_squared;
        y=_rcpsp(x);
        y=y*(2.0-x*y);
        y=y*(2.0-x*y);
#endif
        U[i+1+i*Ncols]-=s;
#pragma MUST_ITERATE(1,,)
        for (k=i+1;k<Ncols;k++) {
#ifndef ENABLE_NR1
          superdiag[k]=U[k+i*Ncols]/half_norm_squared;
#else
          superdiag[k]=U[k+i*Ncols]*y;
#endif
        }
        if (i<Nrows-1) {
          for (j=i+1;j<Nrows;j++) {
        	si=0;
#pragma MUST_ITERATE(1,,)
        	for (k=i+1;k<Ncols;k++) {
        	  si+=U[k+i*Ncols]*U[k+j*Ncols];
        	}
#pragma MUST_ITERATE(1,,)
        	for (k=i+1;k<Ncols;k++) {
        	  U[k+j*Ncols]+=si*superdiag[k];
        	}
          }
        }
      }	/* if (scale>0) */
#pragma MUST_ITERATE(1,,)
      for (k=i+1;k<Ncols;k++) {
    	U[k+i*Ncols]*=scale;
      }
    } /* if ((i<Nrows)&&(i!=Ncols-1)) */
  } /* for (i=0;i<Ncols;i++) */

  /* update V */
  V[Ncols*Ncols-1]=1;
  s=superdiag[Ncols-1];
  for (i=Ncols-2;i>=0;i--) {
	if (s!=0) {
#ifdef ENABLE_NR1
      x=U[i+1+i*Ncols]*s;
      y=_rcpsp(x);
      y=y*(2.0-x*y);
      y=y*(2.0-x*y);
#endif
#pragma MUST_ITERATE(1,,)
	  for (j=i+1;j<Ncols;j++) {
#ifndef ENABLE_NR1
		V[i+j*Ncols]=U[j+i*Ncols]/(U[i+1+i*Ncols]*s);
#else
		V[i+j*Ncols]=U[j+i*Ncols]*y;
#endif
	  }
	  for (j=i+1;j<Ncols;j++) {
	    si=0;
#pragma MUST_ITERATE(1,,)
	    for (k=i+1;k<Ncols;k++) {
	      si+=U[k+i*Ncols]*V[j+k*Ncols];
	    }
#pragma MUST_ITERATE(1,,)
	    for (k=i+1;k<Ncols;k++) {
	      V[j+k*Ncols]+=si*V[i+k*Ncols];
	    }
	  }
	} /* if (s!=0) */
#pragma MUST_ITERATE(1,,)
	for (j=i+1;j<Ncols;j++) {
	  V[j+i*Ncols]=0;
	  V[i+j*Ncols]=0;
	}
	V[i+i*Ncols]=1;
	s=superdiag[i];
  } /* for (i=Ncols-2;i>=0;i--) */

#ifndef ENABLE_REDUCED_FORM
  /* expand U to from Nrows x Ncols to */
  /*                  Nrows x Nrows    */
  if (Nrows>Ncols) {
	for (i=Nrows-1;i>=0;i--) {
	  for (j=Nrows-1;j>=0;j--) {
		 if (j<=Ncols-1) {
		   U[j+i*Nrows]=U[j+i*Ncols];
		 } else {
			 U[j+i*Nrows]=0;
		 }
	  }
	}
  }

  /* update U */
  for (i=Ncols-1;i>=0;i--) {
	s=diag[i];
#ifdef ENABLE_NR1
	x=s;
    y=_rcpsp(x);
    y=y*(2.0-x*y);
    y=y*(2.0-x*y);
    inv_s=y;
#endif
#pragma MUST_ITERATE(1,,)
	for (j=i+1;j<Ncols;j++) {
	  U[j+i*Nrows]=0;
	}
    if (s!=0) {
      for (j=i+1;j<Nrows;j++) {
    	si=0;
#pragma MUST_ITERATE(1,,)
    	for (k=i+1;k<Nrows;k++) {
    	  si+=U[i+k*Nrows]*U[j+k*Nrows];
    	}
    	si=si/(U[i+i*Nrows]*s);
#pragma MUST_ITERATE(1,,)
    	for (k=i;k<Nrows;k++) {
    	  U[j+k*Nrows]+=si*U[i+k*Nrows];
    	}
      }
      /* initial U1 */
      if (i==Ncols-1) {
#ifdef ENABLE_NR1
    	x=U[i+i*Nrows]*s;
        y=_rcpsp(x);
        y=y*(2.0-x*y);
        y=y*(2.0-x*y);
#endif
    	for (j=i;j<Nrows;j++) {
    	  for (k=Nrows-1;k>=i+1;k--) {
#ifndef ENABLE_NR1
    	    U[k+j*Nrows]=U[i+j*Nrows]*U[i+k*Nrows]/(U[i+i*Nrows]*s);
#else
    	    U[k+j*Nrows]=U[i+j*Nrows]*U[i+k*Nrows]*y;
#endif
    	    if (j==k) {
    	      U[k+j*Nrows]+=1;
    	    }
    	  }
    	}
      }
#pragma MUST_ITERATE(1,,)
      for (j=i;j<Nrows;j++) {
#ifndef ENABLE_NR1
    	U[i+j*Nrows]=U[i+j*Nrows]/s;
#else
    	U[i+j*Nrows]=U[i+j*Nrows]*inv_s;
#endif
      }
    } else { /* if (s!=0) */
      if (i==Ncols-1) {
 #pragma MUST_ITERATE(1,,)
       	for (k=1;k<=Nrows-Ncols;k++) {
       	  U[i+k+(i+k)*Nrows]=1;
       	}
      }
#pragma MUST_ITERATE(1,,)
      for (j=i;j<Nrows;j++) {
    	U[i+j*Nrows]=0;
      }
    } /* if (s!=0) */
    U[i+i*Nrows]+=1;
  } /* for (i=Ncols-1;i>=0;i--) */
#else /* #ifndef ENABLE_REDUCED_FORM */
  /* update U */
  for (i=Ncols-1;i>=0;i--) {
	s=diag[i];
#ifdef ENABLE_NR1
	x=s;
    y=_rcpsp(x);
    y=y*(2.0-x*y);
    y=y*(2.0-x*y);
    inv_s=y;
#endif
#pragma MUST_ITERATE(1,,)
	for (j=i+1;j<Ncols;j++) {
	  U[j+i*Ncols]=0;
	}
    if (s!=0) {
      for (j=i+1;j<Ncols;j++) {
    	si=0;
#pragma MUST_ITERATE(1,,)
    	for (k=i+1;k<Nrows;k++) {
    	  si+=U[i+k*Ncols]*U[j+k*Ncols];
    	}
    	si=si/(U[i+i*Ncols]*s);
#pragma MUST_ITERATE(1,,)
    	for (k=i;k<Nrows;k++) {
    	  U[j+k*Ncols]+=si*U[i+k*Ncols];
    	}
      }
#pragma MUST_ITERATE(1,,)
       for (j=i;j<Nrows;j++) {
#ifndef ENABLE_NR1
    	U[i+j*Ncols]=U[i+j*Ncols]/s;
#else
    	U[i+j*Ncols]=U[i+j*Ncols]*inv_s;
#endif
      }
    } else { /* if (s!=0) */
#pragma MUST_ITERATE(1,,)
      for (j=i;j<Nrows;j++) {
    	U[i+j*Ncols]=0;
      }
    } /* if (s!=0) */
    U[i+i*Ncols]+=1;
  } /* for (i=Ncols-1;i>=0;i--) */
#endif

  return 0;
}

static int DSPF_sp_bidiag_to_diag_diy(const int Nrows,const int Ncols,float *restrict U,float *restrict V,float *restrict diag,float *restrict superdiag)
{

  int row,i,k,m,rotation_test,iter,total_iter;
  float x,y,z,epsilon;
  float c,s,f,g,h;
  float xx,yy;
  float x1,y1,z1;
  int   delta;

  _nassert(Nrows>0);
  _nassert(Ncols>0);
  _nassert((int)U % 8 == 0);
  _nassert((int)V % 8 == 0);
  _nassert((int)diag % 8 == 0);
  _nassert((int)superdiag % 8 == 0);

  iter=0;
  total_iter=0;
  /* ------------------------------------------------------------------- */
  /* find max in col                                                     */
  /* ------------------------------------------------------------------- */
  x=0;
#pragma MUST_ITERATE(1,,)
  for (i=0;i<Ncols;i++) {
	y=fabs(diag[i])+fabs(superdiag[i]);
	if (x<y) {
	  x=y;
	}
  }
  epsilon=FLT_EPSILON*x;
#pragma MUST_ITERATE(1,,)
  for (k=Ncols-1;k>=0;k--) {
	total_iter+=iter;
	iter=0;
#pragma MUST_ITERATE(1,,)
    while (1==1) {
      rotation_test=1;
#pragma MUST_ITERATE(1,,)
      for (m=k;m>=0;m--) {
    	if (fabs(superdiag[m])<=epsilon) {
    	  rotation_test=0;
    	  break;
    	}
    	if (fabs(diag[m-1])<=epsilon) {
    	  break;
    	}
      } /* for (m=k;m>=0;m--) */
      if (rotation_test) {
    	c=0;
    	s=1;
#pragma MUST_ITERATE(1,,)
    	for (i=m;i<=k;i++) {
    	  f=s*superdiag[i];
    	  superdiag[i]=c*superdiag[i];
    	  if (fabs(f)<=epsilon) {
    		break;
    	  }
    	  g=diag[i];
    	  h=sqrt(f*f+g*g);
    	  diag[i]=h;
#ifndef ENABLE_NR2
    	  c=g/h;
    	  s=-f/h;
#else
          /* divide using Newton Raphson iterations */
    	  xx=h;
          yy=_rcpsp(xx);
          yy=yy*(2.0-xx*yy);
          yy=yy*(2.0-xx*yy);
    	  c=g*yy;
          s=-f*yy;
#endif
#ifndef ENABLE_REDUCED_FORM
#pragma MUST_ITERATE(1,,)
    	  for (row=0;row<Nrows;row+=2) {
    		delta=1;
    		if (row==(Nrows-1)) delta=0;
    		y=U[m-1+row*Nrows];        y1=U[m-1+(row+delta)*Nrows];
    		z=U[i+row*Nrows];          z1=U[i+(row+delta)*Nrows];
    		U[m-1+row*Nrows]= y*c+z*s; U[m-1+(row+delta)*Nrows]= y1*c+z1*s;
    		U[i+row*Nrows]  =-y*s+z*c; U[i+(row+delta)*Nrows]  =-y1*s+z1*c;
    	  }
#else
#pragma MUST_ITERATE(1,,)
    	  for (row=0;row<Nrows;row+=2) {
    		delta=1;
    		if (row==(Nrows-1)) delta=0;
    		y=U[m-1+row*Ncols];        y1=U[m-1+(row+delta)*Ncols];
    		z=U[i+row*Ncols];          z1=U[i+(row+delta)*Ncols];
    		U[m-1+row*Ncols]= y*c+z*s; U[m-1+(row+delta)*Ncols]= y1*c+z1*s;
    		U[i+row*Ncols]  =-y*s+z*c; U[i+(row+delta)*Ncols]  =-y1*s+z1*c;
    	  }
#endif
     	} /* for (i=m;i<=k;i++) */
      } /* if (rotation_test) */
      z=diag[k];
      if (m==k) {
    	if (z<0) {
    	  diag[k]=-z;
#pragma MUST_ITERATE(1,,)
    	  for (row=0;row<Ncols;row++) {
    		V[k+row*Ncols]=-V[k+row*Ncols];
    	  }
    	} /* if (z>0) */
    	break;
      } else { /* if (m==k) */
        if (iter>=MAX_ITERATION_COUNT) {
          return -1;
        }
        iter++;
    	x=diag[m];
    	y=diag[k-1];
    	g=superdiag[k-1];
    	h=superdiag[k];
#ifndef ENABLE_NR2
    	f=((y-z)*(y+z)+(g-h)*(g+h))/(2*h*y);
#else
        /* divide using Newton Raphson iterations */
  	    xx=2*h*y;
        yy=_rcpsp(xx);
        yy=yy*(2.0-xx*yy);
        yy=yy*(2.0-xx*yy);
        f=((y-z)*(y+z)+(g-h)*(g+h))*yy;
#endif
    	g=sqrt(f*f+1);
    	if (f<0) {
    	  g=-g;
    	}
#ifndef ENABLE_NR2
    	f=((x-z)*(x+z)+h*(y/(f+g)-h))/x;
#else
        /* divide using Newton Raphson iterations */
  	    xx=x;
        yy=_rcpsp(xx);
        yy=yy*(2.0-xx*yy);
        yy=yy*(2.0-xx*yy);
        f=((x-z)*(x+z)+h*(y/(f+g)-h))*yy;
#endif
    	/* next QR transformation */
    	c=1;
    	s=1;
#pragma MUST_ITERATE(1,,)
    	for (i=m+1;i<=k;i++) {
    	  g=superdiag[i];
    	  y=diag[i];
    	  h=s*g;
    	  g=g*c;
    	  z=sqrt(f*f+h*h);
    	  superdiag[i-1]=z;
#ifndef ENABLE_NR2
    	  c=f/z;
    	  s=h/z;
#else
          /* divide using Newton Raphson iterations */
    	  xx=z;
          yy=_rcpsp(xx);
          yy=yy*(2.0-xx*yy);
          yy=yy*(2.0-xx*yy);
    	  c=f*yy;
    	  s=h*yy;
#endif
    	  f= x*c+g*s;
    	  g=-x*s+g*c;
    	  h=y*s;
    	  y=c*y;
#pragma MUST_ITERATE(1,,)
    	  for (row=0;row<Ncols;row+=2) {
    		delta=1;
    		if (row==(Ncols-1)) delta=0;
    		x=V[i-1+row*Ncols];        x1=V[i-1+(row+delta)*Ncols];
    		z=V[i+row*Ncols];          z1=V[i+(row+delta)*Ncols];
    		V[i-1+row*Ncols]= x*c+z*s; V[i-1+(row+delta)*Ncols]= x1*c+z1*s;
    		V[i+row*Ncols]  =-x*s+z*c; V[i+(row+delta)*Ncols]  =-x1*s+z1*c;
    	  }
    	  z=sqrt(f*f+h*h);
    	  diag[i-1]=z;
    	  if (z!=0) {
#ifndef ENABLE_NR2
    		c=f/z;
    		s=h/z;
#else
        	xx=z;
            yy=_rcpsp(xx);
            yy=yy*(2.0-xx*yy);
            yy=yy*(2.0-xx*yy);
    		c=f*yy;
    		s=h*yy;
#endif
    	  }
    	  f= c*g+s*y;
    	  x=-s*g+c*y;
#ifndef ENABLE_REDUCED_FORM
#pragma MUST_ITERATE(1,,)
    	  for (row=0;row<Nrows;row+=2) {
    		delta=1;
    		if (row==Nrows-1) delta=0;
    		y=U[i-1+row*Nrows];        y1=U[i-1+(row+delta)*Nrows];
    		z=U[i+row*Nrows];          z1=U[i+(row+delta)*Nrows];
    		U[i-1+row*Nrows]= c*y+s*z; U[i-1+(row+delta)*Nrows]= c*y1+s*z1;
    		U[i+row*Nrows]  =-s*y+c*z; U[i+(row+delta)*Nrows]  =-s*y1+c*z1;
    	  }
#else
    	  for (row=0;row<Nrows;row+=2) {
    		delta=1;
    		if (row==Nrows-1) delta=0;
    		y=U[i-1+row*Ncols];        y1=U[i-1+(row+delta)*Ncols];
    		z=U[i+row*Ncols];          z1=U[i+(row+delta)*Ncols];
    		U[i-1+row*Ncols]= c*y+s*z; U[i-1+(row+delta)*Ncols]= c*y1+s*z1;
    		U[i+row*Ncols]  =-s*y+c*z; U[i+(row+delta)*Ncols]  =-s*y1+c*z1;
    	  }
#endif
    	} /* for (i=m+1;i<=k;i++) */
    	superdiag[m]=0;
    	superdiag[k]=f;
    	diag[k]=x;
      } /* if (m==k) */
    } /* while (1==1) */
  } /* for (k=Ncols-1:k>=0;k--) */

  return total_iter;
}

static int DSPF_sp_sort_singular_values_diy(const int Nrows,const int Ncols,float *restrict U,float *restrict V,float *restrict singular_values)
{
  int i,j,row,max_index;
  float temp;

  _nassert(Nrows>0);
  _nassert(Ncols>0);
  _nassert((int)U % 8 == 0);
  _nassert((int)V % 8 == 0);
  _nassert((int)singular_values % 8 == 0);

#pragma MUST_ITERATE(1,,)
  for (i=0;i<Ncols-1;i++) {
	max_index=i;
#pragma MUST_ITERATE(1,,)
	for (j=i+1;j<Ncols;j++) {
	  if (singular_values[j]>singular_values[max_index]) {
		max_index=j;
	  }
	}
	if (max_index!=i) {
      temp=singular_values[i];
      singular_values[i]=singular_values[max_index];
      singular_values[max_index]=temp;
#ifndef ENABLE_REDUCED_FORM
#pragma MUST_ITERATE(1,,)
	  for (row=0;row<Nrows;row++) {
		temp=U[max_index+row*Nrows];
		U[max_index+row*Nrows]=U[i+row*Nrows];
		U[i+row*Nrows]=temp;
	  }
#else
	  for (row=0;row<Nrows;row++) {
		temp=U[max_index+row*Ncols];
		U[max_index+row*Ncols]=U[i+row*Ncols];
		U[i+row*Ncols]=temp;
	  }
#endif
#pragma MUST_ITERATE(1,,)
	  for (row=0;row<Ncols;row++) {
		temp=V[max_index+row*Ncols];
		V[max_index+row*Ncols]=V[i+row*Ncols];
		V[i+row*Ncols]=temp;
	  }
	}
  }

  return 0;

}


/**************************************************  svd end  ********************************************************/


/************************************************** LK track part ****************************************************/

/* Number of bits to use as fractional in fixed point math */
#define  W_BITS 14U

/* Upshift coefficients before converting to fixed point */
#define  FRACTIONAL ((uint32_t)1 << W_BITS)

#define  FLT_SCALE (1.0f/(VXLIB_F32)((uint32_t)1 << 20))

/* There is an approximations being done for the square-root operation:
 *
 * The NEWTON_RHAPSON_ITERATIONS parameter is the number of iterations
 * in the Newton-rhapson algorithm to determing the double-precision
 * square root.  In all cases, the exponent of the rsqrdp is accurate,
 * but this value affects the mantissa precision.
 *
 * NEWTON_RHAPSON_ITERATIONS =
 *   0:  8-bit mantissa precision
 *   1: 16-bit mantissa precision
 *   2: 32-bit mantissa precision
 *   3: Full 52-bit mantissa precision
 * 
 * If it is found that the current setting is inappropriate, then the
 * NEWTON_RHAPSON_ITERATIONS parameter can be changed to alter the
 * precision. (see RSQRDP instruction in the C6X instruction manual).
 */
#define NEWTON_RHAPSON_ITERATIONS 2

static inline int32_t INT_ROUND(int32_t x, uint32_t n);
static inline VXLIB_D64 sqrtdp (VXLIB_D64 a);
static inline VXLIB_D64 recipdp (VXLIB_D64 a);
static inline void getWindowsInterpolate(const uint8_t image[restrict],
                                           const int16_t gradX[restrict],
                                           const int16_t gradY[restrict],
                                           int16_t winImage[restrict],
                                           uint32_t winGrad[restrict],
                                           int32_t strideImage,
                                           int32_t strideGrad,
                                           int32_t x,
                                           int32_t y,
                                           const int64_t weights,
                                           int32_t window_size,
                                           VXLIB_F32 *XX,
                                           VXLIB_F32 *XY,
                                           VXLIB_F32 *YY);
static inline void diffNewWindowInterpolate(const uint8_t image[restrict],
                                                 const int16_t winImage[restrict],
                                                 const uint32_t winGrad[restrict],
                                                 int32_t stride,
                                                 int32_t x,
                                                 int32_t y,
                                                 const int64_t weights,
                                                 int32_t window_size,
                                                 VXLIB_F32 *zx,
                                                 VXLIB_F32 *zy);

static inline int32_t INT_ROUND(int32_t x, uint32_t n) {
    uint32_t half = (uint32_t)1 << (n-1U);
    int32_t addhalf = x + (int32_t)half;

    /* MISRA.BITS.NOT_UNSIGNED
     * MISRAC_2004 Rule_12.7
     * MISRAC_WAIVER:
     * An arithmetic right shift of the signed integer "addhalf" by n is required.
     * It can not be cast to an unsigned because that would make a logical right shift
     * instead of the arithmetic shift that I need (I need the sign bit extension) to
     * mantain negative values.  I have confirmed that the DSP compiler is doing as
     * I expect with this code.
     */
    int32_t result = (int32_t)(addhalf >> n);
    return result;
}

static inline VXLIB_D64 sqrtdp (VXLIB_D64 a)
{
    VXLIB_D64    half  =  0.5;
    VXLIB_D64    OneP5 =  1.5;
    VXLIB_D64    x, y;
    int32_t     i;

    x = _rsqrdp(a);

    for( i = 0; i < NEWTON_RHAPSON_ITERATIONS; i++ ) {
        x = x * (OneP5 - (a * x * x * half));
    }

    y = a * x;

    if( a > 1.7976931348623157E+308 ) { /* DBL_MAX */
        y = 1.7976931348623157E+308;    /* DBL_MAX */
    }

    return (y);
}


static inline VXLIB_D64 recipdp (VXLIB_D64 a)
{
    VXLIB_D64    two  =  2.0;
    VXLIB_D64    max  = 1.7976931348623157E+308;
    VXLIB_D64    x;
    int32_t     i;

    x = _rcpdp(a);

    for( i = 0; i < 3; i++ ) {
        x = x * (two - (a * x));
    }

    if( _fabs(a) > max ) {
        x = 0.0;
    }

    return (x);
}

/*
 *  getWindowsInterpolate()
 *
 *  This function fetches the window neighborhood for the old image, the old gradient X, and the old gradient Y.
 *  It then does a bilinear interpolation on each window separatly based on the fractional portion of the x and y coordinates.
 *  The interpolted results are stored in 2 local buffers: winImage and winGrad.  The gradient values are stored
 *  interleaved in order to  reduce load/store instructions and should be better for cache. Finally, the gradient values
 *  are multiply accumulated to return the XX, XY, and YY results.
 *
 * [in]      image[]        Pointer to old image array (U8.0)
 * [in]      gradX[]        Pointer old X gradient image array from Scharr filter (S15.0)
 * [in]      gradY[]        Pointer old Y gradient image array from Scharr filter (S15.0)
 * [out]     winImage[]     Pointer to interpolated image window (SQ16.0)
 * [out]     winGrad[]      Pointer to interleaved interpolated gradient window (UQ32.0)
 * [in]      strideImage    Stride of old image input
 * [in]      strideGrad     Stride of old gradient image inputs
 * [in]      x              X coordinate for center of window (truncated to int) (SQ31.0)
 * [in]      y              Y coordinate for center of window (truncated to int) (SQ31.0)
 * [in]      weights        Packed 4x16-bit weights for bilinear interpolation operation (I64)
 * [in]      window_size    Window size
 * [in,out] *XX             Sum of squares of X gradient from interpolated window
 * [in,out] *XY             Sum of (X gradient)*(Y gradient) from interpolated window
 * [in,out] *YY             Sum of squares of Y gradient from interpolated window
 */
static inline void getWindowsInterpolate(const uint8_t image[restrict],
                                           const int16_t gradX[restrict],
                                           const int16_t gradY[restrict],
                                           int16_t winImage[restrict],
                                           uint32_t winGrad[restrict],
                                           int32_t strideImage,
                                           int32_t strideGrad,
                                           int32_t x,
                                           int32_t y,
                                           const int64_t weights,
                                           int32_t window_size,
                                           VXLIB_F32 *XX,
                                           VXLIB_F32 *XY,
                                           VXLIB_F32 *YY)
{
    int32_t i, j, k;
    int32_t winDiv2 = window_size/2;

    /* Calculate offsets for each row of the images */
    int32_t offsetImage = ((y-winDiv2)*strideImage) + (x-winDiv2);
    int32_t offsetGrad1 = ((y-winDiv2)*strideGrad) + (x-winDiv2);
    int32_t offsetGrad2 = offsetGrad1+strideGrad;

    /* Pack YY and XX into a __float2_t for SIMD-2 operations */
    __float2_t yyxx = _ftof2(*YY, *XX);

    j = k = 0;

    /* Collapse 2D processing into a single loop to reduce cycles due to
     * pipe-up and pipe-down since the loop will be so small */
    for(i=0; i < (window_size*window_size); i++)
    {
        int16_t xval, yval;
        int32_t xyval;
        uint32_t xypack;

        /* Step 1: Image Fetch and Interpolation */

        /* Fetch 4 pixels of old image */
        uint8_t imtl = image[offsetImage + k];
        uint8_t imtr = image[offsetImage + k + 1];
        uint8_t imbl = image[offsetImage + strideImage + k];
        uint8_t imbr = image[offsetImage + strideImage +k + 1];

        /* Pack into 4x16bit lanes for DOTP4H instruction*/
        int64_t left  = _itoll((uint32_t)imbl, (uint32_t)imtl);
        int64_t right = _itoll((uint32_t)imbr, (uint32_t)imtr);
        int64_t pixels = _dpackl2(right, left);

        /* Perform Bilinear interpolation by multuply accumulating all 4 pixels and weights */
        /* Saturation check (fits in 32 bits): MAX Weight = 0x4000, MAX pixel = 0xFF, MAX sum = 0x3FC000 */
        int32_t sum = _dotp4h(pixels, weights);

        /* Round result while shifting off extra fractional bits that were added to the weights,
         * taking into account that scharr gradient results are 5bits more than image (x32) */
        /* Saturation check (fits in 16 bits): MAX of ROUND = 0x3FC000 + 0x100 = 0x3FC100 >> 9 = 0x1FE0  */
        int16_t imval = (int16_t)INT_ROUND(sum, W_BITS-5U);



        /* Step 2: GradX Fetch and Interpolation */

        /* Fetch 4 pixels of old gradient X */
        uint32_t top = _mem4_const(&gradX[offsetGrad1+k]);
        uint32_t bot = _mem4_const(&gradX[offsetGrad2+k]);

        /* Pack into 4x16bit lanes, and perform bilinear interpolation using DOTP4H instruction */
        /* Saturation check (fits in 32 bits): MAX Weight = 0x4000, MAX pixel = 0x1FE0 (+1 sign bit), MAX sum = 0x7F80000 (+1 sign bit)*/
        pixels = _itoll(bot, top);
        sum = _dotp4h(pixels, weights);

        /* Round result while shifting off extra fractional bits that were added to the weights */
        /* Saturation check (fits in 16 bits): MAX of ROUND = 0x7F80000 + 0x2000 = 0x7F82000 >> 14 = 0x1FE0 (+1 sign bit) */
        xval = (int16_t)INT_ROUND(sum, W_BITS);



        /* Step 3: GradY Fetch and Interpolation */

        /* Fetch 4 pixels of old gradient Y */
        top = _mem4_const(&gradY[offsetGrad1+k]);
        bot = _mem4_const(&gradY[offsetGrad2+k]);

        /* Pack into 4x16bit lanes, and perform bilinear interpolation using DOTP4H instruction */
        /* Saturation check (fits in 32 bits): MAX Weight = 0x4000, MAX pixel = 0x1FE0 (+1 sign bit), MAX sum = 0x7F80000 (+1 sign bit)*/
        pixels = _itoll(bot, top);
        sum = _dotp4h(pixels, weights);

        /* Round result while shifting off extra fractional bits that were added to the weights */
        /* Saturation check (fits in 16 bits): MAX of ROUND = 0x7F80000 + 0x2000 = 0x7F82000 >> 14 = 0x1FE0 (+1 sign bit) */
        yval = (int16_t)INT_ROUND(sum, W_BITS);



        /* Store interpolated image and interpolated gradients to their window buffers */
        xypack = _pack2((uint32_t)yval, (uint32_t)xval);
        winImage[i] = imval;
        winGrad[i] = xypack;


        /* Equivalent code rewritten for SIMD of 2 operations at a time:
         *   *XX += (VXLIB_F32)(xval*xval);
         *   *YY += (VXLIB_F32)(yval*yval);
         */
        yyxx = _daddsp(_dintspu(_mpy2ll((int32_t)xypack, (int32_t)xypack)), yyxx);

        /* Saturation check (fits in 32 bits): MAX of MPY = 0x1FE0*0x1FE0 = 0x03F80400 (+1 sign bit) 5 bits headroom */
        xyval= (int32_t)xval*(int32_t)yval;

        /* Saturation check (fits in F32): MAX of SUM will overflow 32 bits, put in F32 */
        *XY += (VXLIB_F32)xyval;

        /* The remaining code is done to manage the update of image pointers when
         * we have reached the end of processing each row of the window */
        j++;

        if(j == window_size) {
            j = 0;
            offsetImage += strideImage;
            offsetGrad1+= strideGrad;
            offsetGrad2+= strideGrad;
        }
        k = _mvd(j);
    }

    /* Update the output data from the inner loop SIMD version of the variable */ 
    *XX = _lof2(yyxx);
    *YY = _hif2(yyxx);
}


/*
 *  diffNewWindowInterpolate()
 *
 *  This function fetches the window neighborhood for the new image, performing a bilinear interpolation based on the fractional
 *  portion of the x and y coordinates. The interpolated results are compared to the old window, and the sum of the products of the
 *  intensity difference and the gradient values across the window are returned.
 *
 * [in]      image[]        Pointer to new image array (U8.0)
 * [in]      winImage[]     Pointer to interpolated image window (SQ15.0)
 * [in]      winGrad[]      Pointer to interleaved interpolated gradient window (UQ32.0)
 * [in]      stride         Stride of new image input
 * [in]      x              X coordinate for center of window (truncated to int) (SQ31.0)
 * [in]      y              Y coordinate for center of window (truncated to int) (SQ31.0)
 * [in]      weights        Packed 4x16-bit weights for bilinear interpolation operation (I64)
 * [in]      window_size    Window size
 * [in,out] *zx             Sum of products of intensity difference and x gradients across window
 * [in,out] *zy             Sum of products of intensity difference and y gradients across window
 */
static inline void diffNewWindowInterpolate(const uint8_t image[restrict],
                                                 const int16_t winImage[restrict],
                                                 const uint32_t winGrad[restrict],
                                                 int32_t stride,
                                                 int32_t x,
                                                 int32_t y,
                                                 const int64_t weights,
                                                 int32_t window_size,
                                                 VXLIB_F32 *zx,
                                                 VXLIB_F32 *zy)
{
    int32_t i, j, k;
    int32_t winDiv2 = window_size/2;

    /* Calculate offsets for each row of the image */
    int32_t offset = ((y-winDiv2)*stride) + (x-winDiv2);

    /* Pack zy and zx into a __float2_t for SIMD-2 operations */
    __float2_t z = _ftof2(*zy, *zx);

    j = k = 0;

    /* Collapse 2D processing into a single loop to reduce cycles due to
     * pipe-up and pipe-down since the loop will be so small */
    for(i=0; i < (window_size*window_size); i++)
    {
        /* Fetch 4 pixels of new image */
        uint8_t imtl = image[offset + k];
        uint8_t imtr = image[offset + k + 1];
        uint8_t imbl = image[offset + stride + k];
        uint8_t imbr = image[offset + stride + k + 1];

        /* Pack into 4x16bit lanes for DOTP4H instruction*/
        int64_t left  = _itoll((uint32_t)imbl, (uint32_t)imtl);
        int64_t right = _itoll((uint32_t)imbr, (uint32_t)imtr);
        int64_t pixels = _dpackl2(right, left);

        /* Perform Bilinear interpolation by multuply accumulating all 4 pixels and weights */
        /* Saturation check (fits in 32 bits): MAX Weight = 0x4000, MAX pixel = 0xFF, MAX sum = 0x3FC000 */
        int32_t sum = _dotp4h(pixels, weights);

        /* Round result while shifting off extra fractional bits that were added to the weights,
         * taking into account that scharr gradient results are 5bits more than image (x32) */
        /* Saturation check (fits in 16 bits): MAX of ROUND = 0x3FC000 + 0x100 = 0x3FC100 >> 9 = 0x1FE0  */
        int16_t imval = (int16_t)INT_ROUND(sum, W_BITS-5U);





        /* Subtract the old image value for the pixel from the new image value */
        /* Saturation check (fits in 16 bits): MAX of DIFF = 0x1FE0 (+1 sign bit) */
        int32_t    diff = (int32_t)imval - (int32_t)winImage[i];

        /* Multiply the difference by the old gradient values, and convert to F32 */
        /* Saturation check (fits in 32 bits): MAX of MPY = 0x1FE0*0x1FE0 = 0x03F80400 (+1 sign bit) 5 bits headroom */
        __float2_t xxyy = _dintsp(_mpy2ll((int32_t)winGrad[i], (int32_t)_pack2((uint32_t)diff, (uint32_t)diff)));

        /* Add to running sum of products */
        /* Saturation check (fits in F32): MAX of SUM will overflow 32 bits, put in F32 */
        z = _daddsp(xxyy, z); 

        /* The remaining code is done to manage the update of image pointers when
         * we have reached the end of processing each row of the window */
        j++;

        if(j == window_size) {
            j = 0;
            offset += stride;
        }
        k = j;
    }

    /* Update the output data from the inner loop SIMD version of the variable */ 
    *zx = _lof2(z);
    *zy = _hif2(z);
}


VXLIB_STATUS VXLIB_trackFeaturesLK_i8u_diy(const uint8_t old_image[restrict],
                                       const VXLIB_bufParams2D_t *old_image_addr,
                                       const uint8_t new_image[restrict],
                                       const VXLIB_bufParams2D_t *new_image_addr,
                                       const int16_t old_gradX[restrict],
                                       const VXLIB_bufParams2D_t *old_gradX_addr,
                                       const int16_t old_gradY[restrict],
                                       const VXLIB_bufParams2D_t *old_gradY_addr,
                                       __float2_t old_points[restrict],
                                       __float2_t new_points[restrict],
                                       uint8_t tracking_status[restrict],
                                       uint32_t num_points,
                                       uint32_t max_iters,
                                       VXLIB_F32 epsilon,
                                       VXLIB_F32 scale,
                                       uint8_t window_size,
                                       uint8_t level,
                                       uint8_t termination,
                                       uint8_t scratch[restrict],
                                       uint32_t scratch_size)
{
    VXLIB_STATUS    status = VXLIB_SUCCESS;

#if defined(VXLIB_CHECK_PARAMS) || defined(VXLIB_TRACKFEATURESLK_I8U_CHECK_PARAMS)
    status = VXLIB_trackFeaturesLK_i8u_checkParams_diy(old_image, old_image_addr, new_image, new_image_addr,
                                                   old_gradX, old_gradX_addr, old_gradY, old_gradY_addr,
                                                   old_points, new_points, tracking_status, num_points, max_iters,
                                                   epsilon, scale, window_size, level, termination, scratch, scratch_size);
    if( status == VXLIB_SUCCESS )
#endif
    {
        int32_t i;
        int32_t winDiv2 = (int32_t)window_size/2;
        int32_t width = old_image_addr->dim_x;
        int32_t height = old_image_addr->dim_y;
        int32_t window_size_sqrd = (int32_t)window_size * (int32_t)window_size;
        int32_t window_size_sqrdx2 = 2*window_size_sqrd;

        /* Partition scratch memory into 2 buffers, one for interpolated old image, and another for
         * interpolated old gradient (interleaved) */
        /* MISRA.CAST.PTR
         * MISRAC_2004 Rule_11.4
         * MISRAC_WAIVER:
         * I need two local buffers which are of different types.  I don't want to expose both
         * to the API, when a single buffer would make the API simpler.  Therefore, I can partition
         * the scratch buffer internal to the function, and I take care that the alignment is correct.
         * The DSP is using the unaligned access instructions anyway.
         */
        int16_t  *winImage = (int16_t *)&scratch[0];
        uint32_t *winGrad = (uint32_t *)&scratch[(2 * window_size_sqrd) + 2];

        /* Packed min and max values for out of bounds comparison */
        int32_t maxY = height - winDiv2 - 2;
        int32_t maxX = width - winDiv2 - 2;
        int32_t minXY = winDiv2 + 1;
        int32_t maxCmp = (int32_t)_pack2((uint32_t)maxY, (uint32_t)maxX);
        int32_t minCmp = (int32_t)_pack2((uint32_t)minXY, (uint32_t)minXY);

        /* Compute 1/(2*window_size*window_size) outside the loop for use later */
        VXLIB_F32 eigDiv = (VXLIB_F32)recipdp((VXLIB_D64)window_size_sqrdx2);

        /* Pack scale into 2 lanes */
        __float2_t scale2 = _ftof2(scale, scale);

        /* Iterate through each point in the list */
        for(i = 0; i < (int32_t)num_points; i++)
        {
            /* Update points according to scale */
            old_points[i] = _dmpysp(old_points[i], scale2);
            new_points[i] = _dmpysp(new_points[i], scale2);

            /* Only operate on valid points */
            if (tracking_status[i] != 0)
            {
                uint32_t outOfBounds = 0;

                /* Get points for center of window, find truncated version of them, and pack for border compare */
                VXLIB_F32 oldXf = _lof2(old_points[i]);
                VXLIB_F32 oldYf = _hif2(old_points[i]);
                int32_t oldXi = (int32_t)oldXf;
                int32_t oldYi = (int32_t)oldYf;
                uint32_t oldPtsi = _pack2((uint32_t)oldYi, (uint32_t)oldXi);

                /* Ignore the borders (+1 accounts for gradient borders)*/
                outOfBounds = (uint32_t)_cmpgt2(minCmp, (int32_t)oldPtsi) | (uint32_t)_cmpgt2((int32_t)oldPtsi, maxCmp);

                /* Tracking is lost coordinate goes out of bounds */
                if( outOfBounds != 0) {
                    if(level == 0) {
                        tracking_status[i] = 0;
                    }
                } else {
                      
                    VXLIB_F32 dx = oldXf - (VXLIB_F32)oldXi;
                    VXLIB_F32 dy = oldYf - (VXLIB_F32)oldYi;
                    int32_t a[4];
                    VXLIB_F32 XX = 0.0;
                    VXLIB_F32 XY = 0.0;
                    VXLIB_F32 YY = 0.0;
                    VXLIB_F32 Df;
                    VXLIB_D64 D;
                    int64_t weights;
                    VXLIB_F32 minEig;
                    VXLIB_F32 ftmp, tmp;
                    VXLIB_F32 flt_scale = FLT_SCALE;

                    /* Compute coefficients for bilinear interpolation and pack them into 4x16bits */
                    ftmp = ((1.0f - dx) * (1.0f - dy) * (VXLIB_F32)FRACTIONAL) + 0.5f; a[0] = (int32_t)ftmp;
                    ftmp = (       (dx) * (1.0f - dy) * (VXLIB_F32)FRACTIONAL) + 0.5f; a[1] = (int32_t)ftmp;
                    ftmp = ((1.0f - dx) * (dy)        * (VXLIB_F32)FRACTIONAL) + 0.5f; a[2] = (int32_t)ftmp;
                    a[3] = (int32_t)FRACTIONAL - a[0] - a[1] - a[2];

                    weights = _itoll(_pack2((uint32_t)a[3],(uint32_t)a[2]), _pack2((uint32_t)a[1],(uint32_t)a[0]));

                    /* Fetch window around each point for old image, and gradient images.  Interpolate,
                     * and put in local window buffers.  Compute XX, XY, and YY. */
                    getWindowsInterpolate(old_image, old_gradX, old_gradY, 
                                          winImage, winGrad,
                                          old_image_addr->stride_y, old_gradX_addr->stride_y/2,
                                          oldXi, oldYi, weights, (int32_t)window_size, &XX, &XY, &YY);

                    /* Remove the fractional bits that were added to the coefficients */
                    XX *= flt_scale;
                    XY *= flt_scale;
                    YY *= flt_scale;

                    /* Compute the determinate and minimum eigenvalue of the matrix */
                    Df = (XX*YY) - (XY*XY);
                    D = (VXLIB_D64)Df;
                    tmp = ((XX-YY)*(XX-YY)) + (4.0f*(XY*XY));
                    minEig = (VXLIB_F32)((YY + XX) - (VXLIB_F32)sqrtdp((VXLIB_D64)tmp))*eigDiv;

                    /* Tracking is lost if the matrix is invertible ( D < 10e-7 ) or
                     * Minimum eigenvalue of the matrix is smaller than 10e-4. */
                    if( (minEig < 1.0e-04F) || (D < 1.0e-07F) )
                    {
                        if( level == 0 )
                        {
                            tracking_status[i] = 0;
                        }
                    } else {
                        /* MISRA.ITER.ONETERM
                         * MISRAC_2004 Rule_14.6
                         * MISRAC_WAIVER:
                         * This loop has 3 break statments.
                         * I am able to remove one or both of these by adding a flag that the
                         * loop can conditionally terminate in, but the performance suffers.
                         * Therefore, we are requesting a waiver for this in the interest of
                         * performance.
                         */

                        uint32_t j = 0;
                        VXLIB_F32 newXf, newYf;

                        /* Initialize the previous delta values that will help determine when to stop iterating */
                        VXLIB_F32 prevDelta_x = 0.0f;
                        VXLIB_F32 prevDelta_y = 0.0f;

                        /* 1.0/D */
                        D = recipdp(D);

                        /* Fetch new points from list */
                        newXf = _lof2(new_points[i]);
                        newYf = _hif2(new_points[i]);

                        /* Iterate until converged based on policy */
                        while( (j < max_iters) || (termination == VXLIB_TERM_EPSILON) )
                        {
                            /* Get points for center of window, find truncated version of them, and pack for border compare */
                            int32_t newXi = (int32_t)newXf;
                            int32_t newYi = (int32_t)newYf;
                            uint32_t newPtsi = _pack2((uint32_t)newYi, (uint32_t)newXi);
                            VXLIB_F32    zxf = 0.0f;
                            VXLIB_F32    zyf = 0.0f;
                            VXLIB_D64    zx, zy;
                            VXLIB_F32    delta_x, delta_y;

                            /* Ignore the borders (+1 accounts for gradient borders)*/
                            outOfBounds = (uint32_t)_cmpgt2(minCmp, (int32_t)newPtsi) | (uint32_t)_cmpgt2((int32_t)newPtsi, maxCmp);

                            /* Tracking is lost coordinate goes out of bounds */
                            if( outOfBounds != 0) {
                                if(level == 0) {
                                    tracking_status[i] = 0;
                                }
                                break;
                            }

                            dx = newXf - (VXLIB_F32)newXi;
                            dy = newYf - (VXLIB_F32)newYi;

                            /* Compute coefficients for bilinear interpolation and pack them into 4x16bits */
                            ftmp = ((1.0f - dx) * (1.0f - dy) * (VXLIB_F32)FRACTIONAL) + 0.5f; a[0] = (int32_t)ftmp;
                            ftmp = (       (dx) * (1.0f - dy) * (VXLIB_F32)FRACTIONAL) + 0.5f; a[1] = (int32_t)ftmp;
                            ftmp = ((1.0f - dx) * (dy)        * (VXLIB_F32)FRACTIONAL) + 0.5f; a[2] = (int32_t)ftmp;
                            a[3] = (int32_t)FRACTIONAL - a[0] - a[1] - a[2];

                            weights = _itoll(_pack2((uint32_t)a[3],(uint32_t)a[2]), _pack2((uint32_t)a[1],(uint32_t)a[0]));

                            /* Fetch window around each point for new image and compute  zx and zy differences */
                            diffNewWindowInterpolate(new_image, winImage, winGrad, new_image_addr->stride_y,
                                                     newXi, newYi, weights, (int32_t)window_size, &zxf, &zyf);

                            zx = (VXLIB_D64)zxf * (VXLIB_D64)flt_scale;
                            zy = (VXLIB_D64)zyf * (VXLIB_D64)flt_scale;

                            /* Compute deltas for each coordinate */
                            delta_x = (VXLIB_F32)((((VXLIB_D64)XY*zy) - ((VXLIB_D64)YY*zx)) * D);
                            delta_y = (VXLIB_F32)((((VXLIB_D64)XY*zx) - ((VXLIB_D64)XX*zy)) * D);

                            newXf += delta_x;
                            newYf += delta_y;

                            /* Stop iterating if termination policy is "epsilon" AND the residual of the
                             * affine motion vector is < epsilon */
                            if( (((delta_x*delta_x) + (delta_y*delta_y)) <= epsilon) &&
                                ( (termination == VXLIB_TERM_EPSILON) || (termination == VXLIB_TERM_BOTH) ))
                            {
                                break;
                            } 

                            /* Stop iterating if sum of current change and previous change is < 0.01 */
                            if( (j > 0) && (_fabsf(delta_x + prevDelta_x) < 0.01f) &&
                                           (_fabsf(delta_y + prevDelta_y) < 0.01f) )
                            {
                                newXf -= delta_x*0.5f;
                                newYf -= delta_y*0.5f;
                                break;
                            }
                            prevDelta_x = delta_x;
                            prevDelta_y = delta_y;
                            j++;
                        }

                        /* Save points back to new list */
                        new_points[i] = _ftof2(newYf, newXf);
                    }
                }
            }
        }
    }

    return (status);
}

#if (!defined(VXLIB_REMOVE_CHECK_PARAMS) && !defined(VXLIB_TRACKFEATURESLK_I8U_REMOVE_CHECK_PARAMS)) || (defined(VXLIB_CHECK_PARAMS)) || (defined(VXLIB_TRACKFEATURESLK_I8U_CHECK_PARAMS))

VXLIB_STATUS    VXLIB_trackFeaturesLK_i8u_checkParams_diy(const uint8_t old_image[restrict],
                                                   const VXLIB_bufParams2D_t *old_image_addr,
                                                   const uint8_t new_image[restrict],
                                                   const VXLIB_bufParams2D_t *new_image_addr,
                                                   const int16_t old_gradX[restrict],
                                                   const VXLIB_bufParams2D_t *old_gradX_addr,
                                                   const int16_t old_gradY[restrict],
                                                   const VXLIB_bufParams2D_t *old_gradY_addr,
                                                   const __float2_t old_points[restrict],
                                                   const __float2_t new_points[restrict],
                                                   const uint8_t tracking_status[restrict],
                                                   uint32_t num_points,
                                                   uint32_t max_iters,
                                                   VXLIB_F32 epsilon,
                                                   VXLIB_F32 scale,
                                                   uint8_t window_size,
                                                   uint8_t level,
                                                   uint8_t termination,
                                                   const uint8_t scratch[restrict],
                                                   uint32_t scratch_size)
{
    VXLIB_STATUS    status = VXLIB_SUCCESS;

    if((old_image == NULL) || (new_image == NULL) || (old_gradX == NULL) ||
       (old_gradY == NULL) || (old_points == NULL) || (new_points == NULL) ||
       (tracking_status == NULL) || (scratch == NULL)) {
        status = VXLIB_ERR_NULL_POINTER;
    } else if((old_image_addr->dim_x != new_image_addr->dim_x) ||
              (old_gradX_addr->dim_x != old_gradY_addr->dim_x) ||
              (old_image_addr->dim_y != new_image_addr->dim_y) ||
              (old_gradX_addr->dim_y != old_gradY_addr->dim_y) ||
              (old_image_addr->stride_y < old_image_addr->dim_x) ||
              (new_image_addr->stride_y < new_image_addr->dim_x) ||
              (old_gradX_addr->stride_y != old_gradY_addr->stride_y) ||
              (old_gradX_addr->stride_y < (old_gradX_addr->dim_x*2))) {
        status = VXLIB_ERR_INVALID_DIMENSION;
    } else if((old_image_addr->data_type != VXLIB_UINT8) ||
               (new_image_addr->data_type != VXLIB_UINT8) ||
               (old_gradX_addr->data_type != VXLIB_INT16) ||
               (old_gradY_addr->data_type != VXLIB_INT16)) {
        status = VXLIB_ERR_INVALID_TYPE;
    } else if((termination != VXLIB_TERM_ITERATIONS) &&
             (termination != VXLIB_TERM_EPSILON) &&
             (termination != VXLIB_TERM_BOTH)) {
        status = VXLIB_ERR_INVALID_VALUE;
    } else if((int32_t)scratch_size < (((int32_t)window_size*(int32_t)window_size*6) + 2)) {
        status = VXLIB_ERR_BUFFER_TOO_SMALL;
    } else {
        status = VXLIB_SUCCESS;
    }
    return (status);
}

#endif

/************************ LK end *************************************************/


/************************** utils part ******************************************/
float max_(float a, float b){
  return(a > b ? a : b);
}
float min_(float a, float b){
  return(a <= b ? a : b);
}

int32_t max_I32(int32_t a, int32_t b){
  return(a > b ? a : b);
}
int32_t min_I32(int32_t a, int32_t b){
  return(a <= b ? a : b);
}

void swap_(float* a, float* b, int32_t size){

    float* tmp = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, size*sizeof(float), 32);
    memcpy(tmp, a, size*sizeof(float));
    memcpy(a, b, size*sizeof(float));
    memcpy(b, tmp, size*sizeof(float));
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, tmp, size*sizeof(float));
   

}

void swap_f(float* a, float* b){
    float tmp = *a;
    *a = *b;
    *b = tmp;
}

void swap_U16(uint16_t* a, uint16_t* b){
    uint16_t tmp = *a;
    *a = *b;
    *b = tmp;
}

/***************** utisl end *******************************/

/*************** Random number generation *******************/
    
#define CV_RNG_COEFF 4164903690U

static uint64_t cvRNG( int64_t seed);
static unsigned cvRandInt( uint64_t* rng );

static uint64_t cvRNG( int64_t seed)
{
    uint64_t rng = seed ? (uint64_t)seed : (uint64_t)(int64_t)-1;
    return rng;
}

/* Return random 32-bit unsigned integer: */
static unsigned cvRandInt( uint64_t* rng )
{
    uint64_t temp = *rng;
    temp = (uint64_t)(unsigned)temp*CV_RNG_COEFF + (temp >> 32);
    *rng = temp;
    return (unsigned)temp;
}   
/*****************************************/    


void get_bounding_rect(TBKTracker* tracker ,float* points,
                                TBKRect* rect, uint8_t* tracking_status) {
      float* point = points;
      int32_t i = 0;
      for (i = 0; i < tracker->points_num; i++) {
        // calculate rect of feature points
        if(tracking_status[i] == 3){
          rect->left = min_(rect->left, *point);
          rect->right = max_(rect->right, *point);
          rect->top = min_(rect->top, *(point + 1));
          rect->bottom = max_(rect->bottom, *(point + 1));
        }
        point += 2;
      }

}

void get_rect_sample_points(TBKRect* rect, float points[][2]) {
  points[0][0] = rect->left;
  points[0][1] = rect->top;
  points[1][0] = rect->right;
  points[1][1] = rect->top;
  points[2][0] = rect->right;
  points[2][1] = rect->bottom;
}

void perform_affine_transform(float affine_matrix[][3], float point[],
                                float *x, float *y) {
    // affine transformation
    *x = point[0] * affine_matrix[0][0] +
              point[1] * affine_matrix[0][1] +
              affine_matrix[0][2];
    *y = point[1] * affine_matrix[1][1] +
              point[0] * affine_matrix[1][0] +
              affine_matrix[1][2];
}

void update_car_rect(TBKTracker* tracker, float affine_matrix[3][3], float* x_scale,
                       float* y_scale) {
    // trans rect coord
    float x1 = tracker->calc_roi.left;
    float y1 = tracker->calc_roi.top;

    float pre_tran_p1[2], cur_tran_p1[2];
    float pre_tran_p2[2], cur_tran_p2[2];
    pre_tran_p1[0] = (tracker->rect.left - x1) / (*x_scale);
    pre_tran_p1[1] = (tracker->rect.top - y1) / (*y_scale);
    pre_tran_p2[0] = (tracker->rect.right - x1) / (*x_scale);
    pre_tran_p2[1] = (tracker->rect.bottom - y1) / (*y_scale);

    perform_affine_transform(affine_matrix, pre_tran_p1, &cur_tran_p1[0], &cur_tran_p1[1]);
    perform_affine_transform(affine_matrix, pre_tran_p2, &cur_tran_p2[0], &cur_tran_p2[1]);
    // re_trans rect coord
    tracker->rect.left = cur_tran_p1[0] * (*x_scale) + x1;
    tracker->rect.right = cur_tran_p2[1] * (*x_scale) + x1;
    tracker->rect.top = cur_tran_p1[0] * (*y_scale) + y1;
    tracker->rect.bottom = cur_tran_p2[1] * (*y_scale) + y1;
}


void update_car_roi(TBKTracker* tracker, TBKRect* car, int32_t img_cols, int32_t img_rows){

  //set roi parameters
  float LEFT_SHRINK_RATIO = 0.08;
  float RIGHT_SHRINK_RATIO = 0.08;
  float TOP_SHRINK_RATIO = 0.4;
  float BOTTOM_SHRINK_RATIO = 0.2;
  float EXPANDING_RATIO = 0.02;

  memcpy(&tracker->rect, car, sizeof(TBKRect));
  float width = (float)tracker->rect.right - tracker->rect.left;
  float height = (float)tracker->rect.bottom - tracker->rect.top;

  int32_t boundary_left = floor(tracker->rect.left + width * LEFT_SHRINK_RATIO);
  int32_t boundary_right = floor(tracker->rect.right - width * RIGHT_SHRINK_RATIO);
  int32_t boundary_top = floor(tracker->rect.top + height * TOP_SHRINK_RATIO);
  int32_t boundary_bottom = floor(tracker->rect.bottom - height * BOTTOM_SHRINK_RATIO);

  tracker->calc_roi.left = max_(floor((boundary_left * (1 - EXPANDING_RATIO))), 0);
  tracker->calc_roi.right = min_(floor((boundary_right * (1 + EXPANDING_RATIO))), img_cols);
  tracker->calc_roi.top = max_(floor((boundary_top * (1 - EXPANDING_RATIO))), 0);
  tracker->calc_roi.bottom = min_(floor((boundary_bottom * (1 + EXPANDING_RATIO))), img_rows);
  printf("tracker calc_roi are : x1 : %f x2 : %f x3 : %f x4 : %f\n", tracker->calc_roi.left, tracker->calc_roi.right, tracker->calc_roi.top, tracker->calc_roi.bottom);

}


//link to PROCESSOR_SDK_VISION_03_04_00_00/ti_components/algorithms/vxlib_c66x_1_1_1_0/packages/ti/vxlib/src/vx/VXLIB_warpAffineBilinear_i8u_c32f_o8u
void get_car_resized_roi_img(TBKTracker* tracker, uint8_t* frame, int32_t frame_width, int32_t frame_height, uint8_t* target_image,
                               float* x_scale, float* y_scale, int32_t* resize_width, int32_t* resize_height){
    
    int32_t ROI_RESIZE_WIDTH_ = 200;

    //cut roi frame
    int32_t roi_width = floor(tracker->calc_roi.right - tracker->calc_roi.left);
    int32_t roi_height = floor(tracker->calc_roi.bottom - tracker->calc_roi.top);
    int32_t roi_size = roi_width*roi_height;

    uint8_t *roi_img = (uint8_t *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, roi_size * sizeof(uint8_t), 32);
    memset(roi_img, 0, roi_size * sizeof(uint8_t));
   // uint8_t *roi_img = (uint8_t *)malloc(roi_size * sizeof(uint8_t));
    int32_t roi_ptr = 0;
    //uint8_t *ptr;
    //ptr = frame;
    int32_t frame_stride;

    printf("frane width,frame_height are : %d %d :\n", frame_width, frame_height);
    printf("x1 x2 y1 y2 are : %f %f %f %f\n", tracker->calc_roi.left, tracker->calc_roi.right, tracker->calc_roi.top, tracker->calc_roi.bottom);
    printf("roi width,height are : %d %d\n", roi_width, roi_height);
    for(frame_stride = 0; frame_stride < frame_height;){

      if(frame_stride < floor(tracker->calc_roi.top) - 1){
        frame_stride++;
        continue;
      }
      if(frame_stride >= floor(tracker->calc_roi.bottom) - 1)
        break;
      int32_t frame_col;
      for(frame_col = 0; frame_col < frame_width;){
        if(frame_col < floor(tracker->calc_roi.left)){
          frame_col++;
          continue;
        }
        if(frame_col >= floor(tracker->calc_roi.right))
          break;
        *(roi_img + roi_ptr) = *(frame + frame_stride*frame_width + frame_col);
        frame_col++;
        roi_ptr++;
      }
      frame_stride++;
    }

    //resize roi
    if (roi_width > ROI_RESIZE_WIDTH_) {
      int32_t roi_resize_height;
      roi_resize_height = (int32_t)(ROI_RESIZE_WIDTH_*roi_height/roi_width);

      *resize_width = ROI_RESIZE_WIDTH_;
      *resize_height = roi_resize_height;
      printf("resized width,geight are: %d %d\n", *(resize_width), *(resize_height));

      uint8_t *roi_dst = (uint8_t *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, roi_size * sizeof(uint8_t), 32);
      memset(roi_dst, 2, roi_size * sizeof(uint8_t));

      //img is too big, resiz frame to aimed size
      VXLIB_bufParams2D_t src_addr, dst_addr;
      src_addr.dim_x = roi_width;
      src_addr.dim_y = roi_height;
      src_addr.stride_y = roi_width;
      src_addr.data_type = VXLIB_UINT32;

      dst_addr.dim_x = roi_width;
      dst_addr.dim_y = roi_height;
      dst_addr.stride_y = roi_width;
      dst_addr.data_type = VXLIB_UINT32;

      //uint8_t borderValue = 0;
      *x_scale = (float )(*(resize_width))/(float)(roi_width);
      *y_scale = (float )(*(resize_height))/(float)(roi_height);
      int32_t x_offset = (roi_width - *(resize_width))/2;
      int32_t y_offset = (roi_height - *(resize_height))/2;

      float x_offset_float = (float)x_offset;
      float y_offset_float = (float)y_offset;

      float warpMatrix[6] = {1.0f, 0.0f, 
                             0.0f, 1.0f, 
                             0.0f, 0.0f};
      warpMatrix[0] = 1/(*x_scale);
      warpMatrix[3] = 1/(*y_scale);
      warpMatrix[4] = (0.0f - x_offset_float)*(1/(*x_scale));
      warpMatrix[5] = (0.0f - y_offset_float)*(1/(*y_scale));

      VXLIB_STATUS status_opt = VXLIB_SUCCESS;
      status_opt = VXLIB_warpAffineBilinear_i8u_c32f_o8u(roi_img, &src_addr, roi_dst, &dst_addr, warpMatrix, 0, 0, 0, 0);
      if(status_opt != VXLIB_SUCCESS)
        tracker->track_on = false;

      if(target_image != NULL)
        memset(target_image, 0, tracker->roi_store_size*sizeof(uint8_t));

      int32_t tar_ptr = 0;
      //uint8_t *ptr_roi;
      //ptr_roi = roi_dst;

      int32_t tar_stride;
      //for(tar_stride = 0; tar_stride < roi_width*roi_height; tar_stride++){
        //printf("%d:%d\n", tar_stride, *(ptr + tar_stride));
      //}

      for(tar_stride = 0; tar_stride < roi_height;){
        if(tar_stride < y_offset){
          tar_stride ++;
          continue;
        }
        if(tar_stride >= y_offset + *(resize_height))
          break;
        int32_t tar_col;
        for(tar_col = 0; tar_col < roi_width;){
          if(tar_col < x_offset){
            tar_col ++;
            continue;
          }
          if(tar_col >= x_offset + *(resize_width))
            break;
          *(target_image + tar_ptr) = *(roi_dst + tar_stride*roi_width + tar_col);
          tar_ptr++;
          tar_col++;
        }
        tar_stride++;
      }
      Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, roi_dst, roi_size * sizeof(uint8_t));
    } else {

      *resize_width = roi_width;
      *resize_height = roi_height;
      //printf("before memset targe img\n");
      if(target_image != NULL)
        memset(target_image, 0, tracker->roi_store_size*sizeof(uint8_t));

      //printf("before memcopy target img\n");
      memcpy(target_image, roi_img, roi_size*sizeof(uint8_t));
      //printf("after memcopy target img\n");
      *x_scale = 1.0;
      *y_scale = 1.0;
    }
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, roi_img, roi_size * sizeof(uint8_t));
    //free(roi_img);
}

void perc_down(uint16_t* array, int32_t i, uint32_t K){

    int32_t child;
    uint16_t tmp;

    for(tmp = array[i]; 2*i + 1 < K; i = child){
        child = 2*i + 1;
        if(child != K - 1 && array[child + 1] < array[child])
            child++;
        if(tmp > array[child])
            array[i] = array[child];
        else
            break;
    }
    array[i] = tmp;
    
}

void sort_by_heap(uint16_t* array, int32_t top_k, uint32_t im_size){

    int32_t i;
    for(i = top_k/2; i >=0; i--)
        perc_down(array, i, top_k);

    for(i = top_k; i < im_size; i++){
        if(array[i] < array[0])
            continue;
        
        swap_U16(array, array + i);
        perc_down(array, 0, top_k);
    }

    for(i = top_k -1 ; i >0; i--){
        swap_U16(array, array + i);
        perc_down(array, 0, i);
    }
    
}

void perc_down_f(float* array, int32_t i, uint32_t K){

    int32_t child;
    float tmp;

    for(tmp = array[i]; 2*i + 1 < K; i = child){
        child = 2*i + 1;
        if(child != K - 1 && array[child + 1] < array[child])
            child++;
        if(tmp > array[child])
            array[i] = array[child];
        else
            break;
    }
    array[i] = tmp;
    
}

void sort_by_heap_f(float* array, int32_t top_k, uint32_t im_size){

    int32_t i;
    for(i = top_k/2; i >=0; i--)
        perc_down_f(array, i, top_k);

    for(i = top_k; i < im_size; i++){
        if(array[i] < array[0])
            continue;
        
        swap_f(array, array + i);
        perc_down_f(array, 0, top_k);
    }

    for(i = top_k -1 ; i >0; i--){
        swap_f(array, array + i);
        perc_down_f(array, 0, i);
    }
    
}

//sort_by_heap with location
void perc_down_withloc_f(float* array_score, float* array_loc, int32_t i, uint32_t K){

    int32_t child, child_loc;
    float tmp;
    float tmp_x, tmp_y;
    int32_t i_loc = 2*i;

    tmp_x = array_loc[i_loc];
    tmp_y = array_loc[i_loc + 1];
    for(tmp = array_score[i]; 2*i + 1 < K; i = child){
        child = 2*i + 1;
        child_loc = 2*i_loc + 2;
        if(child != K - 1 && array_score[child + 1] < array_score[child]){
            child++;
            child_loc += 2;
        }
        if(tmp > array_score[child]){
            array_score[i] = array_score[child];
            array_loc[i_loc] = array_loc[child_loc];
            array_loc[i_loc + 1] = array_loc[child_loc + 1];
        }else
            break;
        i_loc = child_loc;
    }
    array_score[i] = tmp;
    array_loc[i_loc] = tmp_x;
    array_loc[i_loc + 1] = tmp_y;
}

void sort_by_heap_withloc_f(float* array_score, float* array_loc, int32_t top_k, uint32_t im_size){

    int32_t i;
    for(i = top_k/2; i >=0; i--)
        perc_down_withloc_f(array_score, array_loc, i, top_k);

    for(i = top_k; i < im_size; i++){
        if(array_score[i] < array_score[0])
            continue;
        
        swap_f(array_score, array_score + i);
        swap_f(array_loc, array_loc + i*2);
        swap_f(array_loc + 1, array_loc + i*2 + 1);

        perc_down_withloc_f(array_score, array_loc, 0, top_k);
    }

    for(i = top_k -1 ; i >0; i--){
        swap_f(array_score, array_score + i);
        swap_f(array_loc, array_loc + i*2);
        swap_f(array_loc + 1, array_loc + i*2 + 1);
        perc_down_withloc_f(array_score, array_loc, 0, i);
    }
}
static void sort_ind_cn(const uint16_t* corners, int32_t array_size, int32_t* indices);

static void sort_ind_cn(const uint16_t* corners, int32_t array_size, int32_t* indices);
 
/* bubble sort descending, plus index ordering */
static void sort_ind_cn(const uint16_t* corners, int32_t array_size, int32_t* indices)
{
    int32_t  i_sort=0, j_sort, index_sort;
 
    while( i_sort == 0 ) {
        i_sort = 1;
 
        for( j_sort=0; j_sort < (array_size - 1); j_sort++ ) {
            if( corners[indices[j_sort]] < corners[indices[j_sort + 1]] ) {
                index_sort = indices[j_sort];
                indices[j_sort] = indices[j_sort + 1];
                indices[j_sort + 1] = index_sort;
                i_sort = 0;
            }
        }
    }
}
 

/**************************** get features points ************************************/


 void warp_affine_by_linear_interpolation(uint8_t* src, uint8_t* target_buffer, float* affine_matrix, int32_t target_rows, int32_t target_cols) {

    uint8_t border_value = 127;

    uint8_t *image_data = src;

    int32_t i,j,c;
    float matrix[2][3] = {0, 0, 0, 0, 0, 0};
    for(i = 0; i < 6; i++)
        matrix[i/3][i%3] = affine_matrix[i];


    uint8_t *pbuf = target_buffer;

    int32_t line_pixel_num = target_cols;

    int32_t SCALE_VALUE = 1024;
    int32_t DOUBLE_SCALE = 20;
    
    int32_t width_bound = target_cols - 1;
    int32_t height_bound = target_rows - 1;
    
    
    for (i = 0; i < target_rows; ++i) {
    
      float trans_x = i * matrix[0][1] + matrix[0][2];
      float trans_y = i * matrix[1][1] + matrix[1][2];
      
      for (j = 0; j < target_cols; ++j) {
        int32_t px = round(trans_x);
        int32_t py = round(trans_y);
        bool valid =
            (px >= 0 && py >= 0 && px <= width_bound && py <= height_bound);
        if (valid) {
          int dx = (trans_x - px) * SCALE_VALUE;
          int dy = (trans_y - py) * SCALE_VALUE;
          int idx = SCALE_VALUE - dx;
          int idy = SCALE_VALUE - dy;

          uint8_t *pixel_left_top =
              image_data + py * line_pixel_num + px + 1;

          uint8_t *pixel_right_top = px == width_bound
                                         ? pixel_left_top
                                         : pixel_left_top + 1;

          uint8_t *pixel_left_bottom = py == height_bound
                                           ? pixel_left_top
                                           : pixel_left_top + line_pixel_num;

          uint8_t *pixel_right_bottom =
              px == width_bound ? pixel_left_bottom
                                : pixel_left_bottom + 1;


          *pbuf++ =
                ((pixel_left_top[0] * idx + pixel_right_top[0] * dx) * idy +
                 (pixel_left_bottom[0] * idx + pixel_right_bottom[0] * dx) *
                     dy) >>
                DOUBLE_SCALE;

        } else {
          for (c = 0; c < 1; ++c) {
            *pbuf++ = border_value;
          }
        }
        trans_x += matrix[0][0];
        trans_y += matrix[1][0];
      }
    }
  }

void get_gradient(int32_t rows, int32_t cols, int32_t block_size, uint8_t* img, float* dx, float* dy){

    uint8_t * src_data = img;
    //int32_t gradient_cell_size = 3;
    float scale = 100;//(1 << (gradient_cell_size - 1)) * block_size * 255.0;
    scale = 1. / scale;
    
    int32_t *vsum = (int32_t *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, (cols + 2)*sizeof(int32_t), 32);
    memset(vsum, 0, (cols + 2)*sizeof(int32_t));
    int32_t *ssum = (int32_t *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, (cols + 2)*sizeof(int32_t), 32);
    memset(ssum, 0, (cols + 2)*sizeof(int32_t));

    int32_t pre_cnt = 0;

    int32_t y;
    for (y = 0; y < rows; ++y) {
        int32_t id0 = y - 1 >= 0 ? y - 1 : rows > 1 ? 1 : 0;
        int32_t id1 = y;
        int32_t id2 = y + 1 < rows ? y + 1 : rows > 1 ? rows - 2 : 0;
        uint8_t *row0 = src_data + id0 * cols;
        uint8_t *row1 = src_data + id1 * cols;
        uint8_t *row2 = src_data + id2 * cols;
        float *dy_data = dy + y * cols;
        float *dx_data = dx + y * cols;
        int32_t x;
        for (x = 0; x < cols; ++x) {
            vsum[x + 1] = *(row0 + x) + *(row2 + x) + ((*(row1 + x)) << 1);
            ssum[x + 1] = *(row2 + x) - *(row0 + x);
        }
        if (cols > 1) {
            vsum[0] = vsum[2];
            vsum[cols + 1] = vsum[cols - 1];
            ssum[0] = ssum[2];
            ssum[cols + 1] = ssum[cols - 1];
        }

        for (x = 0; x < cols; ++x) {
            float ly = (float)(ssum[x] + ssum[x + 2]) + (ssum[x + 1] << 1);
            float lx = (float)(vsum[x + 2] - vsum[x]);
            *(dx_data + x) = lx * scale;
            *(dy_data + x) = ly * scale;
            if((*(dx_data + x) > 0 || *(dy_data + x) > 0) && pre_cnt <= 20){
                pre_cnt++;
                //printf("%f, %f\n", *(dx_data + x), *(dy_data + x));
            }
        }
    }

    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, vsum, (cols + 2)*sizeof(int32_t));
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, ssum, (cols + 2)*sizeof(int32_t));
}

uint8_t is_pos_valid(int32_t y, int32_t x, int32_t src_rows, int32_t src_cols) {
    return y >= 0 && x >= 0 && y < src_rows && x < src_cols;
}

uint8_t is_max_eigen_point_neibor(float* eigen_value_vector, int32_t pos, int32_t src_rows, int32_t src_cols) {

    uint8_t is_max = 1;
    int32_t dx,dy;
    int32_t y = pos/src_cols;//round(pos / src_cols);
    int32_t x = pos%src_cols;//round(pos % src_cols);

    for (dy = -1; dy <= 1; ++dy) {
      for (dx = -1; dx <= 1; ++dx) {
        if (dy == 0 && dx == 0)
          continue;
        if (is_pos_valid(y + dy, x + dx, src_rows, src_cols)){
          is_max &= (*(eigen_value_vector + pos) >
                    *(eigen_value_vector + pos + dy * src_cols + dx));
          //printf("%f : %f\n", *(eigen_value_vector + y*src_cols + x), *(eigen_value_vector + y*src_cols + x + dy * src_cols + dx));
          //printf("%d : \n", (*(eigen_value_vector + y*src_cols + x) > *(eigen_value_vector + y*src_cols + x + dy * src_cols + dx)));
          //printf("is_max : %d\n", is_max);
        }
        else
          is_max = 0;
      }
    }
    return is_max;
}

void calc_min_eigenvalue(float* _cov, float *_dst, int32_t src_rows, int32_t src_cols) {

  int32_t src_size = src_rows*src_cols;
  int32_t i;
  const float *cov0_data = _cov;
  const float *cov1_data = _cov + src_size;
  const float *cov2_data = _cov + src_size*2;

  for (i = 0; i < src_size; i++) {
    float a = *(cov0_data + i) * 0.5;
    float b = *(cov1_data + i);
    float c = *(cov2_data + i) * 0.5;
    *(_dst + i) = (float)((a + c) - sqrt((a - c) * (a - c) + b * b));
  }

  /*int32_t pre_cnt = 0;
  printf("check eigen\n");
  for(i = 0; i < src_size; i++){
      if(*(_dst + i)>0 && pre_cnt<=20){
          pre_cnt++;
          printf("%f\n", *(_dst + i));
      }
  }
  printf("checked\n");*/

}

int32_t get_pos(int32_t x, int32_t border) {
    x = min_I32(x, ((border - 1) << 1) - x);
    x = abs(x);
    return x;
}

void box_filter(float* src,float* dst, int32_t block_size, int32_t src_rows, int32_t src_cols) {


  int32_t sum_height = src_rows + 2 * block_size;
  int32_t sum_width = src_cols + 2 * block_size;
  int32_t height = src_rows;
  int32_t width = src_cols;
  
  float *sum_data_ptr = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, sum_height*sum_width* sizeof(float), 32);

  const float *src_data = src;

  float *sum_data = sum_data_ptr;
  int32_t iter = 0;
  int32_t x,y;
  for (y = 0; y < sum_height; ++y) {
    int32_t py = get_pos(y - block_size, height);
    for (x = 0; x < sum_width; ++x) {
      int32_t px = get_pos(x - block_size, width);
      float sum = *(src_data + py * width + px);
      if (y > 0)
        sum += *(sum_data + iter - sum_width);
      if (x > 0)
        sum += *(sum_data + iter - 1);
      if (y > 0 && x > 0)
        sum -= *(sum_data + iter - sum_width - 1);
      *(sum_data + iter) = sum;
      iter++;
    }
  }
  
  float *dst_data = dst;
  int32_t block_size2 = 2 * block_size;
  for (y = 0; y < height; ++y) {
    int32_t up_y = y + block_size2;
    int dn_y = y - 1;
    const float *up_border_sum = sum_data + up_y * sum_width;
    const float *dn_border_sum = sum_data + dn_y * sum_width;
    for (x = 0; x < width; ++x) {
      int32_t up_x = x + block_size2;
      int32_t dn_x = x - 1;
      float sum = *(up_border_sum + up_x);
      if (up_y > block_size2)
        sum -= *(dn_border_sum + up_x);
      if (up_x > block_size2)
        sum -= *(up_border_sum + dn_x);
      if (up_y > block_size2 && up_x > block_size2)
        sum += *(dn_border_sum + dn_x);
      *dst_data++ = sum;
    }
  }

  /*int32_t pre_cnt = 0;
  printf("check boxfilter\n");
  for(y = 0; y < src_rows*src_cols; y++){
      if(*(dst + y)>0 && pre_cnt<=20){
          pre_cnt++;
          printf("%f\n", *(dst + y));
      }
  }
  printf("checked\n");*/

  Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, sum_data_ptr, sum_width*sum_height*sizeof(float));

}

void get_eigen_value_vec( uint8_t *src, float* eigen_values, int block_size, int32_t src_rows, int32_t src_cols) {

  int32_t src_size = src_rows * src_cols;
  int32_t i;
  float *cov = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, 3*src_size* sizeof(float), 32);
  memset(cov, 0.0f, 3*src_size* sizeof(float));
  float *Dx = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, src_size* sizeof(float), 32);
  memset(Dx, 0.0f, src_size* sizeof(float));
  float *Dy = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, src_size* sizeof(float), 32);
  memset(Dy, 0.0f, src_size* sizeof(float));

  get_gradient(src_rows, src_cols, block_size, src, Dx, Dy);

  float *cov0_data = cov;
  float *cov1_data = cov + src_size;
  float *cov2_data = cov + 2*src_size;
  float *dy_data = Dy;
  float *dx_data = Dx;
  
  for (i = 0; i < src_size; i++) {

    float dy = dy_data[i];
    float dx = dx_data[i];

    *(cov0_data + i) = dx * dx;
    *(cov1_data + i) = dx * dy;
    *(cov2_data + i) = dy * dy;
  }

  for (i = 0; i < 3; ++i) {
    box_filter(cov + i*src_size, cov + i*src_size, 3, src_rows, src_cols);
  }

  //printf("before calc_eigen\n");
  calc_min_eigenvalue(cov, eigen_values, src_rows, src_cols);

  Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, cov, 3*src_size*sizeof(float));
  Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, Dx, src_size*sizeof(float));
  Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, Dy, src_size*sizeof(float));

}

int32_t good_features_to_track(uint8_t* src, float* dst, int32_t block_size, int32_t min_distance, int32_t points_num, int32_t src_rows, int32_t src_cols) {

  int32_t src_size = src_rows*src_cols;
  //printf("src_rows, src_cols are : %d, %d\n", src_rows, src_cols);
  int32_t top_k = 0;
  int32_t i;
  float *eigen_value_vector = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, src_size* sizeof(float), 32);
  memset(eigen_value_vector, 0.0f, src_size*sizeof(float));
  float *eigen_value_vector_cp = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, src_size* sizeof(float), 32);
  float *loc_vector = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, src_size*2*sizeof(float), 32);
  for(i = 0; i < src_size; i++){
      *(loc_vector + i*2) = i/src_cols;
      *(loc_vector + i*2 + 1) = i%src_cols;
  }

  //printf("before get eigen\n");
  get_eigen_value_vec(src, eigen_value_vector, block_size, src_rows, src_cols);
  memcpy(eigen_value_vector_cp, eigen_value_vector, src_size*sizeof(float));
  //printf("after get eigen\n");

  // local max in 3x3 neighborhood && bigger than threshold
  // get max eigen value
  float max_eigen_value = -1;
  float threshold = max_eigen_value * 0.05;

  for(i = 0; i < src_rows*src_cols; i++){
      if(*(eigen_value_vector + i) > max_eigen_value)
          max_eigen_value = *(eigen_value_vector + i); 
  }

  float *tmp_good_features = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, src_size*2*sizeof(float), 32);
  memset(tmp_good_features, 0.0f, top_k*2*sizeof(float));
  int32_t tmp_good_ptr = 0;
  for (i = 0; i < src_size; i++) {
    if (eigen_value_vector[i] > threshold && is_max_eigen_point_neibor(eigen_value_vector, i, src_rows, src_cols)) {
      *(tmp_good_features + tmp_good_ptr) = i/src_cols;
      *(tmp_good_features + tmp_good_ptr + 1) = i%src_cols;
      *(eigen_value_vector_cp + tmp_good_ptr/2) = *(eigen_value_vector + i);
      tmp_good_ptr += 2;
    }
  }
  //printf("check tmp_good_featrues\n");
  //for(i = 0; i < 20; i++)
      //printf("%f %f\n", *(tmp_good_features + i*2), *(tmp_good_features + i*2 + 1));
  //printf("checked\n");

  //top_k
  top_k = tmp_good_ptr/2;
  //printf("top_k is : %d\n", top_k);
  if(top_k == 0)
      return 0;

  // select eigen values that more than a specific threshold
  // sort by eigen values
  sort_by_heap_withloc_f(eigen_value_vector_cp, tmp_good_features, top_k, top_k);

  //printf("sorted tmp points\n");
  //for(i = 0; i < 20; i++)
      //printf("%f %f\n", *(tmp_good_features + i*2), *(tmp_good_features + i*2 + 1));
  //printf("checked\n");
  /*printf("check eigen\n");
  for(i = 0; i < top_k; i++)
      printf("%f|\n", *(eigen_value_vector_cp + i));
  printf("checked\n");
  printf("check loc\n");
  for(i = 0; i < top_k; i++)
      printf("(%f, %f)|\n", *(loc_vector + i*2), *(loc_vector + i*2 + 1));
  printf("checked\n");*/

  

  // minimum distance between any pair of pixels is larger than a given
  // threshold
  int32_t min_distance2 = min_distance * min_distance;
  int32_t first_invalid_index = 0;
  int32_t max_number = points_num;
  for (i = 0; i < tmp_good_ptr/2; ++i) {
    uint8_t valid = 1;
    int32_t j;
    for (j = 0; j < first_invalid_index; ++j) {
      int dis_y = tmp_good_features[i*2] - tmp_good_features[j*2];
      int dis_x = tmp_good_features[i*2 + 1] - tmp_good_features[j*2 +1];
      if (dis_x * dis_x + dis_y * dis_y < min_distance2) {
        valid = 0;
        break;
      }
    }
    if (valid){
      swap_f(tmp_good_features + first_invalid_index*2, tmp_good_features + i*2);
      swap_f(tmp_good_features + first_invalid_index*2 + 1 , tmp_good_features + i*2 + 1);
      first_invalid_index++;
    }
    if (first_invalid_index >= max_number)
      break;
  }
  
  for(i = 0; i < first_invalid_index; i++){
    *(dst + i*2) = *(tmp_good_features + i*2);
    *(dst + i*2 + 1) = *(tmp_good_features + i*2 + 1);
  }
  Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, eigen_value_vector, src_size*sizeof(float));
  Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, eigen_value_vector_cp, src_size*sizeof(float));
  Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, loc_vector, src_size*2*sizeof(float));
  Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, tmp_good_features, top_k*2*sizeof(float));

  return first_invalid_index;
}



int32_t VLIB_harrisScore_7x7_f_diy(const int16_t *restrict gradX,
                               const int16_t *restrict gradY,
                               int32_t width,
                               int32_t height,
                               VLIB_F32 *outm,
                               VLIB_F32 k,
                               VLIB_F32 *buffer)
{

    int32_t     x, y;
    VLIB_F32    zxx;
    VLIB_F32    zxy;
    VLIB_F32    zyy;
    VLIB_F32    d;
    VLIB_F32    trace;
    int32_t PWID = 7;

    /* for all rows of the input image */
    for( y = PWID / 2 + 1; y < height - PWID / 2 - 1; y++ ) {
        /* for all columns of the input image */
        for( x = PWID / 2 + 1; x < width - PWID / 2 - 1; x++ ) {
            int32_t    i, j;

            zxx = 0;
            zxy = 0;
            zyy = 0;

            /* accumulate over a PWID x PWID patch around the current pixel */
            for( i = -PWID / 2; i <= PWID / 2; i++ ) {
                for( j = -PWID / 2; j <= PWID / 2; j++ ) {
                    VLIB_F32    dx = (VLIB_F32) gradX[(y + i) * width + (x + j)];
                    VLIB_F32    dy = (VLIB_F32) gradY[(y + i) * width + (x + j)];

                    zxx += dx * dx;
                    zxy += dx * dy;
                    zyy += dy * dy;
                }
            }

            d = (VLIB_F32)zxx * zyy - (VLIB_F32)zxy * zxy;
            trace = (VLIB_F32) zxx + zyy;
            outm[y * width + x] = d - k * trace * trace;

        } /* all columns */

    } /* all rows */

    return 0;
}


void VLIB_goodFeaturestoTrack_diy
(
    const uint16_t* im,
    uint8_t* out,
    int32_t width,
    int32_t height,
    uint32_t thresh,
    int32_t patch,
    uint8_t qualitypar,
    int16_t topNfeatures,
    uint8_t mindist,
    uint16_t* outTemp,
    int32_t *good_points_number,
    int16_t* pixIndex_gft_cn,
    uint16_t* internalBuf_cn,
    int32_t* ind_cn
)
{
    int32_t    x_gft, y_gft, i_gft, m_gft, n_gft, nPoints = 0;
    uint16_t   v,maxScore = 0;
    int32_t    points_above_quality_cnt, cnt, final_point_count = 0;
    uint8_t    mid_mindist = mindist >> 1;
    int32_t    threshold_gft, thr = 100;
    int16_t    offset_gft;
 
    *good_points_number = topNfeatures;
 
    VLIB_nonMaxSuppress_U16(im, width, height, patch, thresh, internalBuf_cn,
                               pixIndex_gft_cn);
    /* obtain quality parameter threshold to guarantee topNfeatures */
 
    maxScore = 0;
 
    nPoints = pixIndex_gft_cn[0];
 
    for( i_gft=2; i_gft < ((nPoints * 2) + 2); i_gft+=2 ) {
        offset_gft = (pixIndex_gft_cn[i_gft] * (int32_t)width) + pixIndex_gft_cn[i_gft + 1];
        outTemp[offset_gft] = im[offset_gft];
        if( maxScore < outTemp[offset_gft] ) {
            maxScore = outTemp[offset_gft];
        }
    }

 
    points_above_quality_cnt = 0;
    threshold_gft = (int32_t)qualitypar * (int32_t)maxScore;
    /*  ind_cn = (int32_t *) malloc(((height-((int32_t)patch-1)) * (width-((int32_t)patch-1))) * sizeof(int32_t));*/
    cnt = 0;
 
    for( i_gft=2; i_gft < ((nPoints * 2) + 2); i_gft+=2 ) {
        offset_gft = (pixIndex_gft_cn[i_gft] * (int32_t)width) + pixIndex_gft_cn[i_gft + 1];
        v = outTemp[offset_gft];
        if( v ) {
            if((((int32_t)thr * (int32_t) v) >= (int32_t)threshold_gft)) {
                /* prune points using qualitypar */
                points_above_quality_cnt++;
                ind_cn[cnt] = (offset_gft);
                cnt+= 1;
            }
        }
    }
 
    sort_ind_cn(outTemp, points_above_quality_cnt, ind_cn);
    /* Spatial diversity */
    if( points_above_quality_cnt > topNfeatures ) {
        for( y_gft=0; y_gft < points_above_quality_cnt; y_gft++ ) {
            int32_t     temp_in=ind_cn[y_gft];
            uint16_t    v_t =outTemp[temp_in];
            int32_t     xOfst = temp_in % width;
            int32_t     yOfst = temp_in / width;
 
            /* check boundaries */
            int32_t    hSt =  (xOfst < mid_mindist) ? mid_mindist - xOfst : 0;
            int32_t    hEnd = (xOfst > (width - mid_mindist)) ? mindist - (width - xOfst) : mindist;
            int32_t    vSt =  (yOfst < mid_mindist) ? mid_mindist - yOfst : 0;
            int32_t    vEnd = (yOfst > (height - mid_mindist)) ? mindist - (height - yOfst) : mindist;
            cnt=0;
 
            for( n_gft = vSt; n_gft < (int32_t) vEnd; n_gft++ ) {
                for( m_gft = hSt; m_gft < (int32_t) hEnd; m_gft++ ) {
                    i_gft = ind_cn[y_gft] + ((n_gft - (int32_t)mid_mindist) * width) + (m_gft - (int32_t)mid_mindist);
                    if( outTemp[i_gft] > (uint16_t)v_t ) {
                        cnt++;
                    }
                }
            }
 
            if( !cnt ) {
                out[ind_cn[y_gft]] = (uint8_t)255;
                final_point_count++;
            }
 
            if((final_point_count < topNfeatures) && (y_gft == (points_above_quality_cnt - 1))) {
                /*discard spatial diversity*/
                int32_t discard_cnt = 0;
                for( x_gft=0; x_gft < topNfeatures; x_gft++ ) {
                    if(out[ind_cn[x_gft]] != 255)
                      discard_cnt++;
                    out[ind_cn[x_gft]] = (uint8_t)255;
                }
            }
            if( final_point_count == topNfeatures ) {
                break;
            }
        }
    } else {
        /*include all points regardless of spatial diversity*/
        for( y_gft = 0; y_gft < points_above_quality_cnt; y_gft++ ) {
            out[ind_cn[y_gft]] =(uint8_t) 255;
        }
 
        *good_points_number = points_above_quality_cnt;
    }
}


/***************************************************************************************/
 

void update_car_previous_frame(TBKTracker* tracker, uint8_t* img, int32_t img_width, int32_t img_height){

    //set points init parameters
    int32_t CORNER_POINT_BLOCKSIZE = 7;
    //int16_t CORNER_POINTS_LIMIT = 40;
    //uint8_t CORNER_POINT_QUALITY_LEVEL = 5;//5%
    uint8_t CORNER_POINT_MIN_DISTANCE = 7;
    //uint32_t CORNER_POINT_THRESH = 24000;
    //float CORNER_POINT_THRESH_f = 0.0f;
    //uint16_t HARRIS_K = 1310;

    float x_scale = 0;
    float y_scale = 0;
    int32_t resize_width = 0;
    int32_t resize_height = 0;
 
    //printf()
    get_car_resized_roi_img(tracker, img, img_width, img_height, tracker->pre_resized_roi_frame, &x_scale,
                            &y_scale, &resize_width, &resize_height);
    //printf("after get resized roi\n");

    tracker->pre_width = resize_width;
    tracker->pre_height = resize_height;

    //clear pre points
    //printf("before memset points\n");
    if(tracker->points != NULL)
      memset(tracker->points, 0, sizeof(float)*100*2);
    //printf("after memset points\n");

    /************************* goodfeatruestotrack*******************************/
    int32_t required_points_num = 40;
    float *good_features = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, 2*required_points_num*sizeof(float), 32);
    memset(good_features, 0.0f, 2*required_points_num*sizeof(float));
    printf("before diy points\n");
    int32_t points_num = good_features_to_track(tracker->pre_resized_roi_frame, good_features, CORNER_POINT_BLOCKSIZE, CORNER_POINT_MIN_DISTANCE, required_points_num, resize_height, resize_width);
    printf("after diy points\n");
    tracker->points_num = points_num;
    if(points_num == 0)
        return;
    int32_t i;
    for(i = 0; i < points_num; i++){
        *(tracker->points + i*2) = *(good_features + i*2 + 1);
        *(tracker->points + i*2 + 1) = *(good_features + i*2);
    }
    /*printf("check good features\n");
    int32_t gf;
    for(gf = 0; gf < required_points_num; gf++)
        printf("(x, y) : (%f, %f)\n", *(good_features + gf*2), *(good_features + gf*2 + 1));
    printf("checked\n");*/
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, good_features, 2*required_points_num*sizeof(uint16_t));

    /****************************************************************************/

    /***************************************VLIB features**********************/
    /*// Compute buffer sizes 
    uint32_t inp_size         =   resize_width*resize_height;
    int32_t grad_XY_size     =   resize_width*resize_height;
    uint32_t pixIndex_size    =   inp_size * 2 + 2;
    uint32_t internalBuf_size =   inp_size + (2 * CORNER_POINT_BLOCKSIZE);

    // Allocate buffers for each test vector 
    uint8_t *img_ori = (uint8_t *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, inp_size * sizeof(uint8_t), 32);
    memset(img_ori, 0, inp_size * sizeof(uint8_t));
    uint16_t *im_input = (uint16_t *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, inp_size * sizeof(uint16_t), 32);
    memset(im_input, 0, inp_size * sizeof(uint16_t));
    float *im_input_f = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, inp_size * sizeof(float), 32);
    memset(im_input_f, 0.0f, inp_size * sizeof(float));
    uint16_t *im_input_cp = (uint16_t *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, inp_size * sizeof(uint16_t), 32);
    float *im_input_f_cp = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, inp_size * sizeof(float), 32);
    memset(im_input_f_cp, 0.0f, inp_size * sizeof(float));
    int16_t *im_x = (int16_t *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, grad_XY_size * sizeof(int16_t), 32);
    memset(im_x, 0, grad_XY_size * sizeof(int16_t));
    int16_t *im_x_solo = (int16_t *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, grad_XY_size * sizeof(int16_t), 32);
    memset(im_x, 0, grad_XY_size * sizeof(int16_t));
    int16_t *im_y = (int16_t *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, grad_XY_size * sizeof(int16_t), 32);
    memset(im_y, 0, grad_XY_size * sizeof(int16_t));

    float *im_x_f = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, grad_XY_size * sizeof(float), 32);
    memset(im_x_f, 0, grad_XY_size * sizeof(float));
    float *im_y_f = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, grad_XY_size * sizeof(float), 32);
    memset(im_y_f, 0, grad_XY_size * sizeof(float));
    //uint8_t *scratch = (uint8_t *) VLIB_memalign(4, 96 * resize_width);
    uint8_t *scratch = (uint8_t *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, 96 * resize_width, 32);
    float *scratch_f = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, 96 * resize_width*sizeof(float), 32);
    uint8_t *img_output = (uint8_t *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, inp_size * sizeof(uint8_t), 32);
    memset(img_output, 0, inp_size * sizeof(uint8_t));
    uint16_t *outTemp = (uint16_t *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, inp_size * sizeof(uint16_t), 32);
    memset(outTemp, 0, inp_size * sizeof(uint16_t)); 
    int16_t *pixIndex = (int16_t *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, pixIndex_size * sizeof(int16_t), 32);
    memset(pixIndex, 0, pixIndex_size * sizeof(int16_t));
    uint16_t *internalBuf = (uint16_t *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, internalBuf_size * sizeof(uint16_t), 32);
    memset(internalBuf, 0, internalBuf_size * sizeof(uint16_t));
    int32_t *ind = (int32_t *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, inp_size * sizeof(int32_t), 32);
    memset(ind, 0, inp_size * sizeof(int32_t));
    int32_t  good_points_number_opt;

    //printf("compute grad\n");
    //VXLIB_bufParams2D_t  src_addr, dst_addr_x, dst_addr_y;
    //src_addr.dim_x = resize_width;
    //src_addr.dim_y = resize_height;
    //src_addr.stride_y = resize_width;
    //src_addr.data_type = VXLIB_UINT8;

    //dst_addr_x.dim_x = resize_width - 2;
    //dst_addr_x.dim_y = resize_height - 2;
    //dst_addr_x.stride_y = (resize_width - 2)*2;
    //dst_addr_x.data_type = VXLIB_INT16;

    //dst_addr_y.dim_x = resize_width - 2;
    //dst_addr_y.dim_y = resize_height - 2;
    //dst_addr_y.stride_y = (resize_width - 2)*2;
    //dst_addr_y.data_type = VXLIB_INT16;

    //compute grad X Y map
    memcpy(img_ori, tracker->pre_resized_roi_frame, inp_size * sizeof(uint8_t));
    printf("before VLIB points\n");
    VLIB_xyGradients(tracker->pre_resized_roi_frame, im_x + resize_width + 1, im_y + resize_width + 1, resize_width, resize_height);
    //uint8_t status = 1;
    //status = VXLIB_sobel_3x3_i8u_o16s_o16s(img, &src_addr, im_x, &dst_addr_x, im_y, &dst_addr_y);
    //uint8_t success = (status == VXLIB_SUCCESS);
    //printf("%d\n", success);
    
    //status = VXLIB_sobelX_3x3_i8u_o16s(img, &src_addr, im_x_solo, &dst_addr_x);
    //success = (status == VXLIB_SUCCESS);
    //printf("%d\n", success);
    //printf("check sobel x\n");
    //for(i_sobel = 0; i_sobel < grad_XY_size; i_sobel++){
        //if(im_x_solo[i_sobel] != 0)
            //printf("(%d, %d) : %d\n", i_sobel/(resize_height - 2), i_sobel%(resize_height - 2), im_x_solo[i_sobel]);
    //}
    //printf("checked\n");


    //compute harris map
    VLIB_harrisScore_7x7(im_x, im_y, resize_width, resize_height, (int16_t *)im_input, HARRIS_K, scratch);

    memset(im_input_cp, 0, inp_size * sizeof(uint16_t));
    memcpy(im_input_cp, im_input, inp_size * sizeof(uint16_t));
    int32_t top_k = 60;
    
    sort_by_heap(im_input_cp, top_k, inp_size);
    CORNER_POINT_THRESH = im_input_cp[top_k -1];
    int32_t sort_i;
    int32_t sort_cnt = 0;
    for(sort_i = 0; sort_i < inp_size; sort_i++){
        if(im_input_cp[sort_i] > CORNER_POINT_THRESH)
            sort_cnt++;
    }

    //compute harris map
    VLIB_harrisScore_7x7_f_diy(im_x, im_y, resize_width, resize_height, im_input_f, 0.04, scratch_f);
    memcpy(im_input_f_cp, im_input_f, inp_size * sizeof(float));
    sort_by_heap_f(im_input_f_cp, top_k, inp_size);
    CORNER_POINT_THRESH_f = im_input_f_cp[top_k -1];
    for(sort_i = 0; sort_i < inp_size; sort_i++){
        if(im_input_f_cp[sort_i] > CORNER_POINT_THRESH_f)
            sort_cnt++;
    }

    VLIB_goodFeaturestoTrack_diy(im_input, img_output, resize_width, resize_height,
                                     CORNER_POINT_THRESH, CORNER_POINT_BLOCKSIZE, CORNER_POINT_QUALITY_LEVEL,
                                     CORNER_POINTS_LIMIT, CORNER_POINT_MIN_DISTANCE,
                                     outTemp,
                                     &good_points_number_opt,
                                     pixIndex,
                                     internalBuf,
                                     ind);

    printf("after VLIB points\n");
    tracker->points_num = good_points_number_opt;
    printf("points num is : %d\n", good_points_number_opt);
    //uint8_t* img_ptr = img_output;
    float* points_ptr = tracker->points;

    //printf("check feature points\n");
    int32_t outim_stride;
    int32_t check_points_num = 0;
    //printf("resize width, height are: %d %d \n", resize_width, resize_height);
    for(outim_stride = 0; outim_stride < resize_height; outim_stride++){
      int32_t outim_col;
      for(outim_col = 0; outim_col < resize_width; outim_col++){
        if(*(img_output + outim_stride*resize_width + outim_col) < 200)
          continue;
        *(points_ptr) = (float)outim_col;
        *(points_ptr + 1) = (float)outim_stride;
        //printf("(%d , %d) : %d : %f|%f \n", outim_col, outim_stride, *(img_output + outim_stride*resize_width + outim_col), *(points_ptr), *(points_ptr + 1));
        points_ptr += 2;
        check_points_num++;
      }
    }

    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, im_input, inp_size * sizeof(uint16_t));
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, im_input_cp, inp_size * sizeof(uint16_t));
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, im_x, grad_XY_size * sizeof(uint16_t));
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, im_x_f, grad_XY_size * sizeof(float));
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, im_y_f, grad_XY_size * sizeof(float));
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, im_y, grad_XY_size * sizeof(uint16_t));
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, img_output, inp_size * sizeof(uint8_t));
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, outTemp, inp_size * sizeof(uint16_t));
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, pixIndex, pixIndex_size * sizeof(uint16_t));
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, internalBuf, internalBuf_size * sizeof(uint16_t));
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, ind, inp_size * sizeof(int32_t));
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, scratch, 96 * resize_width);*/
    /***************************************************************************************************/
}

uint8_t get_mapping_points_between_two_frames(TBKTracker* tracker, uint8_t* pre_img, uint8_t* cur_img, float *new_float_points, uint8_t* tracking_status){

    float DIMENTION_ABS_THRESHOLD = 0.7;
    float DISTANCE_THRESHOLD = 1.0;
    float MODEL_POINTS_LIMIT = 4;
    uint8_t PYRAMIDAL_LEVELS = 2;
    float HOMOGRAPHY_THRESHOLD = 0.8;
    float PESPECTIVE_MATRIX_VALUE_LIMIT = 6.0;
    uint8_t BLOCK_SIZE = 15;
    uint32_t OPTICAL_LOOP_LIMIT = 1000;
    float OPTICAL_EPS = 0.5;
    float SCALE_FACTOR = 1;

    //init
    VXLIB_STATUS status_nat = VXLIB_SUCCESS;
    VXLIB_bufParams2D_t old_addr, new_addr, gradX_addr, gradY_addr;

    /* Allocate buffers for each test vector */
    int16_t *gradX = (int16_t *)Utils_memAlloc( UTILS_HEAPID_DDR_CACHED_SR, tracker->pre_width*2*tracker->pre_height * sizeof(int16_t), 32);
    memset(gradX, 0, tracker->pre_width*2*tracker->pre_height * sizeof(int16_t));
    int16_t *gradY = (int16_t *)Utils_memAlloc( UTILS_HEAPID_DDR_CACHED_SR, tracker->pre_width*2*tracker->pre_height * sizeof(int16_t), 32);
    memset(gradY, 0, tracker->pre_width*2*tracker->pre_height * sizeof(int16_t));
    __float2_t *old_points = (__float2_t *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, tracker->points_num * sizeof(__float2_t), 32);
    __float2_t *new_points = (__float2_t *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, tracker->points_num * sizeof(__float2_t), 32);
    //int16_t *gradX = (int16_t *)malloc(tracker->pre_width*2*tracker->pre_height * sizeof(int16_t));
    //int16_t *gradY = (int16_t *)malloc(tracker->pre_width*2*tracker->pre_height * sizeof(int16_t));
    //__float2_t *old_points = (__float2_t *)malloc(tracker->points_num * sizeof(__float2_t));
    //__float2_t *new_points = (__float2_t *)malloc(tracker->points_num * sizeof(__float2_t));

    /* scratch buffer required :
     *    optimized = window_size*window_size*6 + 2 bytes.
     *    natural c = window_size*window_size*8
     */
    uint32_t scratch_size = BLOCK_SIZE*BLOCK_SIZE*8 + 2;
    uint8_t *scratch = (uint8_t *)Utils_memAlloc( UTILS_HEAPID_DDR_CACHED_SR, scratch_size, 32);
    memset(scratch, 0, scratch_size);
    //uint8_t *scratch = (uint8_t *)malloc(scratch_size);

    //transform points
    float *points_float_ptr = tracker->points;
    int32_t points_idx;
    for(points_idx = 0; points_idx < (tracker->points_num); points_idx++){
      old_points[points_idx] = _ftof2((VXLIB_F32)(*(points_float_ptr + 1)), (VXLIB_F32)(*(points_float_ptr)));
      points_float_ptr += 2;
    }
    //check points package
    /*float* check_trans_new = (float *)old_points;
    uint32_t check_points_idx = 1;
    for(check_points_idx = 0; check_points_idx < tracker->points_num; check_points_idx++){
      printf("ptr:%f|%f\n", *check_trans_new, *(check_trans_new + 1));
      check_trans_new += 2;
      VXLIB_F32 oldXf = _lof2(old_points[check_points_idx]);
      VXLIB_F32 oldYf = _hif2(old_points[check_points_idx]);   
      printf("unpack:%f|%f\n", oldXf, oldYf);
    }
    printf("points checked\n");*/

    //guss new points
    points_float_ptr = tracker->points;
    for(points_idx = 0; points_idx < tracker->points_num; points_idx++){
      new_points[points_idx] = _ftof2((VXLIB_F32)(*(points_float_ptr + 1)), (VXLIB_F32)(*(points_float_ptr)));
      points_float_ptr += 2;
    }

    old_addr.dim_x = tracker->pre_width;
    old_addr.dim_y = tracker->pre_height;
    old_addr.stride_y = tracker->pre_width;
    old_addr.data_type = VXLIB_UINT8;

    new_addr.dim_x = tracker->pre_width;
    new_addr.dim_y = tracker->pre_height;
    new_addr.stride_y = tracker->pre_width;
    new_addr.data_type = VXLIB_UINT8;

    gradX_addr.dim_x = tracker->pre_width;
    gradX_addr.dim_y = tracker->pre_height-2;
    gradX_addr.stride_y = tracker->pre_width*2;
    gradX_addr.data_type = VXLIB_INT16;

    gradY_addr.dim_x = tracker->pre_width;
    gradY_addr.dim_y = tracker->pre_height-2;
    gradY_addr.stride_y = tracker->pre_width*2;
    gradY_addr.data_type = VXLIB_INT16;
    int32_t i;

    status_nat = VXLIB_scharr_3x3_i8u_o16s_o16s_cn(pre_img, &old_addr,
                                                           gradX + tracker->pre_width/2 + 1, &gradX_addr,
                                                           gradY + tracker->pre_width/2 + 1, &gradY_addr);

    if(status_nat != VXLIB_SUCCESS)
        tracker->track_on = false;

    //find the points
    status_nat = VXLIB_trackFeaturesLK_i8u_diy(pre_img, &old_addr, cur_img, &new_addr, gradX, &gradX_addr, gradY, &gradY_addr,
                                                            old_points, new_points, tracking_status, (uint32_t)tracker->points_num, OPTICAL_LOOP_LIMIT, OPTICAL_EPS,
                                                            SCALE_FACTOR, BLOCK_SIZE, PYRAMIDAL_LEVELS, VXLIB_TERM_EPSILON,
                                                            scratch, scratch_size);
    printf("calc flow is success ? %d\n", status_nat == VXLIB_SUCCESS);
    if(status_nat != VXLIB_SUCCESS)
        tracker->track_on = false;


    //printf("check scratch\n");
    //for(i = 0; i < scratch_size; i++){
        //printf("%d ", *(scratch + i));
    //}
    //printf("\nchecked\n");

    int32_t track_count = 0;
    //printf("check 1st tracking status\n");
    for(i = 0; i < tracker->points_num; i++){
      //printf("%d ", tracking_status[i]);
      if(tracking_status[i] == 1){
        track_count++;
      }else{
        tracking_status[i] = 1;
      }
    }
    //printf("\nchecked, 1st tracking status num are : %d\n", track_count);*/
    if (track_count < MODEL_POINTS_LIMIT )
      return false;

    /*printf("check points calced before fb\n");
    float* check_new = (float *)new_points;
    int32_t check_idx;
    for(check_idx = 0; check_idx < tracker->points_num; check_idx++){
      printf("ptr:%f|%f\n", *check_new, *(check_new + 1));
      check_new += 2;
      VXLIB_F32 newXf = _lof2(new_points[check_idx]);
      VXLIB_F32 newYf = _hif2(new_points[check_idx]);
      printf("unpack:%f|%f\n", newXf, newYf);

    }
    printf("calced points checked\n");*/

    //FB method
    status_nat = VXLIB_SUCCESS;
    __float2_t *ver_points = (__float2_t *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, tracker->points_num * sizeof(__float2_t), 32);
    //__float2_t *ver_points = (__float2_t *)malloc(tracker->points_num * sizeof(__float2_t));
    points_float_ptr = tracker->points;

    for(points_idx = 0; points_idx < tracker->points_num; points_idx++){
      ver_points[points_idx] = _ftof2((VXLIB_F32)(*(points_float_ptr + 1)), (VXLIB_F32)(*(points_float_ptr)));
      points_float_ptr += 2;
    }

    memset(gradX, 0,tracker->pre_width*tracker->pre_height* sizeof(int16_t));
    memset(gradY, 0,tracker->pre_width*tracker->pre_height* sizeof(int16_t));
    uint8_t *scratch_ver = (uint8_t *)Utils_memAlloc( UTILS_HEAPID_DDR_CACHED_SR, scratch_size, 32);
    //uint8_t *scratch_ver = (uint8_t *)malloc(scratch_size);

    status_nat = VXLIB_scharr_3x3_i8u_o16s_o16s_cn(cur_img, &new_addr,
                                                           gradX + tracker->pre_width/2 + 1, &gradX_addr,
                                                           gradY + tracker->pre_width/2 + 1, &gradY_addr);
    status_nat = VXLIB_trackFeaturesLK_i8u(cur_img, &new_addr, pre_img, &old_addr, gradX, &gradX_addr, gradY, &gradY_addr,
                                                            new_points, ver_points, tracking_status, tracker->points_num, OPTICAL_LOOP_LIMIT, OPTICAL_EPS,
                                                            SCALE_FACTOR, BLOCK_SIZE, PYRAMIDAL_LEVELS, VXLIB_TERM_BOTH, scratch_ver, scratch_size);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, gradX, tracker->pre_width*2*tracker->pre_height * sizeof(int16_t));
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, gradY, tracker->pre_width*2*tracker->pre_height * sizeof(int16_t));
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, scratch, scratch_size);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, scratch_ver, scratch_size);
    //free(gradX);
    //free(gradY);
    //free(scratch);
    //free(scratch_ver);

    // verify if a good point
    float* ver_float_points = (float *)Utils_memAlloc( UTILS_HEAPID_DDR_CACHED_SR, tracker->points_num * 2 * sizeof(float), 32);
    //float* ver_float_points = (float *)malloc(tracker->points_num * 2 * sizeof(float));
    float* ver_ptr = ver_float_points;
    float* trans_ver = (float *)(ver_points);
    for(points_idx = 0; points_idx < tracker->points_num; points_idx++){
      *ver_ptr =  (*trans_ver + 1);
      *(ver_ptr + 1) = *trans_ver;
      ver_ptr += 2;
      trans_ver += 2;
    }

    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, ver_points, tracker->points_num * sizeof(__float2_t));
    //free(ver_points);

    ver_ptr = ver_float_points;
    float* old_ptr = tracker->points;
    for(points_idx = 0; points_idx < tracker->points_num; points_idx++){
      float dis = sqrt( ((*ver_ptr) - (*old_ptr))*((*ver_ptr) - (*old_ptr)) + ((*ver_ptr + 1) - (*old_ptr + 1))*((*ver_ptr + 1) - (*old_ptr + 1)));
      if(dis <= DISTANCE_THRESHOLD){tracking_status[points_idx] = 2;}
      else{tracking_status[points_idx] = 0;}
      ver_ptr += 2;
      old_ptr += 2;
    }

    track_count = 0;
    for(i = 0; i < tracker->points_num; i++){
      if(tracking_status[i] == 2){
        track_count++;
      }
    }

    //re-construct pre_points
    float* pre_ptr = tracker->points;
    for(i = 0; i < tracker->points_num; i++){
        float tmp = *(pre_ptr);
        *(pre_ptr) = *(pre_ptr + 1);
        *(pre_ptr + 1) = tmp;
        pre_ptr += 2;
    }

    //printf("check calced and pre points\n");
    float* new_ptr = new_float_points;
    float* trans_new = (float *)new_points;
    pre_ptr = tracker->points;
    for(points_idx = 0; points_idx < tracker->points_num; points_idx++){
      *new_ptr =   *(trans_new + 1);
      *(new_ptr + 1) =  *trans_new;
      //printf("(%f, %f) -> (%f, %f)\n", *(pre_ptr), *(pre_ptr + 1), *new_ptr, *(new_ptr + 1));
      new_ptr += 2;
      trans_new += 2;
      pre_ptr += 2;
    }
    //printf("checked\n");

    //###############################################(to be checked)############################
    //if (track_count <= MODEL_POINTS_LIMIT) {
      //return false;
    //}
    //######################################################################################

    // calc transformation matraix for cleaning
    float V_H[3][3] = { 1.0, 0.0f , 0.0f , 0.0f , 1.0, 0.0f, 0.0f, 0.0f, 1.0f};
    printf("before findhomo\n");
    findHomography(tracker ,tracker->points, new_float_points, HOMOGRAPHY_THRESHOLD, V_H);
    printf("after findhomo\n");

    printf("check H matrix\n");
    for(i = 0; i < 9; i++){
        printf("%f| ", V_H[i/3][i%3]);
    }
    printf("\nchecked\n");

    if (V_H[0][2] > PESPECTIVE_MATRIX_VALUE_LIMIT) {
      tracker->track_on = 0;
      printf("V_H wrong\n");
      return false;
    }

    //verification V_H
    float* per_float_points = (float *)Utils_memAlloc( UTILS_HEAPID_DDR_CACHED_SR, tracker->points_num * 2 * sizeof(float), 32);
    //float* per_float_points = (float *)malloc(tracker->points_num * 2 * sizeof(float));
    float* per_ptr = per_float_points;
    old_ptr = tracker->points;
    for(points_idx = 0; points_idx < tracker->points_num; points_idx++){
      *per_ptr = V_H[0][0]*(*(old_ptr)) + V_H[0][1]*(*(old_ptr + 1)) + V_H[0][2];
      *(per_ptr + 1) = V_H[1][0]*(*(old_ptr)) + V_H[1][1]*(*(old_ptr + 1)) + V_H[1][2];
      per_ptr += 2;
      old_ptr += 2;
    }

    // delete bad points that not from the car
    old_ptr = per_float_points;
    new_ptr = new_float_points;
    for (points_idx = 0; points_idx < tracker->points_num; points_idx++) {
      // clean bad points by ransac
      float dx_ran = abs(*new_ptr - *old_ptr);
      float dy_ran = abs(*(new_ptr + 1) - *(old_ptr + 1));
      //uint8_t is_same = *new_ptr == *old_ptr && *(new_ptr + 1) == *(old_ptr + 1);
      if (dx_ran <= DIMENTION_ABS_THRESHOLD &&
          dy_ran <= DIMENTION_ABS_THRESHOLD) {
        tracking_status[points_idx] = 3;
      }
      old_ptr += 2;
      new_ptr += 2;
    }

    //if not track on
    track_count = 0;
    for(i = 0; i < tracker->points_num; i++){
      if(tracking_status[i] == 3){
        track_count++;
      }
    }
    printf(" track count is : %d\n", track_count);
    if (track_count < MODEL_POINTS_LIMIT) {
      tracker->live = 0;
      tracker->track_on = false;
      printf("motion failed\n");
      return false;
    }


    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, old_points, tracker->points_num * sizeof(__float2_t));
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, new_points, tracker->points_num * sizeof(__float2_t));
    //free(old_points);
    //free(new_points);

    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, per_float_points, tracker->points_num * 2 * sizeof(float));
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, ver_float_points, tracker->points_num * 2 * sizeof(float));
    //free(per_float_points);
    //free(ver_float_points);

    return true;

}

void getHmatrix(float pre_sample_points[][2], float cur_sample_points[][2], float affine_matrix[][3]){

    float* A = (float *)Utils_memAlloc( UTILS_HEAPID_DDR_CACHED_SR, 36*sizeof(float), 32);
    //float* per_float_points = (float *)Utils_memAlloc( UTILS_HEAPID_DDR_CACHED_SR, tracker->points_num * 2 * sizeof(float), 32);
    //float* U = (float* )malloc(36*sizeof(float));
    //float* V = (float* )malloc(36*sizeof(float));
    //float* U1 = (float* )malloc(36*sizeof(float));
    //float* diag = (float* )malloc(6*sizeof(float));
    //float* superdiag = (float* )malloc(6*sizeof(float));
    float* b = (float *)Utils_memAlloc( UTILS_HEAPID_DDR_CACHED_SR, 6*sizeof(float), 32);
    //float* b_ = (float* )malloc(6*sizeof(float));
    float* y = (float *)Utils_memAlloc( UTILS_HEAPID_DDR_CACHED_SR, 6*sizeof(float), 32);
 
    memset(A, 0.0f, 36*sizeof(float));
    //memset(U, 0.0f, 36*sizeof(float));  
    //memset(V, 0.0f, 36*sizeof(float));
    //memset(U1, 0.0f, 36*sizeof(float));
    //memset(diag, 0.0f, 6*sizeof(float));
    //memset(superdiag, 0.0f, 6*sizeof(float));
    memset(b, 0.0f, 6*sizeof(float));
    //memset(b_, 0.0f, 6*sizeof(float));
    memset(y, 0.0f, 6*sizeof(float));

    uint32_t rows_idx = 0;
    for(; rows_idx < 3; rows_idx ++){
        A[rows_idx*6] = pre_sample_points[rows_idx][0];
        A[rows_idx*6 + 1] = pre_sample_points[rows_idx][1];
        A[rows_idx*6 + 2] = 1.0f;
        A[(rows_idx+3)*6 + 3] = pre_sample_points[rows_idx][0];
        A[(rows_idx+3)*6 + 4] = pre_sample_points[rows_idx][1];
        A[(rows_idx+3)*6 + 5] = 1;
    }



    for(rows_idx = 0; rows_idx < 3; rows_idx++){
      b[rows_idx] = cur_sample_points[rows_idx][0];
      b[rows_idx + 3] = cur_sample_points[rows_idx][1];
    }

    get_solution(A, y, b, 6, 6);
    for(rows_idx = 0; rows_idx < 6; rows_idx++){
        affine_matrix[rows_idx/3][rows_idx%3] = *(y + rows_idx);
    }
    /*int32_t status; 
    status = DSPF_sp_svd( 6, 6, A, U, V, U1, diag, superdiag);
    //status = DSPF_dp_svd( 6, 6, A, U, V, U1, diag, superdiag);
    if (status==-1) {
    }
    
    for(rows_idx = 0; rows_idx < 6; rows_idx++){
        uint32_t cols_idx = 0;
        for(; cols_idx < 6; cols_idx++){
            b_[rows_idx] += b[cols_idx]*U[cols_idx*6 + rows_idx];
        }
    }

    for(rows_idx = 0; rows_idx < 6; rows_idx++){
        uint32_t cols_idx = 0;
        for(; cols_idx < 6; cols_idx++){
        }
    }
    for(rows_idx = 0; rows_idx < 6; rows_idx++){
        uint32_t cols_idx = 0;
        for(; cols_idx < 6; cols_idx++){
        }
    }

    for(rows_idx = 0; rows_idx < 6; rows_idx++){
        y[rows_idx] = b_[rows_idx]/diag[rows_idx];
    }

    for(rows_idx = 0; rows_idx < 6; rows_idx++){
        uint32_t cols_idx = 0;
        for(; cols_idx < 6; cols_idx++){
            affine_matrix[rows_idx/3][rows_idx%3] += V[rows_idx*6 + cols_idx]*y[cols_idx];
        }
    }*/
    
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, A, 36* sizeof(float));
    //Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, old_points, tracker->points_num * sizeof(__float2_t));
    //free(U);
    //free(V);
    //free(U1);
    //free(diag);
    //free(superdiag);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, b, 6* sizeof(float));
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, y, 6* sizeof(float));
}

void get_affine_matrix(TBKTracker* tracker, float* src,
                            float* dst, uint8_t* tracking_status, int32_t frame_height,
                            int32_t frame_width, float affine_matrix[3][3]){

   // coord transformation
    TBKRect pre_bounding_rect = {0.0f, 0.0f , 0.0f, 0.0f};
    
    pre_bounding_rect.left = (float)(frame_width);
    pre_bounding_rect.right = 0.0f;
    pre_bounding_rect.top = (float)(frame_height);
    pre_bounding_rect.bottom = 0.0f;

    TBKRect cur_bounding_rect = {0.0f, 0.0f, 0.0f, 0.0f};
    cur_bounding_rect.left = (float)(frame_width);
    cur_bounding_rect.right = 0.0f;
    cur_bounding_rect.top = (float)(frame_height);
    cur_bounding_rect.bottom = 0.0f;

    get_bounding_rect(tracker, src, &pre_bounding_rect, tracking_status);
    get_bounding_rect(tracker, dst, &cur_bounding_rect, tracking_status);

    float pre_sample_points[3][2], cur_sample_points[3][2];
    get_rect_sample_points(&pre_bounding_rect, pre_sample_points);
    get_rect_sample_points(&cur_bounding_rect, cur_sample_points);
 
    getHmatrix(pre_sample_points, cur_sample_points, affine_matrix);


    //return cv::getAffineTransform(pre_sample_points, cur_sample_points);
    //affine_matrix[0][0] = 1.0f;
    //affine_matrix[1][1] = 1.0f;
    affine_matrix[2][2] = 1.0f;

}

uint8_t check_subset(float points[][2], uint32_t model_points){

    if(model_points == 3){
        float delta = 0.2;

        if(fabs(points[0][0] - points[1][0]) < delta && fabs(points[0][0] - points[2][0]) < delta && fabs(points[1][0] - points[2][0]) < delta)
            return 0;
        if(fabs(points[0][1] - points[1][1]) < delta && fabs(points[0][1] - points[2][1]) < delta && fabs(points[1][1] - points[2][1]) < delta)
            return 0;

        float k1 = (points[0][0] - points[1][0])/(points[0][1] - points[1][1]);
        float k2 = (points[0][0] - points[2][0])/(points[0][1] - points[2][1]);

        if(fabs(k1 - k2) < delta)
            return 0;
        return 1;
    }
    if(model_points == 4){
        int32_t i0, i1, i, j, k;
        i0 = 0;
        i1 = model_points - 1;
     
        for(i = i0; i <= i1; i++){
            for(j = 0; k < j; k++){
                double dx1 = points[j][0] - points[i][0];
                double dy1 = points[j][1] - points[i][1];
                for(k = 0; k < j; k++){
                    double dx2 = points[k][0] - points[i][0];
                    double dy2 = points[k][1] - points[i][1];
                    if(fabs(dx2*dy1 - dy2*dx1) <= FLT_EPSILON*(fabs(dx1) + fabs(dy1) + fabs(dx2) + fabs(dy2)))
                        break;
                }
                if(k < j)
                    break;
            }
            if(j < i)
                break;
        }
        return i >= i1;
    }
  
    return 0;

}

uint8_t get_subset(float* pre_points, float* cur_points, float pre_set[][2], float cur_set[][2], int points_num, uint32_t model_points, uint64_t* seed){

    uint32_t max_iters = 100;
    uint32_t iters = 0;
    uint32_t i;
    uint32_t j;
    uint32_t k;

    int32_t idx[4] = { 5, 5, 5, 5};

    for(i = 0; i < model_points && iters <= max_iters; i++){

        int32_t cur_candidate = -1;
        cur_candidate--;
        for(; iters < max_iters + 1; iters++){
            //int32_t candidate_id = rand()%points_num;
            int32_t candidate_id = cvRandInt(seed)%points_num;
            for(j = 0; j < i; j++){
                if(idx[j] == candidate_id)
                    break;
            }
            if(j == i){
                cur_candidate = candidate_id;
                idx[i] = candidate_id;
                break;
            }
        }
        for(k = 0; k < 2; k++){
            pre_set[i][k] = *(pre_points + cur_candidate*2 + k);
            cur_set[i][k] = *(cur_points + cur_candidate*2 + k);
        }
        if(check_subset(pre_set, i) == 0 && check_subset(cur_set, i) == 0 && i == model_points){
            i = rand()%(i+1) - 1;
            iters++;
            continue;
        }
    }

    if(i < model_points)
        return 0;
    return 1;
}

int32_t find_inliers(float* pre_points, float* cur_points, float matrix[][3], int32_t points_num, uint8_t* mask, float THRESHOLD){

   //float* err = (float*)malloc(points_num*sizeof(float));
   float* err = (float *)Utils_memAlloc( UTILS_HEAPID_DDR_CACHED_SR, points_num*sizeof(float), 32);
   memset(err, 0.0f, points_num*sizeof(float));
   int32_t i = 0;
   int32_t inline_points_num = 0;
   for(; i < points_num; i++){
       float tmp[3] = {0.0f, 0.0f, 0.0f};
       tmp[0] = pre_points[i*2]*matrix[0][0] + pre_points[i*2 + 1]*matrix[0][1] + matrix[0][2];
       tmp[1] = pre_points[i*2]*matrix[1][0] + pre_points[i*2 + 1]*matrix[1][1] + matrix[1][2];
       float scale = 1.0/(pre_points[i*2]*matrix[2][0] + pre_points[i*2 + 1]*matrix[2][1] + 1.0);
        
       *(err + i) = (tmp[0]*scale - cur_points[i*2])*(tmp[0]*scale - cur_points[i*2]) + (tmp[1]*scale - cur_points[i*2 + 1])*(tmp[1]*scale - cur_points[i*2 + 1]);
       if(sqrt(*(err+ i)) <= THRESHOLD*THRESHOLD){
       //if(*(err+ i) <= THRESHOLD*THRESHOLD){
           //printf("err is %f\n", sqrt(*(err + i)));
           //printf("err is %f\n", *(err + i));
           inline_points_num += 1;
       }else{
         *(mask + i) = 0;
       }
   }
   //free(err);
   //printf("this iter inline num is : %d\n", inline_points_num);
   Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, err, points_num*sizeof(float));
   return inline_points_num;

}

int32_t RANSAC_update_numiters(float p, float ep, int32_t modelPoints, int32_t maxIters) {

  p = max_(p, 0.);
  p = min_(p, 1.);
  ep = max_(ep, 0.);
  ep = min_(ep, 1.);

  // avoid inf's & nan's
  float num = max_(1. - p, FLT_MIN);
  float denom = 1. - pow(1. - ep, (float)modelPoints);
  if (denom < FLT_MIN)
    return 0;

  num = log(num);
  denom = log(denom);

  float out = 1.0f;
  out -= 1.0f;
  denom >= 0 || -num >= maxIters * (-denom) ? (out = maxIters) : (out = round(num / denom)); 

  return denom >= 0 || -num >= maxIters * (-denom) ? maxIters : round(num / denom);
    
}

void calcHmatrix(float* pre_points,float* cur_points, float matrix[][3], int32_t points_num){

    float average_x1 = 0.0f, average_y1 = 0.0f;
    float average_x2 = 0.0f, average_y2 = 0.0f;

    int32_t i = 0;
    for (; i < points_num; ++i) {
        average_x2 += *(pre_points + i*2);
        average_y2 += *(pre_points + i*2 + 1);
        average_x1 += *(cur_points + i*2);
        average_y1 += *(cur_points + i*2 + 1);
    }
    average_x1 /= points_num;
    average_y1 /= points_num;
    average_x2 /= points_num;
    average_y2 /= points_num;

    float gap_x1 = 0.0f, gap_y1 = 0.0f;
    float gap_x2 = 0.0f, gap_y2 = 0.0f;

    for (i = 0; i < points_num; ++i) {
      gap_x2 += fabs(*(pre_points + i*2) - average_x2);
      gap_y2 += fabs(*(pre_points + i*2 + 1) - average_y2);
      gap_x1 += fabs(*(cur_points + i*2) - average_x1);
      gap_y1 += fabs(*(cur_points + i*2 + 1) - average_y1);
    }
    gap_x1 /= points_num;
    gap_y1 /= points_num;
    gap_x2 /= points_num;
    gap_y2 /= points_num;
 
    //printf("ave are : %f, %f, %f, %f\n", average_x1, average_y1, average_x2, average_y2);
    //printf("gap are : %f, %f, %f, %f\n", gap_x1, gap_y1, gap_x2, gap_y2);

    //float* invHnorm = (float *)malloc(16*sizeof(float));
    float* invHnorm = (float *)Utils_memAlloc( UTILS_HEAPID_DDR_CACHED_SR, 16*sizeof(float), 32);
    memset(invHnorm, 0.0f, 16*sizeof(float));
    *(invHnorm) = gap_x1;
    *(invHnorm + 2) = average_x1;
    *(invHnorm + 5) = gap_y1;
    *(invHnorm + 6) = average_y1;
    *(invHnorm + 10) = 1.0; 
    //float* Hnorm = (float *)malloc(16*sizeof(float));
    float* Hnorm = (float *)Utils_memAlloc( UTILS_HEAPID_DDR_CACHED_SR, 16*sizeof(float), 32);
    memset(Hnorm, 0.0f, 16*sizeof(float));
    *(Hnorm) = 1.0f/gap_x2;
    *(Hnorm + 2) = -average_x2 / gap_x2;
    *(Hnorm + 5) = 1.0f / gap_y2;
    *(Hnorm + 6) = -average_y2 / gap_y2;
    *(Hnorm + 10) = 1.0; 

    //float *LtL = (float*)malloc(9*2*points_num*sizeof(float));
    float* LtL = (float *)Utils_memAlloc( UTILS_HEAPID_DDR_CACHED_SR, 9*2*points_num*sizeof(float), 32);
    memset(LtL, 0.0f, 9*2*points_num*sizeof(float));

    for (i = 0; i < points_num; i++) {
        float X = (*(pre_points + i*2) - average_x2) / gap_x2,
              Y = (*(pre_points + i*2 + 1) - average_y2) / gap_y2;
        float x = (*(cur_points + i*2) - average_x1) / gap_x1,
              y = (*(cur_points + i*2 + 1) - average_y1) / gap_y1;
        float Lx[] = {0.0, 0.0, 1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
        Lx[0] = X;
        Lx[1] = Y;
        Lx[6] = -x*X;
        Lx[7] = -x*Y;
        Lx[8] = -x;
        float Ly[] = {0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0};
        Ly[3] = X;
        Ly[4] = Y;
        Ly[6] = -y*X;
        Ly[7] = -y*Y;
        Ly[8] = -y;
             
        int32_t k;
        for(k = 0; k < 9; k++){
            *(LtL + i*2*9 + k) = Lx[k];
            *(LtL + (i*2+1)*9 + k) = Ly[k];
        }

    }

    int32_t max_rc = max_I32(9, 2*points_num);
    //printf("max N is : %d\n", max_rc);
    //float* U = (float* )malloc(2*points_num*2*points_num*sizeof(float));
    float* U = (float *)Utils_memAlloc( UTILS_HEAPID_DDR_CACHED_SR, 2*points_num*2*points_num*sizeof(float), 32);
    //float* V = (float* )malloc(9*9*sizeof(float));
    float* V = (float *)Utils_memAlloc( UTILS_HEAPID_DDR_CACHED_SR, 9*9*sizeof(float), 32);
    //float* U1 = (float* )malloc(max_rc*max_rc*sizeof(float));
    float* U1 = (float *)Utils_memAlloc( UTILS_HEAPID_DDR_CACHED_SR, max_rc*max_rc*sizeof(float), 32);
    //float* diag = (float* )malloc(max_rc*sizeof(float));
    float* diag = (float *)Utils_memAlloc( UTILS_HEAPID_DDR_CACHED_SR, max_rc*sizeof(float), 32);
    //float* superdiag = (float* )malloc(max_rc*sizeof(float));
    float* superdiag = (float *)Utils_memAlloc( UTILS_HEAPID_DDR_CACHED_SR, max_rc*sizeof(float), 32);
    //float* B = (float* )malloc(9*sizeof(float));
 
    memset(U, 0.0f, 2*points_num*2*points_num*sizeof(float));  
    memset(V, 0.0f, 9*9*sizeof(float));
    memset(U1, 0.0f, max_rc*max_rc*sizeof(float));
    memset(diag, 0.0f, max_rc*sizeof(float));
    memset(superdiag, 0.0f, max_rc*sizeof(float));
    //memset(B, 1.0f, 9*sizeof(float));

    //test svd 
    //float* A_test = (float*)malloc(4*sizeof(float));
    //float* U_test = (float*)malloc(4*sizeof(float));
    //float* V_test = (float*)malloc(4*sizeof(float));

    //int32_t status_test = 1;
    //status--;
    //status = DSPF_sp_svd( 2, 2, LtL, U, V, U1, diag, superdiag);


    int32_t status = 1;
    status++;
    status = DSPF_sp_svd_diy( 2*points_num, 9, LtL, U, V, U1, diag, superdiag);
    //printf("svd is : %d\n", status);

    //float* H_ = (float*)malloc(16*sizeof(float));
    float* H_ = (float *)Utils_memAlloc( UTILS_HEAPID_DDR_CACHED_SR, 16*sizeof(float), 32);
	//float* H_med = (float*)malloc(16*sizeof(float));
    float* H_med = (float *)Utils_memAlloc( UTILS_HEAPID_DDR_CACHED_SR, 16*sizeof(float), 32);
    //float* H = (float*)malloc(16*sizeof(float));
    float* H = (float *)Utils_memAlloc( UTILS_HEAPID_DDR_CACHED_SR, 16*sizeof(float), 32);
    memset(H_, 0.0f, 16*sizeof(float));
    memset(H_med, 0.0f, 16*sizeof(float));
    memset(H, 0.0f, 16*sizeof(float));

    /*printf("check LtL\n");
    for( i = 0; i < 2*points_num; i++){
        int32_t j;
        for(j = 0; j < 9; j++)
            printf("%f| ", *(LtL + i*9 + j));
        printf("\n");
    }
    printf("checked\n");
  
    printf("check diag matrix\n");
    for(i = 0; i < 2*points_num; i++){
        printf("%f| ", *(diag + i));
    }
    printf("\nchecked\n");  
 
    printf("check U matrix\n");
    for(i = 0; i < 2*points_num; i++){
        int32_t j_cols;
        for(j_cols = 0; j_cols < 2*points_num; j_cols++){
            printf("%f| ", *(U + i*2*points_num + j_cols));
        }
        printf("\n");
    }
    printf("checked\n");
    printf("check V matrix\n");
    for(i = 0; i < 9; i++){
        int32_t j_cols;
        for(j_cols = 0; j_cols < 9; j_cols++){
            printf("%f| ", *(V + i*9 + j_cols));
        }
        printf("\n");
    }
    printf("checked\n");*/

    for(i = 0; i < 9; i++){
        *(H_ + i + i/3) = *(V + i*9 + 8);
    }

    /*printf("check H_ matrix\n");
    for(i = 0; i < 16; i++){
        printf("%f| ", *(H_ + i));
        printf("\n");
    }
    printf("\nchecked\n");

    printf("check invHnorm\n");
    for(i = 0; i < 16; i++){
        printf("%f |", *(invHnorm + i));
        printf("\n");
    }
    printf("\nchecked\n");
    printf("check Hnorm\n");
    for(i = 0; i < 16; i++){
        printf("%f |", *(Hnorm + i));
        printf("\n");
    }
    printf("\nchecked\n");*/
    DSPF_sp_mat_mul(invHnorm, 4, 4, H_, 4, H_med);

    /*printf("check H_med matrix\n");
    for(i = 0; i < 16; i++){
        printf("%f |", *(H_med + i));
        printf("\n");
    }
    printf("\nchecked\n");*/

    DSPF_sp_mat_mul(H_med, 4, 4, Hnorm, 4, H);

    /*printf("check H matrix\n");
    for(i = 0; i < 16; i++){
        printf("%f |", *(H + i));
    }
    printf("\nchecked\n");*/

    float scale = 1.0f/ *(H + 10);
    //printf("H8 is : %f\n", *(H + 10));
    //printf("scale is : %f\n", scale);
    for(i = 0; i < 3; i++){
        int32_t j;
        for(j = 0; j < 3; j++){
            matrix[i][j] = (*(H + i*4 + j)) * scale; 
            //printf("%f\n", *(H + i*4 + j)*scale);
        }
    }

    /*DSPF_sp_mat_mul(LtL, 2*points_num, 9, H_, 1, B);
    printf("check B matrix\n");
    for(i = 0; i < 9; i++){
        printf("%f |", *(B + i));
    }
    printf("\nchecked\n");*/

    //free(invHnorm);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, invHnorm, 16*sizeof(float));
    //free(Hnorm);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, Hnorm, 16*sizeof(float));

    //free(LtL);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, LtL, 2*points_num*9*sizeof(float));
    //free(U);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, U, points_num*2*points_num*2*sizeof(float));
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, V, 9*9*sizeof(float));
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, U1, max_rc*max_rc*sizeof(float));
    //free(V);
    ///free(U1);
    //free(diag);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, diag, max_rc*sizeof(float));
    //free(superdiag);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, superdiag, max_rc*sizeof(float));
    //free(H_);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, H_, 16*sizeof(float));
    //free(H_med);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, H_med, 16*sizeof(float));
    //free(H);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, H, 16*sizeof(float));
     
}

void compress_elems(float* points, uint8_t* mask, float* subpoints, int32_t points_num){

    int32_t i = 0;
    int32_t ptr = 0;
    for(; i < points_num; i++){
        if(*(mask + i) == 1){
            *(subpoints + ptr) = *(points + i*2);
            *(subpoints + ptr + 1) = *(points + i*2 + 1);
            ptr += 2;
        }
    }

}

void copy(float* src, float* dst, int32_t rows, int32_t cols){

    memcpy(dst, src, rows*cols*sizeof(float));

}

uint8_t levmarq_compute(float* presub, float* cursub, float* h, float* err, float* J, int32_t points_num, int32_t J_rows){

  
    int32_t i;
    for (i = 0; i < points_num; i++) {
        float Mx = *(presub + i*2), My = *(presub + i*2 + 1);
        float ww = *(h + 6)*Mx + *(h + 7)*My + 1.0;
        //printf("ww is : %f\n", ww);
        ww = fabs(ww) > FLT_EPSILON ? 1. / ww : 0;
        //printf("ww is : %f\n", ww);
        float xi = (*(h) * Mx + *(h + 1) * My + *(h + 2)) * ww;
        float yi = (*(h + 3) * Mx + *(h + 4) * My + *(h + 5)) * ww;
        *(err + 2*i) = xi - *(cursub + i*2);
        *(err + 2*i + 1) = yi - *(cursub + i*2 + 1);

        *(J + 2*i*J_rows) = Mx * ww;
        *(J + 2*i*J_rows + 1) = My * ww;
        *(J + 2*i*J_rows + 2) = ww;
        *(J + 2*i*J_rows + 3) = 0.0f;
        *(J + 2*i*J_rows + 4) = 0.0f;
        *(J + 2*i*J_rows + 5) = 0.0f;
        *(J + 2*i*J_rows + 6) = -Mx * ww * xi;
        *(J + 2*i*J_rows + 7) = -My * ww * xi;
        *(J + (2*i+1)*J_rows) = 0.0f;
        *(J + (2*i+1)*J_rows + 1) = 0.0f;
        *(J + (2*i+1)*J_rows + 2) = 0.0f;
        *(J + (2*i+1)*J_rows + 3) = Mx * ww;
        *(J + (2*i+1)*J_rows + 4) = My * ww;
        *(J + (2*i+1)*J_rows + 5) = ww;
        *(J + (2*i+1)*J_rows + 6) = -Mx * ww * yi;
        *(J + (2*i+1)*J_rows + 7) = -My * ww * yi;
    }

    return 1;
}

float norm(float* src, int32_t src_rows, int32_t src_cols, uint8_t do_sqrt) {
  
  float result = 0.0f;
  int32_t i = 0;
  for (; i < src_rows; ++i) {
    int32_t j;
    for (j = 0; j < src_cols; ++j) {
      float diag_val = *(src + i*src_cols + j);
      result += diag_val*diag_val;
    }
  }

  if (do_sqrt == 1) {
    result = sqrt(result);
  }

  return result;
}

void diagonal(float* matrix, float* diag_vector, int32_t matrix_rows, int32_t matrix_cols){

    int32_t i;
    for(i = 0; i < matrix_rows; i++){
        *(diag_vector + i) = *(matrix + i*matrix_cols + i);
    }

}

void get_solution(float* A, float* X, float* b, int32_t rows, int32_t cols){

    //float* U = (float* )malloc(rows*cols*sizeof(float));
    float* U = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, rows*cols*sizeof(float), 32);
    //float* V = (float* )malloc(rows*cols*sizeof(float));
    float* V = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, rows*cols*sizeof(float), 32);
    int32_t max_rc = (rows > cols ? rows : cols);
    //float* U1 = (float* )malloc(max_rc*max_rc*sizeof(float));
    float* U1 = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, max_rc*max_rc*sizeof(float), 32);
    //float* diag = (float* )malloc(cols*sizeof(float));
    float* diag = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, max_rc*sizeof(float), 32);
    //float* superdiag = (float* )malloc(max_rc*sizeof(float));
    float* superdiag = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, max_rc*sizeof(float), 32);
    //float* b_ = (float* )malloc(rows*sizeof(float));
    float* b_ = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, rows*sizeof(float), 32);
    //float* y = (float* )malloc(rows*sizeof(float));
    float* y = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, rows*sizeof(float), 32);
 
    memset(U, 0.0f, rows*cols*sizeof(float));  
    memset(V, 0.0f, rows*cols*sizeof(float));
    memset(U1, 0.0f, max_rc*max_rc*sizeof(float));
    memset(diag, 0.0f, max_rc*sizeof(float));
    memset(superdiag, 0.0f, max_rc*sizeof(float));
    memset(b_, 0.0f, rows*sizeof(float));
    memset(y, 0.0f, rows*sizeof(float));

    int32_t status = 1;
    status--; 
    status = DSPF_sp_svd( rows, cols, A, U, V, U1, diag, superdiag);
    //status = DSPF_dp_svd( 6, 6, A, U, V, U1, diag, superdiag);
    //if (status==-1) {
    //}
    
    int32_t rows_idx;
    for(rows_idx = 0; rows_idx < rows; rows_idx++){
        uint32_t cols_idx = 0;
        for(; cols_idx < cols; cols_idx++){
            b_[rows_idx] += b[cols_idx]*U[cols_idx*cols + rows_idx];
        }
    }

    for(rows_idx = 0; rows_idx < rows; rows_idx++){
        y[rows_idx] = b_[rows_idx]/diag[rows_idx];
    }

    for(rows_idx = 0; rows_idx < rows; rows_idx++){
        uint32_t cols_idx = 0;
        for(; cols_idx < cols; cols_idx++){
            *(X + rows_idx) += V[rows_idx*cols + cols_idx]*y[cols_idx];
        }
    }
    
    //free(U);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, U, rows*cols*sizeof(float));
    //free(V);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, V, rows*cols*sizeof(float));
    //free(U1);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, U1, max_rc*max_rc*sizeof(float));
    //free(diag);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, diag, max_rc*sizeof(float));
    //free(superdiag);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, superdiag, max_rc*sizeof(float));
    //free(b_);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, b_, rows*sizeof(float));
    //free(y);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, y, rows*sizeof(float));
}

void get_inverse(float* A, float* A_, int32_t rows, int32_t cols){

    if(rows != cols){
        return;
    }
    uint8_t status = 1;
    status--;

    //float* L = (float*)malloc(rows*cols*sizeof(float));
    float* L = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, rows*cols*sizeof(float), 32);
    //float* U = (float*)malloc(rows*cols*sizeof(float));
    float* U = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, rows*cols*sizeof(float), 32);
    //unsigned short* P = (unsigned short*)malloc(rows*cols*sizeof(unsigned short));
    unsigned short* P = (unsigned short *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, rows*cols*sizeof(unsigned short), 32);
    memset(L, 0.0f, rows*cols*sizeof(float));
    memset(U, 0.0f, rows*cols*sizeof(float));
    memset(P, 0, rows*cols*sizeof(unsigned short));
    memset(A_, 0.0f, rows*cols*sizeof(float));

    //LU decp
    status = DSPF_sp_lud(rows, A, L, U, P);

    //LU invers
    DSPF_sp_lud_inverse(rows, P, L, U, A_);

    //free(L);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, L, rows*cols*sizeof(float));
    //free(U);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, U, rows*cols*sizeof(float));
    //free(P);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, P, rows*cols*sizeof(unsigned short));

}

void levmarq(float* presub, float* cursub, float matrix[][3], int32_t points_num){

    //float* x = (float*)malloc(8*sizeof(float));
    float* x = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, 8*sizeof(float), 32);
    memset(x, 0.0f, 8*sizeof(float));
    //float* err = (float*)malloc(points_num*2*sizeof(float));
    float* err = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, points_num*2*sizeof(float), 32);
    memset(err, 0, points_num*2*sizeof(float));
    //float* err_2 = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, points_num*2*2*sizeof(float), 32);
    //memset(err_2, 0, points_num*2*2*sizeof(float));
    //float* J = (float*)malloc(points_num*2*8*sizeof(float));
    float* J = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, points_num*2*8*sizeof(float), 32);
    memset(J, 0, points_num*2*8*sizeof(float));

    int32_t lx = 8;
    int32_t i;

    for(i = 0; i < 8; i++){
        *(x + i) = matrix[i/3][i%3];
    }

    uint8_t compute_flag = levmarq_compute(presub, cursub, x, err, J, points_num, 8);
    
    float S = norm(err, points_num*2, 1, 0);
    int32_t nfJ = 2;
  
    //float* A = (float*)malloc(8*8*sizeof(float));
    float* A = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, 8*8*sizeof(float), 32);
    //float* A_T = (float*)malloc(8*8*sizeof(float));
    float* A_T = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, 8*8*sizeof(float), 32);
    //float* J_T = (float*)malloc(points_num*2*8*sizeof(float));
    float* J_T = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, points_num*2*8*sizeof(float), 32);
    //float* v = (float*)malloc(8*sizeof(float));
    float* v = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, 8*sizeof(float), 32);
    //float* v_2 = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, 2*8*sizeof(float), 32);
    //float* D = (float*)malloc(8*sizeof(float));
    float* D = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, 8*sizeof(float), 32);
    //float* Ap = (float*)malloc(8*8*sizeof(float));
    float* Ap = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, 8*8*sizeof(float), 32);
    //float* d = (float*)malloc(8*sizeof(float));
    float* d = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, 8*sizeof(float), 32);
    //float* xd = (float*)malloc(8*sizeof(float));
    float* xd = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, 8*sizeof(float), 32);
    //float* err_d = (float*)malloc(points_num*2*sizeof(float));
    float* err_d = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, points_num*2*sizeof(float), 32);
    //float* empty = (float*)malloc(points_num*2*8*sizeof(float));
    float* empty = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, points_num*2*8*sizeof(float), 32);
    //float* temp_d = (float*)malloc(8*sizeof(float));
    float* temp_d = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, 8*sizeof(float), 32);
 
    memset(A, 0, 8*8*sizeof(float));
    memset(A_T, 0, 8*8*sizeof(float));
    memset(J_T, 0, points_num*2*8*sizeof(float));
    memset(v, 0, 8*sizeof(float));
    //memset(v_2, 0, 2*8*sizeof(float));
    memset(D, 0, 8*sizeof(float));
    memset(Ap, 0, 8*8*sizeof(float));
    memset(d, 0, 8*sizeof(float));
    memset(xd, 0, 8*sizeof(float));
    memset(err_d, 0, points_num*2*sizeof(float));
    memset(empty, 0, points_num*2*8*sizeof(float));
    memset(temp_d, 0, 8*sizeof(float));

    DSPF_sp_mat_trans(J, points_num*2, 8, J_T);

    DSPF_sp_mat_mul(J_T, 8, points_num*2, J, 8, A);

    /*for(i = 0; i < 2*points_num; i++){
        *(err_2 + i) = *(err + i); 
    }
    DSPF_sp_mat_mul(J_T, 8, points_num*2, err_2, 2, v_2);
    for(i = 0; i < 2*points_num; i++){
        *(v + i) = *(v_2 + i); 
    }*/

    for(i = 0; i < 8; i++){
        int32_t j;
        for(j = 0; j < points_num*2; j++){
            *(v + i) += *(J_T + i*points_num*2 + j)*(*(err + j));
        }
    }

    diagonal(A, D, 8, 8);   

    float Rlo = 0.25, Rhi = 0.75;
    float lambda = 1, lc = 0.75;
    int32_t iter = 0;

    for (;;) {
    
        //copy Ap from A
        copy(A, Ap, 8, 8);

        for (i = 0; i < lx; i++)
          *(Ap + i*8 + i) += *(D + i)*lambda;

        get_solution(Ap, d, v, 8, 8);
    
        int32_t x_idx;
        for(x_idx = 0; x_idx < 8; x_idx++)
            *(xd + x_idx) = *(x + x_idx) - *(d + x_idx);
       
        levmarq_compute(presub, cursub, xd, err_d, empty, points_num, 8);
        nfJ++;
        float Sd = norm(err_d, points_num*2, 1, 0);

        memset(A_T, 0, 8*8*sizeof(float));
        DSPF_sp_mat_trans(A, 8, 8, A_T);
        //DSPF_sp_mat_mul(A_T, 8, 8, d, 8, temp_d);
        for(i = 0; i < 8; i++){
            int32_t j;
            for(j = 0; j < 8; j++){
                *(temp_d + i) += *(A_T + i*8 + j)*(*(d + j));
            }
        }

        for(x_idx = 0; x_idx < 8; x_idx++)
           *(temp_d + x_idx) = *(temp_d + x_idx) * (-1) + *(v + x_idx) * 2;

        float dS = 0.0f;
        for(x_idx = 0; x_idx < 8; x_idx++)
           dS += *(d + x_idx) *(*(temp_d + x_idx));
        float R = (S - Sd) / (fabs(dS) > FLT_EPSILON ? dS : 1);

        if (R > Rhi) {
            lambda *= 0.5;
            if (lambda < lc)
                lambda = 0;
        } else if (R < Rlo) {
            // find new nu if R too low
            float t = 0.0f;
            for(x_idx = 0; x_idx < 8; x_idx++)
               t += *(d + x_idx) *(*(v + x_idx));
            float nu = (Sd - S) / (fabs(t) > FLT_EPSILON ? t : 1) + 2;

            nu = min_(max_(nu, 2.0f), 10.0f);

            if (lambda == 0) {

                get_inverse(A, Ap, 8, 8);

                float maxval = FLT_EPSILON;
                for (i = 0; i < lx; i++)
                    maxval = max_(maxval, fabs(*(Ap + i*8 + i)));

                lambda = lc = 1. / maxval;
                nu *= 0.5;
             }
             lambda *= nu;
        } 

        if (Sd <= S) {
            nfJ++;
            S = Sd;
            swap_(x, xd, 8);
            levmarq_compute(presub, cursub, x, err, J, points_num, 8);

            memset(A, 0, 8*8*sizeof(float));
            memset(J_T, 0, points_num*2*8*sizeof(float));
            memset(v, 0, 8*sizeof(float));

            DSPF_sp_mat_trans(J, points_num*2, 8, J_T);
            DSPF_sp_mat_mul(J_T, 8, points_num*2, J, 8, A);
            //DSPF_sp_mat_mul(J_T, 8, points_num*2, err, 1, v);
            for(i = 0; i < 8; i++){
                int32_t j;
                for(j = 0; j < points_num*2; j++){
                    *(v + i) += *(J_T + i*points_num*2 + j)*(*(err + j));
                }
            }
        }

        iter++;
        uint8_t proceed = iter < 10 && norm(d, 1, 8, 1) >= FLT_EPSILON &&
                   norm(err, 1, 8, 1) >= FLT_EPSILON;

        if (proceed == 0)
            break;
    }

    for(i = 0; i < 8; i++)
        matrix[i/3][i%3] = *(x + i);
    
    if (iter == 10)
        iter = -iter;

    //free(x);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, x, 8*sizeof(float));
    //free(err);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, err, points_num*2*sizeof(float));
    //free(J);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, J, points_num*2*8*sizeof(float));
    //free(A);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, A, 8*8*sizeof(float));
    //free(A_T);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, A_T, 8*8*sizeof(float));
    //free(J_T);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, J_T, points_num*2*8*sizeof(float));
    //free(v);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, v, 8*sizeof(float));
    //free(D);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, D, 8*sizeof(float));
    //free(Ap); 
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, Ap, 8*8*sizeof(float));
    //free(d);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, d, 8*sizeof(float));
    //free(xd);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, xd, 8*sizeof(float));
    //free(err_d);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, err_d, points_num*2*sizeof(float));
    //free(empty);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, empty, points_num*2*8*sizeof(float));
    //free(temp_d);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, temp_d, 8*sizeof(float));

    //return iter;
    return;
}

void findHomography(TBKTracker* tracker, float* pre_points, float* cur_points, float THRESHOLD, float matrix[][3]){

    int32_t best_match_num = 0;
    int32_t points_num = tracker->points_num;
    int32_t niters = 100;
    float confidence = 0.995;
    int32_t inline_num = 0;
    //uint8_t* mask = (uint8_t*)malloc(points_num*sizeof(uint8_t));
    uint8_t* mask = (uint8_t *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, points_num*sizeof(uint8_t), 32);
    memset(mask, 1, points_num*sizeof(uint8_t));
    uint8_t* best_mask = (uint8_t *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, points_num*sizeof(uint8_t), 32);
    memset(best_mask, 1, points_num*sizeof(uint8_t));
    int32_t iter = 0;
    uint64_t seed = cvRNG(-1);
    for(; iter < niters; iter++){
        float pre_set[4][2] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
        float cur_set[4][2] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
        uint8_t status = get_subset(pre_points, cur_points, pre_set, cur_set, points_num, 4, &seed);
        float guess_H[3][3] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

        //getHmatrix(pre_set, cur_set, guess_H);
        //float* pre_ptr = (float* )malloc(8*sizeof(float));
        float* pre_ptr = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, 8*sizeof(float), 32);
        float* cur_ptr = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, 8*sizeof(float), 32);
        //float* cur_ptr = (float* )malloc(8*sizeof(float));
        int32_t i_tmp;
        for(i_tmp = 0; i_tmp < 4; i_tmp++){
            *(pre_ptr + i_tmp*2) = pre_set[i_tmp][0];
            *(pre_ptr + i_tmp*2 + 1) = pre_set[i_tmp][1];
            //printf("pre : %f %f ; pre set : %f %f \n", *(pre_ptr + i_tmp*2), *(pre_ptr + i_tmp*2 + 1), pre_set[i_tmp][0], pre_set[i_tmp][1]);
            *(cur_ptr + i_tmp*2) = cur_set[i_tmp][0];
            *(cur_ptr + i_tmp*2 + 1) = cur_set[i_tmp][1];
            //printf("cur : %f %f ; cur set : %f %f \n", *(cur_ptr + i_tmp*2), *(cur_ptr + i_tmp*2 + 1), cur_set[i_tmp][0], cur_set[i_tmp][1]);
        }
        /*for(i_tmp = 0; i_tmp < 9; i_tmp++){
            printf("%f | ", guess_H[i_tmp/3][i_tmp%3]);
        }   
        printf("\n");*/

        calcHmatrix(pre_ptr, cur_ptr, guess_H, 4);

        //for(i_tmp = 0; i_tmp < 9; i_tmp++){
            //printf("%f | ", guess_H[i_tmp/3][i_tmp%3]);
        //}   
       // printf("\n");

        //free(pre_ptr);
        Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, pre_ptr, 8*sizeof(uint8_t));
        //free(cur_ptr);
        Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, cur_ptr, 8*sizeof(uint8_t));
   
        inline_num = 0;
        memset(mask, 1, points_num*sizeof(uint8_t));
        inline_num = find_inliers(pre_points, cur_points, guess_H, points_num, mask, THRESHOLD);
        if (inline_num > max_I32(best_match_num, 3)) {
            //printf("update matrix, inline_num is : %d\n", inline_num);
            best_match_num = inline_num;
            memset(best_mask, 1, points_num*sizeof(uint8_t));
            memcpy(best_mask, mask, points_num*sizeof(uint8_t));
            int32_t m_i,m_j;
            for(m_i = 0; m_i < 3; m_i++){
                for(m_j = 0; m_j < 3; m_j++){
                    matrix[m_i][m_j] = guess_H[m_i][m_j];
                }
            }
            niters = RANSAC_update_numiters(
            confidence, (float)(points_num - inline_num) / points_num,
            4, niters);
        }
        //printf("iter == %d\n", iter);
    }

    if(best_match_num <= 0)    
        return;


    //float* pre_subpoints = (float*)malloc(2*inline_num*sizeof(float));
    float* pre_subpoints = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, 2*best_match_num*sizeof(float), 32);
    memset(pre_subpoints, 0.0f, 2*best_match_num*sizeof(float));
    //float* cur_subpoints = (float*)malloc(2*inline_num*sizeof(float));
    float* cur_subpoints = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, 2*best_match_num*sizeof(float), 32);
    memset(cur_subpoints, 0.0f, 2*best_match_num*sizeof(float));
    compress_elems(pre_points, best_mask, pre_subpoints, points_num);
    compress_elems(cur_points, best_mask, cur_subpoints, points_num);


    if(best_match_num >= 4){
        //scale
        int32_t i_tmp;
        for(i_tmp = 0; i_tmp < best_match_num*2; i_tmp++){
            *(pre_subpoints + i_tmp) /= 10;
            *(cur_subpoints + i_tmp) /= 10;
        }
        calcHmatrix(pre_subpoints, cur_subpoints, matrix, best_match_num);
        levmarq(pre_subpoints, cur_subpoints, matrix, best_match_num);
        matrix[0][2] *= 10;
        matrix[1][2] *= 10;
    }

    //free(mask);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, mask, points_num*sizeof(uint8_t));
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, best_mask, points_num*sizeof(uint8_t));
    //free(pre_subpoints);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, pre_subpoints, 2*best_match_num*sizeof(uint8_t));
    //free(cur_subpoints);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, cur_subpoints, 2*best_match_num*sizeof(uint8_t));
    
    /*int32_t i_tmp;
    printf("check matrix in func\n");    
    for(i_tmp = 0; i_tmp < 9; i_tmp++){
        printf("%f | ", matrix[i_tmp/3][i_tmp%3]);
    }   
    printf("\n");*/
}

void track_by_klt(TBKTracker* tracker, uint8_t* img, TBKRect* rect_car, int32_t frame_width, int32_t frame_height){

  if(!tracker->init){
    //get intrested roi
    update_car_roi(tracker, rect_car, frame_width, frame_height);
    //get roi img from img and init points
    update_car_previous_frame(tracker, img, frame_width, frame_height);
    tracker->live = 999;
    tracker->track_times++;
    tracker->track_on = true;
    tracker->init = true;
  }else{
    if (tracker->live <= 0) {
        tracker->track_on = false;
    } else if (tracker->points_num == 0) {
        tracker->track_on = false;
    } else {

      // get resized cur img
      uint8_t *cur_resized_img = (uint8_t *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, tracker->roi_store_size * sizeof(uint8_t), 32);
      //uint8_t *cur_resized_img = (uint8_t *)malloc(tracker->roi_store_size * sizeof(uint8_t));

      float x_scale = 0.0f;
      float y_scale = 0.0f;
      int32_t pre_width = tracker->pre_width;
      int32_t pre_height = tracker->pre_height;
      get_car_resized_roi_img(tracker, img, frame_width, frame_height, cur_resized_img, &x_scale, &y_scale, &pre_width, &pre_height);

      // find the good clean points
      uint8_t *tracking_status = (uint8_t *)Utils_memAlloc( UTILS_HEAPID_DDR_CACHED_SR, tracker->points_num, 32);
      //uint8_t *tracking_status = (uint8_t *)malloc(tracker->points_num);
      memset(tracking_status, 1, tracker->points_num*sizeof(uint8_t));

      float* new_float_points = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, tracker->points_num * 2  * sizeof(float), 32);
      //float* new_float_points = (float *)malloc(tracker->points_num * 2  * sizeof(float));
      uint8_t success = get_mapping_points_between_two_frames(tracker, tracker->pre_resized_roi_frame, cur_resized_img, new_float_points, tracking_status);
      if (!success) {
        tracker->track_on = false;
        return;
      }
      Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, cur_resized_img, tracker->roi_store_size * sizeof(uint8_t));
      //free(cur_resized_img);

      float affine_matrix[3][3] = {0,0,0, 0,0,0, 0,0,0};
      get_affine_matrix(tracker, tracker->points, new_float_points, tracking_status, frame_width, frame_height, affine_matrix);
      Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, new_float_points, tracker->points_num * 2 * sizeof(uint8_t));
      //free(new_float_points);   

      update_car_rect(tracker, affine_matrix, &x_scale, &y_scale);
      printf("after update tracker->rect are: %f, %f, %f, %f\n", tracker->rect.left, tracker->rect.right, tracker->rect.top, tracker->rect.bottom);

      // tracking failed
      // If affine_matrix(0, 0) == 0
      // it means our rect would transform to empty rect,
      // which presents that tracking process of this car is failed.
      if (affine_matrix[0][0] == 0.0) {
        tracker->track_on = false;
      } else {
        // update bboxes
        update_car_roi(tracker, &tracker->rect, frame_width, frame_height);
        // update pre_img
        update_car_previous_frame(tracker, img, frame_width, frame_height);
        tracker->live -= 1;
        tracker->track_times++;
      }
    }
  }
}

/**********************majior module****************************************/

uint8_t init_tracklet(TBKTracker* tracker, uint8_t* img, TBKRect* rect, int32_t frame_width, int32_t frame_height){

  update_car_roi(tracker, rect, frame_width, frame_height);
  update_car_previous_frame(tracker, img, frame_width, frame_height);
  if(tracker->points_num == 0)
      return 0;
  tracker->live = 999;
  tracker->track_times++;
  tracker->track_on = true;
  tracker->init = true;
  return 1;
}

uint8_t update_tracklet(TBKTracker* tracker, uint8_t* img, TBKRect* rect, int32_t frame_width, int32_t frame_height){

    if (tracker->live <= 0) {
        tracker->track_on = false;
        printf("live <= 0\n");
    } else if (tracker->points_num == 0) {
        tracker->track_on = false;
        printf("points_num = 0\n");
    } else {

      // get resized cur img
      uint8_t *cur_resized_img = (uint8_t *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, tracker->roi_store_size * sizeof(uint8_t), 32);
      //uint8_t *cur_resized_img = (uint8_t *)malloc(tracker->roi_store_size * sizeof(uint8_t));

      float x_scale = 0.0f;
      float y_scale = 0.0f;
      int32_t pre_width = tracker->pre_width;
      int32_t pre_height = tracker->pre_height;
      get_car_resized_roi_img(tracker, img, frame_width, frame_height, cur_resized_img, &x_scale, &y_scale, &pre_width, &pre_height);

      // find the good clean points
      uint8_t *tracking_status = (uint8_t *)Utils_memAlloc( UTILS_HEAPID_DDR_CACHED_SR, tracker->points_num, 32);
      //uint8_t *tracking_status = (uint8_t *)malloc(tracker->points_num);
      memset(tracking_status, 1, tracker->points_num*sizeof(uint8_t));

      float* new_float_points = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, tracker->points_num * 2  * sizeof(float), 32);
      //float* new_float_points = (float *)malloc(tracker->points_num * 2  * sizeof(float));
      uint8_t success = get_mapping_points_between_two_frames(tracker, tracker->pre_resized_roi_frame, cur_resized_img, new_float_points, tracking_status);
      if (!success) {
        tracker->track_on = false;
        printf("unsucese\n");
        return 0;
      }
      Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, cur_resized_img, tracker->roi_store_size * sizeof(uint8_t));
      //free(cur_resized_img);

      float affine_matrix[3][3] = {0,0,0, 0,0,0, 0,0,0};
      get_affine_matrix(tracker, tracker->points, new_float_points, tracking_status, frame_width, frame_height, affine_matrix);
      Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, new_float_points, tracker->points_num * 2 * sizeof(uint8_t));
      //free(new_float_points);   

      update_car_rect(tracker, affine_matrix, &x_scale, &y_scale);
      printf("after update tracker->rect are: %f, %f, %f, %f\n", tracker->rect.left, tracker->rect.right, tracker->rect.top, tracker->rect.bottom);

      // tracking failed
      // If affine_matrix(0, 0) == 0
      // it means our rect would transform to empty rect,
      // which presents that tracking process of this car is failed.
      if (affine_matrix[0][0] == 0.0) {
        tracker->track_on = false;
        printf("affine_matrix wrong\n");
      } else {
        // update bboxes
        update_car_roi(tracker, &tracker->rect, frame_width, frame_height);
        // update pre_img
        update_car_previous_frame(tracker, img, frame_width, frame_height);
        tracker->live -= 1;
      }
    }

   if(tracker->track_on == 0)
       return 0;
   
   return 1;
}

uint8_t tracklet_tracking(TBKTracker* tracker){
    if(tracker->track_on)
        return 1;
    return 0;
}

uint8_t optical_flow(TBKTracker* tracker, uint8_t* img, TBKRect* rect, int32_t frame_width, int32_t frame_height, uint8_t type){

    uint8_t res = 2;

    if(type == 0)
        res = init_tracklet(tracker, img, rect, frame_width, frame_height);
    if(type == 1)
        res = update_tracklet(tracker, img, rect, frame_width, frame_height);
    if(type == 2)
        res = tracklet_tracking(tracker);

    printf("####res is : %d\n", res);

    return res;

}

/*******************************************************************/

void test_warp_function(){
  
    uint8_t src_img[40] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                           1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                           1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                           1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    //uint8_t* src_ptr = (uint8_t *)malloc(40*sizeof(uint8_t)); 
    uint8_t *src_ptr = (uint8_t *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, 40* sizeof(uint8_t), 32);
    memset(src_ptr, 1, 40*sizeof(uint8_t));

    uint8_t borderValue = 0;
    VXLIB_bufParams2D_t src_addr, dst_addr;
    src_addr.dim_x = (uint8_t)(10);
    src_addr.dim_y = (uint8_t)(4);
    src_addr.stride_y = (uint8_t)(10);
    src_addr.data_type = VXLIB_UINT8;

    uint8_t dst_img[40] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    //uint8_t* dst_ptr = (uint8_t *)malloc(40*sizeof(uint8_t)); 
    uint8_t *dst_ptr = (uint8_t *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, 40* sizeof(uint8_t), 32);
    memset(dst_ptr, 0, 40*sizeof(uint8_t));
    
    dst_addr.dim_x = (uint8_t)(10);
    dst_addr.dim_y = (uint8_t)(4);
    dst_addr.stride_y = (uint8_t)(10);
    dst_addr.data_type = VXLIB_UINT8;

    float warpMatrix[6] = {1.0f, 0.0f, 
                          0.0f, 1.0f, 
                          0.0f, 0.0f};

    float x_scale = 0.5f;
    float y_scale = 0.5f;
    uint32_t x_offset = 2;
    uint32_t y_offset = 1;
      
    warpMatrix[0] = 1/(x_scale);
    warpMatrix[3] = 1/(y_scale);
    warpMatrix[4] = (0.0f - (float)x_offset)*(1/(x_scale));
    warpMatrix[5] = (0.0f - (float)y_offset)*(1/(y_scale));
    

    VXLIB_STATUS status_opt = VXLIB_SUCCESS;
    status_opt = VXLIB_warpAffineBilinear_bc_i8u_c32f_o8u(src_img, &src_addr, dst_img, &dst_addr, warpMatrix, borderValue, 0, 0, 0, 0);
    
    status_opt = VXLIB_SUCCESS;
    status_opt = VXLIB_warpAffineBilinear_bc_i8u_c32f_o8u(src_ptr, &src_addr, dst_ptr, &dst_addr, warpMatrix, borderValue, 0, 0, 0, 0);

    //free(src_ptr);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, src_ptr, 40 * sizeof(uint8_t));
    //free(dst_ptr);
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, dst_ptr, 40 * sizeof(uint8_t));
    status_opt--;
}

void test_getHmatirx(){

  float pre_points[3][2] = {25.0, 27.0, 362.0, 57.0 , 422.0, 168.0};
  float matrix[3][3] = {0.5, 0, 3, 0, 0.7, 7};
  float matrix_calc[3][3] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
  float cur_points[3][2] = {15.5, 25.9, 184, 46.9, 214, 124.6};

  getHmatrix(pre_points, cur_points, matrix_calc);

  uint32_t rows_idx = 0;
  printf("printf ori matrix\n");
  for(; rows_idx < 3; rows_idx++){
    uint32_t cols_idx = 0;
    for(; cols_idx < 3; cols_idx++){
      printf("%f|", matrix[rows_idx][cols_idx]);
    }
    printf("\n");
  }
  printf("printf calc matrix\n");
  for(rows_idx = 0; rows_idx < 3; rows_idx++){
    uint32_t cols_idx = 0;
    for(; cols_idx < 3; cols_idx++){
      printf("%f|", matrix_calc[rows_idx][cols_idx]);
    }
    printf("\n");
  }
}

void test_homography(){

    float pre_points[60] = {139.000000, 5.000000, 147.000000, 6.000000, 125.000000, 10.000000, 134.000000, 10.000000, 147.000000, 13.000000, 174.000000, 16.000000, 84.000000, 17.000000, 96.000000, 19.000000, 102.000000, 23.000000, 130.000000, 24.000000, 84.000000, 26.000000, 146.000000, 26.000000, 163.000000, 28.000000, 170.000000, 28.000000, 92.000000, 28.000000, 6.000000, 29.000000, 99.000000, 30.00000, 185.000000, 31.000000, 107.000000, 32.000000, 116.000000, 34.000000, 126.000000, 36.000000, 137.000000, 37.000000, 145.000000, 42.000000, 2.000000, 43.000000, 63.000000, 43.000000, 153.000000, 43.000000, 193.000000, 47.000000, 77.000000, 48.000000, 164.000000, 48.000000, 76.000000, 55.000000};
    float cur_points[60] = {142.522186, 5.540123, 150.667023, 6.215244, 127.827393, 10.203844, 136.953720, 10.638108, 150.588242, 13.241501, 179.569016, 16.425581, 84.528862, 17.082722, 96.376106, 18.955650, 102.494118, 23.034725, 130.983780, 24.208759, 84.032791, 26.114580, 152.598984, 26.992306, 169.907440, 28.941034, 176.783539, 28.928049, 92.067902, 28.203394, 6.244359, 28.940403, 99.116302, 30.062067, 192.204163, 31.987968, 107.018875, 31.906706, 119.609520, 35.063560, 129.650177, 37.037426, 140.668747, 38.018070, 148.828461, 43.164871, 2.107847, 43.045662, 63.037796, 43.064476, 160.217514, 44.853809, 197.606812, 48.046127, 76.877510, 48.429482, 171.156815, 49.742886, 75.856178, 55.157825};
    //float pre_points[30] = {139, 5 ,147, 6 ,125, 10 ,134, 10 ,147, 13 ,174, 16 ,84, 17 ,96, 19 ,102, 23 ,130, 24 ,84, 26 ,92, 28 ,99, 30 ,107, 32 ,193, 47};
    //float cur_points[30] = {142.522, 5.54012 ,150.667, 6.21524 ,127.827, 10.2038 ,136.954, 10.6381 ,150.588, 13.2415 ,179.569, 16.4256 ,84.5289, 17.0827 ,96.3761, 18.9557 ,102.494, 23.0347 ,130.984, 24.2088 ,84.0328, 26.1146 ,92.0679, 28.2034 ,99.1163, 30.0621 ,107.019, 31.9067 ,197.607, 48.0461}
    float matrix[3][3] = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
    
    float* pre_ptr = pre_points;
    float* cur_ptr = cur_points;
    TBKTracker tracker;
    tracker.points_num = 30;
    TBKTracker* tracker_ptr = &tracker;

    findHomography(tracker_ptr, pre_ptr, cur_ptr, 0.8, matrix);  

    int32_t i = 0;
    printf("check pre points\n");
    for(; i < 30; i++){
        printf("%f, %f, ", *(pre_ptr + i*2), *(pre_ptr + i*2 + 1));
    }   
    printf("\nchecked\n");
    printf("check cur points\n");
    for(i = 0; i < 30; i++){
        printf("%f, %f, ", *(cur_ptr + i*2), *(cur_ptr + i*2 + 1));
    }
    printf("\nchecked\n");

    printf("check H matrix\n");
    for(i = 0; i < 9; i++){
        printf("%f ", matrix[i/3][i%3]);
    }
    printf("\nchecked\n");


}

void test_calcHmatrix(){

  
  float* A = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, 8*8*sizeof(float), 32);
  float* A_ = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, 8*8*sizeof(float), 32);
  float* D = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, 8*sizeof(float), 32);
  int32_t i;
  for(i = 0; i < 56; i++){
      *(A + i) = i;
  }

  //float pre_points[4][2] = {25.0, 27.0, 362.0, 57.0 , 422.0, 168.0, 206, 73};
  /*float matrix[3][3] = {0.5, 0, 3, 0, 0.7, 7};
  float matrix_calc[3][3] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
  //float cur_points[4][2] = {15.5, 25.9, 184, 46.9, 214, 124.6, 106, 58.1};
  float pre_points[30] = {139, 5 ,147, 6 ,125, 10 ,134, 10 ,147, 13 ,174, 16 ,84, 17 ,96, 19 ,102, 23 ,130, 24 ,84, 26 ,92, 28 ,99, 30 ,107, 32 ,193, 47};
  float cur_points[30] = {142.522, 5.54012 ,150.667, 6.21524 ,127.827, 10.2038 ,136.954, 10.6381 ,150.588, 13.2415 ,179.569, 16.4256 ,84.5289, 17.0827 ,96.3761, 18.9557 ,102.494, 23.0347 ,130.984, 24.2088 ,84.0328, 26.1146 ,92.0679, 28.2034 ,99.1163, 30.0621 ,107.019, 31.9067 ,197.607, 48.0461};
  //float pre_points[4][2] = {25.0, 27.0, 362.0, 57.0 , 422.0, 168.0, 206, 73};
  //float matrix[3][3] = {0.5, 0, 3, 0, 0.7, 7};
  //float matrix_calc[3][3] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
  //float cur_points[4][2] = {15.5, 25.9, 184, 46.9, 214, 124.6, 106, 58.1};

  int32_t points_num = 15;
  //float* pre_ptr = (float* )malloc(8*sizeof(float));
  float *pre_ptr = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, points_num*2* sizeof(float), 32);
  float *cur_ptr = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, points_num*2* sizeof(float), 32);
  //float *cur_ptr = (float* )malloc(8*sizeof(float));
  int32_t i_tmp = 0;
  for(; i_tmp < points_num; i_tmp++){
      *(pre_ptr + i_tmp*2) = pre_points[i_tmp*2];
      *(pre_ptr + i_tmp*2 + 1) = pre_points[i_tmp*2 + 1];
      printf("pre : %f %f \n", *(pre_ptr + i_tmp*2), *(pre_ptr + i_tmp*2 + 1));
      *(cur_ptr + i_tmp*2) = cur_points[i_tmp*2];
      *(cur_ptr + i_tmp*2 + 1) = cur_points[i_tmp*2 + 1];
      printf("cur : %f %f \n", *(cur_ptr + i_tmp*2), *(cur_ptr + i_tmp*2 + 1));
  }
  //calcHmatrix(pre_ptr, cur_ptr, matrix_calc, 4);
  calcHmatrix(pre_ptr, cur_ptr, matrix_calc, points_num);

  uint32_t rows_idx = 0;
  printf("printf ori matrix\n");
  for(; rows_idx < 3; rows_idx++){
    uint32_t cols_idx = 0;
    for(; cols_idx < 3; cols_idx++){
      printf("%f|", matrix[rows_idx][cols_idx]);
    }
    printf("\n");
  }
  printf("printf calc matrix\n");
  for(rows_idx = 0; rows_idx < 3; rows_idx++){
    uint32_t cols_idx = 0;
    for(; cols_idx < 3; cols_idx++){
      printf("%f|", matrix_calc[rows_idx][cols_idx]);
    }
    printf("\n");
  }*/

  get_inverse(A, A_, 8, 8);
  diagonal(A, D, 8, 8);

  printf("check diagonal matrix\n");
  for(i = 0; i < 8; i++){
      printf("%f| ", *(D + i));
  }
  printf("\nchecked\n");

  printf("check reverse matrix\n");
  for(i = 0; i < 8; i++){
      int32_t j;
      for(j = 0; j < 8; j++)
          printf("%f| ", *(A_ + i*8 + j));
      printf("\n");
  }
  printf("\nchecked\n");
 
  //Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, pre_ptr, 2*points_num*sizeof(float));
  //Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, cur_ptr, 2*points_num*sizeof(float));
  Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, A, 8*8*sizeof(float));
  Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, A_, 8*8*sizeof(float));
  Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, D, 8*sizeof(float));
  /*
  //test svd func float
  int32_t k;
  int32_t times = 3;
  for(k = 0; k < times; k++){
      int32_t rows_num = 7 + k;
      int32_t cols_num = 8;
      int32_t max_rc = (rows_num > cols_num ? rows_num : cols_num);
      float *restrict A = (float* )malloc(rows_num*cols_num*sizeof(float));
      float *restrict U = (float* )malloc(rows_num*rows_num*sizeof(float));
      float *restrict V = (float* )malloc(cols_num*cols_num*sizeof(float));
      float *restrict U1 = (float* )malloc(max_rc*max_rc*sizeof(float));
      float *restrict diag = (float* )malloc(max_rc*sizeof(float));
      float *restrict superdiag = (float* )malloc(max_rc*sizeof(float));
 
  	  memset(A, 0.0f, rows_num*cols_num*sizeof(float));  
      memset(U, 0.0f, rows_num*rows_num*sizeof(float));  
      memset(V, 0.0f, cols_num*cols_num*sizeof(float));
      memset(U1, 0.0f, max_rc*max_rc*sizeof(float));
      memset(diag, 0.0f, max_rc*sizeof(float));
      memset(superdiag, 0.0f, max_rc*sizeof(float));

      int32_t i;
      for(i = 0; i < rows_num*cols_num; i++){
          *(A + i) = i;
      }
      printf("check A\n");
      for(i = 0; i < rows_num; i++){
          int32_t j;
          for(j = 0; j < cols_num; j++){
              printf("%f| ", *(A + i*cols_num + j));
          }
          printf("\n");
      }
      printf("checked\n");

      int8_t status = 1;
      status--;
      status = DSPF_sp_svd_diy(rows_num, cols_num, A, U, V, U1, diag, superdiag);
      printf("%d\n", status);

      printf("check diag\n");
      for(i = 0; i < max_rc; i++)
          printf("%f| ", *(diag + i));
      printf("\nchecked\n");
 
      printf("check V\n");
      for(i = 0; i < cols_num; i++){
          int32_t j;
          for(j = 0; j < cols_num; j++){
              printf("%f| ", *(V + i*cols_num + j));
          }
          printf("\n");
      }
      printf("checked\n");
 
      free(A);
      free(U);
      free(U1);
      free(diag);
      free(superdiag);
  }*/
  //test svd func double
  /*int32_t k;
  int32_t times = 3;
  for(k = 0; k < times; k++){
      int32_t rows_num = 7 + k;
      int32_t cols_num = 8;
      int32_t max_rc = (rows_num > cols_num ? rows_num : cols_num);
      double *restrict A = (double* )malloc(rows_num*cols_num*sizeof(double));
      double *restrict U = (double* )malloc(rows_num*rows_num*sizeof(double));
      double *restrict V = (double* )malloc(cols_num*cols_num*sizeof(double));
      double *restrict U1 = (double* )malloc(max_rc*max_rc*sizeof(double));
      double *restrict diag = (double* )malloc(max_rc*sizeof(double));
      double *restrict superdiag = (double* )malloc(max_rc*sizeof(double));
 
  	  memset(A, 0, rows_num*cols_num*sizeof(double));  
      memset(U, 0, rows_num*rows_num*sizeof(double));  
      memset(V, 0, cols_num*cols_num*sizeof(double));
      memset(U1, 0, max_rc*max_rc*sizeof(double));
      memset(diag, 0, max_rc*sizeof(double));
      memset(superdiag, 0, max_rc*sizeof(double));

      int32_t i;
      for(i = 0; i < rows_num*cols_num; i++){
          *(A + i) = i;
      }
      printf("check A\n");
      for(i = 0; i < rows_num; i++){
          int32_t j;
          for(j = 0; j < cols_num; j++){
              printf("%f| ", *(A + i*cols_num + j));
          }
          printf("\n");
      }
      printf("checked\n");

      int8_t status = 1;
      status--;
      status = DSPF_dp_svd(rows_num, cols_num, A, U, V, U1, diag, superdiag);
      printf("%d\n", status);

      printf("check diag\n");
      for(i = 0; i < max_rc; i++)
          printf("%f| ", *(diag + i));
      printf("\nchecked\n");
 
      printf("check V\n");
      for(i = 0; i < cols_num; i++){
          int32_t j;
          for(j = 0; j < cols_num; j++){
              printf("%f| ", *(V + i*cols_num + j));
          }
          printf("\n");
      }
      printf("checked\n");
 
      free(A);
      free(U);
      free(U1);
      free(diag);
      free(superdiag);
  }*/
}


void test_get_gradient(uint8_t* input_img, uint32_t src_cols, uint32_t src_rows){

    //int32_t src_size = src_rows*src_cols; 

    int32_t required_points_num = 40;
    float *good_features = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, 2*required_points_num*sizeof(float), 32);
    memset(good_features, 0.0f, 2*required_points_num*sizeof(float));
    good_features_to_track(input_img, good_features, 7, 7, required_points_num, src_rows, src_cols);
    printf("check good features\n");
    int32_t gf;
    for(gf = 0; gf < required_points_num; gf++)
        printf("(x, y) : (%f, %f)\n", *(good_features + gf*2), *(good_features + gf*2 + 1));
    printf("checked\n");
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, good_features, 2*required_points_num*sizeof(uint16_t));
   

    /*float *Dx = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, src_size* sizeof(float), 32);
    memset(Dx, 0.0f, src_size* sizeof(float));
    float *Dy = (float *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, src_size* sizeof(float), 32);
    memset(Dy, 0.0f, src_size* sizeof(float));
    
    get_gradient(src_rows, src_cols, 7, input_img, Dx, Dy);

    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, Dx, src_size*sizeof(float));
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, Dy, src_size*sizeof(float));
    */
}

