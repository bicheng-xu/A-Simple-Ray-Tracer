//
// this provide functions to set up the scene
//
#include "sphere.h"
#include <stdio.h>
#include <iostream>
using namespace std;

extern Point light1;
extern float light1_intensity[3];

extern float global_ambient[3];
extern Chesses *scene;
extern Boards chessboard;

extern RGB_float background_clr;
extern float decay_a;
extern float decay_b;
extern float decay_c;

extern Point chess1_vertices[154];
extern int chess1_faces[304][3];

extern Point chess2_vertices[250];
extern int chess2_faces[496][3];

void load_smf()
{
    FILE *fp;
    char buf[100];
    char v;
    int temp_int;
    Point tmp;
    
    fp = fopen("./chess_pieces/chess_piece.smf","r");
    if (fp == NULL)
    {
        cout <<"File open error."<<endl;
    }
    
    fgets(buf, 100, fp);
    for (int i = 0; i < 154; i++)
    {
        temp_int = fscanf(fp, "%c", &v);
        temp_int = fscanf(fp, "%c", &v);
        temp_int = fscanf(fp, "%f", &tmp.x);
        temp_int = fscanf(fp, "%c", &v);
        temp_int = fscanf(fp, "%f", &tmp.y);
        temp_int = fscanf(fp, "%c", &v);
        temp_int = fscanf(fp, "%f", &tmp.z);
        fgets(buf, 100, fp);
        chess1_vertices[i] = tmp;
    }
    for (int i = 0; i < 304; i++)
    {
        temp_int = fscanf(fp, "%c", &v);
        temp_int = fscanf(fp, "%c", &v);
        temp_int = fscanf(fp, "%d", &chess1_faces[i][0]);
        temp_int = fscanf(fp, "%c", &v);
        temp_int = fscanf(fp, "%d", &chess1_faces[i][1]);
        temp_int = fscanf(fp, "%c", &v);
        temp_int = fscanf(fp, "%d", &chess1_faces[i][2]);
        fgets(buf, 100, fp);
    }
    fclose(fp);

    fp = fopen("./chess_pieces/bishop.smf","r");
    if (fp == NULL)
    {
        cout <<"File open error."<<endl;
    }

    fgets(buf, 100, fp);
    for (int i = 0; i < 250; i++)
    {
        temp_int = fscanf(fp, "%c", &v);
        temp_int = fscanf(fp, "%c", &v);
        temp_int = fscanf(fp, "%f", &tmp.x);
        temp_int = fscanf(fp, "%c", &v);
        temp_int = fscanf(fp, "%f", &tmp.y);
        temp_int = fscanf(fp, "%c", &v);
        temp_int = fscanf(fp, "%f", &tmp.z);
        fgets(buf, 100, fp);
        chess2_vertices[i] = tmp;
    }
    for (int i = 0; i < 496; i++)
    {
        temp_int = fscanf(fp, "%c", &v);
        temp_int = fscanf(fp, "%c", &v);
        temp_int = fscanf(fp, "%d", &chess2_faces[i][0]);
        temp_int = fscanf(fp, "%c", &v);
        temp_int = fscanf(fp, "%d", &chess2_faces[i][1]);
        temp_int = fscanf(fp, "%c", &v);
        temp_int = fscanf(fp, "%d", &chess2_faces[i][2]);
        fgets(buf, 100, fp);
    }
    fclose(fp);

}

Vector get_surface_normal(Point tmp[3])
{
    Vector ret_v;

    Vector ab = get_vec(tmp[0], tmp[1]);
    Vector bc = get_vec(tmp[1], tmp[2]);

    ret_v.x = (ab.y * bc.z) - (ab.z * bc.y);
    ret_v.y = -(ab.x * bc.z - ab.z * bc.x);
    ret_v.z = ab.x * bc.y - ab.y * bc.x;
  
    normalize(&ret_v);

    return ret_v;
}

//////////////////////////////////////////////////////////////////////////

/*******************************************
 * set up the default scene - DO NOT CHANGE
 *******************************************/
void set_up_default_scene() {
  // set background color
  background_clr.r = 0.5;
  background_clr.g = 0.05;
  background_clr.b = 0.8;

  // set up global ambient term
  global_ambient[0] = global_ambient[1] = global_ambient[2] = 0.2;

  // set up light 1
  light1.x = -2.0;
  light1.y = 5.0;
  light1.z = 1.0;
  light1_intensity[0] = light1_intensity[1] = light1_intensity[2] = 1.0;

  // set up decay parameters
  decay_a = 0.5;
  decay_b = 0.3;
  decay_c = 0.0;

  // chess 1
  float chess1_ambient[] = {0.7, 0.7, 0.7};
  float chess1_diffuse[] = {0.1, 0.5, 0.8};
  float chess1_specular[] = {1.0, 1.0, 1.0};
  float chess1_shineness = 10;
  float chess1_reflectance = 0.4;
  float chess1_transmissivity = 0;
  float chess1_refractivity = 1.51;

  Point tmp[3];
  Vector nor;

  for (int i = 0; i < 304; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      tmp[j].x = chess1_vertices[chess1_faces[i][j] - 1].x * 5 - 3;
      tmp[j].y = chess1_vertices[chess1_faces[i][j] - 1].y * 5 - 3;
      tmp[j].z = chess1_vertices[chess1_faces[i][j] - 1].z * 5 - 10;
    }
    nor = get_surface_normal(tmp);
    scene = add_chess(scene, tmp, nor, chess1_ambient, chess1_diffuse, chess1_specular,
      chess1_shineness, chess1_reflectance, chess1_transmissivity, chess1_refractivity, i+1);
  }  

  // chess 2
  float chess2_ambient[] = {0.6, 0.6, 0.6};
  float chess2_diffuse[] = {1, 0, 0.25};
  float chess2_specular[] = {1.0, 1.0, 1.0};
  float chess2_shineness = 6;
  float chess2_reflectance = 0.3;
  float chess2_transmissivity = 0;
  float chess2_refractivity = 1.51;

  for (int i = 0; i < 496; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      tmp[j].x = chess2_vertices[chess2_faces[i][j] - 1].x * 70 + 2;
      tmp[j].y = chess2_vertices[chess2_faces[i][j] - 1].y * 70 - 3;
      tmp[j].z = chess2_vertices[chess2_faces[i][j] - 1].z * 70 - 8;
    }
    nor = get_surface_normal(tmp);
    scene = add_chess(scene, tmp, nor, chess2_ambient, chess2_diffuse, chess2_specular,
      chess2_shineness, chess2_reflectance, chess2_transmissivity, chess2_refractivity, i+1+304);
  }
  
  // chessboard
  chessboard.norm = { 0, 1, 0 };
  chessboard.center = { 0, -3, -7 };
  chessboard.width = 1200;
  chessboard.mat_ambient[0] = 0.3;
  chessboard.mat_ambient[1] = 0.3;
  chessboard.mat_ambient[2] = 0.3;
  chessboard.reflectance = 0.4;
  chessboard.refractivity = 1.53;
}

/***************************************
 * You can create your own scene here
 ***************************************/
void set_up_user_scene() {
  // set background color
  background_clr.r = 0.5;
  background_clr.g = 0.05;
  background_clr.b = 0.8;

  // set up global ambient term
  global_ambient[0] = global_ambient[1] = global_ambient[2] = 0.2;

  // set up light 1
  light1.x = -2.0;
  light1.y = 5.0;
  light1.z = 1.0;
  light1_intensity[0] = light1_intensity[1] = light1_intensity[2] = 1.0;

  // set up decay parameters
  decay_a = 0.5;
  decay_b = 0.3;
  decay_c = 0.0;

  // chess 1
  float chess1_ambient[] = {0.7, 0.7, 0.7};
  float chess1_diffuse[] = {0.1, 0.5, 0.8};
  float chess1_specular[] = {1.0, 1.0, 1.0};
  float chess1_shineness = 10;
  float chess1_reflectance = 0.4;
  float chess1_transmissivity = 0.6;
  float chess1_refractivity = 1.5;

  Point tmp[3];
  Vector nor;

  for (int i = 0; i < 304; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      tmp[j].x = chess1_vertices[chess1_faces[i][j] - 1].x * 5 - 3;
      tmp[j].y = chess1_vertices[chess1_faces[i][j] - 1].y * 5 - 3;
      tmp[j].z = chess1_vertices[chess1_faces[i][j] - 1].z * 5 - 10;
    }
    nor = get_surface_normal(tmp);
    scene = add_chess(scene, tmp, nor, chess1_ambient, chess1_diffuse, chess1_specular,
      chess1_shineness, chess1_reflectance, chess1_transmissivity, chess1_refractivity, i+1);
  }  

  // chess 2
  float chess2_ambient[] = {0.6, 0.6, 0.6};
  float chess2_diffuse[] = {1, 0, 0.25};
  float chess2_specular[] = {1.0, 1.0, 1.0};
  float chess2_shineness = 6;
  float chess2_reflectance = 0.3;
  float chess2_transmissivity = 0.6;
  float chess2_refractivity = 1.5;

  for (int i = 0; i < 496; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      tmp[j].x = chess2_vertices[chess2_faces[i][j] - 1].x * 70 + 2;
      tmp[j].y = chess2_vertices[chess2_faces[i][j] - 1].y * 70 - 3;
      tmp[j].z = chess2_vertices[chess2_faces[i][j] - 1].z * 70 - 8;
    }
    nor = get_surface_normal(tmp);
    scene = add_chess(scene, tmp, nor, chess2_ambient, chess2_diffuse, chess2_specular,
      chess2_shineness, chess2_reflectance, chess2_transmissivity, chess2_refractivity, i+1+304);
  }
  
  // chessboard
  chessboard.norm = { 0, 1, 0 };
  chessboard.center = { 0, -3, -7 };
  chessboard.width = 1000;
  chessboard.mat_ambient[0] = 0.3;
  chessboard.mat_ambient[1] = 0.3;
  chessboard.mat_ambient[2] = 0.3;
  chessboard.reflectance = 0.4;
  chessboard.refractivity = 1.53;
}
