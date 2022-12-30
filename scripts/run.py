"""
Reads and records buffered data from a device
"""

import datetime
import textwrap
import pathlib
import struct
import random
import asyncio

from bleak import BleakClient

RXED = b""
TS = None

def run(ADDR):
  
  
    global RXED
    global TS
    
    def notification(A, B):
        global RXED
        print(" >  " + str(B))
        RXED += B
        
    async def main(ADDR):
        global RXED
        global TS
        
        async with BleakClient(ADDR, timeout=30.) as client:
            for service in client.services:
                print(service)
                print(service.description)
                print(service.uuid)
                if service.description.startswith("Nordic"):
                    for char in service.characteristics:
                        print(char)
                        print(char.description)
                        print(char.uuid)
                        print(char.service_handle)
                        print(char.properties)
                        print(char.handle)
                        
                        if str(char.uuid) == "6e400003-b5a3-f393-e0a9-e50e24dcca9e":    # Nordic UART service, read characteristic
                            TS = datetime.datetime.now()
                            
                            
                            # Turn on notifications, and wait for 10 seconds. The device
                            # should push any data it has during that time
                            
                            x = await client.start_notify(char, notification)
                            print(x)
                            
                            await asyncio.sleep(10.)
                            
                            x = await client.stop_notify(char)
                            print(x)
                            

    RXED = b""
    TS = None
    asyncio.run(main(ADDR))
    
    if TS is not None:
      
        TIMES = []
        TEMPS = []
        HUMS = []
        
        I = 0
        while I+8 <= len(RXED):
            TIMES.append(struct.unpack_from("<I", RXED, I+0)[0])
            TEMPS.append(struct.unpack_from(">H", RXED, I+4)[0])
            HUMS.append(struct.unpack_from(">H", RXED, I+6)[0])
            
            I += 8
        
        if len(TIMES) > 0:
            START_TIME = TS - datetime.timedelta(seconds = max(TIMES)) # Assumes the last time is time of reading. That could be off quite a bit
        
        ID = random.randint(0, 9999999)
        
        with open(pathlib.Path(__file__).parent.joinpath("Received_Data_{0}_{1:07d}.txt".format(  \
                    TS.strftime("%y%m%d%H%M%S"), \
                    ID   \
                )), "w") as f1:
            f1.write("Timestamp: {0}\n".format(TS.strftime("%d/%m/%Y %H:%M:%S")))
            f1.write("Data:\n")
            f1.write(textwrap.indent(textwrap.fill(RXED.hex(" ").upper(), 48), "    ") + "\n\n")
            f1.write("Decode: (times are approximate)\n")
            

            for N in range(len(TIMES)):
                f1.write("{0}".format((START_TIME + datetime.timedelta(seconds = TIMES[N])).strftime("%d/%m/%Y %H:%M:%S").ljust(26)))
                f1.write("{0:0.2f} degC".format( -45. + 175.*(float(TEMPS[N])/65535.)   ).ljust(24))
                f1.write("{0:0.1f} %RH".format( -6. + 125.*(float(HUMS[N])/65535.)   ).ljust(24) +  "\n")

        with open(pathlib.Path(__file__).parent.joinpath("COLLATED.csv"), "a") as f2:
            for N in range(len(TIMES)):
                f2.write("{0:d},{1},".format(ID, (START_TIME + datetime.timedelta(seconds = TIMES[N])).strftime("%d/%m/%Y %H:%M:%S")))
                f2.write("{0:f},degC,".format( -45. + 175.*(float(TEMPS[N])/65535.)   ))
                f2.write("{0:f},%RH,\n".format( -6. + 125.*(float(HUMS[N])/65535.)   ))




if __name__=="__main__":
    run("F2:9D:C9:B8:79:9D")

