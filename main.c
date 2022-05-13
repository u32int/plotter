#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define WIDTH 600
#define HEIGHT 800

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

typedef uint8_t Color[3];
typedef Color Image[HEIGHT][WIDTH];

typedef struct Point {
    float x,y;
} Point;

bool debug = false;

void set_pixel_color(Image img, int px, int py, Color color) {
    memcpy(img[py][px], color, sizeof(char)*3);
}

void set_pixel_color_center(Image img, int px, int py, Color color) {
    set_pixel_color(img, px+HEIGHT/2, py+WIDTH/2, color);
}

void draw_rect(Image img, Color color, int lx, int ly, int x, int y) {
    for(int row = 0; row < ly; row++) {
	for(int col = 0; col < lx; col++) {
	    set_pixel_color(img, col+x, row+y, color);
	}
    }
}

void image_fill(Image img, Color color) {
    draw_rect(img,color,WIDTH,HEIGHT,0,0);
}

void draw_line(Image img, Color line_color, Point a, Point b) {
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
	set_pixel_color(img, round(curr.x), round(curr.y), line_color);
    }
}

void draw_quadratic(Image img, float a, float b, float c) {
    // Assuming the formula looks like this
    // f(x) = a(x*x) + bx + c
    assert(a != 0);
    a *= -1; b *= -1; c *= -1;
    Color line_color = { 255, 105, 55 };

    float p = -1*b/(2*a);
    float q = a*(p*p) + b*p + c;
    Point curr = { p+WIDTH/2, q+HEIGHT/2 }, prev = { 0, 0 };

    int x = p;
    while((curr.y < HEIGHT && curr.y > 0) && (curr.x < WIDTH && curr.x > 0)) {
	if (x > p) {
	    draw_line(img, line_color, prev, curr);
	    // mirror and draw the other side
	    prev.x = (prev.x*-1) + 2*p; curr.x = (curr.x*-1) + 2*p;
	    prev.y += 1; curr.y += 1;
	    draw_line(img, line_color, prev, curr);
	    prev.y -= 1; curr.y -= 1;
	    prev.x = (prev.x*-1) + 2*p; curr.x = (curr.x*-1) + 2*p;
	}
	prev = curr;
	curr.x = round(x + WIDTH/2);
	curr.y = round(a*(x*x) + b*x + c + HEIGHT/2);
	x++;
    }
}

void generate_plot_bg(Image img) {
    Color bg_color = { 40,40,40 };
    Color axis_color = { 90,90,90 };
    image_fill(img, bg_color);
    // draw axis
    Point left = { 0, HEIGHT/2 };
    Point right = { WIDTH, HEIGHT/2 };
    draw_line(img, axis_color, left, right);
    Point top = { WIDTH/2, 0 };
    Point bot = { WIDTH/2, HEIGHT };
    draw_line(img, axis_color, top, bot);
}

// https://en.wikipedia.org/wiki/Netpbm#File_formats
void save_as_ppm(Image img, const char* file_name) {
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
    Color (*out)[WIDTH] = malloc(sizeof(Image));

    generate_plot_bg(out);
    draw_quadratic(out, 1.0/50, 2, 0);
    save_as_ppm(out, "test.ppm");

    free(out);
}
