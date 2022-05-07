import serial
import re

STATUS_LINE_RE = re.compile(r"(OK|ERROR) ?(.*)")
TIMESTATS_LINE_RE = re.compile(r"TIMESTATS (\d+) (\d+) \((\d+)\)")

class CmdResponse:
    def __init__(self):
        self.status = ""
        self.value = ""
        self.log = ""
        self.tstart = 0
        self.tend = 0
        self.tdelta = 0


class SdProtocol:
    def __init__(self):
        self.port = serial.Serial('COM6', baudrate=115200)
        self.tstart = 0
        self.tend = 0
        self.verbose = False


    def sendCommand(self, cmd):
        self.port.write((cmd + "\n").encode())

        log = ""
        while True:
            line = self.port.readline().decode().rstrip()
            if self.verbose:
                print("Received: " + line)

            # Wait and parse status line
            m = STATUS_LINE_RE.match(line)
            if not m:
                log += line + "\n"
                continue

            # Fill the response object
            resp = CmdResponse()
            resp.status = m.group(1)
            resp.value = m.group(2)
            resp.log = log

            # Wait for TIMESTATS
            line = self.port.readline().decode().rstrip()
            if self.verbose:
                print("Received: " + line)
            m = TIMESTATS_LINE_RE.match(line)
            if not m:
                raise RuntimeError("Expecting TIMESTATS line")

            # Parse time stats data
            resp.tstart = int(m.group(1))
            resp.tend = int(m.group(2))
            resp.tdelta = int(m.group(3))

            # Update timing stats
            if self.tstart == 0:
                self.tstart = resp.tstart
            self.tend = resp.tend

            # Return the result
            if resp.status == "OK":
                return resp

            if resp.status == "ERROR":
                raise RuntimeError(resp.value)



    def init(self, mode, prescaler):
        self.sendCommand(mode + "_INIT " + str(prescaler))

    def reset(self):
        self.sendCommand("RESET")

    def cmd0(self):
        self.sendCommand("CMD0")
    
    def cmd8(self):
        response = self.sendCommand("CMD8")
        if self.verbose:
            assert("R7 = 000001aa" in response.log              # Normal R7 response
                   or "R1 = 05" in response.log                 # CMD8 will return illegal command in SPI mode for v1 cards
                   or "error code: 00000004" in response.log)   # CMD8 will timeout in SDIO mode for v1 cards

        # The value contains version of the card (1 or 2)
        v2card = response.value 
        return int(v2card)

    def acmd41(self, hostSupportsSDHC):
        self.sendCommand("CMD55")
        resp = self.sendCommand("ACMD41 "+ ("1" if hostSupportsSDHC else "0"))
        return resp.value   # The value contains whether the card is Busy or Valid

    def cmd58(self):
        resp = self.sendCommand("CMD58")
        return resp.value   # The value contains whether the card is SDHC or SDSC

    def cmd2(self):
        resp = self.sendCommand("CMD2")
        return resp.log     # The log contains a dump of CID register

    def cmd3(self):
        resp = self.sendCommand("CMD3")
        return resp.value   # Value contains RCA address

    def cmd7(self, rca):
        self.sendCommand("CMD7 " + str(rca))

    def cmd9(self, rca):
        resp = self.sendCommand("CMD9 " + str(rca))
        return resp.log     # The log contains a dump of CSD register

    def cmd10(self, rca):
        resp = self.sendCommand("CMD10 " + str(rca))
        return resp.log     # The log contains a dump of CID register

    def resetTimer(self):
        self.tstart = 0

    def getElapsedTime(self):
        return self.tend - self.tstart

    def setVerbose(self, verbose):
        self.verbose = verbose
        self.sendCommand("VERBOSE " + ("1" if verbose else "0"))
