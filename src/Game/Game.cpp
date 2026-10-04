#include "Game.h"
#include "../../include/RCUT.h"
#include "../Pathfinding/Pathfinding.h"
#include "../Handler/TEMPSETS.h"

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
#include <algorithm>

namespace game
{    
    static const bool FLIP_FRAMEBUFFER_Y = false;

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
    
    static const int kRayW = 640;
    static const int kRayH = 360;
    
    static RCUT_Camera g_cam;
    static const float kPlayerStartX = 2.5f;
    static const float kPlayerStartY = 1.5f;
    static const float kMoveSpeed = 3.0f;
    static const float kRotSpeed = 2.0f;
    static const float kMouseLookScale = 0.003f;

    
    static int  g_lives = 3;
    static bool g_gameOver = false;

    enum class GhostState { SCATTER, CHASE, FRIGHTENED, EATEN };

    struct Ghost
    {
        RCUT_SpriteId spriteId;
        float x, y;                    
        int col, row;                  
        pathfinding::GridPos nextTile;  
        GhostState state;
        int texIdx;                     
                                         
                                         
        pathfinding::GridPos scatterCorner;
    };
    static std::vector<Ghost> g_ghosts;

    
    
    
    enum class GlobalMode { SCATTER, CHASE };
    static GlobalMode g_globalMode = GlobalMode::SCATTER;
    static float      g_modeTimer = 0.0f;
    static int         g_modeIndex = 0;

    
    
    
    
    static const float kModeSchedule[] = { 7.0f, 20.0f, 7.0f, 20.0f, 5.0f, 20.0f, 5.0f };

    
    
    
    
    static bool  g_frightenedActive = false;
    static float g_frightenedTimer = 0.0f;
    static const float kFrightenedDuration = 8.0f;

    
    
    struct Orb { RCUT_SpriteId spriteId; float x, y; bool big; bool collected; };
    static std::vector<Orb> g_orbs;

    
    static RCUT_SpriteId g_fruitSprite = -1;
    static float g_fruitX = 0, g_fruitY = 0;
    static bool  g_fruitCollected = true; 

    
    static RCUT_TextureId g_wallTex, g_floorTex, g_roofTex, g_doorTex;
    static RCUT_TextureId g_ghostTex[5]; 
    static RCUT_TextureId g_orbSmallTex, g_orbBigTex, g_fruitTex;

    static bool g_initialised = false;

    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    static bool  g_mouseReady = false;
    static int   g_mouseLastX = 0, g_mouseLastY = 0; 
    static float g_mouseDeltaXAccum = 0.0f;
    static float g_mouseDeltaYAccum = 0.0f;

    static void PollAndRecentreMouse()
    {
        #ifdef _WIN32
            POINT p;
            GetCursorPos(&p);

            if (g_mouseReady)
            {
                float dx = (float)(p.x - g_mouseLastX);
                float dy = (float)(p.y - g_mouseLastY);

                if (INVERT_MOUSE_X_SETTING) dx *= -1.0f;
                if (INVERT_MOUSE_Y_SETTING) dy *= -1.0f;

                g_mouseDeltaXAccum += dx;
                g_mouseDeltaYAccum += dy;
            }

            int winX = glutGet(GLUT_WINDOW_X);
            int winY = glutGet(GLUT_WINDOW_Y);
            int winW = glutGet(GLUT_WINDOW_WIDTH);
            int winH = glutGet(GLUT_WINDOW_HEIGHT);
            int centreX = winX + winW / 2;
            int centreY = winY + winH / 2;

            SetCursorPos(centreX, centreY);
            
            POINT afterWarp;
            GetCursorPos(&afterWarp);
            g_mouseLastX = afterWarp.x;
            g_mouseLastY = afterWarp.y;
            g_mouseReady = true;
        #endif
    }

    
    
    
    
    
    
    
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
        if (g_doorTex  >= 0) RCUT_Raycaster_SetWallTexture(2, g_doorTex); 
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
        
        
        
        
        struct { int col, row, texIdx; pathfinding::GridPos corner; } spawns[4] = {
            { 9, 13, 0, {19, 1} },   
            { 10, 13, 1, {1, 1} },   
            { 11, 13, 2, {19, 25} }, 
            { 10, 12, 3, {1, 25} },  
        };

        for (auto& s : spawns)
        {
            float x = s.col + 0.5f;
            float y = s.row + 0.5f;
            RCUT_SpriteId id = RCUT_Sprite_Add(x, y, g_ghostTex[s.texIdx]);

            Ghost ghost;
            ghost.spriteId = id;
            ghost.x = x;
            ghost.y = y;
            ghost.col = s.col;
            ghost.row = s.row;
            ghost.nextTile = { s.col, s.row }; 
            ghost.state = GhostState::SCATTER;
            ghost.texIdx = s.texIdx;
            ghost.scatterCorner = s.corner;

            g_ghosts.push_back(ghost);
        }
    }

    static bool IsGhostHouseOrStart(int col, int row)
    {
        
        
        if (row == 13 && col >= 9 && col <= 11) return true;
        if (row == 12 && col == 10) return true;
        if (col == (int)kPlayerStartX && row == (int)kPlayerStartY) return true;
        return false;
    }

    static void SpawnOrbs()
    {
        g_orbs.clear();

        
        int bigSpots[4][2] = { {1,1}, {19,1}, {1,25}, {19,25} };
        for (auto& p : bigSpots)
        {
            float x = p[0] + 0.5f, y = p[1] + 0.5f;
            RCUT_SpriteId id = RCUT_Sprite_Add(x, y, g_orbBigTex);
            g_orbs.push_back({ id, x, y, true, false });
        }

        
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

    
    
    

    static float DistSq(float ax, float ay, float bx, float by)
    {
        float dx = ax - bx, dy = ay - by;
        return dx * dx + dy * dy;
    }

    
    static bool GhostIsWalkable(int col, int row)
    {
        int t = Tile(col, row);
        return t == 0 || t == 2;
    }

    static pathfinding::GridPos PlayerTile()
    {
        return { (int)g_cam.x, (int)g_cam.y };
    }

    
    
    
    static pathfinding::GridPos DirToGridOffset(float dirX, float dirY)
    {
        if (fabsf(dirX) > fabsf(dirY))
            return { dirX > 0.0f ? 1 : -1, 0 };
        else
            return { 0, dirY > 0.0f ? 1 : -1 };
    }

    
    
    
    
    
    
    static pathfinding::GridPos ClampTargetToWalkable(pathfinding::GridPos target, pathfinding::GridPos fallback)
    {
        target.col = std::max(0, std::min(kMapWidth - 1, target.col));
        target.row = std::max(0, std::min(kMapHeight - 1, target.row));

        if (Tile(target.col, target.row) == 1)
            return fallback;

        return target;
    }

    
    
    
    
    static pathfinding::GridPos ComputeChaseTarget(Ghost& ghost)
    {
        pathfinding::GridPos playerTile = PlayerTile();

        switch (ghost.texIdx)
        {
            case 0: 
                return playerTile;

            case 1: 
            {
                pathfinding::GridPos offset = DirToGridOffset(g_cam.dirX, g_cam.dirY);
                pathfinding::GridPos target{ playerTile.col + offset.col * 4, playerTile.row + offset.row * 4 };
                return ClampTargetToWalkable(target, playerTile);
            }

            case 2: 
                    
                    
                    
            {
                if (g_ghosts.empty()) return playerTile;

                pathfinding::GridPos offset = DirToGridOffset(g_cam.dirX, g_cam.dirY);
                pathfinding::GridPos twoAhead{ playerTile.col + offset.col * 2, playerTile.row + offset.row * 2 };
                pathfinding::GridPos blinkyTile{ g_ghosts[0].col, g_ghosts[0].row };

                pathfinding::GridPos target{
                    blinkyTile.col + 2 * (twoAhead.col - blinkyTile.col),
                    blinkyTile.row + 2 * (twoAhead.row - blinkyTile.row)
                };
                return ClampTargetToWalkable(target, playerTile);
            }

            case 3: 
                    
            {
                const float kClydeShyDistSq = 8.0f * 8.0f;
                return (DistSq(ghost.x, ghost.y, g_cam.x, g_cam.y) > kClydeShyDistSq)
                    ? playerTile
                    : ghost.scatterCorner;
            }

            default:
                return playerTile;
        }
    }

    
    
    
    static pathfinding::GridPos ComputeFleeTarget(Ghost& ghost)
    {
        pathfinding::GridPos playerTile = PlayerTile();

        int dCol = ghost.col - playerTile.col;
        int dRow = ghost.row - playerTile.row;

        pathfinding::GridPos target{ ghost.col + dCol * 4, ghost.row + dRow * 4 };
        return ClampTargetToWalkable(target, { ghost.col, ghost.row });
    }

    
    
    
    static void SetGhostTexture(Ghost& ghost, RCUT_TextureId tex)
    {
        RCUT_Sprite_Remove(ghost.spriteId);
        ghost.spriteId = RCUT_Sprite_Add(ghost.x, ghost.y, tex);
    }

    
    
    
    
    static void UpdateGhostTarget(Ghost& ghost)
    {
        pathfinding::GridPos target;

        switch (ghost.state)
        {
            case GhostState::CHASE:      target = ComputeChaseTarget(ghost); break;
            case GhostState::SCATTER:    target = ghost.scatterCorner;       break;
            case GhostState::FRIGHTENED: target = ComputeFleeTarget(ghost);  break;
            case GhostState::EATEN:      target = { 10, 13 };                break; 
        }

        pathfinding::GridPos current{ ghost.col, ghost.row };
        ghost.nextTile = pathfinding::FindNextStepBFS(current, target, kMapWidth, kMapHeight, GhostIsWalkable);
    }

    
    
    
    
    
    
    static void UpdateGlobalMode(float dt)
    {
        if (g_frightenedActive) return; 

        g_modeTimer += dt;

        const int kScheduleLen = (int)(sizeof(kModeSchedule) / sizeof(kModeSchedule[0]));
        float duration = kModeSchedule[std::min(g_modeIndex, kScheduleLen - 1)];

        if (g_modeTimer < duration) return;

        g_modeTimer = 0.0f;
        g_modeIndex++;
        g_globalMode = (g_globalMode == GlobalMode::SCATTER) ? GlobalMode::CHASE : GlobalMode::SCATTER;

        for (auto& ghost : g_ghosts)
        {
            if (ghost.state != GhostState::SCATTER && ghost.state != GhostState::CHASE) continue;

            ghost.state = (g_globalMode == GlobalMode::SCATTER) ? GhostState::SCATTER : GhostState::CHASE;
            UpdateGhostTarget(ghost);
        }
    }

    
    
    
    static void StartFrightenedMode()
    {
        g_frightenedActive = true;
        g_frightenedTimer = kFrightenedDuration;

        for (auto& ghost : g_ghosts)
        {
            if (ghost.state == GhostState::EATEN) continue;

            ghost.state = GhostState::FRIGHTENED;
            SetGhostTexture(ghost, g_ghostTex[4]);
            UpdateGhostTarget(ghost);
        }
    }

    
    
    static void UpdateFrightenedTimer(float dt)
    {
        if (!g_frightenedActive) return;

        g_frightenedTimer -= dt;
        if (g_frightenedTimer > 0.0f) return;

        g_frightenedActive = false;

        for (auto& ghost : g_ghosts)
        {
            if (ghost.state != GhostState::FRIGHTENED) continue;

            ghost.state = (g_globalMode == GlobalMode::SCATTER) ? GhostState::SCATTER : GhostState::CHASE;
            SetGhostTexture(ghost, g_ghostTex[ghost.texIdx]);
            UpdateGhostTarget(ghost);
        }
    }

    static const float kGhostNormalSpeed = 2.0f;
    static const float kGhostFrightenedSpeed = 1.0f;
    static const float kGhostEatenSpeed = 4.0f;
    static const float kTileArriveEpsilon = 0.05f;

    
    
    
    static void UpdateGhost(Ghost& ghost, float dt)
    {
        if (ghost.state == GhostState::EATEN && ghost.col == 10 && ghost.row == 13)
        {
            ghost.state = (g_globalMode == GlobalMode::SCATTER) ? GhostState::SCATTER : GhostState::CHASE;
            SetGhostTexture(ghost, g_ghostTex[ghost.texIdx]);
            UpdateGhostTarget(ghost);
        }

        float speed = kGhostNormalSpeed;
        if (ghost.state == GhostState::FRIGHTENED) speed = kGhostFrightenedSpeed;
        if (ghost.state == GhostState::EATEN)      speed = kGhostEatenSpeed;

        float targetX = ghost.nextTile.col + 0.5f;
        float targetY = ghost.nextTile.row + 0.5f;

        float dx = targetX - ghost.x;
        float dy = targetY - ghost.y;
        float dist = sqrtf(dx * dx + dy * dy);

        if (dist < kTileArriveEpsilon)
        {
            ghost.x = targetX;
            ghost.y = targetY;
            ghost.col = ghost.nextTile.col;
            ghost.row = ghost.nextTile.row;
            UpdateGhostTarget(ghost);
        }
        else
        {
            float move = speed * dt;
            if (move > dist) move = dist;
            ghost.x += dx / dist * move;
            ghost.y += dy / dist * move;
        }

        RCUT_Sprite_SetPos(ghost.spriteId, ghost.x, ghost.y);
    }

    
    void init()
    {
        if (!g_initialised)
        {
            
            
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

        
        
        g_globalMode = GlobalMode::SCATTER;
        g_modeTimer = 0.0f;
        g_modeIndex = 0;
        g_frightenedActive = false;
        g_frightenedTimer = 0.0f;

        for (auto& ghost : g_ghosts)
            UpdateGhostTarget(ghost);

        ResetPlayer();
        g_lives = 3;
        g_gameOver = false;

        g_mouseReady = false;
        g_mouseDeltaXAccum = 0.0f;
    }
    
    static void HandleInput(float dt)
    {
        if (!USE_MOUSE_SETTING)
        {
            g_mouseDeltaXAccum = 0.0f;
            g_mouseDeltaYAccum = 0.0f;
        }

        if (g_specialLeft)  RCUT_Camera_Rotate(&g_cam, -kRotSpeed * dt);
        if (g_specialRight) RCUT_Camera_Rotate(&g_cam,  kRotSpeed * dt);

        if (USE_MOUSE_SETTING)
        {
            float mouseSensitivity = MOUSE_SENSITIVITY_SETTING > 0.0f ? MOUSE_SENSITIVITY_SETTING * kMouseLookScale : 0.0f;
            RCUT_Camera_Rotate(&g_cam, g_mouseDeltaXAccum * mouseSensitivity);
            g_mouseDeltaXAccum = 0.0f;

            if (fabsf(g_mouseDeltaYAccum) > 0.0f)
            {
                g_mouseDeltaYAccum = 0.0f;
            }
        }

        float forward = 0.0f, strafe = 0.0f;
        if (g_keyState['w'] || g_keyState['W']) forward += 1.0f;
        if (g_keyState['s'] || g_keyState['S']) forward -= 1.0f;
        if (g_keyState['a'] || g_keyState['A']) strafe  += 1.0f;
        if (g_keyState['d'] || g_keyState['D']) strafe  -= 1.0f;

        if (forward != 0.0f || strafe != 0.0f)
            RCUT_Raycaster_TryMove(&g_cam, forward * kMoveSpeed * dt, strafe * kMoveSpeed * dt);
    }

    static void HandleCollisions()
    {
        const float kPickupRadiusSq = 0.25f * 0.25f * 4.0f; 
        const float kGhostHitRadiusSq = 0.4f * 0.4f;

        for (auto& orb : g_orbs)
        {
            if (orb.collected) continue;
            if (DistSq(g_cam.x, g_cam.y, orb.x, orb.y) < kPickupRadiusSq)
            {
                orb.collected = true;
                RCUT_Sprite_Remove(orb.spriteId);

                if (orb.big) StartFrightenedMode();
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
            if (ghost.state == GhostState::EATEN) continue; 

            if (DistSq(g_cam.x, g_cam.y, ghost.x, ghost.y) < kGhostHitRadiusSq)
            {
                if (ghost.state == GhostState::FRIGHTENED)
                {
                    ghost.state = GhostState::EATEN;
                    UpdateGhostTarget(ghost);
                    printf("[Game] ate a frightened ghost\n");
                }
                else
                {
                    g_lives--;
                    printf("[Game] hit by ghost - lives left: %d\n", g_lives);

                    if (g_lives <= 0) { g_gameOver = true; }
                    else               { ResetPlayer(); }
                    break;
                }
            }
        }
    }

    void update(float dt)
    {
        if (g_gameOver) return;
        HandleInput(dt);

        UpdateGlobalMode(dt);
        UpdateFrightenedTimer(dt);
        for (auto& ghost : g_ghosts)
            UpdateGhost(ghost, dt);

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
        
        
        
        
        
        
        glDrawPixels(fbW, fbH, GL_RGB, GL_UNSIGNED_BYTE, fb);
        glPixelZoom(1.0f, 1.0f);

        char livesText[32];
        snprintf(livesText, sizeof(livesText), "LIVES: %d", g_lives);
        glColor3f(1.0f, 1.0f, 1.0f);
        DrawHUDText(10, 24, livesText);

        if (g_frightenedActive)
        {
            char frightText[48];
            snprintf(frightText, sizeof(frightText), "FRIGHTENED: %.1fs", g_frightenedTimer);
            glColor3f(0.4f, 0.6f, 1.0f);
            DrawHUDText(10, 48, frightText);
        }

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
