/**********************************************************************
 * Some stuff to handle spheres
 **********************************************************************/
#include "vector.h"

typedef struct sphere {
  int index;               // identifies a sphere; must be greater than 0

  Point center;
  float radius;

  float mat_ambient[3];    // material property used in Phong model
  float mat_diffuse[3];
  float mat_specular[3];
  float mat_shineness;

  float reflectance;       // this number [0,1] determines how much 
                           // reflected light contributes to the color
                           // of a pixel
  float refractivity;
  float transmissivity;

  struct sphere *next;
} Spheres;   // a list of spheres


// intersect ray with sphere
Spheres *intersect_scene(Point, Vector, Spheres *, Point *);
// return the unit normal at a point on sphere
Vector sphere_normal(Point, Spheres *);
// add a sphere to the sphere list
Spheres *add_sphere(Spheres *, Point, float, float [], float [], float [], float, float, float, float, int);


typedef struct board { 

  Vector norm;
  Point center;
  float width;

  float mat_ambient[3];    // material property used in Phong model

  float reflectance;       // this number [0,1] determines how much 
  float refractivity;

} Boards;   // a list of spheres

// intersect ray with plane
bool intersect_board(Point, Vector, Boards, Point *);
RGB_float board_color(Boards, Point);