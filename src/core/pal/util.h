/*
 *   libpal - Automated Placement of Labels Library
 *
 *   Copyright (C) 2008 Maxence Laurent, MIS-TIC, HEIG-VD
 *                      University of Applied Sciences, Western Switzerland
 *                      http://www.hes-so.ch
 *
 *   Contact: 
 *      maxence.laurent <at> heig-vd <dot> ch
 *    or
 *      eric.taillard <at> heig-vd <dot> ch
 *
 * This file is part of libpal.
 * 
 * libpal is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * libpal is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with libpal.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
 
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef _UTIL_H
#define _UTIL_H

#include <cmath>
#include <cstring>
//#include <geos/geom/Geometry.h>
#include <geos_c.h>

#include "rtree.hpp"
#include "pointset.h"

namespace pal{

class LabelPosition;
class Layer;
class Feature;

inline bool ptrFeatureCompare(Feature * a, Feature * b){
   return a == b;
}


/**
 * \brief For translating feature from GEOS to Pal
 */
typedef struct Feat {
   const GEOSGeometry *geom;
   const char *id;
   int type;

   int nbPoints;
   double *x;
   double *y;

   double minmax[4]; // {xmin, ymin, xmax, ymax}

   int nbHoles;
   PointSet **holes;
   
} Feat;


/**
 * \brief split GEOS geom (multilinestring, multipoint, multipolygon) => (point, linestring, polygone)
 */
LinkedList<Feat*> * splitGeom (GEOSGeometry *the_geom, const char *geom_id);




typedef struct _feats {
   Feature *feature;
   PointSet *shape;
   double priority;
   int nblp;
   LabelPosition **lPos;
} Feats;


typedef struct _elementary_transformation{
   int feat;
   int  old_label;
   int  new_label;
} ElemTrans;



#define EPSILON 1e-9


inline int max(int a, int b)
   {return (a > b ?a:b);}

inline double max(double a, double b)
   {return (a > b ?a:b);}

inline int min(int a, int b) 
   {return (a < b?a:b);}

inline double min(double a, double b)
   {return (a < b?a:b);}

inline double vabs(double x)
   { return x >= 0 ? x : -x; }

inline double px2meters (double x, int dpi, double scale)
   { return ((x / double(dpi)) * 0.0254) * scale; }



/* From meters to PostScript Point */
inline void convert (int *x, double scale, int dpi){
   *x = (int)(((double)*x / scale) * 39.3700787402 * dpi + 0.5);
}


inline int convert (double x, double scale, int dpi){
   return (int)((x / scale) * 39.3700787402 * dpi + 0.5);
}


void sort(double* heap, int* x, int* y, int N);


inline bool intCompare(int a, int b){
   return a == b;
}

inline bool strCompare(char * a, char * b){
   return strcmp (a,b) == 0;
}

inline bool ptrLPosCompare(LabelPosition * a, LabelPosition * b){
   return a == b;
}

inline bool ptrPSetCompare(PointSet * a, PointSet * b){
   return a == b;
}


inline bool ptrFeatCompare(Feat * a, Feat * b){
   return a == b;
}

inline bool ptrFeatsCompare(Feats * a, Feats * b){
   return a == b;
}

inline bool ptrLayerCompare(Layer * a, Layer * b){
   return a == b;
}


inline bool ptrETCompare(ElemTrans * a, ElemTrans * b){
   return a == b;
}

/**
 * \brief Sort an array of pointers
 * \param items arays of pointers to sort
 * \param N number of items
 * \param greater function to compare two items
 **/
void sort(void** items, int N, bool (*greater)(void *l, void *r));

void tabcpy(int n, const int* const x, const int* const y, 
            const double* const prob, int *cx, int *cy, double *p);


typedef struct {
   LabelPosition *lp;
   int *nbOv;
   double *cost;
   double *inactiveCost;
   //int *feat;
} CountContext;

/*
 * count overlap, ctx = p_lp
 */
bool countOverlapCallback(LabelPosition *lp, void *ctx);

bool countFullOverlapCallback(LabelPosition *lp, void *ctx);

} // namespace

#endif
