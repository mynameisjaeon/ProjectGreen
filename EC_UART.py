#!/usr/bin/python3
import time
import serial
from datetime import datetime

print("\n\n\n")
print("***************************************************************")
print("*******                                                 *******")
print("*******        EC Log UART comms via Bluetooth          *******")
print("*******             Modified by Jaeon Kim               *******")
print("*******                Original Code:                   *******")
print("*******    https://github.com/JetsonHacksNano/UARTDemo  *******")
print("*******                                                 *******")
print("***************************************************************")
print("\n\n\n\n")

# open a file for logging data
logfile = open('/home/jkim0231/myUART/EC_log.txt', 'a')

#configure serial comms. 8 bits number range
uart_port = serial.Serial(
        port="/dev/ttyTHS1",
        baudrate=9600,
        bytesize=serial.EIGHTBITS,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
)
# Wait a second to let the port initialize
time.sleep(1)


try:
    while True:
        if uart_port.inWaiting() > 0:
            # EC 
            size = uart_port.read(1)        # arduino will send no. of bytes
            intSize = int.from_bytes(size, "big") # byte -> int
            modifier = intSize                    # temporary var  
            num = 0                               
            for i in range(intSize+1):            # byte-int coversion for ppm  
                val = uart_port.read()
                intVal = int.from_bytes(val, "big")
                data = intVal*10**modifier
                modifier = modifier - 1
                num = num + data
            # Temperature
            temp = uart_port.read()         # get temp val and conv. to int
            intTemp = int.from_bytes(temp, "big")
            # Datetime
            now = datetime.now()
            time = now.strftime("%Y/%m/%d %H:%M:%S")

            print(time + '\tEC : ' + str(num)+ ' ppm\tTemp : ' + str(intTemp) + 'C')
            logfile.write(time + ',' + str(num) + ',' + str(intTemp) + '\n')

except KeyboardInterrupt:
    print("Exiting Program")

except Exception as exception_error:
    print("Exiting Program")
    

except Exception as exception_error:
    print("Error occurred. Exiting Program")
    print("Error: " + str(exception_error))

finally:
    logfile.close()
    uart_port.close()
    pass
