
#include "laserselector.h"
#include "utils.h"
#include "math.h"

void wait_click(int event, int x, int y, int flags, void* param);
void least_dist(laser_selector_ctx *ctx, int uPoint, int vPoint);

void to_image_space(laser_selector_ctx *ctx, double x, double y, int *u, int *v);
void to_laser_space(laser_selector_ctx *ctx, int u, int v, double *x, double *y);
void save_laser_data(laser_selector_ctx *ctx);

void init_laser_selector(laser_selector_ctx *ctx, playerc_laser_t *laser, char *filename)
{
    long i;
    int j;
    cvNamedWindow("Laser Draw", CV_WINDOW_AUTOSIZE);
    ctx->image = cvCreateImage(cvSize(800, 400), IPL_DEPTH_8U, 3);
    for(i = 0; i < ctx->image->width * ctx->image->height; i++) {
        ctx->image->imageData[3 * i] = ctx->image->imageData[3 * i + 1] = ctx->image->imageData[3 * i + 2] = 128;
    }
    ctx->laser = laser;
    ctx->last_index = -1;
    
    
    /* file handling */
    ctx->file = fopen(filename, "wt");

    /* Print Arff Header */
    fprintf(ctx->file, "@RELATION carimage\n\n");
    
    for(j = 0; j < ctx->laser->scan_count; j++)
        fprintf(ctx->file, "@ATTRIBUTE range_%d NUMERIC\n", j);
    fprintf(ctx->file, "@ATTRIBUTE car_pos NUMERIC\n");
    fprintf(ctx->file, "\n@DATA\n");
}

void free_laser_selector(laser_selector_ctx *ctx)
{
    cvReleaseImage(&(ctx->image));
    cvDestroyWindow("Laser Draw");
    fclose(ctx->file);
}

void save_laser_data(laser_selector_ctx *ctx)
{
    int i;
    for(i = 0; i < ctx->laser->scan_count;i++)
        fprintf(ctx->file, "%f,", ctx->laser->ranges[i]);

    fprintf(ctx->file, "%d\n", ctx->last_index);
    fflush(ctx->file);
}

/* coloca as coordenadas x e y do espaço do laser nas coordenadas u e v do espaço da imagem*/
void to_image_space(laser_selector_ctx *ctx, double x, double y, int *u, int *v)
{
    *u = (x * (ctx->image->width / 2)) / ctx->laser->max_range;
    *v = (y * ctx->image->height) / ctx->laser->max_range;
    *u = *u + (ctx->image->width / 2);
    *v = ctx->image->height - *v;
}

/* coloca as coordenadas x e y do espaço do laser nas coordenadas u e v do espaço da imagem*/
void to_laser_space(laser_selector_ctx *ctx, int u, int v, double *x, double *y)
{
    u = u - (ctx->image->width / 2.0);
    *x = (ctx->laser->max_range * (float) u) / ((float) ctx->image->width / 2.0);

    v = ctx->image->height - v;
    *y = (ctx->laser->max_range * (float) v) / (float) ctx->image->height;
}

void wait_click(int event, int x, int y, int flags, void* param)
{
    laser_selector_ctx *ctx = (laser_selector_ctx *) param;

    if(event == CV_EVENT_LBUTTONDOWN) {
        least_dist(ctx, x, y);
        save_laser_data(ctx);
    }
}


void least_dist(laser_selector_ctx *ctx, int uPoint, int vPoint)
{
    int i;
    double x, y;
    double xPoint, yPoint;
    float a, d, leastd;


    to_laser_space(ctx, uPoint, vPoint, &xPoint, &yPoint);
    
    ctx->last_index = -1;

    for(i = 1; i < ctx->laser->scan_count; i++) {
        calc_x_y(ctx->laser->scan_count, i, ctx->laser->ranges[i], &x, &y);
        // coloca o ponto final feixe do laser nas coordenadas da imagem

        a = y / x; //coeficiente angular
        d = fabs(a * xPoint - yPoint) / sqrt(a * a + 1); //distancia do ponto a reta
        

        if(ctx->last_index == -1 || d < leastd) {
            leastd = d;
            ctx->last_index = i;
        }
    }
}



void draw_laser_selector(laser_selector_ctx *ctx)
{
    int i;
    double x, y;
    int u, v;
    int originU, originV;
    
    
    cvSetMouseCallback("Laser Draw", wait_click, ctx);

    cvSet(ctx->image, CV_RGB(0, 0, 0), NULL);

        /* coloca a origem do laser nas coordenadas da imagem*/
    to_image_space(ctx, 0.0, 0.0, &originU, &originV);
    
    
    for(i = 0; i < ctx->laser->scan_count; i++) {
        calc_x_y(ctx->laser->scan_count, i, ctx->laser->ranges[i], &x, &y);
        /* coloca o ponto final feixe do laser nas coordenadas da imagem*/
        to_image_space(ctx, x, y, &u, &v);
        cvLine(ctx->image, cvPoint(originU, originV), cvPoint(u, v), CV_RGB(255, 0, 0), 1, CV_AA, 0); //feixe
    }
    
    if(ctx->last_index != -1) {
        calc_x_y(ctx->laser->scan_count, ctx->last_index, ctx->laser->ranges[ctx->last_index], &x, &y);
        /* coloca o ponto final feixe do laser nas coordenadas da imagem*/
        to_image_space(ctx, x, y, &u, &v);
        cvLine(ctx->image, cvPoint(originU, originV), cvPoint(u, v), CV_RGB(0, 0, 255), 1, CV_AA, 0); //feixe
    }
    
    cvCircle(ctx->image, cvPoint(originU, originV), 7, CV_RGB(0, 255, 0), -1, 8, 0);

    cvShowImage("Laser Draw", ctx->image);
}
