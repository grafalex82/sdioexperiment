#ifndef SDIODRIVER_H
#define SDIODRIVER_H


class SDIODriver
{
public:
	SDIODriver();

	void init();
	void cmd0_goIdleState();
	bool cmd8_sendInterfaceConditions();

private:
};

#endif // SDIODRIVER_H
