#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

typedef uint8_t Color[3];
typedef Color Image[HEIGHT][WIDTH];


void set_pixel_color(Image img, int px, int py, Color color)
{
    memcpy(img[py][px], color, sizeof(Color));
}

void set_pixel_color_center(Image img, int px, int py, Color color)
{
    set_pixel_color(img, px+WIDTH/2, HEIGHT/2-py, color);
}

void draw_rect(Image img, Color color, int lx, int ly, int x, int y)
{
    for(int row = 0; row < ly; row++) {
	for(int col = 0; col < lx; col++) {
	    set_pixel_color(img, col+x, row+y, color);
	}
    }
}

void image_fill(Image img, Color color)
{
    draw_rect(img,color,WIDTH,HEIGHT,0,0);
}

void draw_line(Image img, Color c, float ax, float ay, float bx, float by)
{
    int dx = abs(ax - bx);
    int dy = abs(ay - by);
    int index = 0;
    if (dx > dy) {
	for(int x = MIN(ax,bx); x <= MAX(ax, bx); x++) {
	    float y = (ay - by)/(ax - bx)*x + (ay - (ay - by)/(ax - bx) * ax);
	    set_pixel_color_center(img, x, (int)y, c);
	    index++;
	}
    } else {
	for(int y = MIN(ay,by); y <= MAX(ay, by); y++) {
	    float x = (ax - bx)/(ay - by)*y + (ax - (ax - bx)/(ay - by) * ay);
	    set_pixel_color_center(img, (int)x, y, c);
	    index++;
	}
    }
}


void draw_eq(Image img)
{
    Color c = { 255, 105, 55 };
    float lx, ly = 0.0f;
    for(float x = -WIDTH/2; x < WIDTH/2; ++x) {
	float y = f(x/SCALE)*SCALE;
	if (y > -HEIGHT/2 && y < HEIGHT/2 &&
	    ly > -HEIGHT/2 && ly < HEIGHT/2 &&
	    x > -WIDTH/2)
	{
	    draw_line(img, c, lx, ly, x, y);
	}
	lx = x; ly = y;
    } 
}

#define PI 3.14159265358979323846

void draw_circle(Image img, float cx, float cy, float r)
{
    Color c = { 146, 255, 121 };
    assert(r > 0);

    int prev = 0;
    float precision = 1;
    for(float x = -r; x <= r; x += precision) {
	int y = sqrt(r*r - x*x);
	if (x > -r) {
	    draw_line(img, c, (int)(x-precision)+cx, prev+cy, (int)x+cx, y+cy);
	    draw_line(img, c, (int)(x-precision)+cx, -prev+cy, (int)x+cx, -y+cy);
	}
	prev = y;
    }
}

void generate_plot_bg(Image img) {
    Color bg_color = { 40,40,40 };
    Color axis_color = { 90,90,90 };
    image_fill(img, bg_color);
    // left->right
    draw_line(img, axis_color, -WIDTH/2,0, WIDTH/2,0);
    // top->bottom
    draw_line(img, axis_color, 0,HEIGHT/2, 0,-HEIGHT/2);
}

// https://en.wikipedia.org/wiki/Netpbm#File_formats
void save_as_ppm(Image img, const char* file_name)
{
    FILE *file = fopen(file_name, "wb");
    assert(file != NULL);
    // ppm header
    fprintf(file, "P6\n%d %d 255\n", WIDTH, HEIGHT);
    // write color data
    for(int row = 0; row < HEIGHT; row++)
	for(int col = 0; col < WIDTH; col++)
	    fwrite(img[row][col], sizeof(Color), 1, file);
    fclose(file);
}

void print_color(Color c)
{
    printf("[%d, %d, %d]", c[0], c[1], c[2]);
}

void print_img_data(Image img)
{
    for (int row = 0; row < HEIGHT; row++) {
	printf("(%d)", row);
	for(int col = 0; col < WIDTH; col++) {
	    print_color(img[row][col]);
	    printf(", ");
	}
	printf("\n");
    }
}

int main()
{
    printf("WIDTH: %d\nHEIGHT: %d\n", WIDTH, HEIGHT);
    Color (*img)[WIDTH] = malloc(sizeof(Image));
    generate_plot_bg(img);
    draw_eq(img);
    save_as_ppm(img, "test.ppm");

    free(img);
}
