# This script will measure timing characteritics, how long it take to initialize card in different modes

from protocol import SdProtocol
from statistics import mean, median

tickFreq = 100 # kHz

sd = SdProtocol()

def measureFullInitTime():
    sd.resetTimer()

    sd.reset()
    sd.cmd0()
    cardver = sd.cmd8()
    if cardver == 1:
        sd.cmd0()

    status = "Busy"
    retries = 0
    while status == "Busy":
        status = sd.acmd41(cardver > 1)
        retries += 1
        assert(retries < 50)
    assert(status == "Valid")

    sdhc = sd.cmd58()

    sd.cmd2()
    sd.cmd3()

    duration = sd.getElapsedTime()
    return duration / tickFreq


def measureBusyTime():
    sd.reset()
    sd.cmd0()
    cardver = sd.cmd8()
    if cardver == 1:
        sd.cmd0()

    sd.resetTimer()

    status = "Busy"
    retries = 0
    while status == "Busy":
        status = sd.acmd41(cardver > 1)
        retries += 1
        assert(retries < 50)
    assert(status == "Valid")

    sdhc = sd.cmd58()

    duration = sd.getElapsedTime()

    sd.cmd2()
    sd.cmd3()

    return duration / tickFreq


def measureNumRetries():
    sd.reset()
    sd.cmd0()
    cardver = sd.cmd8()
    if cardver == 1:
        sd.cmd0()

    status = "Busy"
    retries = 0
    while status == "Busy":
        status = sd.acmd41(cardver > 1)
        retries += 1
        assert(retries < 50)
    assert(status == "Valid")

    sdhc = sd.cmd58()

    sd.cmd2()
    sd.cmd3()

    return retries


def measureNativeInitTime():
    resp = sd.sendCommand("INIT_CARD")
    return resp.tdelta / tickFreq


def measureNativeBusyTime():
    resp = sd.sendCommand("INIT_CARD")
    return int(resp.value.split(' ')[0]) / tickFreq


def measureNativeRetries():
    resp = sd.sendCommand("INIT_CARD")
    return int(resp.value.split(' ')[1])


def runMeasurement(headline, fn, count, log = None):
    values = [fn() for i in range(count) ]
    avgValue = median(values)
    minValue = min(values)
    maxValue = max(values)
    print(f"{headline}: {avgValue:.2f} ({minValue:.2f} - {maxValue:.2f})");

    if log is not None:
        log.write(f"{avgValue:.2f} ({minValue:.2f} - {maxValue:.2f})\n")
        log.flush()


count = 50
sd.setVerbose(False)

sd.init("SDIO", 118)
sd.reset()
sd.cmd0()

f = open("initTimeMeasurement.txt", "wt")

# SPI initialization does not work without SDIO (for some reason)
print("Run SDIO tests")
for prescaler in [238, 118, 46, 6, 2, 0]: # Run at 200 kHz, 400 kHz, 1MHz, 6 Mhz, 12 MHz, 24 MHz
    freq = 48000 / (prescaler + 2)
    print(f"Running SDIO at {freq} kHz")
    sd.init("SDIO", prescaler)
    sd.reset()
    sd.cmd0()

    # runMeasurement("Full init sequence, SDIO Mode", measureFullInitTime, count, f)
    # runMeasurement("Busy time, SDIO Mode", measureBusyTime, count, f)
    # runMeasurement("Number of retries, SDIO Mode", measureNumRetries, count, f)
    runMeasurement("Native init sequence, SDIO Mode", measureNativeInitTime, count, f)
    runMeasurement("Native Busy time, SDIO Mode", measureNativeBusyTime, count, f)
    runMeasurement("Native Number of retries, SDIO Mode", measureNativeRetries, count, f)

print("Run SPI tests")

for i in range(0, 8):
    div = 7-i
    freq = 24000 >> div
    print(f"Running SPI at {freq} kHz")

    sd.init("SPI", 2 << div)
    sd.reset()
    sd.cmd0()
    
    # runMeasurement("Full init sequence, SPI Mode", measureFullInitTime, count, f)
    # runMeasurement("Busy time, SPI Mode", measureBusyTime, count, f)
    # runMeasurement("Number of retries, SPI Mode", measureNumRetries, count, f)
    runMeasurement("Native init sequence, SPI Mode", measureNativeInitTime, count, f)
    runMeasurement("Native Busy time, SPI Mode", measureNativeBusyTime, count, f)
    runMeasurement("Native Number of retries, SPI Mode", measureNativeRetries, count, f)

