#pragma once
struct OFFSET
{
	bool is_rotate;
	float x_start;
	float y_start;
	float x_range;
	float y_range;
	OFFSET():is_rotate(false),x_start(0),y_start(0),x_range(1),y_range(1){};
};