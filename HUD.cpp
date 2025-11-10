#include <string>
#include <vector>
#include <sstream>

#include "HUD.h"
#include "vecFunctions.h"

// Draw text on the screen at specified (x, y) position
static void drawText(const std::string& text)
{
	for (char c : text)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
}

// Draw Heads-Up Display (HUD) with controls information
void drawHUD(int boidCount)
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
	glOrtho(0, w, 0, h, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// HUD text colors
	auto textColor = Color::White;
	auto shadowColor = Color::Black;

	// HUD text lines
	const int margin = 10;
	const int lineHeight = 20;
	std::vector<std::string> hudLines;

	// Line with boid count
	std::ostringstream oss;
	oss << "Boid Count: " << boidCount;
	hudLines.push_back(oss.str());

	// Control instructions
	hudLines.push_back("");
	hudLines.push_back("Controles:");
	hudLines.push_back("Arrow Keys: Turn/Move Controlled Boid");
	hudLines.push_back("WASD: Move Controlled Boid");
	hudLines.push_back("+/-: Add/Remove Boids");
	hudLines.push_back("1: Follow Camera");
	hudLines.push_back("2: Fixed Camera");
	hudLines.push_back("3: Side Camera");
	hudLines.push_back("Z: Stop Controlled Boid");
	hudLines.push_back("Q/E: Increase/Decrease Height");
	hudLines.push_back("Mouse Wheel: Zoom In/Out");
	hudLines.push_back("F: Toggle Fullscreen");
	hudLines.push_back("Esc: Exit");

	// Reverse order for bottom-up drawing
	std::reverse(hudLines.begin(), hudLines.end());

	// Draw shadow
	glColor3d(shadowColor.x, shadowColor.y, shadowColor.z);
	for (int i = 0; i < hudLines.size(); ++i)
	{
		int x = margin, y = margin + i * lineHeight;

		// Draw shadow text
		glColor3d(shadowColor.x, shadowColor.y, shadowColor.z);
		glRasterPos2i(x + 1, y - 1);
		drawText(hudLines[i]);
		// Draw main text
		glColor3d(textColor.x, textColor.y, textColor.z);
		glRasterPos2i(x, y);
		drawText(hudLines[i]);
	}
	
	// Restore previous OpenGL state
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();
	glMatrixMode(GL_MODELVIEW);
}
