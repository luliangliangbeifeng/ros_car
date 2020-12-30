import os
import serial
import select
import threading

def monitor_serial(ser, halt_fileno, ):

        ser_fileno = ser.fileno()
        readlist = [halt_fileno, ser_fileno]

        while ser.isOpen():
            readable, _, _ = select.select(readlist, [], [], 1)

            if halt_fileno in readable:
                ser.close()
                break
            if ser_fileno not in readable:
                continue        # Timeout.

            serial_line = None
            try:
                serial_line = ser.readline()
            except TypeError:
                pass
            except serial.serialutil.SerialException:
                ser.close()
                break

            # Just because ser_fileno has data doesn't mean an entire line
            # is available yet.
            if serial_line:
                sl = serial_line.decode('utf-8', 'ignore')
                print(sl, end="")

device = "/dev/ttyACM0"
ser = serial.Serial(
        device,
        baudrate=9600,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS,
        timeout=1
        )

ser.flush()
rpipe, wpipe = os.pipe()
t = threading.Thread(target=monitor_serial, daemon=True, args=(ser, rpipe))