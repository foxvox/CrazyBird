#pragma once 
#include "Common.hpp"    
#include "SceneManager.hpp" 

int main() 
{
    std::srand((unsigned)std::time(nullptr));
    SceneManager sm;
    sm.run();
    return 0;
}