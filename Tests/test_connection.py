# To run these tests install pytest, then run this command line:
# py.test -rfeEsxXwa --verbose --showlocals

import pytest
import serial

class SD:
    def __init__(self):
        self.port = serial.Serial('COM6', baudrate=115200)


    def sendCommand(self, cmd):
        self.port.write((cmd + "\n").encode())

        while True:
            line = self.port.readline().decode().rstrip()
            print("Received: " + line)

            if not line:
                continue

            resp, argument = line.split(' ', 1) if ' ' in line else (line, None)

            if resp == "OK":
                return argument

            if resp == "ERROR":
                raise RuntimeError(argument)

    def init(self, mode, prescaler):
        self.sendCommand(mode + "_INIT " + str(prescaler))

    def reset(self):
        self.sendCommand("RESET")

    def cmd0(self):
        self.sendCommand("CMD0")
    
    def cmd8(self):
        return self.sendCommand("CMD8")

    def acmd41(self, hostSupportsSDHC):
        self.sendCommand("CMD55")
        return self.sendCommand("ACMD41 "+str(hostSupportsSDHC))



@pytest.fixture(scope="session", autouse=True)
def sd():
    yield SD()


def test_loopback(sd):
    sd.sendCommand("LOOPBACK")


def test_loopback_ok(sd):
    sd.sendCommand("LOOPBACK OK")


def test_loopback_error(sd):
    with pytest.raises(RuntimeError):
        sd.sendCommand("LOOPBACK ERROR")


def test_loopback_error_msg(sd):
    with pytest.raises(RuntimeError) as err:
        sd.sendCommand("LOOPBACK ERROR error message")

    assert(str(err.value) == "error message")


def test_sdio_init(sd):
    sd.init("SDIO", 256)
    sd.reset()    
    sd.cmd0()
    v2card = sd.cmd8()
    assert(v2card)

    status = "Busy"
    retries = 0
    while status == "Busy":
        status = sd.acmd41(v2card)
        retries += 1
        assert(retries < 10)
    assert(status == "Valid")

    sdhc = sd.sendCommand("CMD58")
    assert(sdhc == "SDHC")


def test_spi_init(sd):
    sd.init("SPI", 256)
    sd.reset()
    sd.sendCommand("CMD0")
    v2card = sd.sendCommand("CMD8")
    assert(v2card)

    sd.sendCommand("CMD58")

    status = "Busy"
    retries = 0
    while status == "Busy":
        status = sd.acmd41(v2card)
        retries += 1
        assert(retries < 10)
    assert(status == "Valid")

    sdhc = sd.sendCommand("CMD58")
    assert(sdhc == "SDHC")


