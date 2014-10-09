#include <RecieverUI.h>

RecieverUI ui(7,8,9);

void setup()
{
	ui->setupDisplay( true, 7); //Maximum brightness
	ui->setDisplayToString("TEST");
}

void loop()
{

}

