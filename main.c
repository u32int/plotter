#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

#define WIDTH 1200
#define HEIGHT 1200

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

typedef uint8_t Color[3];
typedef Color Image[WIDTH][HEIGHT];

typedef struct Point {
    float x,y;
} Point;

bool debug = false;

void set_pixel_color(Image img, int px, int py, Color color) {
    for(int i = 0; i < 3; i++) {
	img[py][px][i] = color[i];
    }
}

void set_pixel_color_center(Image img, int px, int py, Color color) {
    //if (debug) printf("set_pixel: %d, %d\n", px, py);
    set_pixel_color(img, px+HEIGHT/2, WIDTH/2-py, color);
}

void fill(Image img, Color color)
{
    for(int row = 0; row < HEIGHT; row++) {
	for(int col = 0; col < WIDTH; col++) {
	    set_pixel_color(img, row, col, color);
	}
    }
}

void line(Image img, Color line_color, Point a, Point b) {
    int x_distance = abs(a.x - b.x);
    int y_distance = abs(a.y - b.y);
    int distance = MAX(x_distance, y_distance);
    if (debug) printf("d: %d, %d\n", x_distance, y_distance);
    // this can probably be simplified a lot
    // generate points (line approximation) from a to b
    Point points[distance];
    int index = 0;
    if (x_distance > y_distance) {
	for(int x = MIN(a.x,b.x); x <= MAX(a.x, b.x); x++) {
	    points[index].x = x;
	    points[index].y = (a.y - b.y)/(a.x - b.x)*x + (a.y - (a.y - b.y)/(a.x - b.x) * a.x);
	    index++;
	}
    } else {
	for(int y = MIN(a.y,b.y); y <= MAX(a.y, b.y); y++) {
	    points[index].y = y;
	    points[index].x = (a.x - b.x)/(a.y - b.y)*y + (a.x - (a.x - b.x)/(a.y - b.y) * a.y);
	    index++;
	}
    }
    // draw line
    Point curr;
    for(int i = 0; i < distance; i++) {
	curr = points[i];
	set_pixel_color_center(img, round(curr.x), round(curr.y), line_color);
    }
}

void draw_parabola(Image img, float a, float b, float c) {
    // Assuming the formula looks like this
    // f(x) = a(x*x) + b*x + c
    assert(a != 0);
    Color line_color = { 255, 105, 55 };

    Point prev = {0,0}, curr = {0,0};
    curr.y = 0;
    int x = 1;
    while(curr.y < HEIGHT/2) {
	if (x > 1) {
	    if (debug)
		printf("drawing: (%f, %f) (%f, %f)\n", prev.x,prev.y,curr.x,curr.y);
	    line(img, line_color, curr, prev);
	    // mirror and draw the other side
	    curr.x *= -1;
	    prev.x *= -1;
	    line(img, line_color, curr, prev);
	    curr.x *= -1;
	    prev.x *= -1;
	}
	prev = curr;
	curr.x = x;
	curr.y = a*(x*x) + b*x + c;
	x++;
    }
}

void generate_plot_bg(Image img) {
    Color bg_color = { 40,40,40 };
    Color axis_color = { 90,90,90 };
    fill(img, bg_color);
    // draw axis
    Point left = { WIDTH/2*-1, 0 };
    Point right = { WIDTH/2-1, 0 };
    Point top = { 0, HEIGHT/2-1 };
    Point bot = { 0, HEIGHT/2*-1 };
    line(img, axis_color, left, right);
    line(img, axis_color, bot, top);
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

int main() {
    Image out; 
    generate_plot_bg(out);
    draw_parabola(out, 1.0f/50, 0, 0);
    save_as_ppm(out, "test.ppm");
}
