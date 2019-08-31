#ifndef SDIODRIVER_H
#define SDIODRIVER_H


class SDIODriver
{
public:
	SDIODriver();

	void init();
	void cmd0_goIdleState();
	bool cmd8_sendInterfaceConditions();
	void cmd55_sendAppCommand();
	bool acmd41_sendAppOpConditions(bool hostSupportSdhc);
	bool cmd58_readCCS();

private:
};

#endif // SDIODRIVER_H
