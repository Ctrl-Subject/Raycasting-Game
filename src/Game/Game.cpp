#include "Game.h"
#include "../../include/RCUT.h"

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

        printf("[Game] textures: wall=%d door=%d floor=%d roof=%d orbS=%d orbB=%d fruit=%d\n",
               g_wallTex, g_doorTex, g_floorTex, g_roofTex, g_orbSmallTex, g_orbBigTex, g_fruitTex);
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

        printf("[Game] fruit seed=%u -> tile (%d,%d)\n", seed, candidates[pick].first, candidates[pick].second);
    }

    // ------------------------------------------------------------
    void init()
    {
        if (!g_initialised)
        {
            // Internal raycaster resolution - independent of the
            // actual window size, we upscale when blitting.
            RCUT_Raycaster_Init(kRayW, kRayH);
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
    }

    static void HandleInput(float dt)
    {
        if (RCUT_Input_IsSpecialKeyDown(RCUT_KEY_LEFT))  RCUT_Camera_Rotate(&g_cam, -kRotSpeed * dt);
        if (RCUT_Input_IsSpecialKeyDown(RCUT_KEY_RIGHT)) RCUT_Camera_Rotate(&g_cam,  kRotSpeed * dt);

        RCUT_Camera_Rotate(&g_cam, RCUT_Input_GetMouseDeltaX() * kMouseSensitivity);

        float forward = 0.0f, strafe = 0.0f;
        if (RCUT_Input_IsKeyDown('w') || RCUT_Input_IsKeyDown('W')) forward += 1.0f;
        if (RCUT_Input_IsKeyDown('s') || RCUT_Input_IsKeyDown('S')) forward -= 1.0f;
        if (RCUT_Input_IsKeyDown('a') || RCUT_Input_IsKeyDown('A')) strafe  += 1.0f;
        if (RCUT_Input_IsKeyDown('d') || RCUT_Input_IsKeyDown('D')) strafe  -= 1.0f;

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
                printf("[Game] hit by ghost - lives left: %d\n", g_lives);

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
