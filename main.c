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
* https://www.dafont.com/es/arcade-classic-2.font
*
* Audios:
* https://opengameart.org/content/8-bit-platformer-sfx
* https://opengameart.org/content/retro-shooter-sound-effects
*
* 
*
*
********************************************************************************************/

#include "raylib.h"

#define PHYSAC_IMPLEMENTATION
#include "extras/physac.h"

typedef struct
{
    PhysicsBody body;
    int radius;
    bool enable;
    Color color;
}ball_s;

typedef struct 
{
   PhysicsBody body;
   int posX;
   int posY;
   int width;
   int height;
   Color color;
}wall_s;

typedef struct 
{
    Vector2 init;
    Vector2 end;
    int alive;
}shoot_s;


#define MAX_NUM_BALLS        30
#define MINUM_RADIO_BALL     30
#define NUMER_OF_WALLS        4
#define WALL_COLOR       ORANGE
#define BACKGROUND_SPEED    0.1
#define MAX_NUMBER_OF_SHOOTS  5
#define LASER_COLOR         RED
#define LASER_TIME_ALIVE      8

ball_s balls[MAX_NUM_BALLS];
wall_s walls[NUMER_OF_WALLS];
shoot_s shoots[MAX_NUMBER_OF_SHOOTS];

int num_balls = 0;
int points  = 0;
const int screenWidth = 800;
const int screenHeight = 450;

int getdistance(Vector2 p1, Vector2 p2);
void new_ball(int radius, Vector2 speed, Vector2 position, Color color);
void new_random_ball(void);

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    InitAudioDevice();

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "my first videogame");

    Sound sound1 = LoadSound("audio/Jump.wav");  
    Sound music = LoadSound("audio/mixkit-game-level-music-689.wav");
    Sound fire_sound = LoadSound("audio/Fire 1.mp3");
    Texture2D ball_image = LoadTexture("images/ball1.png");
    Texture2D mira_image = LoadTexture("images/mira.png");
    Texture2D landscape = LoadTexture("images/landscape2.png");
    Font ArcadeFont = LoadFont("fonts/ARCADECLASSIC.TTF");  

    
    // Initialize physics and default physics bodies
    InitPhysics();
    
    //PlaySound(music);
    HideCursor();
    
    // floor
    walls[0].posX = 0;
    walls[0].posY = screenHeight - 1;
    walls[0].width = screenWidth;
    walls[0].height = 1;
    walls[0].body = CreatePhysicsBodyRectangle((Vector2){screenWidth/2.0f, (float)screenHeight}, (float)screenWidth, 1, 10);
    walls[0].body->restitution = 1;
    walls[0].body->enabled = false; // Disable body state to convert it to static (no dynamics, but collisions)
    walls[0].color = WALL_COLOR;

    // ceiling
    walls[1].posX = 0;
    walls[1].posY = 0;
    walls[1].width = screenWidth;
    walls[1].height = 1;
    walls[1].body = CreatePhysicsBodyRectangle((Vector2){screenWidth/2.0f, 0}, (float)screenWidth, 1, 10);
    walls[1].body->restitution = 1;
    walls[1].body->enabled = false; // Disable body state to convert it to static (no dynamics, but collisions)
    walls[1].color = WALL_COLOR;

    //left wall
    walls[2].posX = 0;
    walls[2].posY = 0;
    walls[2].width = 1;
    walls[2].height = screenHeight;
    walls[2].body = CreatePhysicsBodyRectangle((Vector2){0, screenHeight/2.0f}, 1, screenHeight, 10);
    walls[2].body->restitution = 1;
    walls[2].body->enabled = false; // Disable body state to convert it to static (no dynamics, but collisions)
    walls[2].color = WALL_COLOR;

    // right wall
    walls[3].posX = screenWidth - 1;
    walls[3].posY = 0;
    walls[3].width = 1;
    walls[3].height = screenHeight;
    walls[3].body = CreatePhysicsBodyRectangle((Vector2){(float)screenWidth, screenHeight/2.0f}, 1, screenHeight, 10);
    walls[3].body->restitution = 1;
    walls[3].body->enabled = false; // Disable body state to convert it to static (no dynamics, but collisions)
    walls[3].color = WALL_COLOR;



    new_random_ball();
    new_random_ball();
    new_random_ball();

    
    SetPhysicsGravity(0.0,0.0);
    
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second


    float background_pos = -300;//(1.0)*(float)landscape.width;
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        UpdatePhysics();            // Update physics system

        Vector2 mousePos = GetMousePosition();
         if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) 
         {
            // Laser part
            PlaySound(fire_sound);
            for (int i = 0; i < MAX_NUMBER_OF_SHOOTS; i++)
            {
                if (shoots[i].alive == 0)
                {
                    shoots[i].end = mousePos;
                    shoots[i].init = (Vector2){ screenWidth/2, screenHeight};
                    shoots[i].alive = LASER_TIME_ALIVE;
                    break;
                }
            }

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

                   PlaySound(sound1);
                   balls[i].enable = false;
                   DestroyPhysicsBody(balls[i].body);
                   points +=10;
                   break;
 
                }
            }

         }      
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(WHITE);

        // Draw background
        float scale_landscape = 2*(float)screenWidth / (float)landscape.width ;
        DrawTextureEx(landscape, (Vector2){background_pos,-300}, 0.0, scale_landscape, LIGHTGRAY);
        background_pos +=BACKGROUND_SPEED;


        //DrawFPS(screenWidth - 110, screenHeight - 70);

        // Draw balls
        for (int i = 0; i < MAX_NUM_BALLS; i++)
        {
            if(balls[i].enable == true)
            {
               // DrawCircle(balls[i].body->position.x ,balls[i].body->position.y, balls[i].radius, balls[i].color);
               // ImageResize(&ball_image, balls[i].radius*2,balls[i].radius*2);

                Vector2 pos = (Vector2){ balls[i].body->position.x - balls[i].radius, balls[i].body->position.y - balls[i].radius};
                float scale = (float)(2* balls[i].radius) / (float)ball_image.width;
                DrawTextureEx(ball_image, pos, 0.0, scale, LIGHTGRAY);
            }
        }

        // Draw punto de mira
        float mira_scale = (float)1/15; 
        Vector2 mira_pos = (Vector2){mousePos.x - mira_scale*(float)mira_image.width/2, mousePos.y - mira_scale*(float)mira_image.height/2};
        DrawTextureEx(mira_image, mira_pos, 0.0, mira_scale, LIGHTGRAY);  



        // Draw Lasers
        for (int i = 0; i < MAX_NUMBER_OF_SHOOTS; i++)
            {
                if (shoots[i].alive != 0)
                {
                    DrawLineEx(shoots[i].init, shoots[i].end, 5.0, LASER_COLOR);
                    shoots[i].alive -= 1;
                }
            }

        // Draw points
        DrawTextEx(ArcadeFont,TextFormat("Points %2i", points),(Vector2){40,40},30,1, ORANGE);
        //DrawText(TextFormat("Points: %2i", points), 40,40,20, ORANGE);



        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------

    DestroyPhysicsBody(walls[0].body);
    DestroyPhysicsBody(walls[1].body);
    DestroyPhysicsBody(walls[2].body);
    DestroyPhysicsBody(walls[3].body);

    ClosePhysics();       // Unitialize physics

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}


int getdistance(Vector2 p1, Vector2 p2)
{

    return (int)sqrt(pow(p2.x - p1.x,2) + pow(p2.y - p1.y,2));
}


void new_random_ball(void)
{
    Vector2 newPos=(Vector2){GetRandomValue(screenWidth*0.2, screenWidth*0.8), GetRandomValue(screenHeight*0.2, screenHeight*0.8) };
    Color newColor=(Color){GetRandomValue(0,255),GetRandomValue(0,255),GetRandomValue(0,255),255};
    int newRadious= GetRandomValue(70, 90);
    Vector2 newSpeed = (Vector2){0.01*GetRandomValue(10, 100),0.01*GetRandomValue(10, 100)};

    new_ball(newRadious, newSpeed, newPos, newColor);
}

void new_ball(int radius, Vector2 speed, Vector2 position, Color color)
{
    if (num_balls < MAX_NUM_BALLS)
    {
        balls[num_balls].radius = radius;
        balls[num_balls].body = CreatePhysicsBodyCircle(position, balls[num_balls].radius, 10);
        balls[num_balls].body->restitution = 1;
        balls[num_balls].body->useGravity = true;
        balls[num_balls].enable = true;
        balls[num_balls].body->velocity = speed; 
        balls[num_balls].color=color;
    }
    num_balls++;
}
