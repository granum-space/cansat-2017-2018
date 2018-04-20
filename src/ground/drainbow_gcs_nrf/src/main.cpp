
#include <iostream>
#include <string>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

#include <RF24/RF24.h>


int main()
{
	RF24 radio1(22, BCM2835_SPI_CS0); //, BCM2835_SPI_SPEED_8MHZ);
	RF24 radio2(24, BCM2835_SPI_CS1); //, BCM2835_SPI_SPEED_8MHZ);

	radio1.begin();
	radio2.begin();

	radio1.openWritingPipe(0xAABBAABBAABBAABB);
	radio2.openWritingPipe(0xCCDDCCDDCCDDCCDD);

	radio1.printDetails();
	radio2.printDetails();

	std::cout << std::endl;
	std::cout << std::endl;
	//radio2.printDetails();

	return 0;
}

