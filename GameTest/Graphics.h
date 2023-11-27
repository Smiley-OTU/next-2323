#pragma once
#include "app\app.h"
#include "Math.h"

struct Color
{
	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
};

inline void DrawCircle(vec2 pos, float radius, Color color = {})
{
	float sx = (radius / APP_VIRTUAL_WIDTH) * 2.0f;
	float sy = (radius / APP_VIRTUAL_HEIGHT) * 2.0f;
	float x = pos.x;
	float y = pos.y;
	APP_VIRTUAL_TO_NATIVE_COORDS(x, y);

	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glScalef(sx, sy, 1.0f);
	glColor3f(color.r, color.g, color.b);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 64; i++) {
		float angle = TWO_PI * i / 64.0f;
		glVertex2f(cos(angle), sin(angle));
	}
	glEnd();
	glPopMatrix();
}

inline void DrawRectangle(float x, float y, float w, float h, float degrees = 0.0f, Color color = {})
{
	float sx = (1.0f / APP_VIRTUAL_WIDTH) * 2.0f;
	float sy = (1.0f / APP_VIRTUAL_HEIGHT) * 2.0f;
	float xMin = x, xMax = x + w, yMin = y, yMax = y + h;
	float cx = (xMax + xMin) * 0.5f;
	float cy = (yMax + yMin) * 0.5f;

	//float xMin = x, xMax = x + w, ;
	//
	//float y = pos.y;
	//APP_VIRTUAL_TO_NATIVE_COORDS(x, y);

	vec2 topLeft;
	vec2 topRight;
	vec2 botLeft;
	vec2 botRight;
}