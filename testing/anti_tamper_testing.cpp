#include <iostream>
#include <stdlib.h>
#include <ugpio/ugpio.h>
#include <unistd.h>

using namespace std;

bool isWireConnected(int* gpio){
        int input = gpio_get_value(gpio);
        if(input == 0) {
                return false;
        }
        return true;
}


int main(int argc, char * argv[]) {

        int tamperDetectionGPIO = 11;

///////////////////////// GPIO SETUP /////////////////////////
        int isRequested;
        int returnValue;

        // CHECK IF GPIO IS ALREADY EXPORTED
        isRequested = gpio_is_requested(tamperDetectionGPIO);
        if (isRequested < 0) {
                return -1; //error occured when checking if it was requested; return error
        }

        // EXPORT GPIO
        if (!isRequested) { //if its not already exported, export it
                returnValue = gpio_request(tamperDetectionGPIO, NULL);
                if (returnValue < 0) { //if request doesn't go through return error
                        return -1;
                }
        }

        // SET GPIO TO INPUT
        gpio_direction_input(tamperDetectionGPIO);
/////////////////////////     /////////////////////////


///////////////////////// LOOP /////////////////////////
        while(true) {
                if(!isWireConnected(tamperDetectionGPIO)) {
                        cout << "Warning: wire device is disconnected" << endl;
                }
                sleep(1);
        }
/////////////////////////     /////////////////////////


        return 0;
}

























//
