#pragma once

#define PLAYER_SPEED 1
#define ROTATION_ANGLE 0.025
#define SECOND *1000
#define PI 3.14159265358979323846
#define P2 PI/2
#define P3 3*PI/2
#define DR 0.0174532925
#define WINDOW_HEIGHT 640 
#define WINDOW_WIDTH 960
#define mapX 8
#define mapY 8
#define mapS 64


//extern float playerX,playerY,playerDeltaX,playerDeltaY,playerAngle,planeX,planeY,
//             horizontalX, horizontalY, veritcalX, verticalY, distanceH, distanceV;
extern const int numberPatterns[10][5][4];

float degToRad(float a);
float FixAng(float a);
float dist(float ax,float ay, float bx, float by, float ang);

