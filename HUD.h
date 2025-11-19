#pragma once
#include <vector>
#include <string>

// Prepare HUD lines with control instructions
std::vector<std::string> prepareHUDLines();

// Draw Heads-Up Display (HUD) with controls information
void drawHUD(int boidCount, int obstacleCount, std::vector<std::string>& hudLines);

// Draw "PAUSED" text at the center of the screen
void drawPausedText();
