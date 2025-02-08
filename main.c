#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <glfw3.h>
#include <GL/gl.h>
#include "include/idk.h"

#include "textures/textures.PPM"
#include "textures/enemy.PPM"
//#include "textures/win.PPM"
//#include "textures/start.PPM"
//#include "textures/loose.PPM"
#include "textures/sky.PPM"


//        TODO:
//  ->colsision with a and d
//  ->fix textures
//  ->start/fisnish screen
//  ->eniemies
//  ->sword
//  ->more levels

//-------------------------------------------------------------------------
GLFWwindow* window;
float degToRad(float a) { return a*PI/180.0;}
float FixAng(float a){ if(a>359){ a-=360;}
                       if(a<0){ a+=360;} 
                       return a;}
float playerX,playerY,playerDeltaX,playerDeltaY,playerAngle,planeX,planeY,frame1,frame2,fps;
int gameState = 0, timer = 0;
float depth[120];

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

int mapFunctionality[] = {
    1,1,1,1,1,1,1,1,
    1,0,0,0,0,1,0,1,
    1,0,0,0,0,1,0,1,
    1,0,0,0,0,1,0,1,
    1,2,1,0,0,0,0,1,
    1,0,1,0,0,0,0,1,
    1,0,1,0,0,0,0,1,
    1,1,1,1,1,1,1,1
};

typedef struct {
  int type;
  int state;
  int texture;
  int x,y,z;
  float distance;
} sprite;
sprite sprites[4];

void init(){
  glClearColor(0.3,0.3,0.3,0);
  playerX = 300; playerY = 300;
  playerAngle = 90; 
  playerDeltaX = cos(playerAngle)*5;
  playerDeltaY = sin(playerAngle)*5;

  sprites[0].x = 1 * 64 +32;  // Center of cell (4,5)
  sprites[0].y = 1 * 64 +32;
  sprites[0].z = 0;  // Height above ground
  sprites[0].state = 1;
}

void movement(GLFWwindow* window) {
  //offsets
  int xOffset = 0;
  if(playerDeltaX < 0){
    xOffset=-30;
  }else{
    xOffset = 30;
  }

  int yOffset = 0;
  if(playerDeltaY < 0){
    yOffset = -30;
  }else{
    yOffset = 30;
  }


  //player grid pos
  int playerGridPosX = playerX / 64.0;
  int playerGridPosX_add_xOffset = (playerX + xOffset)/64.0;
  int playerGridPosX_sub_xOffset = (playerX - xOffset)/64.0;

  int playerGridPosY = playerY / 64.0;
  int playerGridPosY_add_yOffset = (playerY + yOffset)/64.0;
  int playerGridPosY_sub_yOffset = (playerY - yOffset)/64.0;


  //key presses
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    if(mapFunctionality[playerGridPosY*mapX + playerGridPosX_add_xOffset] == 0){
      playerX += (playerDeltaX * PLAYER_SPEED);
    }
    if(mapFunctionality[playerGridPosY_add_yOffset*mapX + playerGridPosX] == 0){
      playerY += (playerDeltaY * PLAYER_SPEED);
    }
  }

  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    if(mapFunctionality[playerGridPosY*mapX + playerGridPosX_sub_xOffset] == 0){
      playerX -= (playerDeltaX * PLAYER_SPEED);
    }
    if(mapFunctionality[playerGridPosY_sub_yOffset*mapX + playerGridPosX] == 0){
      playerY -= (playerDeltaY * PLAYER_SPEED);
    }
  }

  //FIX THIS !!!!!!!!!!!!!!!!!

  //if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    //if(mapFunctionality[playerGridPosY*mapX + playerGridPosX_add_xOffset] == 0){
    //  playerX += (playerDeltaX * PLAYER_SPEED);
    //}
    //if(mapFunctionality[playerGridPosY_sub_yOffset*mapX + playerGridPosX] == 0){
    //  playerY -= (playerDeltaY * PLAYER_SPEED);
    //}
  //}

  //if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    //if(mapFunctionality[playerGridPosY*mapX + playerGridPosX_sub_xOffset] == 0){
    //  playerX -= (playerDeltaX * PLAYER_SPEED);
    //}
    //if(mapFunctionality[playerGridPosY_add_yOffset*mapX + playerGridPosX] == 0){
    //  playerY += (playerDeltaY * PLAYER_SPEED);
    //}
  //}

  //door open
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
    if(mapFunctionality[playerGridPosY_add_yOffset*mapX+playerGridPosX_add_xOffset]==2){
      mapFunctionality[playerGridPosY_add_yOffset*mapX+playerGridPosX_add_xOffset]=0;}
  }
  
}

void mouseLook(GLFWwindow* window) {
  double mouseX, mouseY;
  static double prevMouseX, prevMouseY;
  glfwGetCursorPos(window, &mouseX, &mouseY);

  if (!(mouseX == prevMouseX)) {
    float rotationAngle = (mouseX - prevMouseX) * 0.025;

    float newPlayerDeltaX = playerDeltaX * cos(rotationAngle) - playerDeltaY * sin(rotationAngle);
    float newPlayerDeltaY = playerDeltaX * sin(rotationAngle) + playerDeltaY * cos(rotationAngle);

    playerDeltaX = newPlayerDeltaX;
    playerDeltaY = newPlayerDeltaY;

    float length = sqrt(playerDeltaX*playerDeltaX + playerDeltaY*playerDeltaY);
    playerDeltaX /= length; // Normalize
    playerDeltaY /= length;


    playerAngle = atan2(playerDeltaY, playerDeltaX);
    while (playerAngle < 0) {
      playerAngle += 2 * PI;
    }
    while (playerAngle >= 2 * PI) {
      playerAngle -= 2 * PI;
    }
  }
  prevMouseX = mouseX;
  prevMouseY = mouseY;

  planeX = playerDeltaY * 0.66;
  planeY = -playerDeltaX * 0.66;
}


void screen(int v){
    int x,y;
    int *Texture;
    //if(v==1){Texture = start;}
    //if(v==2){Texture = loose;}
    //if(v==3){Texture = win;}

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

void drawSky() {    //draw sky and rotate based on player rotation
  int x,y;
  float angleToTextureScale = (float)120 / (2 * PI); // 120 = texture width

  for(y=0;y<40;y++){
    for(x=0;x<120;x++){
      int xOffset=(int)(playerAngle*angleToTextureScale)-x; 
      if(xOffset<0){ xOffset+=120;} //return 0-120 based on player angle
      xOffset=xOffset % 120; 

      int pixel=((40-y)*120+xOffset)*3;
      int red   =sky[pixel+0];
      int green =sky[pixel+1];
      int blue  =sky[pixel+2];
      glPointSize(8); glColor3ub(red,green,blue); glBegin(GL_POINTS); glVertex2i(x*8,y*8+320); glEnd();
  }	
 }
}
void sortSprites() {
    for (int i = 0; i < 4; i++) {
        sprites[i].distance = (sprites[i].x - playerX) * (sprites[i].x - playerX) + 
                              (sprites[i].y - playerY) * (sprites[i].y - playerY);
    }

    // Sort sprites in descending order of distance
    for (int i = 0; i < 3; i++) {
        for (int j = i + 1; j < 4; j++) {
            if (sprites[i].distance < sprites[j].distance) {
                sprite temp = sprites[i];
                sprites[i] = sprites[j];
                sprites[j] = temp;
            }
        }
    }
}

void updateSprites() {
    for(int i = 0; i < 4; i++) {
        if(sprites[i].state == 1) {
            // Calculate direction to player
            float dx = playerX - sprites[i].x;
            float dy = playerY - sprites[i].y;
            float distance = sqrt(dx*dx + dy*dy);
            
            // Check for collision with player
            if(distance < 10) { // Collision radius - adjust as needed
                gameState = 0; // Reset game
                return;
            }
            
            // Enemy movement logic
            float moveSpeed = 0.2f; // Adjust for desired difficulty
            
            // Normalize direction for movement
            if(distance > 0) { // Prevent division by zero
                dx /= distance;
                dy /= distance;
                
                // Calculate potential new position
                float newX = sprites[i].x + dx * moveSpeed;
                float newY = sprites[i].y + dy * moveSpeed;
                
                // Check wall collisions separately for X and Y
                int currentGridX = sprites[i].x / 64;
                int currentGridY = sprites[i].y / 64;
                int newGridX = newX / 64;
                int newGridY = newY / 64;
                
                // Try X movement
                if(mapWalls[currentGridY * mapX + newGridX] == 0) {
                    sprites[i].x = (int)newX;
                }
                
                // Try Y movement
                if(mapWalls[newGridY * mapX + currentGridX] == 0) {
                    sprites[i].y = (int)newY;
                }
                
                // If stuck, try to move diagonally
                if(sprites[i].x == newX && sprites[i].y == (int)newY) {
                    // Try alternative paths
                    if(mapWalls[currentGridY * mapX + (currentGridX + (dx > 0 ? 1 : -1))] == 0) {
                        sprites[i].x += (dx > 0 ? 1 : -1) * moveSpeed;
                    }
                    if(mapWalls[(currentGridY + (dy > 0 ? 1 : -1)) * mapX + currentGridX] == 0) {
                        sprites[i].y += (dy > 0 ? 1 : -1) * moveSpeed;
                    }
                }
            }
        }
    }
}


void drawSprite(int spriteIndex){
    float dirX = cos(playerAngle);
    float dirY = sin(playerAngle);
    float planeX = -dirY * 0.66;
    float planeY = dirX * 0.66;

    float spriteX = sprites[spriteIndex].x - playerX;
    float spriteY = sprites[spriteIndex].y - playerY;

    float invDet = 1.0 / (planeX * dirY - dirX * planeY);
    float transformX = invDet * (dirY * spriteX - dirX * spriteY);
    float transformY = invDet * (-planeY * spriteX + planeX * spriteY);

    if (transformY <= 0) return;

    int screenWidth = 960;
    int screenHeight = 640;
    
    float spriteScale = 0.3;  // Adjust for desired size
    int spriteScreenX = (int)((screenWidth / 2) * (1 + transformX / transformY));
    int spriteHeight = abs((int)(screenHeight / (transformY * spriteScale)));
    int spriteWidth = spriteHeight;

    // Position sprite on ground
    int drawStartY = screenHeight / 2 - spriteHeight;  // Top of sprite
    int drawEndY = screenHeight / 2;  // Bottom of sprite (ground level)
    int drawStartX = -spriteWidth / 2 + spriteScreenX;
    int drawEndX = spriteWidth / 2 + spriteScreenX;

    // Screen bounds check
    drawStartY = (drawStartY < 0) ? 0 : drawStartY;
    drawEndY = (drawEndY >= screenHeight) ? screenHeight - 1 : drawEndY;
    drawStartX = (drawStartX < 0) ? 0 : drawStartX;
    drawEndX = (drawEndX >= screenWidth) ? screenWidth - 1 : drawEndX;

    // Draw sprite
    for(int stripe = drawStartX; stripe < drawEndX; stripe++) {
        if(transformY < depth[stripe / 8]) {
            float texX = ((stripe - (-spriteWidth / 2 + spriteScreenX)) * 32.0f) / spriteWidth;
            
            if(texX >= 0 && texX < 32) {
                for(int y = drawStartY; y < drawEndY; y++) {
                    float texY = 31 - ((y - drawStartY) * 32.0f) / (drawEndY - drawStartY);
                    
                    if(texY >= 0 && texY < 32) {
                        int color_index = ((int)texY * 32 + (int)texX) * 3;
                        
                        if(color_index >= 0 && color_index < 32 * 32 * 3 - 2) {
                            int red = enemy[color_index + 0];
                            int green = enemy[color_index + 1];
                            int blue = enemy[color_index + 2];
                            
                            if(!(red == 255 && green == 0 && blue == 255)) {
                                // Distance-based shading
                                float shade = 1.0f / (1.0f + transformY * 0.005f);
                                red = (int)(red * shade);
                                green = (int)(green * shade);
                                blue = (int)(blue * shade);
                                
                                glPointSize(8);
                                glColor3ub(red, green, blue);
                                glBegin(GL_POINTS);
                                glVertex2i(stripe, y);
                                glEnd();
                            }
                        }
                    }
                }
            }
        }
    }
}
    

void drawRays(){
    int ray,mx,my,mp,depthOfField;
    float rayX,rayY,rayAngle,xOffset,yOffset;

    rayAngle=playerAngle - (DR/2)*60;
    if(rayAngle<0){rayAngle +=2*PI;}
    if(rayAngle>2*PI){rayAngle-=2*PI;}

    for(ray=0; ray<120; ray++){
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
        
        //----3d--------------------------------
        //final distance
        float cosAngle = playerAngle-rayAngle;
        if(cosAngle<-PI){cosAngle +=2*PI;}
        if(cosAngle>PI){cosAngle-=2*PI;}

        distanceFinal *= cos(cosAngle);
        depth[ray] = distanceFinal;

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
            glPointSize(8); glColor3ub(red,green,blue); glBegin(GL_POINTS); glVertex2i(ray*8 ,y+lineOffset); glEnd();
            textureY+=textureYStep; 
        }

        for(y=lineOffset+lineH;y<640;y++){
            float distanceY=y-(640/2.0);

            float raFix=cos(degToRad(FixAng(playerAngle-rayAngle)));

            float scale = (0.5 * mapX * mapS) / 1.6; //((0.5*960)/(5/8*640));

            textureX=playerX/2 + cos(rayAngle) * (scale*32*2) / (distanceY*raFix);
            textureY=playerY/2 + sin(rayAngle) * (scale*32*2) / (distanceY*raFix);

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
            glPointSize(8); glColor3ub(red,green,blue); glBegin(GL_POINTS); glVertex2i(ray*8 ,640-y); glEnd(); 
        }

        rayAngle+=(DR/2.0);
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

    if (gameState==1){ //game loop
      drawSky();
      drawRays();

      updateSprites();
      sortSprites();
      for (int i = 0; i < 4; i++) {
            drawSprite(i);
        }
        
      if((int)playerX>>6==1 && (int)playerY>>6==6){gameState=2;}
    }

    if(gameState == 2){
        //screen won
        printf("you won :D");
        gameState = 0;
    }

    glfwSwapBuffers(window);
}

int main() {
    glfwInit();
 
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "maturita", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (glfwRawMouseMotionSupported())
    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    
    glOrtho(0.0,WINDOW_WIDTH,0.0,WINDOW_HEIGHT,0.0,1.0);

    init();

    while (!glfwWindowShouldClose(window)) {
        mouseLook(window);
        movement(window);
        display(window);
        glfwPollEvents();
    }
 
    glfwTerminate();
    return 0;
}