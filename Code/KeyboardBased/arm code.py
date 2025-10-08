import serial
import serial.tools.list_ports
import keyboard
import time

# Port setup
ports = serial.tools.list_ports.comports()
for port in ports:
    print(port.device, port.description)

PORT = input("Please enter the serial port: ")
BAUD = 115200

keymap = {
    'w': 'w',
    's': 's',
    'a': 'a',
    'd': 'd',
    'q': 'q',
    'e': 'e',
    'r': 'r',
    'f': 'f',
    'z': 'z',
    'x': 'x',
}

def main():
    try:
        ser = serial.Serial(PORT, BAUD, timeout=1)
        time.sleep(2)
        print("Connected to Arduino on", PORT)

        # Register press/release handlers
        for key, cmd in keymap.items():
            keyboard.on_press_key(key, lambda e, c=cmd: ser.write(c.encode()))
            keyboard.on_release_key(key, lambda e, c=cmd: ser.write(c.upper().encode()))

        print("Ready. Press ESC to quit.")
        keyboard.wait("esc")

    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()

if __name__ == "__main__":
    main()
