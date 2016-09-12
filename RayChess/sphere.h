/**********************************************************************
 * Some stuff to handle chess and board
 **********************************************************************/
#include "vector.h"

typedef struct chess {
  int index;               // identifies a sphere; must be greater than 0

  Point vertices[3];

  Vector nor;

  float mat_ambient[3];    // material property used in Phong model
  float mat_diffuse[3];
  float mat_specular[3];
  float mat_shineness;

  float reflectance;       // this number [0,1] determines how much 
                           // reflected light contributes to the color
                           // of a pixel
  float transmissivity;
  float refractivity;

  struct chess *next;
} Chesses;   // a list of chesses

float intersect_chess(Point, Vector, Chesses *, Point *);
// intersect ray with chess
Chesses *intersect_scene(Point, Vector, Chesses *, Point *);
// add a chess to the chess list
Chesses *add_chess(Chesses *, Point [], Vector, float [], float [], float [], float, float, float, float, int);


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