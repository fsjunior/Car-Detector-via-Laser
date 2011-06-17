/* 
 * File:   laserdraw.h
 * Author: linuxer
 *
 * Created on June 10, 2011, 8:36 PM
 */

#ifndef LASERDRAW_H
#define	LASERDRAW_H

#include <libplayerc/playerc.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#define LASER_MEAN_SIZE 10




typedef struct {
    playerc_laser_t *laser;
    IplImage *image;
    int last_index;
    FILE *file;
} laser_selector_ctx;

void init_laser_selector(laser_selector_ctx *ctx, playerc_laser_t *laser, char *filename);

void free_laser_selector(laser_selector_ctx *ctx);

void draw_laser_selector(laser_selector_ctx *ctx);


#endif	/* LASERDRAW_H */

