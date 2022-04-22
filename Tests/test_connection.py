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


def test_spi_init(sd):
    sd.sendCommand("SPI_INIT 256")
    sd.sendCommand("RESET")
    sd.sendCommand("CMD0")
    v2card = sd.sendCommand("CMD8")
    assert(v2card)

    sd.sendCommand("CMD58")

    status = "Busy"
    retries = 0
    while status == "Busy":
        sd.sendCommand("CMD55")
        status = sd.sendCommand("ACMD41 " + v2card)
        retries += 1
        assert(retries < 10)
    assert(status == "Valid")

    sdhc = sd.sendCommand("CMD58")
    assert(sdhc == "SDHC")


def test_sdio_init(sd):
    sd.sendCommand("SDIO_INIT 256")
    sd.sendCommand("RESET")
    sd.sendCommand("CMD0")
    v2card = sd.sendCommand("CMD8")
    assert(v2card)

    status = "Busy"
    retries = 0
    while status == "Busy":
        sd.sendCommand("CMD55")
        status = sd.sendCommand("ACMD41 " + v2card)
        retries += 1
        assert(retries < 10)
    assert(status == "Valid")

    sdhc = sd.sendCommand("CMD58")
    assert(sdhc == "SDHC")
