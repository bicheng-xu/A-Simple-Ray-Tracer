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

    float interval = chessboard.width/800;
    
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
float intersect_chess(Point o, Vector u, Chesses *che, Point *hit)
{
    Point p1 = che->vertices[0];
    Point p2 = che->vertices[1];
    Point p3 = che->vertices[2];
    Point p;

    Vector nor = che->nor;
  
    if (vec_dot(u, nor) > 0)
    {
        inverse(&nor);
    }
    
    float y = (p1.x-o.x)*nor.x + (p1.y-o.y)*nor.y + (p1.z-o.z)*nor.z;
    float x = u.x*nor.x + u.y*nor.y + u.z*nor.z;
    float para_t = y/x;

    p.x = o.x + para_t*u.x;
    p.y = o.y + para_t*u.y;
    p.z = o.z + para_t*u.z;

    hit->x = o.x + para_t*u.x;
    hit->y = o.y + para_t*u.y;
    hit->z = o.z + para_t*u.z;

    float alpha = ((p2.y-p3.y)*(p.x-p3.x) + (p3.x-p2.x)*(p.y-p3.y)) / ((p2.y-p3.y)*(p1.x-p3.x) + (p3.x-p2.x)*(p1.y-p3.y));
    float beta = ((p3.y-p1.y)*(p.x-p3.x) + (p1.x-p3.x)*(p.y-p3.y)) / ((p2.y-p3.y)*(p1.x-p3.x) + (p3.x-p2.x)*(p1.y-p3.y));
    float gamma = 1.0f - alpha - beta;

    if (para_t > 0.0001 && alpha >= 0 && beta >= 0 && gamma >= 0)
    {
        return para_t;
    }
    else
    {
        return -1.0;
    }
}

/*********************************************************************
 * This function returns a pointer to the sphere object that the
 * ray intersects first; NULL if no intersection. You should decide
 * which arguments to use for the function. For exmaple, note that you
 * should return the point of intersection to the calling function.
 **********************************************************************/
Chesses *intersect_scene(Point point_o, Vector vector_u, Chesses *scene_list, Point *hit) {

    float shortest_dist = FLT_MAX;
    Chesses *current_chess = scene_list;
    Chesses *closest_chess = NULL;
    Point *temp_point = new Point;
    float current_dist = 0;

    while(current_chess)
    {
        current_dist = intersect_chess(point_o, vector_u, current_chess, temp_point);
        //if has intersection
        if( current_dist > 0.0001 && current_dist < shortest_dist )
        {
            shortest_dist = current_dist; 
            closest_chess = current_chess;
            *hit = *temp_point;
        }
    
        current_chess = current_chess->next;
    }
      
    delete temp_point;
	return closest_chess;
}
/*****************************************************
 * This function adds a sphere into the chess list
 *
 * You need not change this.
 *****************************************************/
Chesses *add_chess(Chesses *clist, Point v[], Vector normal, float amb[],
        float dif[], float spe[], float shine,
        float refl, float refr, float eta, int cindex) {
    
    Chesses *new_chess;

    new_chess = (Chesses *)malloc(sizeof(Chesses));
    new_chess->index = cindex;
    new_chess->nor = normal;
    new_chess->vertices[0] = v[0];
    new_chess->vertices[1] = v[1];
    new_chess->vertices[2] = v[2];
    (new_chess->mat_ambient)[0] = amb[0];
    (new_chess->mat_ambient)[1] = amb[1];
    (new_chess->mat_ambient)[2] = amb[2];
    (new_chess->mat_diffuse)[0] = dif[0];
    (new_chess->mat_diffuse)[1] = dif[1];
    (new_chess->mat_diffuse)[2] = dif[2];
    (new_chess->mat_specular)[0] = spe[0];
    (new_chess->mat_specular)[1] = spe[1];
    (new_chess->mat_specular)[2] = spe[2];
    new_chess->mat_shineness = shine;
    new_chess->reflectance = refl;
    new_chess->transmissivity = refr;
    new_chess->refractivity = eta;
    new_chess->next = NULL;

    if (clist == NULL) { // first object
        clist = new_chess;
    } 
    else 
    { // insert at the beginning
        new_chess->next = clist;
        clist = new_chess;
    }

    return clist;
}
