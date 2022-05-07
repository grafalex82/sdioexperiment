# To run these tests install pytest, then run this command line:
# py.test -rfeEsxXwa --verbose --showlocals

import pytest

from protocol import SdProtocol

@pytest.fixture(scope="session", autouse=True)
def sd():
    yield SdProtocol()


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

