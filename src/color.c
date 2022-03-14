#include "cluster.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

unsigned	rgb_to_int(unsigned r, unsigned g, unsigned b)
{
	return (r << 24 | g << 16 | b << 8 | 0xFF);
}

void rgb_to_hsv(float *r, float *g, float *b, float *h, float *s, float *v)
{
	float x_min, x_max, c;

	*v = fmaxf(*r, *g);
	*v = fmaxf(*v, *b);
	x_max = *v;
	x_min = fminf(*r, *g);
	x_min = fminf(x_min, *b);
	c = x_max - x_min;
	if (c == 0)
		*h = 0;
	else if (*v == *r)
		*h = 1.0 / 6 * (0 + (*g - *b) / c);
	else if (*v == *g)
		*h = 1.0 / 6 * (2 + (*b - *r) / c);
	else if (*v == *b)
		*h = 1.0 / 6 * (4 + (*r - *g) / c);
	if (*h < 0)
		*h += 1;
	if (*v == 0)
		*s = 0;
	else
		*s = c / *v;	
}

void hsv_to_rgb(float *h, float *s, float *v, float *r, float *g, float *b)
{
	float	h_priem;
	float 	m, x, c, r1 = 0, g1 = 0, b1 = 0;	

	h_priem = *h / (1.0 / 6);
	c = *v * *s;
	x = c * (1 - fabs(fmod(h_priem, 2.0) - 1));
	if (0 <= h_priem && h_priem < 1)
		r1 = c, g1 = x, b1 = 0;
	else if (1 <= h_priem && h_priem < 2)
		r1 = x, g1 = c, b1 = 0;
	else if (2 <= h_priem && h_priem < 3)
		r1 = 0, g1 = c, b1 = x;
	else if (3 <= h_priem && h_priem < 4)
		r1 = 0, g1 = x, b1 = c;
	else if (4 <= h_priem && h_priem < 5) 
		r1 = x, g1 = 0, b1 = c;
	else if (5 <= h_priem && h_priem < 6)
		r1 = c, g1 = 0, b1 = x;
	m = *v - c;
	*r = r1 + m;
	*g = g1 + m;
	*b = b1 + m;
}

void	split_rgb(unsigned base_color, float *r, float *g, float *b)
{
	*r = (base_color >> 24 & 0xFF) / 255.0;
	*g = (base_color >> 16 & 0xFF) / 255.0;
	*b = (base_color >> 8 & 0xFF) / 255.0;
}

unsigned	close_color_check(unsigned base_colors_p1, unsigned base_colors_p2)
{
	float	r, g, b;
	float	r1, g1, b1;
	float	r2, g2, b2;
	float 	threshold = 0.5;
	
	split_rgb(base_colors_p1, &r1, &g1, &b1);
	split_rgb(base_colors_p2, &r2, &g2, &b2);
	r = powf(r1 - r2, 2.0);
	g = powf(g1 - g2, 2.0);
	b = powf(b1 - b2, 2.0);
	if (sqrtf(r + g + b) > threshold)
		return (0);
	return (1);
}

unsigned	get_inverted_color(unsigned color)
{
	float r, g, b;
	float h, s, v;

	split_rgb(color, &r, &g, &b);
	rgb_to_hsv(&r, &g, &b, &h, &s, &v);
	h = fmod(h + 0.5, 1.0);
	hsv_to_rgb(&h, &s, &v, &r, &g, &b);
	return (rgb_to_int(r * 255, g * 255, b * 255));
}

void create_chip_colors(game_t *game, unsigned base_color_p1, unsigned base_color_p2)
{
	float	h_increase;
	float	r, g, b;
	float	h, s, v;

	if (close_color_check(base_color_p1, base_color_p2))
		base_color_p2 = get_inverted_color(base_color_p2);
	h_increase = 1.0 / 6.0 / (game->config->color_count / 1.66);
	split_rgb(base_color_p1, &r, &g, &b);
	rgb_to_hsv(&r, &g, &b, &h, &s, &v);
	h = fmod(h - h_increase * game->config->color_count / 4.0 + 1.0, 1.0);
	for (int i = 0; i < game->config->color_count; i += 1) {
		hsv_to_rgb(&h, &s, &v, &r, &g, &b);
		if (i == game->config->color_count / 2)
		{
			split_rgb(base_color_p2, &r, &g, &b);
			rgb_to_hsv(&r, &g, &b, &h, &s, &v);
			h = fmod(h - h_increase * game->config->color_count / 4 + 1.0, 1.0);
			hsv_to_rgb(&h, &s, &v, &r, &g, &b);
		}
		rgb_to_hsv(&r, &g, &b, &h, &s, &v);
		game->colors[i] = rgb_to_int(r * 255, g * 255, b * 255);
		h = fmod(h + h_increase, 1.0);
	}
}
