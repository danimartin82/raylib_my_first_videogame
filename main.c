/*******************************************************************************************
*
*   raylib [physac] example - physics restitution
*
*   This example has been created using raylib 1.5 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   This example uses physac 1.1 (https://github.com/raysan5/raylib/blob/master/src/physac.h)
*
*   Copyright (c) 2016-2021 Victor Fisac (@victorfisac) and Ramon Santamaria (@raysan5)
*

* Font
* https://www.dafont.com/es/press-start-2p.font
*
* Audios:
* https://opengameart.org/content/8-bit-platformer-sfx
* https://opengameart.org/content/retro-shooter-sound-effects
* https://opengameart.org/content/multiple-click-sounds-effects
* 
*
*
********************************************************************************************/
#include <string.h>


#include "raylib.h"

#define PHYSAC_IMPLEMENTATION
#include "extras/physac.h"

/*******************************************************************************************
*  TYPEDEF AND ENUMS
********************************************************************************************/
typedef struct
{
    PhysicsBody body;
    int radius;
    bool enable;
    int explosion_timer;
    int color;
    Vector2 final_position;
}ball_s;

typedef struct 
{
   PhysicsBody body;
   int posX;
   int posY;
   int width;
   int height;
}wall_s;

typedef enum 
{ 
    LOGO = 0,
    TITLE,
    LEVEL_1,
    ENDING,
    POINTS_SCREEN,
} GameScreen_e;

typedef enum
{
    WIN,
    LOOSE,
}level_result_e;

typedef struct main
{
   int points_balls;
   int points_per_shoot;
   int points_per_time;
   int points_level;
   int total_points;
}points_s;

/*******************************************************************************************
*  DEFINES 
********************************************************************************************/
#define MAX_NUM_BALLS        30
#define MINUM_RADIO_BALL     30
#define NUMER_OF_WALLS        4

#define BACKGROUND_SPEED    0.1

#define TOTAL_TIME        30.0
#define TOTAL_SHOOTS        25

#define POINT_PER_BALL      10
#define POINT_PER_SHOOT     50
#define POINT_PER_SECOND    20

/*******************************************************************************************
*  LOCAL VARIABLES 
********************************************************************************************/
ball_s balls[MAX_NUM_BALLS];
wall_s walls[NUMER_OF_WALLS];

int num_balls = 0;
points_s points  = {0};
const int screenWidth = 800;
const int screenHeight = 450;
int destroyed_balls = 0;

float remainging_time = TOTAL_TIME;
float init_time = 0.0;
int remaining_shoots = TOTAL_SHOOTS;
int framesCounter = 0;
GameScreen_e currentScreen = LOGO;
level_result_e level_result = LOOSE;
Vector2 mousePos;

float background_pos = -300;
int level = 1;


/*******************************************************************************************
*  LOCAL FUNCTIONS 
********************************************************************************************/
int getdistance(Vector2 p1, Vector2 p2);
void new_ball(int radius, Vector2 speed, Vector2 position, int color);
void new_random_ball(void);
void createWalls(void);
void destroyWalls(void);


/*******************************************************************************************
*
*  main
* 
********************************************************************************************/
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    InitAudioDevice();

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "my first videogame");



    Sound explosion_sound = LoadSound("audio/Explosion.wav");  
    //Sound music = LoadSound("audio/mixkit-game-level-music-689.wav");
    Sound fire_sound = LoadSound("audio/Fire 1.mp3");
    Sound no_shoots_sound = LoadSound("audio/klick1.wav");

    Texture2D ball_image_1 = LoadTexture("images/ball3.png");
    Texture2D ball_image_2 = LoadTexture("images/ball4.png");
    Texture2D ball_image_3 = LoadTexture("images/ball5.png");
    Texture2D ball_images[3]={ball_image_1, ball_image_2, ball_image_3};
    Texture2D explosion = LoadTexture("images/explosion.png");
    int explosion_width = explosion.width /4;
    Texture2D mira_image = LoadTexture("images/mira2.png");
    Texture2D landscape = LoadTexture("images/landscape2.png");
    Font ArcadeFont = LoadFont("fonts/PressStart2P.ttf");  

    
    // Initialize physics and default physics bodies
    InitPhysics();
    
    //PlaySound(music);
    HideCursor();
  
    
    SetPhysicsGravity(0.0,0.0);
    
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second


    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        switch(currentScreen)
        {
            case LOGO:
            {
                // TODO: Update LOGO screen variables here!

                framesCounter++;    // Count frames

                // Wait for 2 seconds (120 frames) before jumping to TITLE screen
                if (framesCounter > 120)
                {
                    currentScreen = TITLE;
                }
            } break;
            case TITLE:
            {
                for (int i = 0; i < MAX_NUM_BALLS; i++)
                {
                   DestroyPhysicsBody(balls[i].body); 
                   balls[i].color = 0;
                   balls[i].enable = false;
                   balls[i].explosion_timer = 0;
                   balls[i].final_position = (Vector2){0,0};
                   balls[i].radius = 0;
                }
    
                num_balls = 0;
                remainging_time = TOTAL_TIME;
                init_time = 0.0;
                remaining_shoots = TOTAL_SHOOTS;
                background_pos = -300;
                destroyed_balls = 0;
                level_result = LOOSE;

                // Press enter to change to GAMEPLAY screen
                if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
                {
                    createWalls();

                    new_random_ball();
                    new_random_ball();
                    new_random_ball();

                    init_time = GetTime();
                    currentScreen = LEVEL_1;
                }
            } break;
            case LEVEL_1:
            {
                
                UpdatePhysics();
                float actual_time = GetTime();
                remainging_time = TOTAL_TIME - (actual_time - init_time);
 
                mousePos = GetMousePosition();
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) 
                {
                    if (remaining_shoots > 0)
                    {
                         // Laser part
                        PlaySound(fire_sound);
                        remaining_shoots -=1;

                        // Check if a ball is hit
                        for (int i = 0; i < MAX_NUM_BALLS; i++)
                        {
                            if((balls[i].enable == true)&&(getdistance(mousePos, balls[i].body->position) <=  balls[i].radius))
                            {
                       
                                if ((num_balls < (MAX_NUM_BALLS + 1)) && ( balls[i].radius >= MINUM_RADIO_BALL))
                                {
                                    Vector2 newSpeed = (Vector2){-balls[i].body->velocity.x*2,-balls[i].body->velocity.y*2};
                                    new_ball(balls[i].radius/2, newSpeed, balls[i].body->position,balls[i].color);
                            
                                    newSpeed = (Vector2){balls[i].body->velocity.x*2,balls[i].body->velocity.y*2};
                                    new_ball(balls[i].radius/2, newSpeed, balls[i].body->position, balls[i].color);
                                }
                                else if ((num_balls < (MAX_NUM_BALLS)) && ( balls[i].radius >= MINUM_RADIO_BALL))
                                {
                                    new_ball(balls[i].radius/2, balls[i].body->velocity,balls[i].body->position,balls[i].color);
                                }

                                PlaySound(explosion_sound);
                                balls[i].enable = false;
                                balls[i].final_position = balls[i].body->position;
                                DestroyPhysicsBody(balls[i].body);              
                                balls[i].explosion_timer = 16;
                                destroyed_balls++;
                                break;
 
                            }
                        }
                    }
                    else
                    {
                        PlaySound(no_shoots_sound);
                    }
                }     


                // 
                if ((remainging_time <=0) || (remaining_shoots == 0) || (destroyed_balls >= 21))
                {
                    currentScreen = ENDING;
                    destroyWalls();
                }
            } break;
            case ENDING:
            {
                if(destroyed_balls < 21)
                {
                    level_result = LOOSE;
                }
                else
                {
                    level_result = WIN;
                }

                // Press enter to return to TITLE screen
                if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
                {
                    if(destroyed_balls < 21)
                       {
                           level_result = LOOSE;
                           currentScreen = TITLE;
                           memset(&points, 0, sizeof(points));
                           level = 1;
                       }
                       else
                       {
                           level_result = WIN;
                           currentScreen = POINTS_SCREEN;
                           level+=1;
                           framesCounter = 0;
                       } 
                }
            } break;

            case POINTS_SCREEN:
            {
                points.points_balls = destroyed_balls * POINT_PER_BALL;
                points.points_per_shoot = remaining_shoots * POINT_PER_SHOOT;
                points.points_per_time = floor(remainging_time) * POINT_PER_SECOND;
                points.points_level = points.points_balls +  points.points_per_shoot + points.points_per_time;

                if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
                {
                    currentScreen = TITLE;
                    points.total_points += points.points_level;
                    points.points_level = 0;
                }

            }
            break;

            default: break;
        }
        // Update
        //----------------------------------------------------------------------------------
       
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(WHITE);

        switch(currentScreen)
            {
                case LOGO:
                {
                    // TODO: Draw LOGO screen here!

                    DrawRectangle(0, 0, screenWidth, screenHeight, SKYBLUE);           
                    DrawTextEx(ArcadeFont,"Digital Kiwi present....",(Vector2){40,screenHeight/2-20},20,1, MAROON);
         
                } break;
                case TITLE:
                {
                    DrawRectangle(0, 0, screenWidth, screenHeight, SKYBLUE);           
                    DrawTextEx(ArcadeFont,"shoot that PANG!",(Vector2){40,screenHeight/2-20},40,1, MAROON);
                    DrawTextEx(ArcadeFont,"PRESS ENTER to continue...",(Vector2){40,screenHeight/2+40},10,1, MAROON);


                } break;
                case LEVEL_1:
                {
                   // Draw background
                    float scale_landscape = 2*(float)screenWidth / (float)landscape.width ;
                    DrawTextureEx(landscape, (Vector2){background_pos,-300}, 0.0, scale_landscape, LIGHTGRAY);
                    background_pos +=BACKGROUND_SPEED;


                    // Draw balls
                    for (int i = 0; i < MAX_NUM_BALLS; i++)
                    {
                        if (level == 1)
                        {
                            level = 1;
                        }
                        if((balls[i].enable == true) && (balls[i].explosion_timer == 0))
                        {
                            Vector2 pos = (Vector2){ balls[i].body->position.x - balls[i].radius, balls[i].body->position.y - balls[i].radius};
                            float scale = (float)(2* balls[i].radius) / (float)ball_image_1.width;
                            DrawTextureEx(ball_images[balls[i].color],pos, 0.0, scale, WHITE);
                        }
                    }
                    // Draw explosions
                    for (int i = 0; i < MAX_NUM_BALLS; i++)
                    {
                        if(balls[i].explosion_timer != 0)
                        {
                            Vector2 pos = (Vector2){balls[i].final_position.x - balls[i].radius, balls[i].final_position.y - balls[i].radius};
                            int explosion_scale = 2;
                            int frames = 4 - ceil(balls[i].explosion_timer / 4);

                            DrawTextureTiled(explosion,
                                            (Rectangle){frames * explosion_width, 0, explosion_width, explosion_width},
                                            (Rectangle){pos.x, pos.y, explosion_scale*explosion_width, explosion_scale*explosion_width},
                                            (Vector2){0,0},
                                            0.0,
                                            (float)explosion_scale,
                                            WHITE);

                            if(balls[i].explosion_timer == 0)
                            {
                                balls[i].enable = false;
                            }
                            balls[i].explosion_timer -= 1;
                        }     
                    }

                    // Draw punto de mira
                    float mira_scale = (float)1/1; 
                    Vector2 mira_pos = (Vector2){mousePos.x - mira_scale*(float)mira_image.width/2, mousePos.y - mira_scale*(float)mira_image.height/2};
                    DrawTextureEx(mira_image, mira_pos, 0.0, mira_scale, LIGHTGRAY);  


                    // Draw level
                    DrawTextEx(ArcadeFont,TextFormat("Level %2i", level),(Vector2){40,20},20,1, ORANGE);
                    
                    // Draw points
                    DrawTextEx(ArcadeFont,TextFormat("Points %2i", points.total_points),(Vector2){40,40},20,1, ORANGE);

                    // Draw Time
                    DrawTextEx(ArcadeFont,TextFormat("%.1f", remainging_time),(Vector2){screenWidth -110 ,40},20,1, ORANGE);

                    // Draw remaining shoots
                    DrawTextEx(ArcadeFont,TextFormat("Shoots x %2i", remaining_shoots),(Vector2){40,screenHeight -80},20,1, ORANGE);


                } break;
                case ENDING:
                {
                    char* text;
                    Color bkg_clr;
                    Color fore_clr;
                    if (level_result == WIN)
                    {
                        text = "YOU WIN!!!";
                        bkg_clr =SKYBLUE;
                        fore_clr = MAROON;
                    }
                    else
                    {
                        text = "YOU LOOSE...";
                        bkg_clr =BLACK;
                        fore_clr = RED;
                    }

                    DrawRectangle(0, 0, screenWidth, screenHeight, bkg_clr);           
                    DrawTextEx(ArcadeFont,text,(Vector2){40,screenHeight/2-20},40,1, fore_clr);
                    DrawTextEx(ArcadeFont,"PRESS ENTER to continue...",(Vector2){40,screenHeight/2+40},10,1, fore_clr);

                } break;

                case POINTS_SCREEN:
                {
 
                    DrawRectangle(0, 0, screenWidth, screenHeight, SKYBLUE); 
                    DrawTextEx(ArcadeFont,"POINTS",(Vector2){300,60},40,1, MAROON);
                    DrawTextEx(ArcadeFont,TextFormat("%4i", points.points_balls),(Vector2){450,140},20,1, MAROON);
                    DrawTextEx(ArcadeFont,TextFormat("%4i", points.points_per_shoot),(Vector2){450,180},20,1, MAROON);
                    DrawTextEx(ArcadeFont,TextFormat("%4i", points.points_per_time),(Vector2){450,220},20,1, MAROON);
                    DrawTextEx(ArcadeFont,"-----------",(Vector2){300,260},20,1, MAROON);
                    DrawTextEx(ArcadeFont,TextFormat("%4i", points.points_level),(Vector2){450,300},20,1, MAROON);
                    
                    DrawTextEx(ArcadeFont,"PRESS ENTER to continue...",(Vector2){300, 340},10,1, MAROON);

                }
                break;
                default: break;
            }
        

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------

    ClosePhysics();       // Unitialize physics

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

/*******************************************************************************************
*
*  getdistance
* 
********************************************************************************************/
int getdistance(Vector2 p1, Vector2 p2)
{

    return (int)sqrt(pow(p2.x - p1.x,2) + pow(p2.y - p1.y,2));
}

/*******************************************************************************************
*
*  new_random_ball
* 
********************************************************************************************/
void new_random_ball(void)
{
    Vector2 newPos=(Vector2){GetRandomValue(screenWidth*0.2, screenWidth*0.8), GetRandomValue(screenHeight*0.2, screenHeight*0.8) };
    int newColor=GetRandomValue(0,2);
    int newRadious= GetRandomValue(70, 90);
    Vector2 newSpeed = (Vector2){0.01*GetRandomValue(10, 100),0.01*GetRandomValue(10, 100)};

    new_ball(newRadious, newSpeed, newPos, newColor);
}

/*******************************************************************************************
*
*  new_ball
* 
********************************************************************************************/
void new_ball(int radius, Vector2 speed, Vector2 position, int color)
{
    if (num_balls < MAX_NUM_BALLS)
    {
        balls[num_balls].radius = radius;
        balls[num_balls].body = CreatePhysicsBodyCircle(position, balls[num_balls].radius, 10);
        balls[num_balls].body->restitution = 1;
        balls[num_balls].body->useGravity = true;
        balls[num_balls].explosion_timer = 0;
        balls[num_balls].final_position =(Vector2){0.0, 0.0};
        balls[num_balls].enable = true;
        balls[num_balls].body->velocity = speed; 
        balls[num_balls].color=color;
    }
    num_balls++;
}

/*******************************************************************************************
*
*  createWalls
* 
********************************************************************************************/
void createWalls(void)
{
    // floor
    walls[0].posX = 0;
    walls[0].posY = screenHeight - 1;
    walls[0].width = screenWidth;
    walls[0].height = 1;
    walls[0].body = CreatePhysicsBodyRectangle((Vector2){screenWidth/2.0f, (float)screenHeight}, (float)screenWidth, 1, 10);
    walls[0].body->restitution = 1;
    walls[0].body->enabled = false; // Disable body state to convert it to static (no dynamics, but collisions)

    // ceiling
    walls[1].posX = 0;
    walls[1].posY = 0;
    walls[1].width = screenWidth;
    walls[1].height = 1;
    walls[1].body = CreatePhysicsBodyRectangle((Vector2){screenWidth/2.0f, 0}, (float)screenWidth, 1, 10);
    walls[1].body->restitution = 1;
    walls[1].body->enabled = false; // Disable body state to convert it to static (no dynamics, but collisions)

    //left wall
    walls[2].posX = 0;
    walls[2].posY = 0;
    walls[2].width = 1;
    walls[2].height = screenHeight;
    walls[2].body = CreatePhysicsBodyRectangle((Vector2){0, screenHeight/2.0f}, 1, screenHeight, 10);
    walls[2].body->restitution = 1;
    walls[2].body->enabled = false; // Disable body state to convert it to static (no dynamics, but collisions)

    // right wall
    walls[3].posX = screenWidth - 1;
    walls[3].posY = 0;
    walls[3].width = 1;
    walls[3].height = screenHeight;
    walls[3].body = CreatePhysicsBodyRectangle((Vector2){(float)screenWidth, screenHeight/2.0f}, 1, screenHeight, 10);
    walls[3].body->restitution = 1;
    walls[3].body->enabled = false; // Disable body state to convert it to static (no dynamics, but collisions)

}

/*******************************************************************************************
*
*  destroyWalls
* 
********************************************************************************************/
void destroyWalls(void)
{
    DestroyPhysicsBody(walls[0].body);
    DestroyPhysicsBody(walls[1].body);
    DestroyPhysicsBody(walls[2].body);
    DestroyPhysicsBody(walls[3].body);
}