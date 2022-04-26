# To run these tests install pytest, then run this command line:
# py.test -rfeEsxXwa --verbose --showlocals

import pytest
import serial

class SD:
    def __init__(self):
        self.port = serial.Serial('COM6', baudrate=115200)


    def sendCommand(self, cmd):
        self.port.write((cmd + "\n").encode())

        response = ""
        while True:
            line = self.port.readline().decode().rstrip()
            print("Received: " + line)

            if not line:
                continue

            status, argument = line.split(' ', 1) if ' ' in line else (line, None)

            if status == "OK":
                return argument, response

            if status == "ERROR":
                raise RuntimeError(argument)

            response += line + "\n"


    def init(self, mode, prescaler):
        self.sendCommand(mode + "_INIT " + str(prescaler))

    def reset(self):
        self.sendCommand("RESET")

    def cmd0(self):
        self.sendCommand("CMD0")
    
    def cmd8(self):
        v2card, response = self.sendCommand("CMD8")
        assert("R7 = 000001aa" in response or "R1 = 05" in response)
        return int(v2card)

    def acmd41(self, hostSupportsSDHC):
        self.sendCommand("CMD55")
        status, _ = self.sendCommand("ACMD41 "+ ("1" if hostSupportsSDHC else "0"))
        return status

    def cmd58(self):
        sdhc, _ = self.sendCommand("CMD58")
        return sdhc

    def cmd2(self):
        _, resp = self.sendCommand("CMD2")

    def cmd3(self):
        status, resp = self.sendCommand("CMD3")
        assert("RCA=0001" in resp)
        return status


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
    sd.init("SDIO", 178)
    sd.reset()    
    sd.cmd0()
    v2card = sd.cmd8()
    assert(v2card)

    status = "Busy"
    retries = 0
    while status == "Busy":
        status = sd.acmd41(v2card > 1)
        retries += 1
        assert(retries < 10)
    assert(status == "Valid")

    sdhc = sd.cmd58()
    assert(sdhc == "SDHC")

    sd.cmd2()
    sd.cmd3()
    assert(False)


def test_spi_init(sd):
    sd.init("SPI", 256)
    sd.reset()
    sd.cmd0()
    v2card = sd.cmd8()
    assert(v2card)

    sd.cmd58() #TODO Check if this is really needed

    status = "Busy"
    retries = 0
    while status == "Busy":
        status = sd.acmd41(v2card > 1)
        retries += 1
        assert(retries < 10)
    assert(status == "Valid")

    sdhc = sd.cmd58()
    assert(sdhc == "SDHC" if v2card > 1 else "SDSC")

    sd.cmd2()
    assert(False)

