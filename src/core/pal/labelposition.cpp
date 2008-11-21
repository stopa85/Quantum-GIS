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

#define _CRT_SECURE_NO_DEPRECATE

#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <cfloat>

#include <pal/layer.h>
#include <pal/pal.h>
#include <pal/label.h>

#include "feature.h"
#include "geomfunction.h"
#include "labelposition.h"

#ifndef M_PI
#define M_PI 3.1415926535897931159979634685
#endif


namespace pal{

LabelPosition::LabelPosition (int id, double x1, double y1, double w, double h, double alpha, double cost, Feature *feature) : id(id), cost(cost), /*workingCost (0),*/ alpha(alpha), feature(feature), nbOverlap(0), w(w), h(h){


   // alpha take his value bw 0 and 2*pi rad
   while (this->alpha > 2*M_PI)
      this->alpha -= 2*M_PI;

   while (this->alpha < 0)
      this->alpha += 2*M_PI;

   register double beta = this->alpha + (M_PI / 2);

   double dx1, dx2, dy1, dy2;

   double tx, ty;

   dx1 = cos(this->alpha)*w;
   dy1 = sin(this->alpha)*w;

   dx2 = cos(beta)*h;
   dy2 = sin(beta)*h;

   x[0] = x1;
   y[0] = y1;

   x[1] = x1 + dx1; 
   y[1] = y1 + dy1;

   x[2] = x1 + dx1 + dx2;
   y[2] = y1 + dy1 + dy2;

   x[3] = x1 + dx2;
   y[3] = y1 + dy2;

   // upside down ?
   if (this->alpha > M_PI/2 && this->alpha <= 3*M_PI/2){
      tx = x[0];
      ty = y[0];

      x[0] = x[2];
      y[0] = y[2];

      x[2] = tx;
      y[2] = ty;

      tx = x[1];
      ty = y[1];

      x[1] = x[3];
      y[1] = y[3];

      x[3] = tx;
      y[3] = ty;

      if (this->alpha < M_PI)
         this->alpha += M_PI;
      else
         this->alpha -= M_PI;
   }
}

bool LabelPosition::isIn(double *bbox){
   int i;

   for (i=0;i<4;i++){
      if (x[i] >= bbox[0] && x[i] <= bbox[2] &&
            y[i] >= bbox[1] && y[i] <= bbox[3])
         return true;
   }

   return false;

}

void LabelPosition::print(){
   std::cout << feature->getLayer()->getName() << "/" << feature->getUID() << "/"<< id;
   std::cout << " cost: " << cost;
   std::cout << " alpha" << alpha << std::endl;
   std::cout << x[0] << ", " << y[0] << std::endl;
   std::cout << x[1] << ", " << y[1] << std::endl;
   std::cout << x[2] << ", " << y[2] << std::endl;
   std::cout << x[3] << ", " << y[3] << std::endl;
   std::cout << std::endl;
}

bool LabelPosition::isInConflict (LabelPosition *ls){
   int i,i2,j;
   int d1, d2; 

   if (this->probFeat == ls->probFeat) // bugfix #1 
      return false; // always overlaping itself !

   double cp1, cp2;


   //std::cout << "Check intersect" << std::endl;
   for (i=0;i<4;i++){
      i2 = (i+1)%4;
      d1 = -1;
      d2 = -1;
      //std::cout << "new seg..." << std::endl;
      for (j=0;j<4;j++){
         cp1 = cross_product(x[i], y[i], x[i2], y[i2], ls->x[j], ls->y[j]);
         if (cp1 > 0){
            d1 = 1;
            //std::cout << "    cp1: " << cp1 << std::endl;
         }
         cp2 = cross_product(ls->x[i], ls->y[i], 
                  ls->x[i2], ls->y[i2],
                  x[j], y[j]);

         if (cp2 > 0){
            d2 = 1;
            //std::cout << "     cp2 " << cp2 << std::endl;
         }
      }

      if (d1==-1 || d2 == -1) // disjoint
         return false;
   }
   return true;
}

int LabelPosition::getId(){
   return id;
}

double LabelPosition::getX(){
   return x[0];
}

double LabelPosition::getY(){
   return y[0];
}

double LabelPosition::getAlpha(){
   return alpha;
}

double LabelPosition::getCost(){
   return cost;
}

Feature * LabelPosition::getFeature(){
   return feature;
}

bool xGrow(void *l, void *r){
   return ((LabelPosition*)l)->x[0] > ((LabelPosition*)r)->x[0];
}

bool yGrow(void *l, void *r){
   return ((LabelPosition*)l)->y[0] > ((LabelPosition*)r)->y[0];
}

bool xShrink(void *l, void *r){
   return ((LabelPosition*)l)->x[0] < ((LabelPosition*)r)->x[0];
}

bool yShrink(void *l, void *r){
   return ((LabelPosition*)l)->y[0] < ((LabelPosition*)r)->y[0];
}

bool costShrink (void *l, void *r){
   return ((LabelPosition*)l)->cost < ((LabelPosition*)r)->cost;
}

bool costGrow (void *l, void *r){
   return ((LabelPosition*)l)->cost > ((LabelPosition*)r)->cost;
}

/*
bool workingCostShrink (void *l, void *r){
   return ((LabelPosition*)l)->workingCost < ((LabelPosition*)r)->workingCost;
}

bool workingCostGrow (void *l, void *r){
   return ((LabelPosition*)l)->workingCost > ((LabelPosition*)r)->workingCost;
}
*/

Label *LabelPosition::toLabel(bool active){

   //double x[4], y;

   //x = this->x[0];
   //y = this->y[0];

//#warning retourner les coord projeté ou pas ?
   //feature->layer->pal->proj->getLatLong(this->x[0], this->y[0], &x, &y);

   return new Label(this->x, this->y, alpha, feature->uid, feature->layer->name, feature->userGeom);
}

double LabelPosition::getCostFromPolygon(int n, double *x, double *y, double &dist_sq){
   double return_value;

   double px = (this->x[0] + this->x[2]) / 2.0;
   double py = (this->y[0] + this->y[2]) / 2.0;

#ifdef _DEBUG_
   std::cout << "GetCostFromPolygon " << feature->uid << std::endl;
#endif

   if (n == 1){
      return dist_euc2d_sq (x[0], y[0], px, py);
   }

   double d;
   double best_d;

   int a;
   int b;

   double cx, cy;
   double ex, ey;
   double fx, fy;
   double dx, dy;

   double seg_length;
   double cp;
   int i;

   bool isPointIn;
   bool bestPtIn = false;

   double *dist = new double[n];

   double *ext = new double[n];
   double *eyt = new double[n];
   double *fxt = new double[n];
   double *fyt = new double[n];


   for (a=0;a<n;a++){
      
      b = (a+1)%n;
      // compute various data for segment a->b

      dist[a] = dist_euc2d (x[a], y[a], x[b], y[b]); // a->b length TODO dist_euc2d_sq

      // (cx;cy) = a->b center
      cx = (x[a] + x[b]) / 2.0;
      cy = (y[a] + y[b]) / 2.0;

      dx = cy - y[a];
      dy = cx - x[a];

      // eXYt and fXYt is perpendicar than a->b
      ext[a] = cx - dx;
      eyt[a] = cy + dy;

      fxt[a] = cx + dx;
      fyt[a] = cy - dy;

      //std::cout << x[a] << " ; " << y[a] << std::endl;
   }


   return_value = 0;

#ifdef _DEBUG_FULL_
   std::cout << "New setCost" << std::endl;
#endif
   for (i=0;i<5;i++){
      best_d = DBL_MAX;
      
#ifdef _DEBUG_FULL_
      std::cout << " POINT: " << px << ";" << py << std::endl;
#endif

	   isPointIn = isPointInPolygon (n, x, y, px, py);

      for (a=0;a<n;a++){
         b = (a+1)%n;

         seg_length = dist[a];

#ifdef _DEBUG_FULL_
         std::cout << "seg_length: " << seg_length << std::endl;
         std::cout <<  "a, b : " << a << "->"<<  x[a] << " " <<  y[a] << "   " << b << "->"  <<  x[b] << " " <<  y[b] << "    seg:" << seg_length << std::endl;
#endif
         ex = ext[a];
         ey = eyt[a];

         fx = fxt[a];
         fy = fyt[a];

         if (seg_length < EPSILON || vabs(cross_product (ex, ey, fx, fy, px, py) / (seg_length)) > (seg_length / 2)){
            d = min (dist_euc2d_sq (x[b], y[b], px, py), dist_euc2d_sq (x[a], y[a], px, py));
#ifdef _DEBUG_FULL_
            std::cout << "      d² from dist:" << d << std::endl;
#endif
         }
         else{
            cp = cross_product (x[a], y[a], x[b], y[b], px, py) / seg_length;

            d = cp*cp;
#ifdef _DEBUG_FULL_
            std::cout << "      cp: " << x[a] << " " <<  y[a] <<" " <<  x[b] <<" " <<  y[b] <<" " <<  px <<" " <<  py << std::endl;
            std::cout << "      d² from cp:" << d << std::endl;
#endif
         }

         if (d < best_d){
            best_d = d;
            bestPtIn = isPointIn;
         }
      }

      if (!bestPtIn)
         best_d *= -1;


#ifdef _DEBUG_FULL_
      std::cout << "   best_d : " << best_d << std::endl;
#endif

      if (i==0){
         return_value += 4*best_d;
         dist_sq = best_d;
      }
      else{
         return_value += best_d;
      }

      px = this->x[i];
      py = this->y[i];
   }

#undef _DEBUG_FULL_

   delete[] dist;
   delete[] ext;
   delete[] eyt;
   delete[] fxt;
   delete[] fyt;

   return return_value;
}


double LabelPosition::getCostFromPoint(double x, double y){

#ifdef _DEBUG_
   std::cout << "getCostFromPoint" << std::endl;
#endif

   double return_value;

   double px = (this->x[0] + this->x[2]) / 2.0;
   double py = (this->y[0] + this->y[2]) / 2.0;

   return_value = 0;

   double dist;
   int i;

   for (i=0;i<5;i++){
      dist = dist_euc2d_sq (px, py, x, y);
      if (i==0)
         return_value += 4*dist;
      else
         return_value += dist;

      px = this->x[i];
      py = this->y[i];
   }

   return return_value;
}



double LabelPosition::getCostFromLine(int n, double *x, double *y){
   double return_value;

   double px = (this->x[0] + this->x[2]) / 2.0;
   double py = (this->y[0] + this->y[2]) / 2.0;

#ifdef _DEBUG_FULL_
   std::cout << "getCostFromLine " << feature->uid << std::endl;
#endif

   if (n == 1){
      return dist_euc2d_sq (x[0], y[0], px, py);
   }

   double d;
   double best_d;

   int a;
   int b;

   double cx, cy;
   double ex, ey;
   double fx, fy;
   double dx, dy;

   double seg_length;
   double cp;
   int i;

   double *dist = new double[n];

   double *ext = new double[n];
   double *eyt = new double[n];
   double *fxt = new double[n];
   double *fyt = new double[n];


   for (a=0;a<n;a++){
      
      b = (a+1)%n;
      // compute various data for segment a->b

      dist[a] = dist_euc2d (x[a], y[a], x[b], y[b]); // a->b length TODO dist_euc2d_sq

      // (cx;cy) = a->b center
      cx = (x[a] + x[b]) / 2.0;
      cy = (y[a] + y[b]) / 2.0;

      dx = cy - y[a];
      dy = cx - x[a];

      // eXYt and fXYt is perpendicar than a->b
      ext[a] = cx - dx;
      eyt[a] = cy + dy;

      fxt[a] = cx + dx;
      fyt[a] = cy - dy;
   }


   return_value = 0;

#ifdef _DEBUG_FULL_
   std::cout << "New setCost" << std::endl;
#endif
   for (i=0;i<5;i++){
      best_d = DBL_MAX;
      
#ifdef _DEBUG_FULL_
      std::cout << " POINT: " << px << ";" << py << std::endl;
#endif

	   //isPointIn = isPointInPolygon (n, x, y, px, py);

      for (a=0;a<n;a++){
         b = (a+1)%n;

         seg_length = dist[a];

#ifdef _DEBUG_FULL_
         std::cout << "seg_length: " << seg_length << std::endl;
         std::cout <<  "a, b : " << a << "->"<<  x[a] << " " <<  y[a] << "   " << b << "->"  <<  x[b] << " " <<  y[b] << "    seg:" << seg_length << std::endl;
#endif
         ex = ext[a];
         ey = eyt[a];

         fx = fxt[a];
         fy = fyt[a];

         if (seg_length < EPSILON || vabs(cross_product (ex, ey, fx, fy, px, py) / (seg_length)) > (seg_length / 2)){
            d = min (dist_euc2d_sq (x[b], y[b], px, py), dist_euc2d_sq (x[a], y[a], px, py));
#ifdef _DEBUG_FULL_
            std::cout << "      d² from dist:" << d << std::endl;
#endif
         }
         else{
            cp = cross_product (x[a], y[a], x[b], y[b], px, py) / seg_length;

            d = cp*cp;
#ifdef _DEBUG_FULL_
            std::cout << "      cp: " << x[a] << " " <<  y[a] <<" " <<  x[b] <<" " <<  y[b] <<" " <<  px <<" " <<  py << std::endl;
            std::cout << "      d² from cp:" << d << std::endl;
#endif
         }

         if (d < best_d){
            best_d = d;
         }
      }

#ifdef _DEBUG_FULL_
      std::cout << "   best_d : " << best_d << std::endl;
#endif

      if (i==0)
         return_value += 4*best_d;
      else
         return_value += best_d;

      px = this->x[i];
      py = this->y[i];
   }

   delete[] dist;
   delete[] ext;
   delete[] eyt;
   delete[] fxt;
   delete[] fyt;

   return return_value;
}





bool obstacleCallback (PointSet *feat, void *ctx){

   //std::cout << "   ObstacleCallback" << std::endl;

   LabelPosition *lp = (LabelPosition*)ctx;
 

   if ((feat == lp->feature) || (feat->holeOf && feat->holeOf != lp->feature) ){
      return true;
   }

   double tmp_cost = DBL_MAX;
   double dist;

   // if the feature is not a hole we have to fetch corrdinates
   // otherwise holes coordinates are still in memory (feature->selfObs)
   if (feat->holeOf == NULL){
      ((Feature*)feat)->fetchCoordinates();
   }

   switch (feat->type){
    case GEOS_POINT:
      tmp_cost = lp->getCostFromPoint (feat->x[0], feat->y[0]);
      break;
    case GEOS_LINESTRING:
      tmp_cost = lp->getCostFromLine (feat->nbPoints, feat->x, feat->y);
      break;
    case GEOS_POLYGON:
      tmp_cost = - lp->getCostFromPolygon (feat->nbPoints, feat->x, feat->y, dist);
      break;
   }

   if (feat->holeOf == NULL){
      ((Feature*)feat)->releaseCoordinates();
   }

   //std::cout << "     tmp_cost: " << tmp_cost << std::endl;
   //std::cout << "Tmp cost :" << tmp_cost << std::endl;
   lp->cost = (vabs(tmp_cost) < vabs(lp->cost) ? tmp_cost : lp->cost);
   //std::cout << "     cost: " << lp->cost << std::endl;
   //std::cout << "new cost :" << lp->cost << std::endl;

   return true;
}

void LabelPosition::setCostFromPolygon (RTree <PointSet*, double, 2, double> *ftIndex, double bbx[4], double bby[4]){

   double amin[2];
   double amax[2];

   double dist_sq, dist_sq_bb, dist;


   double bb_cost = getCostFromPolygon (4, bbx, bby, dist_sq_bb);

   feature->fetchCoordinates();

   cost = getCostFromPolygon (feature->nbPoints, feature->x, feature->y, dist_sq);

   if (vabs(bb_cost) < vabs(cost)){
      cost = bb_cost;
   }
    
   if (dist_sq > (w*w + h*h) / 4.0){
      dist = sqrt (dist_sq);
      amin[0] = (x[0] + x[2]) / 2.0 - dist;
      amin[1] = (y[0] + y[2]) / 2.0 - dist;
      amax[0] = amin[0] + 2*dist;
      amax[1] = amin[1] + 2*dist;
   }
   else{
      amin[0] = feature->xmin;
      amin[1] = feature->ymin;

      amax[0] = feature->xmax;
      amax[1] = feature->ymax;
   }

   //std::cout << amin[0] << " " << amin[1] << " " << amax[0] << " " <<  amax[1] << std::endl;
   ftIndex->Search(amin, amax, obstacleCallback, this);
   
   feature->releaseCoordinates();
}

void LabelPosition::removeFromIndex (RTree<LabelPosition*, double, 2, double> *index){
   double amin[2];
   double amax[2];
   int c;

   amin[0] = DBL_MAX;
   amax[0] = -DBL_MAX;
   amin[1] = DBL_MAX;
   amax[1] = -DBL_MAX;
   for (c=0;c<4;c++){
      if (x[c] < amin[0])
         amin[0] = x[c];
      if (x[c] > amax[0])
         amax[0] = x[c];
      if (y[c] < amin[1])
         amin[1] = y[c];
      if (y[c] > amax[1])
         amax[1] = y[c];
   }

   index->Remove(amin, amax, this);
}


void LabelPosition::insertIntoIndex (RTree<LabelPosition*, double, 2, double> *index){
   double amin[2];
   double amax[2];
   int c;

   amin[0] = DBL_MAX;
   amax[0] = -DBL_MAX;
   amin[1] = DBL_MAX;
   amax[1] = -DBL_MAX;
   for (c=0;c<4;c++){
      if (x[c] < amin[0])
         amin[0] = x[c];
      if (x[c] > amax[0])
         amax[0] = x[c];
      if (y[c] < amin[1])
         amin[1] = y[c];
      if (y[c] > amax[1])
         amax[1] = y[c];
   }

   index->Insert(amin, amax, this);
}



void setCost (int nblp, LabelPosition **lPos, int max_p, RTree<PointSet*, double, 2, double> *ftIndex, double bbx[4], double bby[4]){
   //std::cout << "setCost:" << std::endl;
   //clock_t clock_start = clock();

   int i;

   double cost_min = DBL_MAX;
   double cost_max = -DBL_MAX;

   double normalizer;

   // compute raw cost 
   
#ifdef _DEBUG_
   std::cout << "LabelPosition for polygone: " << lPos[0]->feature->uid << std::endl;
#endif

   for (i=0;i<nblp;i++)
      lPos[i]->setCostFromPolygon (ftIndex, bbx, bby);
   
   // lPos with big values came fisrts (value = min distance from label to Polygon's Perimeter)
   sort ((void**)lPos, nblp, costShrink);


   // define the value's range
   cost_min = lPos[0]->cost;
   cost_max = lPos[max_p-1]->cost;

   cost_min -= cost_max;

   normalizer = 0.0020 / cost_min;

   // adjust cost => the best is 0.0001, the sorst is 0.0021
   // others are set proportionally between best and worst
   for (i=0;i<max_p;i++){
#ifdef _DEBUG_
      std::cout << "   lpos[" << i << "] = " << lPos[i]->cost;
#endif
      if (cost_min < EPSILON)
         lPos[i]->cost = 0.0001;
      else
         lPos[i]->cost = 0.0021 - (lPos[i]->cost - cost_max)*normalizer;

#ifdef _DEBUG_
      std::cout <<  "  ==>  " << lPos[i]->cost << std::endl;
#endif
   }
}


} // end namespace

