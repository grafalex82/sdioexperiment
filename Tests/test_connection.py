# To run these tests install pytest, then run this command line:
# py.test -rfeEsxXwa --verbose --showlocals

import pytest
import serial
import re

STATUS_LINE_RE = re.compile(r"(OK|ERROR) ?(.*)")
TIMESTATS_LINE_RE = re.compile(r"TIMESTATS (\d+) (\d+) \((\d+)\)")

class SD:
    def __init__(self):
        self.port = serial.Serial('COM6', baudrate=115200)


    def sendCommand(self, cmd):
        self.port.write((cmd + "\n").encode())

        response = ""
        while True:
            line = self.port.readline().decode().rstrip()
            print("Received: " + line)

            # Wait and parse status line
            m = STATUS_LINE_RE.match(line)
            if not m:
                response += line + "\n"
                continue

            status = m.group(1)
            argument = m.group(2)

            # Wait for TIMESTATS
            line = self.port.readline().decode().rstrip()
            print("Received: " + line)
            m = TIMESTATS_LINE_RE.match(line)
            if not m:
                raise RuntimeError("Expecting TIMESTATS line")

            tstart = m.group(1)
            tend = m.group(2)
            tdelta = m.group(3)

            if status == "OK":
                return argument, response

            if status == "ERROR":
                raise RuntimeError(argument)



    def init(self, mode, prescaler):
        self.sendCommand(mode + "_INIT " + str(prescaler))

    def reset(self):
        self.sendCommand("RESET")

    def cmd0(self):
        self.sendCommand("CMD0")
    
    def cmd8(self):
        v2card, response = self.sendCommand("CMD8")
        assert("R7 = 000001aa" in response              # Normal R7 response
               or "R1 = 05" in response                 # CMD8 will return illegal command in SPI mode for v1 cards
               or "error code: 00000004" in response)   # CMD8 will timeout in SDIO mode for v1 cards
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
        return status

    def cmd7(self, rca):
        status, resp = self.sendCommand("CMD7 " + str(rca))

    def cmd9(self, rca):
        status, resp = self.sendCommand("CMD9 " + str(rca))

    def cmd10(self, rca):
        status, resp = self.sendCommand("CMD10 " + str(rca))


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
        assert(retries < 20)
    assert(status == "Valid")

    sdhc = sd.cmd58()

    sd.cmd2()
    rca = sd.cmd3()

    sd.cmd9(rca)
    sd.cmd10(rca)

#    sd.cmd7(rca)
#    sd.cmd7(0)


def test_spi_init(sd):
    # SPI initialization does not work without SDIO (for some reason)
    sd.init("SDIO", 178)
    sd.reset()
    sd.cmd0()

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
        assert(retries < 20)
    assert(status == "Valid")

    sdhc = sd.cmd58()

    sd.cmd2()
    sd.cmd9(0)

