# This script will measure timing characteritics, how long it take to initialize card in different modes

from protocol import SdProtocol
from statistics import mean, median

tickFreq = 100 # kHz

sd = SdProtocol()

def measureFullInitTimeSDIO():
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


def measureBusyTimeSDIO():
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


def measureNumRetriesSDIO():
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


def measureBusyTimeSPI():
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
    
    return duration / tickFreq


def measureFullInitTimeSPI():
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
    duration = sd.getElapsedTime()

    sdhc = sd.cmd58()

    return duration / tickFreq


def measureNumRetriesSPI():
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

    return retries



def runMeasurement(headline, fn, count):
    values = [fn() for i in range(count) ]
    avgValue = median(values)
    minValue = min(values)
    maxValue = max(values)
    print(f"{headline}: {avgValue:.2f} ({minValue:.2f} - {maxValue:.2f})");


count = 20
sd.setVerbose(False)

# SPI initialization does not work without SDIO (for some reason)
print("Run SDIO tests")
for prescaler in [238, 118, 94, 46, 6, 2, 0]:
    freq = 48000 / (prescaler + 2)
    print(f"Running SDIO at {freq} kHz")
    sd.init("SDIO", prescaler)
    sd.reset()
    sd.cmd0()

    runMeasurement("Full init sequence, SDIO Mode", measureFullInitTimeSDIO, count)
    runMeasurement("Busy time, SDIO Mode", measureBusyTimeSDIO, count)
    runMeasurement("Number of retries, SDIO Mode", measureNumRetriesSDIO, count)
    runMeasurement("Native init sequence, SDIO Mode", measureNativeInitTime, count)
    runMeasurement("Native Busy time, SDIO Mode", measureNativeBusyTime, count)
    runMeasurement("Native Number of retries, SDIO Mode", measureNativeRetries, count)

print("Run SPI tests")

for i in range(1, 8):
    div = 7-i
    freq = 24000 >> div
    print(f"Running SPI at {freq} kHz")

    sd.init("SPI", 2 << div)
    sd.reset()
    sd.cmd0()
    
    runMeasurement("Full init sequence, SPI Mode", measureFullInitTimeSPI, count)
    runMeasurement("Busy time, SPI Mode", measureBusyTimeSPI, count)
    runMeasurement("Number of retries, SPI Mode", measureNumRetriesSPI, count)
    runMeasurement("Native init sequence, SPI Mode", measureNativeInitTime, count)
    runMeasurement("Native Busy time, SPI Mode", measureNativeBusyTime, count)
    runMeasurement("Native Number of retries, SPI Mode", measureNativeRetries, count)

