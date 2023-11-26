#pragma once
#include "app\app.h"
#include "Vec3.h"

inline void DrawCircle(vec3 pos, float radius, vec3 col = { 1.0f, 1.0f, 1.0f })
{
	float sx = (radius / APP_VIRTUAL_WIDTH) * 2.0f;
	float sy = (radius / APP_VIRTUAL_HEIGHT) * 2.0f;
	float x = pos.x;
	float y = pos.y;
	APP_VIRTUAL_TO_NATIVE_COORDS(x, y);

	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glScalef(sx, sy, 1.0f);
	glColor3f(col.x, col.y, col.z);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 64; i++) {
		float angle = PI * 2.0f * (float)i / 64.0f;
		glVertex2f(cos(angle), sin(angle));
	}
	glEnd();
	glPopMatrix();
}
