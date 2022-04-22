# To run these tests install pytest, then run this command line:
# py.test -rfeEsxXwa --verbose --showlocals

import pytest
import serial

@pytest.fixture(scope="session", autouse=True)
def board():
    yield serial.Serial('COM6', baudrate=115200)


def sendCommand(port, cmd):
    port.write((cmd + "\n").encode())

    while True:
        line = port.readline().decode().rstrip()
        print("Received: " + line)

        if not line:
            continue

        resp, argument = line.split(' ', 1) if ' ' in line else (line, None)

        if resp == "OK":
            return argument

        if resp == "ERROR":
            raise RuntimeError(argument)


def test_loopback(board):
    sendCommand(board, "LOOPBACK")


def test_loopback_ok(board):
    sendCommand(board, "LOOPBACK OK")


def test_loopback_error(board):
    with pytest.raises(RuntimeError):
        sendCommand(board, "LOOPBACK ERROR")


def test_loopback_error_msg(board):
    with pytest.raises(RuntimeError) as err:
        sendCommand(board, "LOOPBACK ERROR error message")

    assert(str(err.value) == "error message")


def test_spi_init(board):
    sendCommand(board, "SPI_INIT 256")
    sendCommand(board, "RESET")
    sendCommand(board, "CMD0")
    v2card = sendCommand(board, "CMD8")
    assert(v2card)

    sendCommand(board, "CMD58")

    status = "Busy"
    retries = 0
    while status == "Busy":
        sendCommand(board, "CMD55")
        status = sendCommand(board, "ACMD41 " + v2card)
        retries += 1
        assert(retries < 10)
    assert(status == "Valid")

    sdhc = sendCommand(board, "CMD58")
    assert(sdhc == "SDHC")


def test_sdio_init(board):
    sendCommand(board, "SDIO_INIT 256")
    sendCommand(board, "RESET")
    sendCommand(board, "CMD0")
    v2card = sendCommand(board, "CMD8")
    assert(v2card)

    status = "Busy"
    retries = 0
    while status == "Busy":
        sendCommand(board, "CMD55")
        status = sendCommand(board, "ACMD41 " + v2card)
        retries += 1
        assert(retries < 10)
    assert(status == "Valid")

    sdhc = sendCommand(board, "CMD58")
    assert(sdhc == "SDHC")
