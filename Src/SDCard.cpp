#include "SDCard.h"

#include <stdio.h>

SDCard::SDCard()
{

}

bool SDCard::init()
{
	printf("Sending soft reset command (GO IDLE)\n");
	driver.cmd0_goIdleState();

	printf("Card initialized. Checking card version\n");
	if(driver.cmd8_sendInterfaceConditions())
		printf("v2.0 card\n");
	else
		printf("v1.0 card\n");

	return true;
}
