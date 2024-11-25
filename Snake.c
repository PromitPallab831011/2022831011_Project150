#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define WIN_WIDTH 1280
#define WIN_HEIGHT 720
#define SNAKE_SIZE 50
#define SNAKE_W_H 20
#define FOOD_W_H SNAKE_W_H

typedef struct 
{
    int x, y;
    int width, height;
} 
Rect;

bool edgecollide(Rect *snake) 
{
    if (snake[0].x < 0 || snake[0].x + snake[0].width > WIN_WIDTH || 
        snake[0].y < 0 || snake[0].y + snake[0].height > WIN_HEIGHT) 
    {
        return true;
    }
    return false;
}

bool isInRect(Rect r1, Rect r2) 
{
    return (r1.x < r2.x + r2.width && r1.x + r1.width > r2.x &&
    r1.y < r2.y + r2.height && r1.y + r1.height > r2.y);
}

char snakecollide(Rect *snake, int size, Rect food, Rect bonus_food, bool bonus_active) 
{
    for (int i=2;i<size;i++) 
    {
        if (isInRect(snake[0], snake[i])) 
        {
            return 'i';  
        }
    }

    if (bonus_active==false && isInRect(snake[0], food)) 
    {
        return 'f';  
    }

    if (bonus_active && isInRect(snake[0], bonus_food)) 
    {
        return 'b';  
    }

    return '\0';
}

void snakemovement(Rect *snake, int size, char dir) 
{
    for (int i = size - 1; i > 0; i--) 
    {
        snake[i] = snake[i - 1]; 
    }

    int step = SNAKE_W_H; 
    if (dir == 'r') 
    {
        snake[0].x += step;
    } 
    else if (dir == 'l') 
    {
        snake[0].x -= step;
    } 
    else if (dir == 'u') 
    {
        snake[0].y -= step;
    } 
    else if (dir == 'd') 
    {
        snake[0].y += step;
    }
}

void render_text(SDL_Renderer *ren, TTF_Font *font, const char *text, int x, int y, SDL_Color color)
{
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(ren, surface);
    
    SDL_Rect dstRect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(ren, texture, NULL, &dstRect);
    
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void regularfood(SDL_Renderer *renderer, int cx, int cy, int radius)
{
    for (int y = -radius; y <= radius; y++) 
    {
        for (int x = -radius; x <= radius; x++) 
        {
            if (x * x + y * y <= radius * radius) 
            { 
                SDL_RenderDrawPoint(renderer, cx + x, cy + y);
            }
        }
    }
}

int main(int ac, char *av[]) 
{
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window *win = SDL_CreateWindow
    (
        "Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_SHOWN
    );

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    TTF_Font *font = TTF_OpenFont("Arial.ttf", 24);
    SDL_Color white = {255, 255, 255, 255};

    if (font==false) 
    {
        printf("Error loading font: %s\n", TTF_GetError());
        return EXIT_FAILURE;
    }

    bool running = true;
    SDL_Event e;

    char direction = 'r';  

    Rect snake[SNAKE_SIZE];
    int len = 3;

    Rect food;
    Rect bonus;
    bool bonus_active = false;
    int food_counter = 0;
    int score = 0;  

    snake[0].x = 300;
    snake[0].y = 200;

    snake[1].x = snake[0].x - SNAKE_W_H;
    snake[1].y = snake[0].y;

    snake[2].x = snake[1].x - SNAKE_W_H;
    snake[2].y = snake[1].y;

    for (int i = 0; i < SNAKE_SIZE; i++) 
    {
        snake[i].width = snake[i].height = SNAKE_W_H;
    }

    food.width = food.height = FOOD_W_H;
    srand(time(NULL));
    food.x = 20*(rand()%25);
    food.y = 20*(rand()%25);

    bonus.width = bonus.height = FOOD_W_H;

    while (running) 
    {
        while (SDL_PollEvent(&e)) 
        {
            if (e.type == SDL_QUIT) 
            {
                running = false;
            } 
            else if (e.type == SDL_KEYDOWN) 
            {
                int key = e.key.keysym.sym;

                if (key == SDLK_UP && direction != 'd') 
                {
                    direction = 'u';
                } 
                else if (key == SDLK_DOWN && direction != 'u') 
                {
                    direction = 'd';
                } 
                else if (key == SDLK_LEFT && direction != 'r') 
                {
                    direction = 'l';
                } 
                else if (key == SDLK_RIGHT && direction != 'l') 
                {
                    direction = 'r';
                }
            }
        }

        SDL_SetRenderDrawColor(ren, 45, 45, 45, 255);
        SDL_RenderClear(ren);

        for (int i = 0; i < len; i++) 
        {
            SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
            SDL_RenderFillRect(ren, (SDL_Rect *)&snake[i]);
        }

        if (bonus_active==false) 
        {
            SDL_SetRenderDrawColor(ren, 200, 45, 45, 255); 
            regularfood(ren, food.x + FOOD_W_H / 2, food.y + FOOD_W_H / 2, FOOD_W_H / 2);
        }

        if (bonus_active) 
        {
            SDL_SetRenderDrawColor(ren, 135, 115, 0, 255); 
            regularfood(ren, bonus.x + FOOD_W_H / 2, bonus.y + FOOD_W_H / 2, FOOD_W_H / 2);
        }

        snakemovement(snake, len, direction);

       
        if (edgecollide(snake)) 
        {
            running = false;
            break;
        }

        char coll = snakecollide(snake, len, food, bonus, bonus_active);

        if (coll == 'f') 
        {
            score ++;
            food_counter++;
            snake[len].x = snake[len - 1].x;
            snake[len].y = snake[len - 1].y;
            len++;

            if (food_counter == 5) 
            {
                bonus.x = 20 * (rand() % 25);
                bonus.y = 20 * (rand() % 25);
                bonus_active = true;
                food_counter = 0;
            }    
            else 
            {
                food.x = 20 * (rand() % 25);
                food.y = 20 * (rand() % 25);
            }
        } 
        else if (coll == 'b') 
        {
            score +=2;
            snake[len].x = snake[len - 1].x;
            snake[len].y = snake[len - 1].y;
            len += 2;

            bonus_active = false;
            food.x = 20 * (rand() % 25);
            food.y = 20 * (rand() % 25);
        } 
        else if (coll == 'i') 
        {
            running = false;
        }

        char score_text[50];
        sprintf(score_text, "Score: %d", score);
        render_text(ren, font, score_text, 10, 10, white);

        SDL_RenderPresent(ren);
        SDL_Delay(200);
    }

    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);

    char game_over_text[100];
    sprintf(game_over_text, "Game Over! Final Score: %d", score);
    render_text(ren, font, game_over_text, WIN_WIDTH / 4, WIN_HEIGHT / 2, white);

    SDL_RenderPresent(ren);
    SDL_Delay(3000);

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}
