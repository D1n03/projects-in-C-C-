#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>

using namespace std;

struct Object
{
    int x;
    int y;
};
enum Directions
{
    STOP,
    LEFT,
    RIGHT,
    DOWN,
    UP,
};

bool GameOver;
int height;
int width;
Object snake;
Object tail[200];
Object point;
Directions dir;
int scor = 0;
int ntail = 0;

void Setup(){
    GameOver = false;
    height = 15; //15
    width = 25;  //25
    snake.x = 5;
    snake.y = 5;
    point.x = rand() % width;
    point.y = rand() % height;
    dir = STOP;
}
void Draw(){
    system("cls"); //system ("clear");
    for (int i = 0; i < width; i++) cout << "#";
    cout << "\n";
    for (int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            if (j == 0 || j == width - 1) cout << "#";
            else
                if (i == snake.y && j == snake.x) cout << "0";
            else
                if (i == point.y && j == point.x) cout << "*";
            else
            {
              bool coada = false;
              for (int k = 0; k <= ntail; k++)
              {
                  if (tail[k].x == j && tail[k].y == i) {coada = true; break;}
              }
              if (!coada) cout << " ";
              else cout << "o";
            }
        }
        cout << "\n";
    }
    for (int i = 0; i < width; i++) cout <<"#";
    cout << "\n";
    cout <<"Scor : " << scor;
}
void Input(){
    if (_kbhit())
    {
        switch (_getch())
        {
            case 'a' : dir = LEFT; break;
            case 's' : dir = DOWN; break;
            case 'd' : dir = RIGHT; break;
            case 'w' : dir = UP; break;
        }
    }
}
void Logic(){
    switch (dir)
    {
        case LEFT : snake.x--;
             break;
        case RIGHT : snake.x++;
             break;
        case DOWN : snake.y++;
             break;
        case UP : snake.y--;
             break;
    }
    int xprev, yprev;
    xprev = snake.x;
    yprev = snake.y;
    int xprev2,yprev2;
    for (int i = 0; i <= ntail; i++)
    {
        xprev2 = tail[i].x;
        yprev2 = tail[i].y;
        tail[i].x = xprev;
        tail[i].y = yprev;
        xprev = xprev2;
        yprev = yprev2;
    }
    if (snake.x == point.x && snake.y == point.y)
    {
        scor++;
        ntail++;
        point.x = rand() % width;
        point.y = rand() % height;
    }
     if (snake.x == 0) GameOver = true;
     if (snake.x == width) GameOver = true;
     if (snake.y == 0) GameOver = true;
     if (snake.y == height) GameOver = true;
     for (int i = 1; i <= ntail; i++)
     {
         if (snake.x == tail[i].x && snake.y == tail[i].y) {GameOver = true; break;}
     }
}

int main()
{
    Setup();
    while (!GameOver)
    {
        Input();
        Logic();
        Draw();
        Sleep(200);
    }
    return 0;
}