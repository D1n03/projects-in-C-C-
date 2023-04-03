#include <iostream>
#include <thread>
#include <vector>
#include <stdio.h>
#include <Windows.h>

using namespace std;

int ScreenWidth = 80;
int ScreenHeight = 30;
wstring tetro[7];
int FieldWidth = 12;
int FieldHeight = 18;
unsigned char* pField = nullptr;

int Rotate(int px, int py, int r)
{
    int pi = 0;
    switch (r % 4)
    {
    case 0: // 0 degrees
        pi = py * 4 + px;			
        break;						

    case 1: // 90 degrees		
        pi = 12 + py - (px * 4);
        break;					
        //15 11  7  3 

    case 2: // 180 degrees		
        pi = 15 - (py * 4) - px;
        break;					
        // 3  2  1  0

    case 3: // 270 degrees		
        pi = 3 - py + (px * 4);	
        break;					
    }							

    return pi;
}

bool DoesItFit(int nTetro, int nRotation, int PosX, int PosY)
{
    for (int px = 0; px < 4; px++)
        for (int py = 0; py < 4; py++)
        {
            int pi = Rotate(px, py, nRotation);
            int fi = (PosY + py) * FieldWidth + (PosX + px);
            if (PosX + px >= 0 && PosX + px < FieldWidth)
            {
                if (PosY + py >= 0 && PosY + py < FieldHeight)
                {
                    if (tetro[nTetro][pi] != L'.' && pField[fi] != 0)
                        return false; 
                }
            }
        }
    return true;
}

int main()
{
    // Screen Buffer
    wchar_t *screen = new wchar_t[ScreenWidth * ScreenHeight];
    for (int i = 0; i < ScreenWidth * ScreenHeight; i++) screen[i] = L' ';
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    tetro[0].append(L"..X...X...X...X."); // Tetronimos by 4x4
    tetro[1].append(L"..X..XX...X.....");
    tetro[2].append(L".....XX..XX.....");
    tetro[3].append(L"..X..XX..X......");
    tetro[4].append(L".X...XX...X.....");
    tetro[5].append(L".X...X...XX.....");
    tetro[6].append(L"..X...X..XX.....");


    pField = new unsigned char[FieldWidth * FieldHeight]; // Create play field buffer
    for (int x = 0; x < FieldWidth; x++) // Board Boundary
        for (int y = 0; y < FieldHeight; y++)
            pField[y * FieldWidth + x] = (x == 0 || x == FieldWidth - 1 || y == FieldHeight - 1) ? 9 : 0;


    //Logic
    bool key[4]; 
    int CurrentPiece = 0;
    int CurrentRotation = 0;
    int CurrentX = FieldWidth / 2;
    int CurrentY = 0;
    int Speed = 20;
    int SpeedCount = 0;
    bool ForceDown = false;
    bool RotateHold = true;
    int PieceCount = 0;
    int Score = 0;
    vector<int> vLines;
    bool GameOver = false;
    while (!GameOver)
    {
        //timing 
        this_thread::sleep_for(50ms); 
        SpeedCount++;
        ForceDown = (SpeedCount == Speed);

        //input
        for (int k = 0; k < 4; k++)								// R   L   D Z
            key[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;

        // player movement
        CurrentX += (key[0] && DoesItFit(CurrentPiece, CurrentRotation, CurrentX + 1, CurrentY)) ? 1 : 0;
        CurrentX -= (key[1] && DoesItFit(CurrentPiece, CurrentRotation, CurrentX - 1, CurrentY)) ? 1 : 0;
        CurrentY += (key[2] && DoesItFit(CurrentPiece, CurrentRotation, CurrentX, CurrentY + 1)) ? 1 : 0;

        if (key[3])
        {
            CurrentRotation += (RotateHold && DoesItFit(CurrentPiece, CurrentRotation + 1, CurrentX, CurrentY)) ? 1 : 0;
            RotateHold = false;
        }
        else RotateHold = true;

        // Force the piece down the playfield if it's time
        if (ForceDown)
        {
            // Update difficulty every 50 pieces
            SpeedCount = 0;
            PieceCount++;
            if (PieceCount % 50 == 0)
                if (Speed >= 10) Speed--;

            // Test if piece can be moved down
            if (DoesItFit(CurrentPiece, CurrentRotation, CurrentX, CurrentY + 1))
                CurrentY++; // It can, so do it!
            else
            {
                // It can't! Lock the piece in place
                for (int px = 0; px < 4; px++)
                    for (int py = 0; py < 4; py++)
                        if (tetro[CurrentPiece][Rotate(px, py, CurrentRotation)] != L'.')
                            pField[(CurrentY + py) * FieldWidth + (CurrentX + px)] = CurrentPiece + 1;

                // Check for lines
                for (int py = 0; py < 4; py++)
                    if (CurrentY + py < FieldHeight - 1)
                    {
                        bool bLine = true;
                        for (int px = 1; px < FieldWidth - 1; px++)
                            bLine &= (pField[(CurrentY + py) * FieldWidth + px]) != 0;

                        if (bLine)
                        {
                            // Remove Line, set to =
                            for (int px = 1; px < FieldWidth - 1; px++)
                                pField[(CurrentY + py) * FieldWidth + px] = 8;
                            vLines.push_back(CurrentY + py);
                        }
                    }

                Score += 25;
                if (!vLines.empty())	Score += (1 << vLines.size()) * 100;

                // Pick New Piece
                CurrentX = FieldWidth / 2;
                CurrentY = 0;
                CurrentRotation = 0;
                CurrentPiece = rand() % 7;

                // If piece does not fit straight away, game over!
                GameOver = !DoesItFit(CurrentPiece, CurrentRotation, CurrentX, CurrentY);
            }
        }


        // Draw Field
        for (int x = 0; x < FieldWidth; x++)
            for (int y = 0; y < FieldHeight; y++)
                screen[(y + 2) * ScreenWidth + (x + 2)] = L" ABCDEFG=#"[pField[y * FieldWidth + x]];


        // Draw Current Piece
        for (int px = 0; px < 4; px++)
            for (int py = 0; py < 4; py++)
                if (tetro[CurrentPiece][Rotate(px, py, CurrentRotation)] != L'.')
                    screen[(CurrentY + py + 2) * ScreenWidth + (CurrentX + px + 2)] = CurrentPiece + 65;

        // draw Score
        swprintf_s(&screen[2 * ScreenWidth + FieldWidth + 6], 16, L"SCORE: %8d", Score);

        if (!vLines.empty())
        {
            // Display Frame (cheekily to draw lines)
            WriteConsoleOutputCharacter(hConsole, screen, ScreenWidth * ScreenHeight, { 0,0 }, &dwBytesWritten);
            this_thread::sleep_for(400ms); // Delay a bit

            for (auto& v : vLines)
                for (int px = 1; px < FieldWidth - 1; px++)
                {
                    for (int py = v; py > 0; py--)
                        pField[py * FieldWidth + px] = pField[(py - 1) * FieldWidth + px];
                    pField[px] = 0;
                }

            vLines.clear();
        }

        // Display Frame
        WriteConsoleOutputCharacter(hConsole, screen, ScreenWidth * ScreenHeight, { 0,0 }, &dwBytesWritten);
    }
    CloseHandle(hConsole);
    cout << "Game Over!! Score:" << Score << "\n";
    system("pause");
    return 0;
}
