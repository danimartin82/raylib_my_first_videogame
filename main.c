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
#include <math.h>
#include <stdio.h>
#include "raylib.h"


/*******************************************************************************************
*  TYPEDEF AND ENUMS
********************************************************************************************/
typedef struct
{
    Vector2 center;
    int radius;
    bool enable;
    int explosion_timer;
    Vector2 speed;
    int color;
    Vector2 final_position;
    int collision_timer;
}ball_s;

typedef enum 
{ 
    LOGO = 0,
    TITLE,
    LEVEL,
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

typedef struct
{
   int color;
   int shoots;
   int time;
   int number_balls;
   Vector2 speed;
   int backbround;
   float background_speed;
}level_s;

/*******************************************************************************************
*  DEFINES 
********************************************************************************************/
#define NUMER_OF_LEVELS       6

#define MAX_NUM_BALLS        30
#define MINUM_RADIO_BALL     30

#define NUMER_OF_WALLS        4
#define BACKGROUND_SPEED    0.1

#define POINT_PER_BALL      10
#define POINT_PER_SHOOT     50
#define POINT_PER_SECOND    20

#define BALL_RED      2
#define BALL_BLUE     1
#define BALL_MAROON   0
#define SLOW_SPEED   (Vector2){20.0, 20.0}
#define MEDIUM_SPEED (Vector2){30.0, 30.0}
#define FAST_SPEED   (Vector2){40.0, 40.0}


#define COLLISION_TIME 10
/*******************************************************************************************
*  LOCAL VARIABLES 
********************************************************************************************/
ball_s balls[MAX_NUM_BALLS];
Rectangle walls[NUMER_OF_WALLS];

level_s levels[NUMER_OF_LEVELS] =
{  //     color    shoots   time   numer_balls         balls_speed     bkg  bkg_speed
       {BALL_MAROON, 30,      30,         3,          SLOW_SPEED,       1,     0.1},
       {BALL_BLUE,   20,      30,         2,          SLOW_SPEED,       1,     0.1},
       {BALL_RED,    30,      30,         4,          MEDIUM_SPEED,     1,     0.1},
       {BALL_MAROON, 40,      30,         5,          MEDIUM_SPEED,     1,     0.1},
       {BALL_BLUE,   30,      30,         3,          FAST_SPEED,       1,     0.1},
       {BALL_RED,    40,      30,         4,          FAST_SPEED,       1,     0.1},       

};
int num_balls = 0;
points_s points  = {0};
const int screenWidth = 800;
const int screenHeight = 450;
int destroyed_balls = 0;

float remainging_time = 0;
float init_time = 0.0;
int remaining_shoots = 0;
int framesCounter = 0;
GameScreen_e currentScreen = LOGO;
level_result_e level_result = LOOSE;
Vector2 mousePos;

float background_pos = -300;
int level = 0;


/*******************************************************************************************
*  LOCAL FUNCTIONS 
********************************************************************************************/
bool any_active_ball(void);
int getdistance(Vector2 p1, Vector2 p2);
void new_ball(int radius, Vector2 speed, Vector2 position, int color);
void new_random_ball(int newColor, Vector2 Speed);
void createWalls(void);
void updatePhysics(void);
bool newBall_pos(Vector2* position, int* radius, bool sign);
Vector2 newBall_speed(Vector2 speed, bool sign);
bool check_new_bal_position(Vector2 position, int radius);

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

    InitWindow(screenWidth, screenHeight, "shoot that PANG!");



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
    Texture2D landscape = LoadTexture("images/landscape3.png");
    Font ArcadeFont = LoadFont("fonts/PressStart2P.ttf");  

    //PlaySound(music);
    HideCursor();
      
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
                   balls[i].color = 0;
                   balls[i].enable = false;
                   balls[i].explosion_timer = 0;
                   balls[i].final_position = (Vector2){0,0};
                   balls[i].radius = 0;
                }
    
                num_balls = 0;
                destroyed_balls = 0;
                remainging_time = levels[level].time;
                init_time = 0.0;
                remaining_shoots = levels[level].shoots;
                background_pos = -300;
                level_result = LOOSE;

                // Press enter to change to GAMEPLAY screen
                if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
                {
                    createWalls();
                    for (int i = 0; i < levels[level].number_balls; i++)
                    {
                        new_random_ball(levels[level].color, levels[level].speed);
                    }

                    init_time = GetTime();
                    currentScreen = LEVEL;
                }
            } break;
            case LEVEL:
            {
                
                updatePhysics();
                float actual_time = GetTime();
                remainging_time = levels[level].time - (actual_time - init_time);
 
                mousePos = GetMousePosition();
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) 
                {
                    if (remaining_shoots > 0)
                    {
                         // Laser part
                        PlaySound(fire_sound);
                        remaining_shoots--;

                        // Check if a ball is hit
                        for (int i = 0; i < MAX_NUM_BALLS; i++)
                        {
                            if((balls[i].enable == true) && (getdistance(mousePos, balls[i].center) <=  balls[i].radius))
                            {
                                PlaySound(explosion_sound);
                                balls[i].enable = false;
                                balls[i].final_position = balls[i].center;         
                                balls[i].explosion_timer = 16;
                                num_balls--;
                                destroyed_balls++;
                                //printf("DESTROYED BALL r: %d, x: %f, y:%f, i: %d, destroyed_balls: %d, num_balls:%d\n", balls[i].radius, balls[i].center.x,balls[i].center.y,i,destroyed_balls, num_balls);

                                if ((num_balls < (MAX_NUM_BALLS + 1)) && ( balls[i].radius >= MINUM_RADIO_BALL))
                                {
                                    Vector2 newSpeed1, newSpeed2;
                                    Vector2 newPos1, newPos2;
                                    int r1, r2;
                                    if (balls[i].speed.y > 0)
                                    {
                                        newSpeed1 = newBall_speed (balls[i].speed, true);
                                        newSpeed2 = newBall_speed (balls[i].speed, false);                               
                                    }
                                    else
                                    {
                                        newSpeed1 = newBall_speed (balls[i].speed, false);
                                        newSpeed2 = newBall_speed (balls[i].speed, true);    
                                    }
                                    // new ball 1 
                                    newPos1 = balls[i].center;
                                    r1 =  balls[i].radius/2;
                            
                                    // new ball 2
                                    newPos2 = balls[i].center;
                                    r2 =  balls[i].radius/2;

                                    bool valid = newBall_pos(&newPos1, &r1, true);
                                    if (valid == true)
                                    {
                                        new_ball(r1, newSpeed1, newPos1, balls[i].color);
                                    }

                                    valid = newBall_pos(&newPos2, &r2, true);
                                    if (valid == true)
                                    {  
                                        new_ball(r2, newSpeed2, newPos2, balls[i].color);
                                    }
                                }
                                else if ((num_balls < (MAX_NUM_BALLS)) && ( balls[i].radius >= MINUM_RADIO_BALL))
                                {
                                    new_ball(balls[i].radius/2, balls[i].speed, balls[i].center, balls[i].color);
                                }

                                
                                break;
 
                            }
                        }
                    }
                    else
                    {
                        PlaySound(no_shoots_sound);
                    }
                }     



                if ((remainging_time <=0) || (remaining_shoots == 0) || (any_active_ball() == false))
                {
                    currentScreen = ENDING;
                }
            } break;
            case ENDING:
            {
                if(any_active_ball() == true)
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
                    if(any_active_ball() == true)
                       {
                           level_result = LOOSE;
                           currentScreen = TITLE;
                           memset(&points, 0, sizeof(points));
                           level = 0;
                       }
                       else
                       {
                           level_result = WIN;
                           
                           if (level < NUMER_OF_LEVELS)
                           {
                               currentScreen = POINTS_SCREEN;
                               level+=1;
                           }
                           else
                           {
                               currentScreen = LOGO;
                               level = 0;
                           }
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
                case LEVEL:
                {
                   // Draw background
                    float scale_landscape = 2*(float)screenWidth / (float)landscape.width ;
                    DrawTextureEx(landscape, (Vector2){background_pos,-300}, 0.0, scale_landscape, LIGHTGRAY);
                    background_pos +=levels[level].background_speed;


                    // Draw balls
                    for (int i = 0; i < MAX_NUM_BALLS; i++)
                    {

                        if((balls[i].enable == true) && (balls[i].explosion_timer == 0))
                        {
                            Vector2 pos = (Vector2){ balls[i].center.x - balls[i].radius, balls[i].center.y - balls[i].radius};
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
                    DrawTextEx(ArcadeFont,TextFormat("Level %2i", level+1),(Vector2){40,20},20,1, ORANGE);
                    
                    // Draw points
                    DrawTextEx(ArcadeFont,TextFormat("Points %2i", points.total_points),(Vector2){40,40},20,1, ORANGE);

                    // Draw num_balls
                    DrawTextEx(ArcadeFont,TextFormat("num_balls %2i", num_balls),(Vector2){40,60},20,1, ORANGE);

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
                    DrawTextEx(ArcadeFont,TextFormat(" Balls destroyed: %4i", points.points_balls),(Vector2){200,140},20,1, MAROON);
                    DrawTextEx(ArcadeFont,TextFormat("Remaining shoots: %4i", points.points_per_shoot),(Vector2){200,180},20,1, MAROON);
                    DrawTextEx(ArcadeFont,TextFormat("  Remainint time: %4i", points.points_per_time),(Vector2){200,220},20,1, MAROON);
                    DrawTextEx(ArcadeFont,"-----------------------",(Vector2){200,260},20,1, MAROON);
                    DrawTextEx(ArcadeFont,TextFormat("           Total: %4i", points.points_level),(Vector2){200,300},20,1, MAROON);
                    
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

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

/*******************************************************************************************
*
*  any_active_ball
* 
********************************************************************************************/
bool any_active_ball(void)
{
    bool result = false;
    for (int i = 0; i < MAX_NUM_BALLS; i++)
    {
        if (balls[i].enable == true)
        {
            result = true;
            break;
        }
    }
    return result;
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
void new_random_ball(int newColor, Vector2 Speed)
{
    bool valid = false;
    Vector2 newPos;
    int newRadious;

    while(valid == false)
    {
        newPos =(Vector2){GetRandomValue(screenWidth*0.2, screenWidth*0.8), GetRandomValue(screenHeight*0.2, screenHeight*0.8) };
        newRadious = GetRandomValue(60, 110);
        valid = check_new_bal_position(newPos, newRadious);
    }

    new_ball(newRadious, Speed, newPos, newColor);

}

/*******************************************************************************************
*
*  new_ball
* 
********************************************************************************************/
void new_ball(int radius, Vector2 speed, Vector2 position, int color)
{
  for (int i = 0; i < MAX_NUM_BALLS; i++)
    {
        if ((balls[i].enable == false) && (balls[i].explosion_timer == 0) )
        {
            balls[i].radius = radius;
            balls[i].center = position;
            balls[i].explosion_timer = 0;
            balls[i].final_position =(Vector2){0.0, 0.0};
            balls[i].enable = true;
            balls[i].speed = speed;
            balls[i].color = color;
            balls[i].collision_timer = 0;
        
            num_balls++;
            //printf("NEW BALL r: %d, x: %f, y:%f, i: %d, num_balls:%d\n",radius,position.x,position.y,i,num_balls);
            break;
        }
    }
}        




/*******************************************************************************************
*
*  newBall_pos
* 
********************************************************************************************/
bool newBall_pos(Vector2* position, int* radius, bool sign)
{
    int a;
    int i = 10;
    bool valid = false;
    if (sign == false)
    {
        a = -1;
    }
    else
    {
        a = 1;
    }
    for(int j = 0; j<20; j++)
    {
        *position = (Vector2)                                       \
        {                                                           \
            position->x + i,  \
            position->y + a*i  \
        };                                                          \
        valid = check_new_bal_position(*position, *radius);
        a *= (-1);
        i*=2;
        if (valid == true)
        {
            break;
        }
    }
    return valid;
}

/*******************************************************************************************
*
*  check_new_bal_position
* 
********************************************************************************************/
bool check_new_bal_position(Vector2 position, int radius)
{
    bool valid = true;
    if (CheckCollisionCircleRec(position, radius, walls[0]) == true) // floor
    {
        valid = false;
    }
    if (CheckCollisionCircleRec(position, radius, walls[1]) == true) // ceiling
    {
        valid = false;
    }
    if (CheckCollisionCircleRec(position, radius, walls[2]) == true) // left wall
    {
        valid = false;
    }
    if (CheckCollisionCircleRec(position, radius, walls[3]) == true) // right wall
    {
        valid = false;
    }

    // check position with other balls
    for (int j = 0; j < MAX_NUM_BALLS; j++)
    {
        if (balls[j].enable == true)
        {
            if( CheckCollisionCircles(position, radius, balls[j].center, balls[j].radius) == true)
            {    
                valid = false;
            }    
        }
    }

    if ((position.x + radius) > screenWidth)
    {
        valid = false;
    }
    if ((position.x - radius) <= 0)
    {
        valid = false;
    }
    
    if ((position.y + radius) > screenHeight)
    {
        valid = false;
    }
    if ((position.y - radius) <= 0)
    {
        valid = false;
    }

    return valid;
}
/*******************************************************************************************
*
*  createWalls
* 
********************************************************************************************/
void createWalls(void)
{
    // floor
    walls[0].x = 0;
    walls[0].y = screenHeight - 1;
    walls[0].width = screenWidth;
    walls[0].height = 1;


    // ceiling
    walls[1].x = 0;
    walls[1].y = 0;
    walls[1].width = screenWidth;
    walls[1].height = 1;


    //left wall
    walls[2].x = 0;
    walls[2].y = 0;
    walls[2].width = 1;
    walls[2].height = screenHeight;


    // right wall
    walls[3].x = screenWidth - 1;
    walls[3].y = 0;
    walls[3].width = 1;
    walls[3].height = screenHeight;


}


/*******************************************************************************************
*
*  updatePhysics
* 
********************************************************************************************/
void updatePhysics(void)
{

    float frame_time = GetFrameTime();  

    for (int i = 0; i < MAX_NUM_BALLS; i++)
    {
        if (balls[i].collision_timer > 0)
        {
            balls[i].collision_timer--;
        }
    }

    for (int i = 0; i < MAX_NUM_BALLS; i++)
    {
        if (balls[i].enable == true)
        {
            // check colisions with walls

            if (CheckCollisionCircleRec(balls[i].center, balls[i].radius, walls[0]) == true) // floor
            {
                if (balls[i].collision_timer == 0)
                {
                    balls[i].speed.y *=(-1);
                    balls[i].collision_timer = COLLISION_TIME;
                }
            }
            if (CheckCollisionCircleRec(balls[i].center, balls[i].radius, walls[1]) == true) // ceiling
            {
                if (balls[i].collision_timer == 0)
                {
                    balls[i].speed.y *=(-1);
                    balls[i].collision_timer = COLLISION_TIME;
                }
            }
            if (CheckCollisionCircleRec(balls[i].center, balls[i].radius, walls[2]) == true) // left wall
            {
                if (balls[i].collision_timer == 0)
                {
                    balls[i].speed.x *=(-1);
                    balls[i].collision_timer = COLLISION_TIME;
                }
            }
            if (CheckCollisionCircleRec(balls[i].center, balls[i].radius, walls[3]) == true) // right wall
            {
                if (balls[i].collision_timer == 0)
                {
                    balls[i].speed.x *=(-1);
                    balls[i].collision_timer = COLLISION_TIME;
                }
            }

            // Check limits
            for (int j = 0; j < MAX_NUM_BALLS; j++)
            {
                if (balls[j].enable == true)
                {
                    if ((balls[j].center.x > screenWidth)||(balls[j].center.x <= 0)||
                        (balls[j].center.y > screenHeight)||(balls[j].center.y <= 0))
                    {
                        balls[j].enable = false;
                        balls[j].final_position = balls[j].center;         
                        balls[j].explosion_timer = 0;
                        num_balls--;
                    }
                }
            }
            // check collisions with other balls
            for (int j = 0; j < MAX_NUM_BALLS; j++)
            {
                if ((balls[j].enable == true) && (i != j) )
                {
                    if( CheckCollisionCircles(balls[i].center, balls[i].radius, balls[j].center, balls[j].radius) == true)
                    {
                        if (balls[i].collision_timer == 0)
                        {
                            // frontal collision: j and i have different signs in the speed in x and y
                            if ((balls[i].speed.x / balls[j].speed.x < 0) && (balls[i].speed.y / balls[j].speed.y < 0))
                            {
                                balls[i].speed.x *=(-1);
                                balls[i].speed.y *=(-1);
                            }
                            // partial  collision: j and i have different signs in the speed in x  but same sign in y
                            else if ((balls[i].speed.x / balls[j].speed.x < 0) && (balls[i].speed.y / balls[j].speed.y > 0))
                            {
                                balls[i].speed.x *=(-1);
                            }
                            // partial  collision: j and i have different signs in the speed in y  but same sign in x
                            else if ((balls[i].speed.x / balls[j].speed.x > 0) && (balls[i].speed.y / balls[j].speed.y < 0))
                            {
                                balls[i].speed.y *=(-1);
                            }
                            // Both balls in the same direction
                            else if ((balls[i].speed.x / balls[j].speed.x > 0) && (balls[i].speed.y / balls[j].speed.y > 0))
                            {
                                float speed_i = sqrt(pow(balls[i].speed.x,2) + pow(balls[i].speed.y,2));
                                float speed_j = sqrt(pow(balls[j].speed.x,2) + pow(balls[j].speed.y,2));
                                // only one ball changes direction
                                if (speed_i > speed_j)
                                {
                                    balls[i].speed.x *=(-1);
                                    balls[i].speed.y *=(-1);

                                }
                                else
                                {
                                    balls[i].speed.x *=1.1;
                                    balls[i].speed.y *=1.1;
                                }
                            }
                            
                            balls[i].collision_timer = COLLISION_TIME;
                        }
                        
                    }
                }
            }
   
            // update position
            balls[i].center.x += frame_time * balls[i].speed.x;
            balls[i].center.y += frame_time * balls[i].speed.y;

        }
    }
}


/*******************************************************************************************
*
*  newBall_speed
* 
********************************************************************************************/
Vector2 newBall_speed(Vector2 speed, bool sign)
{
    int speed_multiplier;  
    if (sign == false)
    {
        speed_multiplier =-2;
    }
    else
    {
        speed_multiplier = 2;
    }

    Vector2 newSpeed = (Vector2)    \
    {                               \
     speed.x ,  \
     speed.y * speed_multiplier   \
     };                             \
    return newSpeed;
}

