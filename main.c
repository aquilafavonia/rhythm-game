#include "raylib.h"
#include <math.h>
#include <stdio.h>

#define MAX_NOTES 16
#define SAVE_FILE "storage.data"

typedef struct {
    float x, y;
    int lane;
    bool active;
} Note;

static void saveInt(int slot, int value)
{
    FILE *f = fopen(SAVE_FILE, "r+b");
    if (!f) f = fopen(SAVE_FILE, "wb");
    if (!f) return;
    fseek(f, slot * sizeof(int), SEEK_SET);
    fwrite(&value, sizeof(int), 1, f);
    fclose(f);
}

static int loadInt(int slot)
{
    FILE *f = fopen(SAVE_FILE, "rb");
    if (!f) return 0;
    int v = 0;
    fseek(f, slot * sizeof(int), SEEK_SET);
    fread(&v, sizeof(int), 1, f);
    fclose(f);
    return v;
}

// slot 0 = hiscore, slots 1-4 = keybinds
int main(void)
{
    InitWindow(800, 450, "Rhythm Game");
    SetTargetFPS(60);

    // storing horizontal position
    Color laneColors[4] = { BLUE, RED, BLUE, ORANGE };
    float laneX[4];
    for (int i = 0; i < 4; i++)
        laneX[i] = (800 / 2.0f) - (120 * 1.5f) + 120 * i;

    int laneKeys[4];
    int defaults[4] = { KEY_Q, KEY_W, KEY_O, KEY_P };
    for (int i = 0; i < 4; i++)
    {
        int saved = loadInt(i + 1);
        laneKeys[i] = saved ? saved : defaults[i];
    }

    int hiscore = loadInt(0);
    int targetY = 450 - 70;
    int radius = 35;
    int hitWindow = 40;

    Note notes[MAX_NOTES] = {0};
    int score = 0;
    int hearts = 3;
    float spawnTimer = 0;
    bool started = false;
    bool gameover = false;
    bool paused = false;
    int rebinding = -1; // -1 = not rebinding, 0-3 = waiting for key

    // hit or miss text
    const char *hitText[4] = {"", "", "", ""};
    float hitTimer[4] = {0};

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime(); // to increase the speed of notes

        float speedMult = 1.0f + (score / 500) * 0.2f;
        if (speedMult > 6.0f) speedMult = 6.0f;
        float noteSpeed = 200.0f * speedMult;
        float spawnInterval = 1.2f / speedMult;

        // Start screen
        if (!started)
        {
            if (IsKeyPressed(KEY_SPACE)) started = true;
            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("Rhythm Game", 260, 150, 40, BLACK);
            DrawText("Press SPACE to start", 240, 230, 28, DARKGRAY);
            DrawText(TextFormat("Best: %d", hiscore), 330, 280, 24, GRAY);
            EndDrawing();
            continue;
        }

        // Game over screen
        if (gameover)
        {
            if (IsKeyPressed(KEY_SPACE))
            {
                score = 0;
                hearts = 3;
                spawnTimer = 0;
                gameover = false;
                for (int i = 0; i < MAX_NOTES; i++) notes[i].active = false;
            }
            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("GAME OVER", 270, 150, 50, RED);
            DrawText(TextFormat("Score: %d", score), 320, 230, 30, BLACK);
            DrawText(TextFormat("Best:  %d", hiscore), 320, 265, 30, DARKGRAY);
            DrawText("Press SPACE to retry", 240, 320, 28, DARKGRAY);
            EndDrawing();
            continue;
        }

        // Pause toggle
        if (IsKeyPressed(KEY_ESCAPE))
        {
            if (rebinding != -1) { rebinding = -1; }
            else { paused = !paused; }
        }

        // Pause screen
        if (paused)
        {
            if (rebinding != -1)
            {
                int pressed = GetKeyPressed();
                if (pressed != 0 && pressed != KEY_ESCAPE)
                {
                    bool dupe = false;
                    for (int i = 0; i < 4; i++)
                        if (i != rebinding && laneKeys[i] == pressed) { dupe = true; break; }
                    if (!dupe)
                    {
                        laneKeys[rebinding] = pressed;
                        saveInt(rebinding + 1, pressed);
                        rebinding = -1;
                    }
                }
            }

            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("PAUSED", 330, 60, 40, BLACK);
            DrawText("Click a lane button to rebind it", 220, 110, 22, DARKGRAY);
            DrawText("ESC to resume / cancel rebind", 230, 140, 22, DARKGRAY);

            for (int i = 0; i < 4; i++)
            {
                int bx = 200 + i * 110;
                int by = 210;
                bool waiting = (rebinding == i);
                DrawRectangle(bx, by, 90, 50, waiting ? RED : LIGHTGRAY);
                DrawRectangleLines(bx, by, 90, 50, DARKGRAY);

                const char *label;
                char buf[4];
                int k = laneKeys[i];
                if (k >= KEY_A && k <= KEY_Z) { buf[0] = 'A' + (k - KEY_A); buf[1] = 0; label = buf; }
                else if (k == KEY_SPACE) label = "SPC";
                else label = "?";

                DrawText(waiting ? "..." : label, bx + 35 - MeasureText(waiting ? "..." : label, 28)/2, by + 11, 28, BLACK);
                DrawText(TextFormat("Lane %d", i + 1), bx + 10, by + 58, 18, DARKGRAY);

                if (!waiting && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    Vector2 m = GetMousePosition();
                    if (m.x >= bx && m.x <= bx + 90 && m.y >= by && m.y <= by + 50)
                        rebinding = i;
                }
            }

            EndDrawing();
            continue;
        }

        // Spawn
        spawnTimer += dt;
        if (spawnTimer >= spawnInterval)
        {
            spawnTimer = 0;
            int lane = GetRandomValue(0, 3);
            for (int i = 0; i < MAX_NOTES; i++)
            {
                if (!notes[i].active)
                {
                    notes[i] = (Note){ laneX[lane], -(float)radius, lane, true };
                    break;
                }
            }
        }

        // Move notes
        for (int i = 0; i < MAX_NOTES; i++)
        {
            if (!notes[i].active) continue;
            notes[i].y += noteSpeed * dt;
            if (notes[i].y > 450 + radius)
            {
                notes[i].active = false;
                hearts--;
                if (hearts <= 0)
                {
                    hearts = 0;
                    gameover = true;
                    if (score > hiscore) { hiscore = score; saveInt(0, hiscore); }
                }
            }
        }

        // Input
        for (int lane = 0; lane < 4; lane++)
        {
            if (hitTimer[lane] > 0) hitTimer[lane] -= dt;
            if (!IsKeyPressed(laneKeys[lane])) continue;

            int best = -1;
            float bestDist = hitWindow;
            for (int i = 0; i < MAX_NOTES; i++)
            {
                if (!notes[i].active || notes[i].lane != lane) continue;
                float dist = fabsf(notes[i].y - targetY);
                if (dist < bestDist) { bestDist = dist; best = i; }
            }
            if (best >= 0)
            {
                notes[best].active = false;
                score += 100;
                if (score > hiscore) { hiscore = score; saveInt(0, hiscore); }
                hitText[lane] = "HIT!";
            }
            else
            {
                hitText[lane] = "MISS";
                score -= 50; // so they cant spam keybinds
                if (score < 0) score = 0;
            }
            hitTimer[lane] = 0.4f;
        }

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < MAX_NOTES; j++)
            {
                if (!notes[j].active || notes[j].lane != i) continue;
                DrawCircle(laneX[i], notes[j].y, radius + 4, BLACK);
                DrawCircle(laneX[i], notes[j].y, radius, laneColors[i]);
            }

            DrawCircle(laneX[i], targetY, radius + 4, BLACK);
            Color pressedColors[4] = { DARKBLUE, DARKBROWN, DARKBLUE, (Color){180, 100, 0, 255} };
            DrawCircle(laneX[i], targetY, radius, IsKeyDown(laneKeys[i]) ? pressedColors[i] : laneColors[i]);

            if (hitTimer[i] > 0)
                DrawText(hitText[i], laneX[i] - 20, targetY - radius - 30, 20,
                         hitText[i][0] == 'H' ? GREEN : RED);
        }

        DrawText(TextFormat("Score: %d", score), 10, 10, 24, BLACK);
        DrawText(TextFormat("Best:  %d", hiscore), 10, 38, 20, DARKGRAY);
        DrawText(TextFormat("Speed: %.1fx", speedMult), 10, 62, 20,
                 speedMult >= 2.5f ? RED : speedMult >= 1.5f ? ORANGE : DARKGREEN);
        DrawText("ESC to pause", 650, 10, 18, GRAY);

        for (int i = 0; i < 3; i++)
            DrawCircle(750 - i * 30, 430, 12, i < hearts ? RED : DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}