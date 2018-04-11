#pragma once
struct TEXBOUNDINGBOX
{
	bool is_repeat;
	bool is_repeat_x;
	bool is_repeat_y;

	float x_min;
	float x_max;
	float y_min;
	float y_max;

	TEXBOUNDINGBOX():is_repeat(false),is_repeat_x(0),is_repeat_y(0),x_min(0),x_max(1),y_min(1),y_max(1){};
};