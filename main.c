#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <glfw3.h>
#include <GL/gl.h>
#include "include/definitions.h"

#include "textures/textures.PPM"
#include "textures/enemy.PPM"
#include "textures/sky.PPM"
#include "textures/key.PPM"
#include "textures/sword.PPM"


//        TODO:
//  ->start/fisnish screen

//-------------------------------------------------------------------------
GLFWwindow* window;

typedef struct {
    int walls[64];      // 8x8 map (64 tiles)
    int floor[64];      // 8x8 map
    int functionality[64]; // 8x8 map
    int playerStartX;
    int playerStartY;
    float playerStartAngle;
    int exitX;
    int exitY;
    struct {
        int type;
        int x;
        int y;
    } spriteConfigs[3];
} Level;

typedef struct {
  int type;
  int state;
  int texture;
  int x,y,z;
  float distance;
} sprite;

float playerX,playerY,playerDeltaX,playerDeltaY,playerAngle,planeX,planeY;
float depth[120];
int gameState = 0;
int hasKey = 0;     
int hasSword = 0; 
double gameStartTime = 0;
double totalTime = 0;
int victoryTimer = 0;
double finalTime = 0;
bool gameStarted = false;
int currentLevel = 0;
int totalLevels = 3;

sprite sprites[3];
Level levels[3];

int mapWalls[64] = {0};
int mapFloor[64]= {0};
int mapFunctionality[64] = {0};

void initializeLevels() {
    Level level1 = {
        .walls = {
            1,1,1,1,1,1,1,1,
            1,0,0,0,0,1,0,1,
            1,0,0,0,0,1,0,1,
            1,0,0,0,0,1,0,1,
            1,4,1,0,0,0,0,1,
            1,0,1,0,0,0,0,1,
            1,0,1,0,0,0,0,1,
            1,4,1,1,1,1,1,1
        },
        .floor = {
            2,2,2,2,2,2,2,2,
            2,2,2,2,2,2,2,2,
            2,2,2,2,2,2,2,2,
            2,2,2,2,2,2,2,2,
            2,2,2,2,2,2,2,2,
            2,2,2,2,2,2,2,2,
            2,2,2,2,2,2,2,2,
            2,2,2,2,2,2,2,2
        },
        .functionality = {
            1,1,1,1,1,1,1,1,
            1,0,0,0,0,1,0,1,
            1,0,0,0,0,1,0,1,
            1,0,0,0,0,1,0,1,
            1,2,1,0,0,0,0,1,
            1,0,1,0,0,0,0,1,
            1,0,1,0,0,0,0,1,
            1,1,1,1,1,1,1,1
        },
        .playerStartX = 300,
        .playerStartY = 300,
        .playerStartAngle = 80,
        .exitX = 1,
        .exitY = 6,
        .spriteConfigs = {
            {0, 3 * 64 + 32, 1 * 64 + 32},  // enemy
            {1, 1 * 64 + 32, 1 * 64 + 32},  // key
            {2, 6 * 64 + 32, 1 * 64 + 32}   // sword
        }
    };

    Level level2 = {
        .walls = {
            1,1,1,1,1,1,1,1,
            1,0,0,0,0,1,0,1,
            1,4,1,1,0,1,0,1,
            1,0,1,1,0,1,0,1,
            1,0,0,1,1,1,0,1,
            1,0,1,1,1,1,0,1,
            1,0,0,0,0,0,0,1,
            1,1,1,1,1,1,1,1
        },
        .floor = {
            2,2,2,2,2,2,2,2,
            2,2,2,2,2,2,2,2,
            2,2,2,2,2,2,2,2,
            2,2,2,2,2,2,2,2,
            2,2,2,2,2,2,2,2,
            2,2,2,2,2,2,2,2,
            2,2,2,2,2,2,2,2,
            2,2,2,2,2,2,2,2
        },
        .functionality = {
            1,1,1,1,1,1,1,1,
            1,0,0,0,0,1,0,1,
            1,2,1,1,0,1,0,1,
            1,0,1,1,0,1,0,1,
            1,0,0,1,1,1,0,1,
            1,0,1,1,1,1,0,1,
            1,0,0,0,0,0,0,1,
            1,1,1,1,1,1,1,1
        },
        .playerStartX = 384+32,
        .playerStartY = 384+32,
        .playerStartAngle = 180,
        .exitX = 4,
        .exitY = 3,
        .spriteConfigs = {
            {0, 2 * 64 + 32, 1 * 64 + 32},  // enemy
            {1, 6 * 64 + 32, 1 * 64 + 32},  // key
            {2, 2 * 64 + 32, 4 * 64 + 32}   // sword
        }
    };
    // Level 3
    Level level3 = {
        .walls = {
            1,1,1,1,1,1,1,1,
            1,0,1,0,0,4,0,1,
            1,0,1,0,1,1,1,1,
            1,0,0,0,0,0,0,1,
            1,0,1,1,1,1,0,1,
            1,0,0,0,0,1,0,1,
            1,0,1,1,0,0,0,1,
            1,1,1,1,1,1,1,1
        },
        .floor = {
            2,2,2,2,2,2,2,2,
            2,2,2,2,2,2,2,2,
            2,2,2,2,2,2,2,2,
            2,2,2,2,2,2,2,2,
            2,2,2,2,2,2,2,2,
            2,2,2,2,2,2,2,2,
            2,2,2,2,2,2,2,2,
            2,2,2,2,2,2,2,2
        },
        .functionality = {
            1,1,1,1,1,1,1,1,
            1,0,1,0,0,2,0,1,
            1,0,1,0,1,1,1,1,
            1,0,0,0,0,0,0,1,
            1,0,1,1,1,1,0,1,
            1,0,0,0,0,1,0,1,
            1,0,1,1,0,0,0,1,
            1,1,1,1,1,1,1,1
        },
        .playerStartX = 100,
        .playerStartY = 100,
        .playerStartAngle = 90,
        .exitX = 6,
        .exitY = 1,
        .spriteConfigs = {
            {0, 3 * 64 + 32, 3 * 64 + 32},  // enemy 
            {2, 4 * 64 + 32, 5 * 64 + 32},  // key 
            {1, 6 * 64 + 32, 3 * 64 + 32}   // sword 
        }
    };
    levels[0] = level1;
    levels[1] = level2;
    levels[2] = level3;
}

void loadLevel(int levelIndex) {
    // Copy level data to the global arrays
    memcpy(mapWalls, levels[levelIndex].walls, sizeof(mapWalls));
    memcpy(mapFloor, levels[levelIndex].floor, sizeof(mapFloor));
    memcpy(mapFunctionality, levels[levelIndex].functionality, sizeof(mapFunctionality));
    
    // Set player position
    playerX = levels[levelIndex].playerStartX;
    playerY = levels[levelIndex].playerStartY;
    playerAngle = levels[levelIndex].playerStartAngle;
    playerDeltaX = cos(playerAngle)*5;
    playerDeltaY = sin(playerAngle)*5;

    // Reset player state
    hasKey = 0;
    hasSword = 0;

    // Initialize sprites for current level
    for(int i = 0; i < 3; i++) {
        sprites[i].type = levels[levelIndex].spriteConfigs[i].type;
        sprites[i].x = levels[levelIndex].spriteConfigs[i].x;
        sprites[i].y = levels[levelIndex].spriteConfigs[i].y;
        sprites[i].z = (sprites[i].type == 0) ? 10 : 3;  // Enemy taller than items
        sprites[i].state = 1;
    }
}

void checkLevelCompletion() {
    if((int)playerX>>6 == levels[currentLevel].exitX && 
       (int)playerY>>6 == levels[currentLevel].exitY) {
        currentLevel++;
        if(currentLevel >= totalLevels) {
            gameState = 2;  // Win game
        } else {
            loadLevel(currentLevel); 
        }
    }
}

void init(){
    glClearColor(0.3,0.3,0.3,0);
    loadLevel(currentLevel);
    gameStarted = false;
    totalTime = 0;
}
void movement(GLFWwindow* window) {
  // Normalize direction vectors at the start of movement
  float length = sqrt(playerDeltaX*playerDeltaX + playerDeltaY*playerDeltaY);
  if(length != 0) {
    playerDeltaX /= length;
    playerDeltaY /= length;
  }

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

  // Forward/Backward movement
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

  // Rotation movement
  float rotationSpeed = 0.02f;
  
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    // Rotate left
    float oldDirX = playerDeltaX;
    playerDeltaX = playerDeltaX * cos(rotationSpeed) - playerDeltaY * sin(rotationSpeed);
    playerDeltaY = oldDirX * sin(rotationSpeed) + playerDeltaY * cos(rotationSpeed);
    
    // Normalize after rotation
    length = sqrt(playerDeltaX*playerDeltaX + playerDeltaY*playerDeltaY);
    if(length != 0) {
      playerDeltaX /= length;
      playerDeltaY /= length;
    }
    
    // Update player angle
    playerAngle = atan2(playerDeltaY, playerDeltaX);
    if(playerAngle < 0) playerAngle += 2*PI;
    if(playerAngle > 2*PI) playerAngle -= 2*PI;
  }

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    // Rotate right
    float oldDirX = playerDeltaX;
    playerDeltaX = playerDeltaX * cos(-rotationSpeed) - playerDeltaY * sin(-rotationSpeed);
    playerDeltaY = oldDirX * sin(-rotationSpeed) + playerDeltaY * cos(-rotationSpeed);
    
    // Normalize after rotation
    length = sqrt(playerDeltaX*playerDeltaX + playerDeltaY*playerDeltaY);
    if(length != 0) {
      playerDeltaX /= length;
      playerDeltaY /= length;
    }
    
    // Update player angle
    playerAngle = atan2(playerDeltaY, playerDeltaX);
    if(playerAngle < 0) playerAngle += 2*PI;
    if(playerAngle > 2*PI) playerAngle -= 2*PI;
  }

  //door open
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
    if(mapFunctionality[playerGridPosY_add_yOffset*mapX+playerGridPosX_add_xOffset]==2){
        if(hasKey) {
                mapFunctionality[playerGridPosY_add_yOffset*mapX+playerGridPosX_add_xOffset]=0;
                // Play sound or show effect for door opening
            }
        }
    }
  
}

void mouseLook(GLFWwindow* window) {
  double mouseX, mouseY;
  static double prevMouseX, prevMouseY;
  glfwGetCursorPos(window, &mouseX, &mouseY);

  if (!(mouseX == prevMouseX)) {
    float rotationAngle = (mouseX - prevMouseX) * 0.011;

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

void drawPixel(int x, int y, int size, int r, int g, int b) {
    glPointSize(size);
    glColor3ub(r, g, b);
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
}

void drawNumber(int number, int x, int y, int size, int r, int g, int b) {
    for(int row = 0; row < 5; row++) {
        for(int col = 0; col < 4; col++) {
            if(numberPatterns[number][row][col]) {
                drawPixel(x + col * size, y - row * size, size, r, g, b);
            }
        }
    }
}

void drawTime(double time, int x, int y, int size) {
    int minutes = (int)time / 60;
    int seconds = (int)time % 60;
    
    // Draw minutes
    drawNumber(minutes / 10, x, y, size, 255, 255, 255);
    drawNumber(minutes % 10, x + size * 5, y, size, 255, 255, 255);
    
    // Draw colon
    drawPixel(x + size * 9, y - size, size, 255, 255, 255);
    drawPixel(x + size * 9, y - size * 3, size, 255, 255, 255);
    
    // Draw seconds
    drawNumber(seconds / 10, x + size * 11, y, size, 255, 255, 255);
    drawNumber(seconds % 10, x + size * 16, y, size, 255, 255, 255);
}

void drawStatusText() {
    if(hasSword) {
        glPointSize(16);
        glColor3ub(0, 255, 0);  // Green color
        glBegin(GL_POINTS);
        for(int y = 100; y < 150; y += 8) {
            glVertex2i(30, y);  // Draw at top-left corner
        }
        glEnd();
    }
    
    if(hasKey) {
        glPointSize(16);
        glColor3ub(255, 215, 0);  // Gold color
        glBegin(GL_POINTS);
        for(int y = 100; y < 150; y += 8) {
            glVertex2i(10, y);  // Draw below sword indicator
        }
        glEnd();
    }
    
    // Draw timer
    if(gameState == 1) {
        if(!gameStarted) {
            gameStarted = true;
            gameStartTime = glfwGetTime();
        }
        totalTime = glfwGetTime() - gameStartTime;
    }
    drawTime(totalTime, 750, 100, 8);  // Draw at top-right corner
}

void drawVictoryScreen() {
    // Center the text
    int centerX = WINDOW_WIDTH / 2 - 100;
    int centerY = WINDOW_HEIGHT / 2;
    
    if(finalTime == 0){
        finalTime = totalTime; //player haven't died
    }

    // Draw final time
    drawTime(finalTime, centerX, centerY, 16);
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
    for (int i = 0; i < 3; i++) {
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
    static float accumulatedX[3] = {0}; // Track sub-pixel movement
    static float accumulatedY[3] = {0};

    for(int i = 0; i < 3; i++) {
        if(sprites[i].state == 1) {
            // Calculate distance to player
            float dx = playerX - sprites[i].x;
            float dy = playerY - sprites[i].y;
            float distance = sqrt(dx*dx + dy*dy);
            
            // Check for collisions with items and enemies
            if(distance < 30) {  // Collision radius
                switch(sprites[i].type) {
                    case 0:  // Enemy
                        if(hasSword) {
                            sprites[i].state = 0;  // Enemy defeated
                        } else {
                            finalTime += totalTime;
                            gameState = 0;  // Game over
                        }
                        break;
                        
                    case 1:  // Key
                        hasKey = 1;
                        sprites[i].state = 0;  // Collect key
                        break;
                        
                    case 2:  // Sword
                        hasSword = 1;
                        sprites[i].state = 0;  // Collect sword
                        break;
                }
            }
            
            // Only move enemies (type 0)
            if(sprites[i].type == 0) {
                float moveSpeed = 0.5f;
                
                if(distance > 30 && distance < 400) {
                    // Normalize direction vector
                    float length = sqrt(dx*dx + dy*dy);
                    dx = dx / length;
                    dy = dy / length;
                    
                    // Accumulate movement
                    accumulatedX[i] += dx * moveSpeed;
                    accumulatedY[i] += dy * moveSpeed;
                    
                    int moveX = (int)accumulatedX[i];
                    int moveY = (int)accumulatedY[i];
                    
                    if(moveX != 0 || moveY != 0) {
                        // Calculate potential new positions
                        float newX = sprites[i].x + moveX;
                        float newY = sprites[i].y + moveY;
                        
                        // Check wall collisions with some padding
                        int checkX1 = (newX - 10) / 64;  // Left side check
                        int checkX2 = (newX + 10) / 64;  // Right side check
                        int checkY1 = (newY - 10) / 64;  // Top check
                        int checkY2 = (newY + 10) / 64;  // Bottom check
                        
                        // Current position
                        int currentX = sprites[i].x / 64;
                        int currentY = sprites[i].y / 64;
                        
                        // Check if any of the checked positions are walls
                        bool canMoveX = true;
                        bool canMoveY = true;
                        
                        // Check X movement
                        if(checkX1 >= 0 && checkX2 < mapX && currentY >= 0 && currentY < mapY) {
                            if(mapWalls[currentY * mapX + checkX1] != 0 || 
                               mapWalls[currentY * mapX + checkX2] != 0) {
                                canMoveX = false;
                            }
                        }
                        
                        // Check Y movement
                        if(currentX >= 0 && currentX < mapX && checkY1 >= 0 && checkY2 < mapY) {
                            if(mapWalls[checkY1 * mapX + currentX] != 0 || 
                               mapWalls[checkY2 * mapX + currentX] != 0) {
                                canMoveY = false;
                            }
                        }
                        
                        // Apply movement only if no wall collision
                        if(canMoveY) {
                            sprites[i].y = newY;
                            accumulatedY[i] -= moveY;
                        } else {
                            accumulatedY[i] = 0; // Reset accumulator if we hit a wall
                        }
                        
                        if(canMoveX) {
                            sprites[i].x = newX;
                            accumulatedX[i] -= moveX;
                        } else {
                            accumulatedX[i] = 0; // Reset accumulator if we hit a wall
                        }
                    }
                }
            }
        }
    }
}
    



void drawSprite(int spriteIndex){
    if(spriteIndex >= 3 || sprites[spriteIndex].state == 0) return;
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
    
    float spriteScale = 0.05;  // Adjust for desired size
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

    int *currentTexture;
    switch(sprites[spriteIndex].type) {
        case 0:
            currentTexture = enemy;
            break;
        case 1:
            currentTexture = key;
            break;
        case 2:
            currentTexture = sword;
            break;
        default:
            currentTexture = enemy;
            break;
    }

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
                            int red = currentTexture[color_index + 0];
                            int green = currentTexture[color_index + 1];
                            int blue = currentTexture[color_index + 2];
                            
                            if(!(red == 255 && green == 0 && blue == 255)) {
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

    if(gameState==0){ init();  gameState=1;}//main screen

    if (gameState==1){ //game loop
        drawSky();
        drawRays();
        updateSprites();
        sortSprites();
        for (int i = 0; i < 3; i++) {
            drawSprite(i);
        }
        drawStatusText();
        
        checkLevelCompletion();
    }

    if(gameState == 2){
        drawVictoryScreen();
        victoryTimer++;
        
        if(victoryTimer > 18000) { // About 3 seconds at 60 FPS
            gameState = 0;
            currentLevel = 0;
            init();
        }
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

    initializeLevels();

    init();

    while (!glfwWindowShouldClose(window)) {
        movement(window);
        mouseLook(window);
        display(window);
        glfwPollEvents();
    }
 
    glfwTerminate();
    return 0;
}