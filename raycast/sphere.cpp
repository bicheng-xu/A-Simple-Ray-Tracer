#include "sphere.h"
#include <stdlib.h>
#include <math.h>
#include <cfloat>

//check whether a ray intersects with the board
bool intersect_board(Point point_o, Vector vector_u, Boards chessboard, Point *hit)
{
    float numerator = vec_dot(chessboard.norm, get_vec(chessboard.center, point_o));
    float denominator = vec_dot(chessboard.norm, vector_u);
    
    if( denominator == 0 ) 
    {
        return false;
    }

    float para_t = -numerator/denominator;
 
    // no intersection
    if( para_t < 0 ) 
    {
        return false;
    }
    // get hit point
    *hit = get_point(point_o, vec_scale(vector_u, para_t));

    float board_left = chessboard.center.x - chessboard.width/2;
    float board_right = chessboard.center.x + chessboard.width/2;
    float board_front = chessboard.center.z + chessboard.width/2;
    float board_rear = chessboard.center.z - chessboard.width/2;

    // hit point exceeds the border
    if ( hit->x > board_right || hit->x < board_left || hit->z > board_front || hit->z < board_rear )
    {
        return false;
    }
  
    return true;
}

// color the chess board
RGB_float board_color(Boards chessboard, Point hit)
{
    RGB_float color;
    
    float board_left = chessboard.center.x - chessboard.width/2;
    float board_rear = chessboard.center.z - chessboard.width/2;

    float interval = chessboard.width/8;
    
    hit.x -= board_left;
    hit.y -= board_rear;

    int xx = (int)(hit.x / interval);
    int zz = (int)(hit.z / interval);

    if( (xx+zz)%2 == 0)
    {
        color = {0, 0, 0};
    }
    else
    {
        color = {1, 1, 1};
    } 

    return color;
}

/**********************************************************************
 * This function intersects a ray with a given sphere 'sph'. You should
 * use the parametric representation of a line and do the intersection.
 * The function should return the parameter value for the intersection, 
 * which will be compared with others to determine which intersection
 * is closest. The value -1.0 is returned if there is no intersection
 *
 * If there is an intersection, the point of intersection should be
 * stored in the "hit" variable
 **********************************************************************/
float intersect_sphere(Point point_o, Vector vector_u, Spheres *current_sph, Point *hit) {
    
    Vector eye_to_center = get_vec(point_o, current_sph->center);
    float rt_side = vec_dot(vector_u, eye_to_center)/vec_len(vector_u);
    float dist_square = pow(vec_len(eye_to_center), 2) - pow(rt_side, 2);
    float radius_square = pow(current_sph->radius, 2);

    // if no intersection
    if ( dist_square > radius_square )
    {
        return -1.0;
    }
    // if has intersection
    float dis_to_hit = rt_side - sqrt( radius_square - dist_square );

    if (dis_to_hit < 0)
    {
        return -1.0;
    }

    Vector vector_temp = {vector_u.x, vector_u.y, vector_u.z};
    normalize(&vector_temp);

    *hit = get_point(point_o,  vec_scale(vector_temp, dis_to_hit) );
    
    return dis_to_hit;
}

/*********************************************************************
 * This function returns a pointer to the sphere object that the
 * ray intersects first; NULL if no intersection. You should decide
 * which arguments to use for the function. For exmaple, note that you
 * should return the point of intersection to the calling function.
 **********************************************************************/
Spheres *intersect_scene(Point point_o, Vector vector_u, Spheres *scene_list, Point *hit) {

    //float shortest_dist = 999999;
    float shortest_dist = FLT_MAX;
    float current_dist = 0;
    Spheres *current_sph = scene_list;
    Spheres *closest_sph = NULL;

    while(current_sph != NULL)
    {
        current_dist = intersect_sphere(point_o, vector_u, current_sph, hit);

        //if no intersection
        //if ( pow(current_dist-(-1),2) < 0.0001 )
        if( current_dist == -1.0 )
        {
            current_sph = current_sph->next;
            continue;
        }
        //if has intersection
        if( current_dist < shortest_dist )
        {
            shortest_dist = current_dist; 
            closest_sph = current_sph;
        }
    
        current_sph = current_sph->next;
    }

	  return closest_sph;
}

/*****************************************************
 * This function adds a sphere into the sphere list
 *
 * You need not change this.
 *****************************************************/
Spheres *add_sphere(Spheres *slist, Point ctr, float rad, float amb[],
		    float dif[], float spe[], float shine, 
		    float refl, float refr, float reft, int sindex) {
  Spheres *new_sphere;

  new_sphere = (Spheres *)malloc(sizeof(Spheres));
  new_sphere->index = sindex;
  new_sphere->center = ctr;
  new_sphere->radius = rad;
  (new_sphere->mat_ambient)[0] = amb[0];
  (new_sphere->mat_ambient)[1] = amb[1];
  (new_sphere->mat_ambient)[2] = amb[2];
  (new_sphere->mat_diffuse)[0] = dif[0];
  (new_sphere->mat_diffuse)[1] = dif[1];
  (new_sphere->mat_diffuse)[2] = dif[2];
  (new_sphere->mat_specular)[0] = spe[0];
  (new_sphere->mat_specular)[1] = spe[1];
  (new_sphere->mat_specular)[2] = spe[2];
  new_sphere->mat_shineness = shine;
  new_sphere->reflectance = refl;
  new_sphere->refractivity = refr;
  new_sphere->transmissivity = reft;
  new_sphere->next = NULL;

  if (slist == NULL) { // first object
    slist = new_sphere;
  } else { // insert at the beginning
    new_sphere->next = slist;
    slist = new_sphere;
  }

  return slist;
}

/******************************************
 * computes a sphere normal - done for you
 ******************************************/
Vector sphere_normal(Point q, Spheres *sph) {
  Vector rc;

  rc = get_vec(sph->center, q);
  normalize(&rc);
  return rc;
}
