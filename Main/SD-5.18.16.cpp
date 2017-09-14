#include "mraa.hpp"
#include "es08a.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sstream>
#include <string>
#include <fstream>
#include <iostream>

#include <ctime>

using namespace std;
#define SSTR( x ) static_cast< std::ostringstream & >( \
		( std::ostringstream() << std::dec << x ) ).str()

int mmap(int, int, int, int, int);
void my_socket(int, int, int, int, int);
void readCalibration();
void Calibration();

int s0 = socket(AF_INET, SOCK_STREAM, 0);
char const *peerHost = "127.0.0.1";
short peerPort = 8589;
struct sockaddr_in peeraddr;

int pinky_val, ring_val, middle_val, index_val;
uint16_t pinky_servo, ring_servo, middle_servo, index_servo, thumb_servo;
int cali_val[10];
int pinky_low, pinky_high, ring_low, ring_high, middle_low, middle_high,
	index_low, index_high;

mraa::Aio* a0 = new mraa::Aio(0);
mraa::Aio* a1 = new mraa::Aio(1);
mraa::Aio* a2 = new mraa::Aio(2);
mraa::Aio* a3 = new mraa::Aio(3);

long systime = 0, systime2 = 0, tsystime = 0;
int main(){
	upm::ES08A *servo0 = new upm::ES08A(3);
	upm::ES08A *servo1 = new upm::ES08A(5);
	upm::ES08A *servo2 = new upm::ES08A(6);
	upm::ES08A *servo3 = new upm::ES08A(9);
	servo0->setAngle (180);
	servo1->setAngle (180);
	servo2->setAngle (180);
	servo3->setAngle (180);
    Calibration();
    readCalibration();

	int pinky_low = cali_val[0],   pinky_high = cali_val[1],
		ring_low = cali_val[2],    ring_high = cali_val[3],
		middle_low = cali_val[4],  middle_high = cali_val[5],
		index_low = cali_val[6],   index_high = cali_val[7],
		thumb_low = cali_val[8],   thumb_high = cali_val[9];
	memset(&peeraddr, 0, sizeof(peeraddr));
	struct hostent *host = gethostbyname(peerHost);
	peeraddr.sin_family = AF_INET;
	peeraddr.sin_port = htons(peerPort);
	memmove(&(peeraddr.sin_addr.s_addr), host->h_addr_list[0], 4);
	//connect(s0, (struct sockaddr*) &peeraddr, sizeof(peeraddr));
	int count = 0;
	int samples = 30;
    for (;;) {
    	//systime = time(0);

    	usleep(3000);
        pinky_val += a0->read();
        ring_val += a1->read();
        middle_val += a2->read();
        index_val += a3->read();

        //my_socket(pinky_val,ring_val,middle_val,index_val,0);
        //printf("This is the min: %d, max: %d, value: %d\n", ring_low, ring_high, ring_val);
		if(count == samples){
			pinky_val = (((pinky_val+5)/10) * 10)/samples;
			ring_val = (((ring_val+5)/10) * 10)/samples;
			middle_val = (((middle_val+5)/10) * 10)/samples;
			index_val = (((index_val+5)/10) * 10)/samples;
			if((pinky_low) < pinky_val && pinky_val < (pinky_high + 1)){
				pinky_servo = mmap(pinky_val, pinky_low, pinky_high, 180, 0);
				servo0->setAngle (pinky_servo);
			}
			if((ring_low) < ring_val && ring_val < (ring_high + 1)){
				ring_servo = mmap(ring_val, ring_low, ring_high, 180, 0);
				//printf("Ring Servo: %d\n",ring_servo);
				servo1->setAngle (ring_servo);
			}

			if((middle_low) < middle_val && middle_val < (middle_high + 1)){
				middle_servo = mmap(middle_val, middle_low, middle_high, 180, 0);
				servo2->setAngle (middle_servo);
			}
			if((index_low) < index_val && index_val < (index_high + 1)){
				index_servo = mmap(index_val, index_low, index_high, 180, 0);
				servo3->setAngle (index_servo);
			}
			//systime2 = time(0);
			//tsystime = systime2 - systime;
			//printf("Looptime in us: %L\n", systime);
			count = 0;
		}
		count++;
    }
    close(s0);
    return MRAA_SUCCESS;
}

int mmap(int x, int in_min, int in_max, int out_min, int out_max){
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void readCalibration(){
	ifstream file("/home/root/calibration.txt");
	if(file.is_open()){
		for(int i = 0; i < 10; i++){
			file >> cali_val[i];
		}
	}
	file.close();
}
void Calibration(){
	int tmparray[10] = {0,0,0,0,0,0,0,0,0,0};
	int count;
	int tmpvar = 0;
	int samples = 1500;
	//calibration process
	printf("Order of calibration: Pinky, Ring, Middle, Index, Thumb\n");
	printf("Calibrate each finger twice: Baseline, then Max\n");
	printf("Calibration process will start in 3 seconds\n");
	printf("\n");
	sleep(3);

	printf("Calibrating Baseline for Pinky\n");
	for(count = 0; count < samples; count++){
		tmpvar += a0->read();
		usleep(5000);
	}
	tmparray[0] = tmpvar/samples;
	tmpvar = 0;
	printf("Calibrating Max for Pinky\n");
	for(count = 0; count < samples; count++){
		tmpvar += a0->read();
		usleep(5000);
	}

	tmparray[1] = tmpvar/samples;
	tmpvar = 0;

	printf("Calibrating Baseline for Ring\n");
	for(count = 0; count < samples; count++){
		tmpvar += a1->read();
		usleep(5000);
	}
	tmparray[2] = tmpvar/samples;
	tmpvar = 0;
	printf("Calibrating Max for Ring\n");
	for(count = 0; count < samples; count++){
		tmpvar += a1->read();
		usleep(5000);
	}
	tmparray[3] = tmpvar/samples;
	tmpvar = 0;

	printf("Calibrating Baseline for Middle/Index\n");
	for(count = 0; count < samples; count++){
		tmpvar += a2->read();
		usleep(5000);
	}
	tmparray[4] = tmpvar/samples;
	tmpvar = 0;
	printf("Calibrating Max for Middle/Index\n");
	for(count = 0; count < samples; count++){
		tmpvar += a2->read();
		usleep(5000);
	}
	tmparray[5] = tmpvar/samples;
	tmpvar = 0;

	printf("Calibrating Baseline for Thumb\n");
	for(count = 0; count < samples; count++){
		tmpvar += a3->read();
		usleep(5000);
	}
	tmparray[6] = tmpvar/samples;
	tmpvar = 0;
	printf("Calibrating Max for Thumb\n");
	for(count = 0; count < samples; count++){
		tmpvar += a3->read();
		usleep(5000);
	}
	tmparray[7] = tmpvar/samples;
	tmpvar = 0;

	//end of calibration process
	ofstream file("/home/root/calibration.txt");
	if(file.is_open()){
		for(int i = 0; i < 10; i++){
			file << tmparray[i];
			file << " ";
		}
	}
	file.close();
	printf("Calibration Finished...\n");
}

void my_socket(int sval1, int sval2, int sval3, int sval4, int sval5){
	const void *data;
	std::string sensor1 = SSTR( sval1 );
	std::string sensor2 = SSTR( sval2 );
	std::string sensor3 = SSTR( sval3 );
	std::string sensor4 = SSTR( sval4 );
	std::string sensor5 = SSTR( sval5 );
	std::string my_string = sensor1 + "," + sensor2 + "," + sensor3 + "," + sensor4 + "," + sensor5;
	data = my_string.c_str();
	write(s0, data, 20);
}
