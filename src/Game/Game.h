#pragma once










namespace game
{
    
    
    
    void init();

    
    
    void update(float dt);

    
    
    void render();

    
    void shutdown();

    int  getLives();
    bool isGameOver();

    
    
    
    void setFruitSeed(unsigned int seed);

    
    
    
    
    
    void onKeyDown(unsigned char key, int x, int y);
    void onKeyUp(unsigned char key, int x, int y);
    void onSpecialKeyDown(int key, int x, int y);
    void onSpecialKeyUp(int key, int x, int y);
}
