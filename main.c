/*
 * File:   main.c
 * Author: Francisco de Souza Júnior
 *
 * Created on March 29, 2011, 2:43 PM
 */

#include <stdio.h>
#include <libplayerc/playerc.h>
#include <string.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "laserselector.h"



int main(int argc, char *argv[])
{
    playerc_client_t *client;
    playerc_laser_t *laser;
    laser_selector_ctx laser_selector;
    char key_pressed;



    // Create a client and connect it to the server.
    client = playerc_client_create(NULL, "localhost", 6665);
    if(0 != playerc_client_connect(client))
        return -1;


    laser = playerc_laser_create(client, 0);
    if(playerc_laser_subscribe(laser, PLAYER_OPEN_MODE))
        return -2;
    
    playerc_client_read(client); //leitura inicial


    init_laser_selector(&laser_selector, laser, "test.arff");
    
    do {

        if(!playerc_client_read(client))
            return -1;

        draw_laser_selector(&laser_selector);

    } while((key_pressed = cvWaitKey(10)) != 27);
    
    free_laser_selector(&laser_selector);
    playerc_laser_unsubscribe(laser);
    playerc_laser_destroy(laser);
    playerc_client_disconnect(client);
    playerc_client_destroy(client);

    return 0;
}
