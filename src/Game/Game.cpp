#include "Game.h"
#include "../../include/RCUT.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <GL/freeglut.h>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <vector>
#include <cmath>

namespace game
{
    // ==================================================================
    // NOTE ON TWO UNVERIFIED ASSUMPTIONS
    //
    // RCUT.h groups sprites under "Objects" separately from the
    // "Raycaster" section, and RCUT is a closed DLL, so two things
    // can't be confirmed from the header alone and need a quick test
    // run to check:
    //
    //   1) Whether RCUT_Raycaster_Render() draws the global sprite
    //      list automatically. This code assumes it does. If ghosts/
    //      orbs don't appear on screen at all, that assumption is
    //      wrong and sprites need compositing by hand instead - come
    //      back and we'll do that.
    //
    //   2) Row order of the framebuffer RCUT_Raycaster_GetFramebuffer()
    //      returns. If the view renders upside-down, flip
    //      FLIP_FRAMEBUFFER_Y below to true.
    // ==================================================================
    static const bool FLIP_FRAMEBUFFER_Y = false;

    // ------------------------------------------------------------
    // Map - reused from the RCUT test project: 0 = empty, 1 = wall,
    // 2 = teleport tunnel. It's already Pac-Man shaped (symmetric,
    // side tunnels, a 3-wide gap at the centre that we use as the
    // ghost house).
    // ------------------------------------------------------------
    static const int kMapWidth = 21;
    static const int kMapHeight = 27;

    static int g_map[] = {
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,
        1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,
        1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
        1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,
        1,0,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,
        1,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,
        1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,
        0,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,1,
        0,0,0,0,1,0,1,0,1,1,0,1,1,0,1,0,1,0,0,0,1,
        1,1,1,1,1,0,1,0,1,0,0,0,1,0,1,0,1,1,1,1,1,
        2,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,2,
        1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,
        0,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,1,
        0,0,0,0,1,0,1,0,1,1,1,1,1,0,1,0,1,0,0,0,1,
        1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,
        1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,
        1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,
        1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,
        1,1,1,0,1,0,1,0,1,1,1,1,1,0,1,0,1,0,1,1,1,
        1,1,1,0,1,0,1,0,1,1,1,1,1,0,1,0,1,0,1,1,1,
        1,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,
        1,0,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    };

    static int Tile(int col, int row)
    {
        if (col < 0 || row < 0 || col >= kMapWidth || row >= kMapHeight) return 1;
        return g_map[row * kMapWidth + col];
    }

    // ------------------------------------------------------------
    // Internal render resolution for the raycaster. Blitted up to
    // fill whatever the window size currently is.
    // ------------------------------------------------------------
    static const int kRayW = 640;
    static const int kRayH = 360;

    // Player
    static RCUT_Camera g_cam;
    static const float kPlayerStartX = 2.5f;
    static const float kPlayerStartY = 1.5f;
    static const float kMoveSpeed = 3.0f;
    static const float kRotSpeed = 2.0f;
    static const float kMouseSensitivity = 0.003f;

    // Lives
    static int  g_lives = 3;
    static bool g_gameOver = false;

    // Ghosts: fixed spawn points in/around the centre "ghost house" gap.
    struct Ghost { RCUT_SpriteId spriteId; float x, y; };
    static std::vector<Ghost> g_ghosts;

    // Orbs (pellets): small ones fill the maze, big "power" ones sit
    // in the four corner pockets, classic Pac-Man style.
    struct Orb { RCUT_SpriteId spriteId; float x, y; bool big; bool collected; };
    static std::vector<Orb> g_orbs;

    // Fruit: single bonus item, placed by seeded RNG on init().
    static RCUT_SpriteId g_fruitSprite = -1;
    static float g_fruitX = 0, g_fruitY = 0;
    static bool  g_fruitCollected = true; // true until placed

    // Textures
    static RCUT_TextureId g_wallTex, g_floorTex, g_roofTex, g_doorTex;
    static RCUT_TextureId g_ghostTex[5]; // red, pink, blue, yellow, scared
    static RCUT_TextureId g_orbSmallTex, g_orbBigTex, g_fruitTex;

    static bool g_initialised = false;

    // ------------------------------------------------------------
    // Mouse-look: tested twice now with pure FreeGLUT
    // (glutMotionFunc/glutPassiveMotionFunc) and passive delivery
    // just isn't reliable in this shared-window setup - most likely
    // SolarUI's own message handling on this window intercepts plain
    // mouse-move messages before FreeGLUT's passive-motion handler
    // sees them (button-drag messages get through some other path
    // SolarUI doesn't touch, which is why holding a click "worked").
    //
    // Falling back to direct Win32 polling instead (GetCursorPos /
    // SetCursorPos each frame) - functionally this is what RCUT's own
    // Engine_BeginFrame is almost certainly doing internally for its
    // own window anyway. This previously caused a runaway spin, but
    // that was tracked down to RCUT_Input_Update() doing its own
    // hidden cursor recentring and fighting with ours - since that's
    // no longer called anywhere (keyboard uses IsKeyDown/
    // IsSpecialKeyDown directly, unaffected), nothing else should be
    // touching the cursor now.
    // ------------------------------------------------------------
    static bool  g_mouseReady = false;
    static int   g_mouseLastX = 0, g_mouseLastY = 0; // screen-space
    static float g_mouseDeltaXAccum = 0.0f;

    static void PollAndRecentreMouse()
    {
#ifdef _WIN32
        POINT p;
        GetCursorPos(&p);

        if (g_mouseReady)
            g_mouseDeltaXAccum += (float)(p.x - g_mouseLastX);

        int winX = glutGet(GLUT_WINDOW_X);
        int winY = glutGet(GLUT_WINDOW_Y);
        int winW = glutGet(GLUT_WINDOW_WIDTH);
        int winH = glutGet(GLUT_WINDOW_HEIGHT);
        int centreX = winX + winW / 2;
        int centreY = winY + winH / 2;

        SetCursorPos(centreX, centreY);

        // Read back where the cursor actually landed rather than
        // trusting our calculated target, in case GLUT's window
        // metrics and Win32's cursor coordinates don't quite agree
        // (e.g. DPI scaling) - keeps every frame self-consistent
        // regardless of that.
        POINT afterWarp;
        GetCursorPos(&afterWarp);
        g_mouseLastX = afterWarp.x;
        g_mouseLastY = afterWarp.y;
        g_mouseReady = true;
#endif
    }

    // ------------------------------------------------------------
    // Keyboard: tracked via FreeGLUT's own keyboard callbacks (fed in
    // from main.cpp) instead of RCUT_Input_IsKeyDown/IsSpecialKeyDown -
    // those require RCUT_Input_Init(), which also hides the cursor and
    // fights our own mouse-look recentring, so RCUT's input system is
    // avoided entirely rather than just partially.
    // ------------------------------------------------------------
    static bool g_keyState[256] = { false };
    static bool g_specialLeft = false, g_specialRight = false;

    void onKeyDown(unsigned char key, int, int)      { g_keyState[key] = true; }
    void onKeyUp(unsigned char key, int, int)        { g_keyState[key] = false; }

    void onSpecialKeyDown(int key, int, int)
    {
        if (key == GLUT_KEY_LEFT)  g_specialLeft = true;
        if (key == GLUT_KEY_RIGHT) g_specialRight = true;
    }
   
    void onSpecialKeyUp(int key, int, int)
    {
        if (key == GLUT_KEY_LEFT)  g_specialLeft = false;
        if (key == GLUT_KEY_RIGHT) g_specialRight = false;
    }

    // ------------------------------------------------------------
    static void ResetPlayer()
    {
        RCUT_Camera_Set(&g_cam, kPlayerStartX, kPlayerStartY, 0.0f, 0.66f);
    }

    static void LoadTextures()
    {
        g_wallTex  = RCUT_Textures_Load("Assets/Textures/Tile/Wall/Wall.bmp",   nullptr, 1.0f);
        g_doorTex  = RCUT_Textures_Load("Assets/Textures/Tile/Wall/TPDOOR.bmp", nullptr, 1.0f);
        g_floorTex = RCUT_Textures_Load("Assets/Textures/Tile/Floor/Floor.bmp", nullptr, 1.0f);
        g_roofTex  = RCUT_Textures_Load("Assets/Textures/Tile/Roof/Roof.bmp",   nullptr, 1.0f);

        if (g_wallTex  >= 0) RCUT_Raycaster_SetWallTexture(1, g_wallTex);
        if (g_doorTex  >= 0) RCUT_Raycaster_SetWallTexture(2, g_doorTex); // teleport tile
        if (g_floorTex >= 0) RCUT_Raycaster_SetFloorTexture(g_floorTex);
        if (g_roofTex  >= 0) RCUT_Raycaster_SetCeilingTexture(g_roofTex);

        g_ghostTex[0] = RCUT_Textures_Load("Assets/Textures/Sprite/Ghosts/Red/Front.png",    nullptr, 1.0f);
        g_ghostTex[1] = RCUT_Textures_Load("Assets/Textures/Sprite/Ghosts/Pink/Front.png",   nullptr, 1.0f);
        g_ghostTex[2] = RCUT_Textures_Load("Assets/Textures/Sprite/Ghosts/Blue/Front.png",   nullptr, 1.0f);
        g_ghostTex[3] = RCUT_Textures_Load("Assets/Textures/Sprite/Ghosts/Yellow/Front.png", nullptr, 1.0f);
        g_ghostTex[4] = RCUT_Textures_Load("Assets/Textures/Sprite/Ghosts/Scared/Front.png", nullptr, 1.0f);

        g_orbSmallTex = RCUT_Textures_Load("Assets/Textures/Sprite/Orbs/Small/orb.png", nullptr, 1.0f);
        g_orbBigTex   = RCUT_Textures_Load("Assets/Textures/Sprite/Orbs/Big/orb.png",   nullptr, 1.0f);
        g_fruitTex    = RCUT_Textures_Load("Assets/Textures/Sprite/Items/Fruit/Apple.png", nullptr, 1.0f);
    }

    static void SpawnGhosts()
    {
        g_ghosts.clear();
        // The centre gap (row 13, cols 9-11) plus the tile directly
        // above it (row 12, col 10) - four natural ghost-house spots.
        struct { int col, row, texIdx; } spawns[4] = {
            { 9, 13, 0 }, // red
            { 10, 13, 1 }, // pink
            { 11, 13, 2 }, // blue
            { 10, 12, 3 }, // yellow
        };

        for (auto& s : spawns)
        {
            float x = s.col + 0.5f;
            float y = s.row + 0.5f;
            RCUT_SpriteId id = RCUT_Sprite_Add(x, y, g_ghostTex[s.texIdx]);
            g_ghosts.push_back({ id, x, y });
        }
    }

    static bool IsGhostHouseOrStart(int col, int row)
    {
        // Skip the ghost house interior and the player's own start tile
        // when scattering pellets.
        if (row == 13 && col >= 9 && col <= 11) return true;
        if (row == 12 && col == 10) return true;
        if (col == (int)kPlayerStartX && row == (int)kPlayerStartY) return true;
        return false;
    }

    static void SpawnOrbs()
    {
        g_orbs.clear();

        // Big power-pellets in the four open corner pockets.
        int bigSpots[4][2] = { {1,1}, {19,1}, {1,25}, {19,25} };
        for (auto& p : bigSpots)
        {
            float x = p[0] + 0.5f, y = p[1] + 0.5f;
            RCUT_SpriteId id = RCUT_Sprite_Add(x, y, g_orbBigTex);
            g_orbs.push_back({ id, x, y, true, false });
        }

        // Small pellets on every other open floor tile.
        for (int row = 0; row < kMapHeight; row++)
        {
            for (int col = 0; col < kMapWidth; col++)
            {
                if (Tile(col, row) != 0) continue;
                if (IsGhostHouseOrStart(col, row)) continue;

                bool isBigSpot = false;
                for (auto& p : bigSpots)
                    if (p[0] == col && p[1] == row) { isBigSpot = true; break; }
                if (isBigSpot) continue;

                float x = col + 0.5f, y = row + 0.5f;
                RCUT_SpriteId id = RCUT_Sprite_Add(x, y, g_orbSmallTex);
                g_orbs.push_back({ id, x, y, false, false });
            }
        }
    }

    void setFruitSeed(unsigned int seed)
    {
        srand(seed);

        // Collect every empty tile that isn't the ghost house/start,
        // then pick one at random. Simple + reproducible given a seed.
        std::vector<std::pair<int,int>> candidates;
        for (int row = 0; row < kMapHeight; row++)
            for (int col = 0; col < kMapWidth; col++)
                if (Tile(col, row) == 0 && !IsGhostHouseOrStart(col, row))
                    candidates.push_back({ col, row });

        if (candidates.empty()) return;

        int pick = rand() % (int)candidates.size();
        g_fruitX = candidates[pick].first + 0.5f;
        g_fruitY = candidates[pick].second + 0.5f;

        if (g_fruitSprite >= 0) RCUT_Sprite_Remove(g_fruitSprite);
        g_fruitSprite = RCUT_Sprite_Add(g_fruitX, g_fruitY, g_fruitTex);
        g_fruitCollected = false;
    }

    // ------------------------------------------------------------
    void init()
    {
        if (!g_initialised)
        {
            // Internal raycaster resolution - independent of the
            // actual window size, we upscale when blitting.
            RCUT_Raycaster_Init(kRayW, kRayH);
            // RCUT_Input_Init() intentionally NOT called - removing
            // RCUT_Input_Update() didn't stop the cursor snapping back
            // to its position from when Init() ran, so Init() itself
            // is the remaining suspect (e.g. installing a persistent
            // hook/timer that resets the cursor independent of
            // whether Update() gets called each frame). Testing
            // whether IsKeyDown/IsSpecialKeyDown still work without
            // any RCUT input setup call at all.
            g_initialised = true;
        }

        RCUT_MapDesc mapDesc = { g_map, kMapWidth, kMapHeight };
        RCUT_Raycaster_SetMap(&mapDesc);
        RCUT_Raycaster_SetTeleportTile(2);

        RCUT_Sprite_RemoveAll();

        LoadTextures();
        SpawnGhosts();
        SpawnOrbs();
        setFruitSeed((unsigned int)time(nullptr));

        ResetPlayer();
        g_lives = 3;
        g_gameOver = false;

        g_mouseReady = false;
        g_mouseDeltaXAccum = 0.0f;
    }

    static void HandleInput(float dt)
    {
        // Mouse look uses g_mouseDeltaXAccum, filled by
        // PollAndRecentreMouse() (direct Win32 polling) once per
        // frame in update(). Keyboard uses g_keyState/g_specialLeft/
        // g_specialRight, filled by main.cpp's FreeGLUT keyboard
        // callbacks. RCUT's input system (Init/Update/IsKeyDown/etc)
        // is not used anywhere any more - RCUT_Input_Init() turned out
        // to hide the cursor and install something that kept fighting
        // our own mouse-look recentring, causing a runaway spin.

        if (g_specialLeft)  RCUT_Camera_Rotate(&g_cam, -kRotSpeed * dt);
        if (g_specialRight) RCUT_Camera_Rotate(&g_cam,  kRotSpeed * dt);

        RCUT_Camera_Rotate(&g_cam, g_mouseDeltaXAccum * kMouseSensitivity);
        g_mouseDeltaXAccum = 0.0f;

        float forward = 0.0f, strafe = 0.0f;
        if (g_keyState['w'] || g_keyState['W']) forward += 1.0f;
        if (g_keyState['s'] || g_keyState['S']) forward -= 1.0f;
        if (g_keyState['a'] || g_keyState['A']) strafe  += 1.0f;
        if (g_keyState['d'] || g_keyState['D']) strafe  -= 1.0f;

        if (forward != 0.0f || strafe != 0.0f)
            RCUT_Raycaster_TryMove(&g_cam, forward * kMoveSpeed * dt, strafe * kMoveSpeed * dt);
    }

    static float DistSq(float ax, float ay, float bx, float by)
    {
        float dx = ax - bx, dy = ay - by;
        return dx * dx + dy * dy;
    }

    static void HandleCollisions()
    {
        const float kPickupRadiusSq = 0.25f * 0.25f * 4.0f; // ~0.5 tile radius
        const float kGhostHitRadiusSq = 0.4f * 0.4f;

        for (auto& orb : g_orbs)
        {
            if (orb.collected) continue;
            if (DistSq(g_cam.x, g_cam.y, orb.x, orb.y) < kPickupRadiusSq)
            {
                orb.collected = true;
                RCUT_Sprite_Remove(orb.spriteId);
            }
        }

        if (!g_fruitCollected && DistSq(g_cam.x, g_cam.y, g_fruitX, g_fruitY) < kPickupRadiusSq)
        {
            g_fruitCollected = true;
            RCUT_Sprite_Remove(g_fruitSprite);
        }

        if (g_gameOver) return;

        for (auto& ghost : g_ghosts)
        {
            if (DistSq(g_cam.x, g_cam.y, ghost.x, ghost.y) < kGhostHitRadiusSq)
            {
                g_lives--;

                if (g_lives <= 0) { g_gameOver = true; }
                else               { ResetPlayer(); }
                break;
            }
        }
    }

    void update(float dt)
    {
        if (g_gameOver) return;
        HandleInput(dt);
        HandleCollisions();
        PollAndRecentreMouse();
    }

    static void DrawHUDText(int x, int y, const char* text)
    {
        glRasterPos2i(x, y);
        for (const char* c = text; *c; c++)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    void render()
    {
        RCUT_Raycaster_Render(&g_cam);
        const unsigned char* fb = RCUT_Raycaster_GetFramebuffer();
        int fbW = RCUT_Raycaster_GetWidth();
        int fbH = RCUT_Raycaster_GetHeight();

        int winW = glutGet(GLUT_WINDOW_WIDTH);
        int winH = glutGet(GLUT_WINDOW_HEIGHT);

        glPixelZoom((float)winW / fbW, (FLIP_FRAMEBUFFER_Y ? -1.0f : 1.0f) * (float)winH / fbH);
        glRasterPos2i(0, FLIP_FRAMEBUFFER_Y ? 0 : winH);
        // NOTE: with gluOrtho2D(0,w,h,0) already set in main.cpp's
        // reshape(), y=0 is the TOP of the window and y=winH is the
        // BOTTOM. glDrawPixels moves upward from the raster pos in
        // window space, so starting at winH draws upward to fill the
        // screen top-down correctly (assuming fb is top-down; flip
        // the flag above if it's the other way round).
        glDrawPixels(fbW, fbH, GL_RGB, GL_UNSIGNED_BYTE, fb);
        glPixelZoom(1.0f, 1.0f);

        char livesText[32];
        snprintf(livesText, sizeof(livesText), "LIVES: %d", g_lives);
        glColor3f(1.0f, 1.0f, 1.0f);
        DrawHUDText(10, 24, livesText);

        if (g_gameOver)
            DrawHUDText(winW / 2 - 40, winH / 2, "GAME OVER");
    }

    void shutdown()
    {
        if (g_initialised)
        {
            RCUT_Sprite_RemoveAll();
            RCUT_Textures_UnloadAll();
            RCUT_Raycaster_Shutdown();
            g_initialised = false;
        }
    }

    int  getLives()     { return g_lives; }
    bool isGameOver()   { return g_gameOver; }
}
