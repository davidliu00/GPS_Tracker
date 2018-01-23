#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <ctime>

using namespace std;

int logCoordinates (const char filename[], const double longitude, const double latitude){
//grabs time: obtained from reference @ http://www.cplusplus.com/reference/ctime/localtime/
        time_t rawtime;
        struct tm * timeinfo;
        time (&rawtime);
        timeinfo = localtime (&rawtime);

//Open file for appending
        ofstream outfile;
        outfile.open(filename, fstream::app);
        if(!outfile.is_open()) {
                return -1;
        }


//Append coordinates on a new line
        outfile << asctime(timeinfo) << "Longitude: " << longitude << "   Latitude: " << latitude <<  endl << endl;


        outfile.close();
        return 0;
}





int main(int argc, char * argv[]) {

        for(int i = 0; i < 5; i++) {
                logCoordinates("log.txt", 0, 0);
        }

        return 0;
}
