#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <glfw3.h>
#include <GL/gl.h>
#include "include/idk.h"

#include "textures/textures.PPM"
#include "textures/enemy.PPM"
#include "textures/win.PPM"
#include "textures/start.PPM"
#include "textures/loose.PPM"

//-------------Variables--------------------
float degToRad(float a) { return a*PI/180.0;}
float FixAng(float a){ if(a>359){ a-=360;}
                       if(a<0){ a+=360;} 
                       return a;}
float playerX,playerY,playerDeltaX,playerDeltaY,playerAngle,planeX,planeY,frame1,frame2,fps;
int gameState = 0, timer = 0;

int mapWalls[]= {
    1,1,1,1,1,1,1,1,
    1,0,0,0,0,1,0,1,
    1,0,0,0,0,1,0,1,
    1,0,0,0,0,1,0,1,
    1,4,1,0,0,0,0,1,
    1,0,1,0,0,0,0,1,
    1,0,1,0,0,0,0,1,
    1,4,1,1,1,1,1,1
};
int mapCeiling[]= {
    3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,3,3
};
int mapFloor[]= {
    2,2,2,2,2,2,2,2,	
    2,2,2,2,2,2,2,2,	
    2,2,2,2,2,2,2,2,	
    2,2,2,2,2,2,2,2,	
    2,2,2,2,2,2,2,2,	
    2,2,2,2,2,2,2,2,	
    2,2,2,2,2,2,2,2,	
    2,2,2,2,2,2,2,2
};
//--------------------------------------------

void drawPlayer(){
  glColor3f(1,1,0);
  glPointSize(8);
  glBegin(GL_POINTS);
  glVertex2i(playerX,playerY);
  glEnd();

  glLineWidth(3);
  glBegin(GL_LINES);
  glVertex2i(playerX,playerY);
  glVertex2i(playerX+playerDeltaX*5,playerY+playerDeltaY*5);
  glEnd();
}

void screen(int v){
    int x,y;
    int *Texture;
    if(v==1){Texture = start;}
    if(v==2){Texture = loose;}
    if(v==3){Texture = win;}

    for(y=0;y<80;y++){
        for(x=0;x<120;x++){
            int pixel = (y*120+x)*3;
            int red = Texture[pixel+0];
            int green = Texture[pixel+1];
            int blue = Texture[pixel+2];
            glPointSize(8); glColor3ub(red,green,blue); glBegin(GL_POINTS); glVertex2i(x+200,y+100); glEnd();
        }
    }
}

void Draw2dmap(){
    int x,y,xOffset,yOffset;
    for(y=0;y<mapY;y++){
        for(x=0;x<mapX;x++){
        if(mapWalls[y*mapX+x]>0){ glColor3f(1,1,1);} else{ glColor3f(0,0,0);}
            xOffset=x*mapS; yOffset=y*mapS;
        glBegin(GL_QUADS); 
        glVertex2i( 0   +xOffset+1, 0   +yOffset+1); 
        glVertex2i( 0   +xOffset+1, mapS+yOffset-1); 
        glVertex2i( mapS+xOffset-1, mapS+yOffset-1);  
        glVertex2i( mapS+xOffset-1, 0   +yOffset+1); 
        glEnd();
        } 
    } 
}

void drawRays(){
    int ray,mx,my,mp,depthOfField;
    float rayX,rayY,rayAngle,xOffset,yOffset;

    rayAngle=playerAngle - DR*35;
    if(rayAngle<0){rayAngle +=2*PI;}
    if(rayAngle>2*PI){rayAngle-=2*PI;}

    for(ray=0; ray<70; ray++){
        //horizontalne ciary---------------------------------------------
        depthOfField = 0; 
        int verticalTextrueNumber = 0, horizontalTextureNumber = 0;
        float aTan = -1/tan(rayAngle);
        float distanceH = 100000, horizontalX = playerX, horizontalY = playerY, distanceFinal;
        
        if(rayAngle > PI){ //pozera hore
            rayY =(((int)playerY>>6)<<6)-0.0001;
            rayX = (playerY-rayY)*aTan+playerX;
            yOffset = -64;
            xOffset = -yOffset*aTan;
        }

        if(rayAngle < PI){ //pozera dole
            rayY =(((int)playerY>>6)<<6)+64;
            rayX = (playerY-rayY)*aTan+playerX;
            yOffset = 64;
            xOffset = -yOffset*aTan;
        }

        if(rayAngle == 0 || rayAngle == PI){
            rayX = playerX;
            rayY = playerY;
            depthOfField = 8;
        }

        while(depthOfField<8){
            mx = (int)(rayX)>>6;
            my = (int)(rayY)>>6;
            mp = my*mapX+mx;
            if(mp>0 && mp<mapX*mapY && mapWalls[mp]>0){
                horizontalTextureNumber = mapWalls[mp]-1;
                horizontalX = rayX;
                horizontalY = rayY;
                distanceH = dist(playerX,playerY,horizontalX,horizontalY,rayAngle);
                depthOfField=8;//stena
            }else{
               rayX+=xOffset;
               rayY+=yOffset;
               depthOfField+=1; 
            }
        }

        //vertikalne ciary---------------------------------------------
        depthOfField = 0; 
        float nTan = -tan(rayAngle);
        float distanceV = 100000, veritcalX = playerX, verticalY = playerY;

        
        if(rayAngle>P2 && rayAngle<P3){ //pozera lavo 
            rayX =(((int)playerX>>6)<<6)-0.0001;
            rayY = (playerX-rayX)*nTan+playerY;
            xOffset = -64;
            yOffset = -xOffset*nTan;
        }

        if(rayAngle<P2 || rayAngle>P3){ //pozera pravo 
            rayX =(((int)playerX>>6)<<6)+64;
            rayY = (playerX-rayX)*nTan+playerY;
            xOffset = 64;
            yOffset = -xOffset*nTan;
        }

        if(rayAngle == 0 || rayAngle == PI){
            rayX = playerX;
            rayY = playerY;
            depthOfField = 8;
        }

        while(depthOfField<8){
            mx = (int)(rayX)>>6;
            my = (int)(rayY)>>6;
            mp = my*mapX+mx;
            if(mp>0 && mp<mapX*mapY && mapWalls[mp]>0){
                verticalTextrueNumber = mapWalls[mp]-1;
                veritcalX = rayX;
                verticalY = rayY;
                distanceV = dist(playerX,playerY,veritcalX,verticalY,rayAngle);
                depthOfField=8;//stena
            }else{
               rayX+=xOffset;
               rayY+=yOffset;
               depthOfField+=1; 
            }
        }
        
        float shade = 1;
        if(distanceH>distanceV){
            rayX=veritcalX; rayY = verticalY; distanceFinal = distanceV; horizontalTextureNumber=verticalTextrueNumber;
            glColor3f(0.9,0,0);
        }
        if(distanceH<distanceV){
            rayX=horizontalX; rayY = horizontalY; distanceFinal = distanceH; shade = 0.5;
            glColor3f(0.7,0,0);
        }
        
        glLineWidth(1);
        glBegin(GL_LINES);
        glVertex2i(playerX,playerY);
        glVertex2i(rayX,rayY);
        glEnd();


        //----3d--------------------------------
        //final distance
        float cosAngle = playerAngle-rayAngle;
        if(cosAngle<0){cosAngle +=2*PI;}
        if(cosAngle>2*PI){cosAngle-=2*PI;}
        distanceFinal = distanceFinal*cos(cosAngle);

        //line height
        float lineH = (mapS*640)/distanceFinal;
        float textureYStep=32.0/(float)lineH;
        float textureYOffset = 0;
        if (lineH>640){
            textureYOffset=(lineH-640)/2.0;
            lineH=640;
        }
        float lineOffset = 320 - (lineH/2);


        float textureY=textureYOffset*textureYStep;
        float textureX;
        if(shade != 1){textureX=(int)(rayX/2.0)%32;
                       if (rayAngle>180){textureX=31-textureX;}}
        else          {textureX=(int)(rayY/2.0)%32;
                       if (rayAngle>90 && rayAngle < 270){textureX=31-textureX;}}
                       
        //walls
        int y;
        for(y=0;y<lineH;y++){
            int pixel = ((31 - (int)textureY) * 32 + (int)textureX) * 3 + (horizontalTextureNumber*32*32*3);
            int red = textures[pixel+0]*shade;
            int green = textures[pixel+1]*shade;
            int blue = textures[pixel+2]*shade;
            glPointSize(8); glColor3ub(red,green,blue); glBegin(GL_POINTS); glVertex2i(ray*8+530,y+lineOffset); glEnd();
            textureY+=textureYStep; //if(textureY<0){textureY = 0 ;}
        }

        for(y=lineOffset+lineH;y<640;y++){
            float dy=y-(640/2.0);
            if (fabs(dy) < 0.001) dy = 0.001;

            float raFix=cos(degToRad(FixAng(playerAngle-rayAngle)));

            textureX=playerX/2 + cos(rayAngle) * (158*32) / (dy*raFix);
            textureY=playerY/2 - sin(rayAngle) * (158*32) / (dy*raFix);

            if (textureX < 0) textureX = 0;
            if (textureY < 0) textureY = 0;
            if (textureX >= mapX * 32) textureX = (mapX * 32) - 1;
            if (textureY >= mapY * 32) textureY = (mapY * 32) - 1;

            //draw floors
            int mp=mapFloor[(int)(textureY/32.0)*mapX+(int)(textureX/32.0)]*32*32;
            int pixel=(((int)(textureY)&31)*32 + ((int)(textureX)&31))*3+(mp*3);
            int red   =textures[pixel+0]*0.7;
            int green =textures[pixel+1]*0.7;
            int blue  =textures[pixel+2]*0.7;
            glPointSize(8); glColor3ub(red,green,blue); glBegin(GL_POINTS); glVertex2i(ray*8+530,640-y); glEnd(); 
        
            //draw ceilings
            mp=mapCeiling[(int)(textureY/32.0)*mapX+(int)(textureX/32.0)]*32*32;
            pixel=(( (int)(textureY) & 31)*32 + ( (int)(textureX) & 31))*3+(mp*3);
            red   =textures[pixel+0];
            green =textures[pixel+1];
            blue  =textures[pixel+2];
            if(mp>0){glPointSize(8); glColor3ub(red,green,blue); glBegin(GL_POINTS); glVertex2i(ray*8+530,y); glEnd();}
        }

        rayAngle+=DR;
        if(rayAngle<0){rayAngle +=2*PI;}
        if(rayAngle>2*PI){rayAngle-=2*PI;}
    }
}

void display(GLFWwindow* window){
    int xOfset = 0;
    if (playerDeltaX < 0) { xOfset =-20;} else {xOfset = 20;}
    int yOfset = 0;
    if (playerDeltaY < 0) { yOfset =-20;} else {yOfset = 20;}    

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    frame2 = glfwGetTime(); 
    fps = (frame2 - frame1);
    frame1 = glfwGetTime();

    if(gameState==0){ init(); timer += 1; if(timer>3000){timer=0; gameState=1;}}//main screen
    if (gameState==1){
    Draw2dmap();
    drawPlayer();
    drawRays();

    if((int)playerX>>6==1 && (int)playerY>>6==1){gameState=2;}
    }
    if(gameState == 2){
        //screen won
        printf("you won :D");
        gameState = 1;
    }

    glfwSwapBuffers(window);
}