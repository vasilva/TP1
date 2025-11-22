#include <GL/glut.h>
#include <sstream>
#include <algorithm>

#include "HUD.h"
#include "vecFunctions.h"

// Prepare HUD lines with control instructions
std::vector<std::string> prepareHUDLines()
{
	std::vector<std::string> hudLines;

	// Control instructions
	hudLines.push_back("");
	hudLines.push_back("Controls:");
	hudLines.push_back("Arrow Keys/WASD: Control Boid Leader");
	hudLines.push_back("+/-: Add/Remove Boids");
	hudLines.push_back("Z: Stop Boid Leader");
	hudLines.push_back("Q/E: Increase/Decrease Height");
	hudLines.push_back("O/P: Add/Remove Obstacle");
	hudLines.push_back("R: Reset Obstacles");
	hudLines.push_back("Mouse Wheel: Zoom In/Out");
	hudLines.push_back("1/2/3: Switch Camera (Follow/Fixed/Side)");
	hudLines.push_back("F: Toggle Fullscreen");
	hudLines.push_back("N: Toggle Fog");
	hudLines.push_back("Space: Pause/Unpause Simulation");
	hudLines.push_back("Esc: Exit");

	// Reverse order for bottom-up drawing
	std::reverse(hudLines.begin(), hudLines.end());
	return hudLines;
}

// Draw text on the screen at specified (x, y) position
static void drawText(const std::string& text)
{
	for (char c : text)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
}

// Draw Heads-Up Display (HUD) with controls information
void drawHUD(int boidCount, int obstacleCount, std::vector<std::string>& hudLines)
{
	// Save current OpenGL state
	glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT);

	// Disable lighting and depth test for HUD rendering
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	// Set up orthographic projection for 2D HUD
	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, w, 0.0, h, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// HUD text colors
	auto textColor = Color::White;
	auto shadowColor = Color::Black;

	// HUD text lines
	const int margin = 10;
	const int lineHeight = 20;

	// Add boid and obstacle counts to HUD lines
	std::stringstream oss;
	oss << "Obstacles: " << obstacleCount;
	hudLines.push_back(oss.str());
	
	oss.str("");
	oss << "Boids: " << boidCount;
	hudLines.push_back(oss.str());

	// Draw each HUD line
	int lineCount = static_cast<int>(hudLines.size());
	for (int i = 0; i < lineCount; ++i)
	{
		int x = margin, y = margin + i * lineHeight;

		// Draw shadow text
		glColor3f(shadowColor.x, shadowColor.y, shadowColor.z);
		glRasterPos2i(x + 1, y - 1);
		drawText(hudLines[i]);

		// Draw main text
		glColor3f(textColor.x, textColor.y, textColor.z);
		glRasterPos2i(x, y);
		drawText(hudLines[i]);
	}
	// Remove the last two lines (boid and obstacle counts) for next frame
	hudLines.pop_back();
	hudLines.pop_back();

	// Restore previous OpenGL state
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();
	glMatrixMode(GL_MODELVIEW);
}

// Draw "PAUSED" text at the center of the screen
void drawPausedText()
{
	// Save current OpenGL state
	glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	// Set up orthographic projection for 2D text
	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, w, 0.0, h, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// Draw "PAUSED" text at center
	std::string pauseText = "PAUSED";
	int x = w / 2 - (pauseText.length() * 9);
	int y = h / 2;
	
	// Draw shadow
	auto color = Color::Black;
	glColor3f(color.x, color.y, color.z);
	glRasterPos2i(x + 2, y - 2);
	drawText(pauseText);

	// Draw main text
	color = Color::Yellow;
	glColor3f(color.x, color.y, color.z);
	glRasterPos2i(x, y);
	drawText(pauseText);

	// Restore previous OpenGL state
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();
	glMatrixMode(GL_MODELVIEW);
}
