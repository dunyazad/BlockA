/*
 *  Triangle-Triangle Overlap Test Routines
 *  July, 2002
 *  Updated December 2003
 *
 *  This file contains C implementation of algorithms for
 *  performing two and three-dimensional triangle-triangle intersection test
 *  The algorithms and underlying theory are described in
 *
 * "Fast and Robust Triangle-Triangle Overlap Test
 *  Using Orientation Predicates"  P. Guigue - O. Devillers
 *
 *  Journal of Graphics Tools, 8(1), 2003
 *
 *  Several geometric predicates are defined.  Their parameters are all
 *  points.  Each point is an array of two or three _real precision
 *  floating point numbers. The geometric predicates implemented in
 *  this file are:
 *
 *    int tri_tri_overlap_test_3d(p1,q1,r1,p2,q2,r2)
 *    int tri_tri_overlap_test_2d(p1,q1,r1,p2,q2,r2)
 *
 *    int tri_tri_intersection_test_3d(p1,q1,r1,p2,q2,r2,
 *                                     coplanar,source,target)
 *
 *       is a version that computes the segment of intersection when
 *       the triangles overlap (and are not coplanar)
 *
 *    each function returns 1 if the triangles (including their
 *    boundary) intersect, otherwise 0
 *
 *
 *  Other information are available from the Web page
 *  http:<i>//www.acm.org/jgt/papers/GuigueDevillers03/
 *
 */

 // modified by Aaron to better detect coplanarity

#include <Neon/TriangleTriangleOverlap.h>

#define ZERO_TEST(x)  (x == 0)
//#define ZERO_TEST(x)  ((x) > -0.001 && (x) < .001)

#include "stdio.h"

/* function prototype */

int tri_tri_overlap_test_3d(_real p1[3], _real q1[3], _real r1[3],
    _real p2[3], _real q2[3], _real r2[3]);


int coplanar_tri_tri3d(_real  p1[3], _real  q1[3], _real  r1[3],
    _real  p2[3], _real  q2[3], _real  r2[3],
    _real  N1[3], _real  N2[3]);


int tri_tri_overlap_test_2d(_real p1[2], _real q1[2], _real r1[2],
    _real p2[2], _real q2[2], _real r2[2]);


int tri_tri_intersection_test_3d(_real p1[3], _real q1[3], _real r1[3],
    _real p2[3], _real q2[3], _real r2[3],
    int* coplanar,
    _real source[3], _real target[3]);

/* coplanar returns whether the triangles are coplanar
 *  source and target are the endpoints of the segment of
 *  intersection if it exists)
 */


 /* some 3D macros */

#define CROSS(dest,v1,v2)                       \
dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
dest[2]=v1[0]*v2[1]-v1[1]*v2[0];

#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])

#define SUB(dest,v1,v2) dest[0]=v1[0]-v2[0]; \
dest[1]=v1[1]-v2[1]; \
dest[2]=v1[2]-v2[2]; 

#define SCALAR(dest,alpha,v) dest[0] = alpha * v[0]; \
dest[1] = alpha * v[1]; \
dest[2] = alpha * v[2];

#define CHECK_MIN_MAX(p1,q1,r1,p2,q2,r2) {\
SUB(v1,p2,q1)\
SUB(v2,p1,q1)\
CROSS(N1,v1,v2)\
SUB(v1,q2,q1)\
if (DOT(v1,N1) > 0.0) return 0;\
SUB(v1,p2,p1)\
SUB(v2,r1,p1)\
CROSS(N1,v1,v2)\
SUB(v1,r2,p1) \
if (DOT(v1,N1) > 0.0) return 0;\
else return 1; }



/* Permutation in a canonical form of T2's vertices */

#define TRI_TRI_3D(p1,q1,r1,p2,q2,r2,dp2,dq2,dr2) { \
if (dp2 > 0.0) { \
if (dq2 > 0.0) CHECK_MIN_MAX(p1,r1,q1,r2,p2,q2) \
else if (dr2 > 0.0) CHECK_MIN_MAX(p1,r1,q1,q2,r2,p2)\
else CHECK_MIN_MAX(p1,q1,r1,p2,q2,r2) }\
else if (dp2 < 0.0) { \
if (dq2 < 0.0) CHECK_MIN_MAX(p1,q1,r1,r2,p2,q2)\
else if (dr2 < 0.0) CHECK_MIN_MAX(p1,q1,r1,q2,r2,p2)\
else CHECK_MIN_MAX(p1,r1,q1,p2,q2,r2)\
} else { \
if (dq2 < 0.0) { \
if (dr2 >= 0.0)  CHECK_MIN_MAX(p1,r1,q1,q2,r2,p2)\
else CHECK_MIN_MAX(p1,q1,r1,p2,q2,r2)\
} \
else if (dq2 > 0.0) { \
if (dr2 > 0.0) CHECK_MIN_MAX(p1,r1,q1,p2,q2,r2)\
else  CHECK_MIN_MAX(p1,q1,r1,q2,r2,p2)\
} \
else  { \
if (dr2 > 0.0) CHECK_MIN_MAX(p1,q1,r1,r2,p2,q2)\
else if (dr2 < 0.0) CHECK_MIN_MAX(p1,r1,q1,r2,p2,q2)\
else return coplanar_tri_tri3d(p1,q1,r1,p2,q2,r2,N1,N2);\
}}}



/*
 *
 *  Three-dimensional Triangle-Triangle Overlap Test
 *
 */


int tri_tri_overlap_test_3d(_real p1[3], _real q1[3], _real r1[3],

    _real p2[3], _real q2[3], _real r2[3])
{
    _real dp1, dq1, dr1, dp2, dq2, dr2;
    _real v1[3], v2[3];
    _real N1[3], N2[3];

    /* Compute distance signs  of p1, q1 and r1 to the plane of
     triangle(p2,q2,r2) */


    SUB(v1, p2, r2)
        SUB(v2, q2, r2)
        CROSS(N2, v1, v2)

        SUB(v1, p1, r2)
        dp1 = DOT(v1, N2);
    SUB(v1, q1, r2)
        dq1 = DOT(v1, N2);
    SUB(v1, r1, r2)
        dr1 = DOT(v1, N2);

    if (((dp1 * dq1) > 0.0) && ((dp1 * dr1) > 0.0))  return 0;

    /* Compute distance signs  of p2, q2 and r2 to the plane of
     triangle(p1,q1,r1) */


    SUB(v1, q1, p1)
        SUB(v2, r1, p1)
        CROSS(N1, v1, v2)

        SUB(v1, p2, r1)
        dp2 = DOT(v1, N1);
    SUB(v1, q2, r1)
        dq2 = DOT(v1, N1);
    SUB(v1, r2, r1)
        dr2 = DOT(v1, N1);

    if (((dp2 * dq2) > 0.0) && ((dp2 * dr2) > 0.0)) return 0;

    /* Permutation in a canonical form of T1's vertices */




    if (dp1 > 0.0) {
        if (dq1 > 0.0) TRI_TRI_3D(r1, p1, q1, p2, r2, q2, dp2, dr2, dq2)
        else if (dr1 > 0.0) TRI_TRI_3D(q1, r1, p1, p2, r2, q2, dp2, dr2, dq2)
        else TRI_TRI_3D(p1, q1, r1, p2, q2, r2, dp2, dq2, dr2)
    }
    else if (dp1 < 0.0) {
        if (dq1 < 0.0) TRI_TRI_3D(r1, p1, q1, p2, q2, r2, dp2, dq2, dr2)
        else if (dr1 < 0.0) TRI_TRI_3D(q1, r1, p1, p2, q2, r2, dp2, dq2, dr2)
        else TRI_TRI_3D(p1, q1, r1, p2, r2, q2, dp2, dr2, dq2)
    }
    else {
        if (dq1 < 0.0) {
            if (dr1 >= 0.0) TRI_TRI_3D(q1, r1, p1, p2, r2, q2, dp2, dr2, dq2)
            else TRI_TRI_3D(p1, q1, r1, p2, q2, r2, dp2, dq2, dr2)
        }
        else if (dq1 > 0.0) {
            if (dr1 > 0.0) TRI_TRI_3D(p1, q1, r1, p2, r2, q2, dp2, dr2, dq2)
            else TRI_TRI_3D(q1, r1, p1, p2, q2, r2, dp2, dq2, dr2)
        }
        else {
            if (dr1 > 0.0) TRI_TRI_3D(r1, p1, q1, p2, q2, r2, dp2, dq2, dr2)
            else if (dr1 < 0.0) TRI_TRI_3D(r1, p1, q1, p2, r2, q2, dp2, dr2, dq2)
            else return coplanar_tri_tri3d(p1, q1, r1, p2, q2, r2, N1, N2);
        }
    }
};



int coplanar_tri_tri3d(_real p1[3], _real q1[3], _real r1[3],
    _real p2[3], _real q2[3], _real r2[3],
    _real normal_1[3], _real normal_2[3]) {

    _real P1[2], Q1[2], R1[2];
    _real P2[2], Q2[2], R2[2];

    _real n_x, n_y, n_z;

    n_x = ((normal_1[0] < 0) ? -normal_1[0] : normal_1[0]);
    n_y = ((normal_1[1] < 0) ? -normal_1[1] : normal_1[1]);
    n_z = ((normal_1[2] < 0) ? -normal_1[2] : normal_1[2]);


    /* Projection of the triangles in 3D onto 2D such that the area of
     the projection is maximized. */


    if ((n_x > n_z) && (n_x >= n_y)) {
        // Project onto plane YZ

        P1[0] = q1[2]; P1[1] = q1[1];
        Q1[0] = p1[2]; Q1[1] = p1[1];
        R1[0] = r1[2]; R1[1] = r1[1];

        P2[0] = q2[2]; P2[1] = q2[1];
        Q2[0] = p2[2]; Q2[1] = p2[1];
        R2[0] = r2[2]; R2[1] = r2[1];

    }
    else if ((n_y > n_z) && (n_y >= n_x)) {
        // Project onto plane XZ

        P1[0] = q1[0]; P1[1] = q1[2];
        Q1[0] = p1[0]; Q1[1] = p1[2];
        R1[0] = r1[0]; R1[1] = r1[2];

        P2[0] = q2[0]; P2[1] = q2[2];
        Q2[0] = p2[0]; Q2[1] = p2[2];
        R2[0] = r2[0]; R2[1] = r2[2];

    }
    else {
        // Project onto plane XY

        P1[0] = p1[0]; P1[1] = p1[1];
        Q1[0] = q1[0]; Q1[1] = q1[1];
        R1[0] = r1[0]; R1[1] = r1[1];

        P2[0] = p2[0]; P2[1] = p2[1];
        Q2[0] = q2[0]; Q2[1] = q2[1];
        R2[0] = r2[0]; R2[1] = r2[1];
    }

    return tri_tri_overlap_test_2d(P1, Q1, R1, P2, Q2, R2);

};



/*
 *
 *  Three-dimensional Triangle-Triangle Intersection
 *
 */

 /*
  This macro is called when the triangles surely intersect
  It constructs the segment of intersection of the two triangles
  if they are not coplanar.
  */

#define CONSTRUCT_INTERSECTION(p1,q1,r1,p2,q2,r2) { \
SUB(v1,q1,p1) \
SUB(v2,r2,p1) \
CROSS(N,v1,v2) \
SUB(v,p2,p1) \
if (DOT(v,N) > 0.0) {\
SUB(v1,r1,p1) \
CROSS(N,v1,v2) \
if (DOT(v,N) <= 0.0) { \
SUB(v2,q2,p1) \
CROSS(N,v1,v2) \
if (DOT(v,N) > 0.0) { \
SUB(v1,p1,p2) \
SUB(v2,p1,r1) \
alpha = DOT(v1,N2) / DOT(v2,N2); \
SCALAR(v1,alpha,v2) \
SUB(source,p1,v1) \
SUB(v1,p2,p1) \
SUB(v2,p2,r2) \
alpha = DOT(v1,N1) / DOT(v2,N1); \
SCALAR(v1,alpha,v2) \
SUB(target,p2,v1) \
return 1; \
} else { \
SUB(v1,p2,p1) \
SUB(v2,p2,q2) \
alpha = DOT(v1,N1) / DOT(v2,N1); \
SCALAR(v1,alpha,v2) \
SUB(source,p2,v1) \
SUB(v1,p2,p1) \
SUB(v2,p2,r2) \
alpha = DOT(v1,N1) / DOT(v2,N1); \
SCALAR(v1,alpha,v2) \
SUB(target,p2,v1) \
return 1; \
} \
} else { \
return 0; \
} \
} else { \
SUB(v2,q2,p1) \
CROSS(N,v1,v2) \
if (DOT(v,N) < 0.0) { \
return 0; \
} else { \
SUB(v1,r1,p1) \
CROSS(N,v1,v2) \
if (DOT(v,N) >= 0.0) { \
SUB(v1,p1,p2) \
SUB(v2,p1,r1) \
alpha = DOT(v1,N2) / DOT(v2,N2); \
SCALAR(v1,alpha,v2) \
SUB(source,p1,v1) \
SUB(v1,p1,p2) \
SUB(v2,p1,q1) \
alpha = DOT(v1,N2) / DOT(v2,N2); \
SCALAR(v1,alpha,v2) \
SUB(target,p1,v1) \
return 1; \
} else { \
SUB(v1,p2,p1) \
SUB(v2,p2,q2) \
alpha = DOT(v1,N1) / DOT(v2,N1); \
SCALAR(v1,alpha,v2) \
SUB(source,p2,v1) \
SUB(v1,p1,p2) \
SUB(v2,p1,q1) \
alpha = DOT(v1,N2) / DOT(v2,N2); \
SCALAR(v1,alpha,v2) \
SUB(target,p1,v1) \
return 1; \
}}}} 



#define TRI_TRI_INTER_3D(p1,q1,r1,p2,q2,r2,dp2,dq2,dr2) { \
if (dp2 > 0.0) { \
if (dq2 > 0.0) CONSTRUCT_INTERSECTION(p1,r1,q1,r2,p2,q2) \
else if (dr2 > 0.0) CONSTRUCT_INTERSECTION(p1,r1,q1,q2,r2,p2)\
else CONSTRUCT_INTERSECTION(p1,q1,r1,p2,q2,r2) }\
else if (dp2 < 0.0) { \
if (dq2 < 0.0) CONSTRUCT_INTERSECTION(p1,q1,r1,r2,p2,q2)\
else if (dr2 < 0.0) CONSTRUCT_INTERSECTION(p1,q1,r1,q2,r2,p2)\
else CONSTRUCT_INTERSECTION(p1,r1,q1,p2,q2,r2)\
} else { \
if (dq2 < 0.0) { \
if (dr2 >= 0.0)  CONSTRUCT_INTERSECTION(p1,r1,q1,q2,r2,p2)\
else CONSTRUCT_INTERSECTION(p1,q1,r1,p2,q2,r2)\
} \
else if (dq2 > 0.0) { \
if (dr2 > 0.0) CONSTRUCT_INTERSECTION(p1,r1,q1,p2,q2,r2)\
else  CONSTRUCT_INTERSECTION(p1,q1,r1,q2,r2,p2)\
} \
else  { \
if (dr2 > 0.0) CONSTRUCT_INTERSECTION(p1,q1,r1,r2,p2,q2)\
else if (dr2 < 0.0) CONSTRUCT_INTERSECTION(p1,r1,q1,r2,p2,q2)\
else { \
*coplanar = 1; \
return coplanar_tri_tri3d(p1,q1,r1,p2,q2,r2,N1,N2);\
} \
}} }


  /*
   The following version computes the segment of intersection of the
   two triangles if it exists.
   coplanar returns whether the triangles are coplanar
   source and target are the endpoints of the line segment of intersection
   */

int tri_tri_intersection_test_3d(_real p1[3], _real q1[3], _real r1[3],
    _real p2[3], _real q2[3], _real r2[3],
    int* coplanar,
    _real source[3], _real target[3])

{
    _real dp1, dq1, dr1, dp2, dq2, dr2;
    _real v1[3], v2[3], v[3];
    _real N1[3], N2[3], N[3];
    _real alpha;

    // Compute distance signs  of p1, q1 and r1 
    // to the plane of triangle(p2,q2,r2)


    SUB(v1, p2, r2)
        SUB(v2, q2, r2)
        CROSS(N2, v1, v2)

        SUB(v1, p1, r2)
        dp1 = DOT(v1, N2);
    SUB(v1, q1, r2)
        dq1 = DOT(v1, N2);
    SUB(v1, r1, r2)
        dr1 = DOT(v1, N2);

    if (((dp1 * dq1) > 0.0) && ((dp1 * dr1) > 0.0))  return 0;

    // Compute distance signs  of p2, q2 and r2 
    // to the plane of triangle(p1,q1,r1)


    SUB(v1, q1, p1)
        SUB(v2, r1, p1)
        CROSS(N1, v1, v2)

        SUB(v1, p2, r1)
        dp2 = DOT(v1, N1);
    SUB(v1, q2, r1)
        dq2 = DOT(v1, N1);
    SUB(v1, r2, r1)
        dr2 = DOT(v1, N1);

    if (((dp2 * dq2) > 0.0) && ((dp2 * dr2) > 0.0)) return 0;

    // Permutation in a canonical form of T1's vertices


    //  printf("d1 = [%f %f %f], d2 = [%f %f %f]\n", dp1, dq1, dr1, dp2, dq2, dr2);
    /*
     // added by Aaron
     if (ZERO_TEST(dp1) || ZERO_TEST(dq1) ||ZERO_TEST(dr1) ||ZERO_TEST(dp2) ||ZERO_TEST(dq2) ||ZERO_TEST(dr2))
     {
     coplanar = 1;
     return 0;
     }
     */


    if (dp1 > 0.0) {
        if (dq1 > 0.0) TRI_TRI_INTER_3D(r1, p1, q1, p2, r2, q2, dp2, dr2, dq2)
        else if (dr1 > 0.0) TRI_TRI_INTER_3D(q1, r1, p1, p2, r2, q2, dp2, dr2, dq2)

        else TRI_TRI_INTER_3D(p1, q1, r1, p2, q2, r2, dp2, dq2, dr2)
    }
    else if (dp1 < 0.0) {
        if (dq1 < 0.0) TRI_TRI_INTER_3D(r1, p1, q1, p2, q2, r2, dp2, dq2, dr2)
        else if (dr1 < 0.0) TRI_TRI_INTER_3D(q1, r1, p1, p2, q2, r2, dp2, dq2, dr2)
        else TRI_TRI_INTER_3D(p1, q1, r1, p2, r2, q2, dp2, dr2, dq2)
    }
    else {
        if (dq1 < 0.0) {
            if (dr1 >= 0.0) TRI_TRI_INTER_3D(q1, r1, p1, p2, r2, q2, dp2, dr2, dq2)
            else TRI_TRI_INTER_3D(p1, q1, r1, p2, q2, r2, dp2, dq2, dr2)
        }
        else if (dq1 > 0.0) {
            if (dr1 > 0.0) TRI_TRI_INTER_3D(p1, q1, r1, p2, r2, q2, dp2, dr2, dq2)
            else TRI_TRI_INTER_3D(q1, r1, p1, p2, q2, r2, dp2, dq2, dr2)
        }
        else {
            if (dr1 > 0.0) TRI_TRI_INTER_3D(r1, p1, q1, p2, q2, r2, dp2, dq2, dr2)
            else if (dr1 < 0.0) TRI_TRI_INTER_3D(r1, p1, q1, p2, r2, q2, dp2, dr2, dq2)
            else {
                // triangles are co-planar

                *coplanar = 1;
                return coplanar_tri_tri3d(p1, q1, r1, p2, q2, r2, N1, N2);
            }
        }
    }
};





/*
 *
 *  Two dimensional Triangle-Triangle Overlap Test
 *
 */


 /* some 2D macros */

#define ORIENT_2D(a, b, c)  ((a[0]-c[0])*(b[1]-c[1])-(a[1]-c[1])*(b[0]-c[0]))


#define INTERSECTION_TEST_VERTEXA(P1, Q1, R1, P2, Q2, R2) {\
if (ORIENT_2D(R2,P2,Q1) >= 0.0)\
if (ORIENT_2D(R2,Q2,Q1) <= 0.0)\
if (ORIENT_2D(P1,P2,Q1) > 0.0) {\
if (ORIENT_2D(P1,Q2,Q1) <= 0.0) return 1; \
else return 0;} else {\
if (ORIENT_2D(P1,P2,R1) >= 0.0)\
if (ORIENT_2D(Q1,R1,P2) >= 0.0) return 1; \
else return 0;\
else return 0;}\
else \
if (ORIENT_2D(P1,Q2,Q1) <= 0.0)\
if (ORIENT_2D(R2,Q2,R1) <= 0.0)\
if (ORIENT_2D(Q1,R1,Q2) >= 0.0) return 1; \
else return 0;\
else return 0;\
else return 0;\
else\
if (ORIENT_2D(R2,P2,R1) >= 0.0) \
if (ORIENT_2D(Q1,R1,R2) >= 0.0)\
if (ORIENT_2D(P1,P2,R1) >= 0.0) return 1;\
else return 0;\
else \
if (ORIENT_2D(Q1,R1,Q2) >= 0.0) {\
if (ORIENT_2D(R2,R1,Q2) >= 0.0) return 1; \
else return 0; }\
else return 0; \
else  return 0; \
};

#define INTERSECTION_TEST_VERTEX(P1, Q1, R1, P2, Q2, R2) {\
  if (ORIENT_2D(R2,P2,Q1) >= 0.0)\
    if (ORIENT_2D(R2,Q2,Q1) <= 0.0)\
      if (ORIENT_2D(P1,P2,Q1) > 0.0) {\
        if (ORIENT_2D(P1,Q2,Q1) <= 0.0) return 1; \
        else return 0;} else {\
        if (ORIENT_2D(P1,P2,R1) >= 0.0)\
          if (ORIENT_2D(Q1,R1,P2) >= 0.0) return 1; \
          else return 0;\
        else return 0;}\
    else \
      if (ORIENT_2D(P1,Q2,Q1) <= 0.0)\
        if (ORIENT_2D(R2,Q2,R1) <= 0.0)\
          if (ORIENT_2D(Q1,R1,Q2) >= 0.0) return 1; \
          else return 0;\
        else return 0;\
      else return 0;\
  else\
    if (ORIENT_2D(R2,P2,R1) >= 0.0) \
      if (ORIENT_2D(Q1,R1,R2) >= 0.0)\
        if (ORIENT_2D(P1,P2,R1) >= 0.0) return 1;\
        else return 0;\
      else \
        if (ORIENT_2D(Q1,R1,Q2) >= 0.0) {\
          if (ORIENT_2D(R2,R1,Q2) >= 0.0) return 1; \
          else return 0; }\
        else return 0; \
    else  return 0; \
 };


#define INTERSECTION_TEST_EDGE(P1, Q1, R1, P2, Q2, R2) { \
if (ORIENT_2D(R2,P2,Q1) >= 0.0) {\
if (ORIENT_2D(P1,P2,Q1) >= 0.0) { \
if (ORIENT_2D(P1,Q1,R2) >= 0.0) return 1; \
else return 0;} else { \
if (ORIENT_2D(Q1,R1,P2) >= 0.0){ \
if (ORIENT_2D(R1,P1,P2) >= 0.0) return 1; else return 0;} \
else return 0; } \
} else {\
if (ORIENT_2D(R2,P2,R1) >= 0.0) {\
if (ORIENT_2D(P1,P2,R1) >= 0.0) {\
if (ORIENT_2D(P1,R1,R2) >= 0.0) return 1;  \
else {\
if (ORIENT_2D(Q1,R1,R2) >= 0.0) return 1; else return 0;}}\
else  return 0; }\
else return 0; }}



int ccw_tri_tri_intersection_2d(_real p1[2], _real q1[2], _real r1[2],
    _real p2[2], _real q2[2], _real r2[2]) {
    if (ORIENT_2D(p2, q2, p1) >= 0.0) {
        if (ORIENT_2D(q2, r2, p1) >= 0.0) {
            if (ORIENT_2D(r2, p2, p1) >= 0.0) return 1;
            else INTERSECTION_TEST_EDGE(p1, q1, r1, p2, q2, r2)
        }
        else {
            if (ORIENT_2D(r2, p2, p1) >= 0.0)
                INTERSECTION_TEST_EDGE(p1, q1, r1, r2, p2, q2)
            else INTERSECTION_TEST_VERTEX(p1, q1, r1, p2, q2, r2)
        }
    }
    else {
        if (ORIENT_2D(q2, r2, p1) >= 0.0) {
            if (ORIENT_2D(r2, p2, p1) >= 0.0)
                INTERSECTION_TEST_EDGE(p1, q1, r1, q2, r2, p2)
            else  INTERSECTION_TEST_VERTEX(p1, q1, r1, q2, r2, p2)
        }
        else INTERSECTION_TEST_VERTEX(p1, q1, r1, r2, p2, q2)
    }
};


int tri_tri_overlap_test_2d(_real p1[2], _real q1[2], _real r1[2],
    _real p2[2], _real q2[2], _real r2[2]) {
    if (ORIENT_2D(p1, q1, r1) < 0.0)
        if (ORIENT_2D(p2, q2, r2) < 0.0)
            return ccw_tri_tri_intersection_2d(p1, r1, q1, p2, r2, q2);
        else
            return ccw_tri_tri_intersection_2d(p1, r1, q1, p2, q2, r2);
    else
        if (ORIENT_2D(p2, q2, r2) < 0.0)
            return ccw_tri_tri_intersection_2d(p1, q1, r1, p2, r2, q2);
        else
            return ccw_tri_tri_intersection_2d(p1, q1, r1, p2, q2, r2);

};

//
//int main()
//{
//    float p1[3] = { -0.323523998f, 1.68264794f, -1.20740700f };
//    float p2[3] = { -0.478354007f, 1.68264794f, -1.15484905f };
//    float p3[3] = { -0.465537012f, 1.80764794f, -1.12390494f };
//
//    float q1[3] = { 1.00000000f, -0.682647943f, -0.999998987f };
//    float q2[3] = { 1.00000000f, -2.68264794f, -1.00000000f };
//    float q3[3] = { -1.00000000f, -0.682647943f, -1.00000000f };
//
//    int i = tri_tri_overlap_test_3d(p1, p2, p3, q1, q2, q3);
//    printf("Result = %d\n", i);
//    return 0;
//}