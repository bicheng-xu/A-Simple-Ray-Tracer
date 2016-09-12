#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include "global.h"
#include "sphere.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>
using namespace std;
//
// Global variables
//
extern int win_width;
extern int win_height;

extern GLfloat frame[WIN_HEIGHT][WIN_WIDTH][3];  

extern float image_width;
extern float image_height;

extern Point eye_pos;
extern float image_plane;
extern RGB_float background_clr;
extern RGB_float null_clr;

extern Spheres *scene;
extern Boards chessboard;

// light 1 position and color
extern Point light1;
extern float light1_intensity[3];

// global ambient term
extern float global_ambient[3];

// light decay parameters
extern float decay_a;
extern float decay_b;
extern float decay_c;

extern int shadow_on;
extern int reflection_on;
extern int chessboard_on;
extern int refraction_on;
extern int stochastic_ray_on;
extern int supersampling_on;

extern int step_max;

/////////////////////////////////////////////////////////////////////
// get the ambient color of the object
RGB_float get_ambient_color(float *local_ambient)
{
	RGB_float ambient_color = {0,0,0};

    ambient_color.r = global_ambient[0] * local_ambient[0];
    ambient_color.g = global_ambient[1] * local_ambient[1];
    ambient_color.b = global_ambient[2] * local_ambient[2];

	return ambient_color;
}

bool has_shadow(Point hit)
{
    Spheres *current_sph = scene;
    Vector light_to_hit = get_vec(light1, hit);

    float x1, y1, z1, coef_a, coef_b, coef_c;

    while(current_sph != NULL)
    {
        x1 = light1.x - current_sph->center.x;
        y1 = light1.y - current_sph->center.y;
        z1 = light1.z - current_sph->center.z;
        coef_a = vec_dot(light_to_hit, light_to_hit);
        coef_b = 2.0 * (light_to_hit.x*x1 + light_to_hit.y*y1 + light_to_hit.z*z1);
        coef_c = x1*x1 + y1*y1 + z1*z1 - (current_sph->radius)*(current_sph->radius);
        
        float delta = coef_b*coef_b - 4*coef_a*coef_c;
        if (delta >= 0) 
        {
            float t = (-coef_b-sqrt(delta)) / (coef_a*2);
            if ( (1-t) > 0.0001 )
            {
            	return true;
            }
        }

    	current_sph = current_sph->next;
    }

    return false;
}

/*********************************************************************
 * Phong illumination - you need to implement this!
 *********************************************************************/
RGB_float phong(Point hit, Vector vector_u, Vector hit_norm, Spheres *sph) {

    RGB_float ambient_color = get_ambient_color(sph->mat_ambient);
    
    Vector hit_to_light = get_vec(hit, light1);
    float hit_light_dist = vec_len(hit_to_light);
    normalize(&hit_to_light);

    Vector light_to_hit = vec_scale(hit_to_light,-1);
    float temp_dot = vec_dot(light_to_hit, hit_norm);
    Vector temp_vector = vec_scale( hit_norm, (-2)*temp_dot);
    Vector hit_to_reflect_light = vec_plus(light_to_hit, temp_vector);
    normalize(&hit_to_reflect_light);

    Vector hit_to_eye = vec_scale(vector_u,-1);
    normalize(&hit_to_eye);

    float attenuation_coef;
    attenuation_coef = 1.0/( decay_a + decay_b*hit_light_dist + decay_c*pow(hit_light_dist,2) );
    if (attenuation_coef > 1)
    {
		    attenuation_coef = 1;
    }

  	float diffuse_coef;
	diffuse_coef = vec_dot(hit_norm, hit_to_light);
	// if(diffuse_coef < 0)
	// {
	// 	diffuse_coef = 0;
	// }

    float specular_coef;
    specular_coef = pow( vec_dot(hit_to_reflect_light, hit_to_eye), sph->mat_shineness);
	// if(specular_coef < 0)
	// {
	// 	specular_coef = 0;
	// }

    int shadow_multi = 1;
    if (shadow_on && has_shadow(hit))
    {
    	shadow_multi = 0;
    }

	RGB_float phong_color = {0, 0, 0};

	phong_color = clr_add(phong_color, ambient_color);

    phong_color.r += (shadow_multi*attenuation_coef*light1_intensity[0]) * (sph->mat_diffuse[0]*diffuse_coef + sph->mat_specular[0]*specular_coef);
    phong_color.g += (shadow_multi*attenuation_coef*light1_intensity[1]) * (sph->mat_diffuse[1]*diffuse_coef + sph->mat_specular[1]*specular_coef);
    phong_color.b += (shadow_multi*attenuation_coef*light1_intensity[2]) * (sph->mat_diffuse[2]*diffuse_coef + sph->mat_specular[2]*specular_coef);

    // phong_color.r += (attenuation_coef*light1_intensity[0]) * (sph->mat_diffuse[0]*diffuse_coef);
    // phong_color.g += (attenuation_coef*light1_intensity[1]) * (sph->mat_diffuse[1]*diffuse_coef);
    // phong_color.b += (attenuation_coef*light1_intensity[2]) * (sph->mat_diffuse[2]*diffuse_coef);
    

	return phong_color;
}

void intersect_refraction(Point o, Vector u, Spheres* sph, Point* hit)
{
    //float coef_a = u.x*u.x + u.y * u.y + u.z* u.z;
    float coef_a = vec_dot(u, u);
    float coef_b = 2 * ( u.x*(o.x-sph->center.x) + u.y*(o.y-sph->center.y) + u.z*(o.z-sph->center.z) );
    float coef_c = pow((o.x-sph->center.x), 2) + pow((o.y-sph->center.y), 2) + pow((o.z-sph->center.z), 2)- pow(sph->radius, 2);
    float delta = pow(coef_b, 2) - 4*coef_a*coef_c;

    float t1,t2;
    t1 = (-coef_b+sqrt(delta)) / (2*coef_a);
    //t2 = (-coef_b-sqrt(delta)) / (2*coef_a);
    hit->x = o.x + u.x*t1;
    hit->y = o.y + u.y*t1;
    hit->z = o.z + u.z*t1;
}

bool get_refraction_light(float eta_ratio, Vector hit_norm, Vector hit_to_eye, Vector* refraction_light)
{
    float dot_NV = vec_dot(hit_norm, hit_to_eye);

    float tmp1 = eta_ratio * dot_NV;
    
    float temp = 1-pow(eta_ratio, 2)*(1-pow(dot_NV, 2));
    if(temp <= 0)
    {
    	return false;
    }

    float tmp2 = sqrt(temp);

    *refraction_light = vec_minus( vec_scale(hit_norm, tmp1-tmp2), vec_scale(hit_to_eye, eta_ratio) );

    return true;
}

Vector get_diffuse_light(Vector hit_norm)
{
    Vector temp_light;

    srand((int)time(0));
    temp_light.x = rand();
    temp_light.y = rand();
    temp_light.z = rand();
    normalize(&temp_light);

    while( (temp_light.x == (-1)*hit_norm.x) && (temp_light.y == (-1)*hit_norm.y) && (temp_light.z == (-1)*hit_norm.z) )
    {
        temp_light.x = rand();
        temp_light.y = rand();
        temp_light.z = rand();
        normalize(&temp_light);	
    }

    Vector diffuse_light;

    diffuse_light = vec_plus(hit_norm, temp_light);
    normalize(&diffuse_light);

    return diffuse_light;
}

/************************************************************************
 * This is the recursive ray tracer - you need to implement this!
 * You should decide what arguments to use.
 ************************************************************************/
RGB_float recursive_ray_trace(Point point_o, Vector vector_u, int step, bool shoot_in, Spheres* cur) {

	RGB_float color = background_clr;
  
    // if hits the chessboard
    Point *hit_board = new Point;
    
    if( chessboard_on && intersect_board(point_o, vector_u, chessboard, hit_board) )
    {
        color = board_color(chessboard, *hit_board);
        // if there is shadow
        if(shadow_on && has_shadow(*hit_board) )
        {
          //color = ambient_color(board_ambi);
            color = clr_add(clr_scale(color, 0.3), get_ambient_color(chessboard.mat_ambient));
        }
        else
        {
            color = clr_add(color, get_ambient_color(chessboard.mat_ambient));
        }
    }


    Spheres *closest_sph;
    Point *hit_sph = new Point;
    closest_sph = intersect_scene( point_o, vector_u, scene, hit_sph );

    if(shoot_in)
    { 
        closest_sph=cur;
        intersect_refraction(point_o, vector_u, cur, hit_sph);
    }

    if(closest_sph == NULL)
    {
        delete hit_sph;
        delete hit_board;
        return color;
    }

    // if it hits a sphere first, reset the color
    Vector hit_norm = sphere_normal(*hit_sph, closest_sph);
    normalize(&hit_norm);
    if(shoot_in)
    {
        hit_norm=vec_scale(hit_norm,-1);
        normalize(&hit_norm);
    }

    color = phong(*hit_sph, vector_u, hit_norm, closest_sph);
 
    if(step < step_max)
    {
		step++;
    }
	else
	{
		delete hit_sph;
        delete hit_board;
		return color;
	}

	Vector hit_to_eye = get_vec(*hit_sph, point_o);
	normalize(&hit_to_eye);

    //float diffuse_ratio = 0.7;

    if(reflection_on)
	{
		RGB_float reflected_color = { 0, 0, 0 };
		Vector hit_to_eye_reflect = vec_minus(vec_scale(hit_norm, vec_dot(hit_norm, hit_to_eye)*2), hit_to_eye);
		
        //if( stochastic_ray_on && (step<2) )
        if( stochastic_ray_on && (!shoot_in) )
        {
        	for(int num = 0; num < DIFFUSE_RAY_NUM; num++)
        	{
        	    Vector diffuse_light;
        	    diffuse_light = get_diffuse_light(hit_norm);
        	    reflected_color = clr_add( reflected_color, recursive_ray_trace(*hit_sph, diffuse_light, step, shoot_in, cur) );
        	}
        	reflected_color = clr_scale(reflected_color, 0.2);

        	float ratio = 0.6;
            reflected_color = clr_scale(reflected_color, ratio);

            reflected_color = clr_add( reflected_color, clr_scale(recursive_ray_trace(*hit_sph, hit_to_eye_reflect, step, shoot_in, cur), 1-ratio) );
        }
        else
        {
        	reflected_color = recursive_ray_trace(*hit_sph, hit_to_eye_reflect, step, shoot_in, cur);
        }
		
		reflected_color = clr_scale(reflected_color, closest_sph->reflectance);
		color = clr_add(color, reflected_color);
	}

    if(refraction_on)
    {
    	RGB_float refracted_color = {0, 0, 0};
    	
    	Vector refraction_light;

        float air_index = 1.00029;
        float eta_ratio = air_index/(closest_sph->refractivity);

        if(shoot_in) 
        { 
            //refract_index = 1.51;
            eta_ratio = (closest_sph->refractivity)/air_index;
            shoot_in = false;
            cur = NULL;
        }
        else
        { 
        	shoot_in = true;
            cur = closest_sph;
        }

        if ( get_refraction_light(eta_ratio, hit_norm, hit_to_eye, &refraction_light) )
        {
            refracted_color = recursive_ray_trace(*hit_sph, refraction_light, step, shoot_in, cur);
            refracted_color = clr_scale(refracted_color, closest_sph->transmissivity);
            if( stochastic_ray_on )
            {
                float ratio = 0.98;
                refracted_color = clr_scale(refracted_color, ratio);
            }
            color = clr_add(color, refracted_color);
        }
    }

    delete hit_sph;
    delete hit_board;
	return color;
}

RGB_float supersampling(Point cur_pixel_pos, float x_grid_size, float y_grid_size, RGB_float color)
{
	RGB_float ret_color = color;

	float dx_size = x_grid_size/4;
    float dy_size = y_grid_size/4;
          
    Point pixel1 = {cur_pixel_pos.x-dx_size, cur_pixel_pos.y+dy_size, cur_pixel_pos.z};
    Point pixel2 = {cur_pixel_pos.x+dx_size, cur_pixel_pos.y+dy_size, cur_pixel_pos.z};
    Point pixel3 = {cur_pixel_pos.x-dx_size, cur_pixel_pos.y-dy_size, cur_pixel_pos.z};
    Point pixel4 = {cur_pixel_pos.x+dx_size, cur_pixel_pos.y-dy_size, cur_pixel_pos.z};

    ret_color = clr_add( ret_color, recursive_ray_trace(eye_pos, get_vec(eye_pos, pixel1), 0, false, NULL) );
    ret_color = clr_add( ret_color, recursive_ray_trace(eye_pos, get_vec(eye_pos, pixel2), 0, false, NULL) );
    ret_color = clr_add( ret_color, recursive_ray_trace(eye_pos, get_vec(eye_pos, pixel3), 0, false, NULL) );
    ret_color = clr_add( ret_color, recursive_ray_trace(eye_pos, get_vec(eye_pos, pixel4), 0, false, NULL) );
	
	clr_scale(ret_color, 0.2);
	
	return ret_color;
 }

/*********************************************************************
 * This function traverses all the pixels and cast rays. It calls the
 * recursive ray tracer and assign return color to frame
 *
 * You should not need to change it except for the call to the recursive
 * ray tracer. Feel free to change other parts of the function however,
 * if you must.
 *********************************************************************/
void ray_trace() {
  int i, j;
  float x_grid_size = image_width / float(win_width);
  float y_grid_size = image_height / float(win_height);
  float x_start = -0.5 * image_width;
  float y_start = -0.5 * image_height;
  RGB_float ret_color;
  Point cur_pixel_pos;
  Vector ray;

  // ray is cast through center of pixel
  cur_pixel_pos.x = x_start + 0.5 * x_grid_size;
  cur_pixel_pos.y = y_start + 0.5 * y_grid_size;
  cur_pixel_pos.z = image_plane;

  for (i=0; i<win_height; i++) {
    for (j=0; j<win_width; j++) {
      ray = get_vec(eye_pos, cur_pixel_pos);

      //
      // You need to change this!!!
      //
      ret_color = recursive_ray_trace(eye_pos, ray, 0, false, NULL);
      if(supersampling_on)
      {
          ret_color = supersampling(cur_pixel_pos, x_grid_size, y_grid_size, ret_color);
      }

      frame[i][j][0] = GLfloat(ret_color.r);
      frame[i][j][1] = GLfloat(ret_color.g);
      frame[i][j][2] = GLfloat(ret_color.b);

      cur_pixel_pos.x += x_grid_size;
    }

    cur_pixel_pos.y += y_grid_size;
    cur_pixel_pos.x = x_start;
  }
}
