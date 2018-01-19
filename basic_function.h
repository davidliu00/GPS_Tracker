// #include <ugpio/ugpio.h>
#include <math.h>

double degToRad(double deg);
double getDistance(double lat1, double long1, double lat2, double long2);
bool gpioSetup(const int gpio, const int io);
int LED(char color, int value, int blink);
void LEDAlert(int value);
bool isWireConnected(const int gpio);

//converts degrees to radians
double degToRad(double deg) {
        double rad = deg * (3.141592653 / 180);
        return rad;
}

//Haversine formula to calculate the distance between two latitude and longitudes: https://stackoverflow.com/a/10205532
double getDistance(double lat1, double long1, double lat2, double long2) {
        double earthRadiusKm = 6371.0;
        double lat1rad = degToRad(lat1);
        double long1rad = degToRad(long1);
        double lat2rad = degToRad(lat2);
        double long2rad = degToRad(long2);
        double u = sin((lat2rad - lat1rad) / 2);
        double v = sin((long2rad - long1rad) / 2);
        return 2.0 * earthRadiusKm * asin(sqrt(u * u + cos(lat1rad) * cos(lat2rad) * v * v));
}

//Quick gpio setup. Usage: gpio:<pinout number> io:<1 for input, 0 for output>
bool gpioSetup(const int gpio, const int io) {
        int isRequested;
        int returnValue;

        // CHECK IF GPIO IS ALREADY EXPORTED
        isRequested = gpio_is_requested(gpio);
        if (isRequested < 0) {
                return false; //error occured when checking if it was requested; return error
        }

        // EXPORT GPIO
        if (!isRequested) { //if its not already exported, export it
                returnValue = gpio_request(gpio, NULL);
                if (returnValue < 0) { //if request doesn't go through return error
                        return false;
                }
        }

        if (io == 1) { // SET GPIO TO INPUT
                gpio_direction_input(gpio);
        } else if (io == 0) { // SET GPIO TO OUTPUT
                gpio_direction_output(gpio, 0);
        }
        return true;
}

//Led function which controlls 2 leds
int LED(char color, int value, int blink) {
        if (blink == 0) {
                //green LED, switch on once
                if (color == 'g' || color == 'G') {
                        if (value == 0) {
                                gpio_set_value(ledG, 0);
                        } else if (value == 1) {
                                gpio_set_value(ledG, 1);
                        }
                }

                //red LED, switch on once
                if (color == 'r' || color == 'R') {
                        if (value == 0) {
                                gpio_set_value(ledR, 0);
                        } else if (value == 1) {
                                gpio_set_value(ledR, 1);
                        }
                }
        } else if (blink == 1) {
                //green LED, blink
                if (color == 'g' || color == 'G') {
                        if (value == 0) {
                                for (int x = 0; x < 4; x++) {
                                        gpio_set_value(ledG, 1);
                                        usleep(250000);
                                        gpio_set_value(ledG, 0);
                                        usleep(250000);
                                }
                        } else if (value == 1) {
                                for (int x = 0; x < 4; x++) {
                                        gpio_set_value(ledG, 1);
                                        usleep(250000);
                                        gpio_set_value(ledG, 0);
                                        usleep(250000);
                                }
                                gpio_set_value(ledG, 1);
                        } else {
                                //any other input for led
                                gpio_set_value(ledG, 0);
                        }
                }

                //red LED
                if (color == 'r' || color == 'R') {
                        if (value == 0) {
                                for (int x = 0; x < 4; x++) {
                                        gpio_set_value(ledR, 1);
                                        usleep(250000);
                                        gpio_set_value(ledR, 0);
                                        usleep(250000);
                                }

                        } else if (value == 1) {
                                for (int x = 0; x < 4; x++) {
                                        gpio_set_value(ledR, 1);
                                        usleep(250000);
                                        gpio_set_value(ledR, 0);
                                        usleep(250000);
                                }
                                gpio_set_value(ledR, 1);
                        }
                } else {
                        //any other input for led
                        gpio_set_value(ledR, 0);
                }
        }
        return 0;
}

//additional led function reserved for alerts
void LEDAlert(int value) {
        if (value == 1) {
                for (int x = 0; x < 10; x++) {
                        gpio_set_value(ledR, 1);
                        gpio_set_value(ledG, 1);
                        usleep(50000);
                        gpio_set_value(ledR, 0);
                        gpio_set_value(ledG, 0);
                        usleep(50000);
                }
        } else if (value == 0) {
                gpio_set_value(ledR, 0);
                gpio_set_value(ledG, 0);
        } else {
                gpio_set_value(ledR, 0);
                gpio_set_value(ledG, 0);
        }
}

//Checks the anti tamper device. returns true if no issue, false if the device has been disconnected
bool isWireConnected(const int gpio) {
        int input = gpio_get_value(gpio);
        if (input == 0) {
                return false;
        }
        return true;
}
