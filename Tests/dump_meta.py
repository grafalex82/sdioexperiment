# This script dumps card meta information, stored in CID and CSD registers

from protocol import SdProtocol

sd = SdProtocol()

sd.setVerbose(True)

# SPI initialization does not work without SDIO (for some reason)
sd.init("SDIO", 178)
sd.reset()
sd.cmd0()

# sd.init("SPI", 256)
# sd.reset()
# sd.cmd0()
cardver = sd.cmd8()
if cardver == 1:
    sd.cmd0();

status = "Busy"
retries = 0
while status == "Busy":
    status = sd.acmd41(cardver > 1)
    retries += 1
    assert(retries < 20)
assert(status == "Valid")

sdhc = sd.cmd58()

cid = sd.cmd2()
rca = sd.cmd3()
csd = sd.cmd9(rca)


print(cid)
print(csd)

