int ledR = 18;
int ledG = 19;
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "stats_logging.h"
#include "basic_function.h"

using namespace std;


//Generates a random latitude within a given radius: https://gis.stackexchange.com/a/25883
double randLat(double centerLat, double r) {
        double centerLatRad = degToRad(centerLat);
        //Generate two random values between 0 and 1
        double u = ((double) rand() / (RAND_MAX));
        double v = ((double) rand() / (RAND_MAX));
        //Convert radius from degrees to km
        r = r / 111.300;
        double w = r * sqrt(u);
        double t = 2.0 * 3.141592653 * v;
        double x = w * cos(t);
        double y = w * sin(t);

        //Adjust the x-coordinate for the shrinking of the east-west distances:
        x = x / cos(centerLatRad);
        double newLat = y + centerLat;
        return newLat;
}

//Generates a random longitude within a given radius: https://gis.stackexchange.com/a/25883
double randLong(double centerLong, double r) {
        //Generate two random values between 0 and 1
        double u = ((double) rand() / (RAND_MAX));
        double v = ((double) rand() / (RAND_MAX));
        //Convert radius from degrees to km
        r = r / 111.300;
        double w = r * sqrt(u);
        double t = 2.0 * 3.141592653 * v;
        double x = w * cos(t);
        double y = w * sin(t);

        double newLong = x + centerLong;
        return newLong;
}

//misc
double moveLat(double curLat, double endLat) {
        if (endLat > curLat) {
                curLat += 0.00001;
                return curLat;
        }
        if (endLat < curLat) {
                curLat -= 0.00001;
                return curLat;
        } else {
                return endLat;
        }
}

//misc
double moveLong(double curLong, double endLong) {
        if (endLong > curLong) {
                curLong += 0.00001;
                return curLong;
        }
        if (endLong < curLong) {
                curLong -= 0.00001;
                return curLong;
        } else {
                return endLong;
        }
}

int main() {
///////////////////////// GPIO SETUP /////////////////////////
        //led gpios
        gpioSetup(ledR, 0);
        gpioSetup(ledG, 0);

        //wire gpios
        int tamperDetectionGPIO = 11;
        gpioSetup(tamperDetectionGPIO, 1);


///////////////////////// VARIABLE SETUP /////////////////////////
        double curLat = 0;
        double curLong = 0;
        bool tamper = false;

///////////////////////// LOOP /////////////////////////
        bool done = false;
        while (!done) {
                sleep(2);

                curLat = randLat(43.471084, 0.100);
                curLong = randLong(-80.539157, 0.100);
                double distance = getDistance(43.471084, -80.539157, curLat, curLong);


                //If wire is tampered with, LEDAlert
                if (!isWireConnected(tamperDetectionGPIO)) {
                        tamper = true;
                        done = true;

                } else if (distance > 0.070) { //If person is past 70m from the starting point, LEDAlert
                        logCoordinates("log.txt", curLong, curLat, "WARNING, OUT OF BOUNDRY");
                        trackingStats("log.txt", "stats.txt");
                        LEDAlert(1);

                } else if ((distance < 0.070) && (distance >= 0.065)) { //If person is within the caution zone (5m of the 70m fence), blink red LED
                        logCoordinates("log.txt", curLong, curLat, "WARNING, APPROACHING BOUNDRY");
                        trackingStats("log.txt", "stats.txt");
                        LED('R', 0, 1);

                } else if (distance < 0.065) {  //If person is within the safe zone (less than 65m from the starting point), blink green LED
                        logCoordinates("log.txt", curLong, curLat, "Normal");
                        trackingStats("log.txt", "stats.txt");
                        LED('G', 0, 1);
                }

        }



///////////////////////// Wire Broken Loop /////////////////////////
        while (tamper) {
                logCoordinates("log.txt", curLong, curLat, "WARNING, DISCONNECTED");
                trackingStats("log.txt", "stats.txt");
                LEDAlert(1);
        }



        return 0;
}
