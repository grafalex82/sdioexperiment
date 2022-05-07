# This script dumps card meta information, stored in CID and CSD registers

from protocol import SdProtocol

sd = SdProtocol()

# SPI initialization does not work without SDIO (for some reason)
sd.init("SDIO", 178)
sd.reset()
sd.cmd0()

sd.init("SPI", 256)
sd.reset()
sd.cmd0()
cardver = sd.cmd8()

sd.cmd58() #TODO Check if this is really needed

status = "Busy"
retries = 0
while status == "Busy":
    status = sd.acmd41(cardver > 1)
    retries += 1
    assert(retries < 20)
assert(status == "Valid")

sdhc = sd.cmd58()

cid = sd.cmd2()
csd = sd.cmd9(0)


print(cid)
print(csd)

