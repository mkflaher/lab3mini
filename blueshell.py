#Sean Flaherty
#ECE 3334-303
#BlueSMiRF to MSP430 command-line interface
#The commands in this program will control a servo motor and LED bar.
#For instance, the command "right 45" will instruct the MSP430 to move a servo 45 degrees to the right.

import bluetooth

def parse(cmd): #cmd is a valid array of strings
    #example: 'right 5' => 'aaaaa'
    char = chr(valid.index(cmd[0])+97) #determine a-z character to send via bt
    num = int(cmd[1])
    string = char * num + '\r'
    return string

valid = ["right", "left", "faster1", "slower1", "faster2", "slower2", "quit"] #list of valid commands

print("Setting up socket.")
sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)

addr = "00:06:66:DA:08:67" #BlueSMiRF MAC address
port = 1

#note: make sure to verify availability of modem in final product
print("Connecting...")
sock.connect((addr,port)) #connect to BlueSMiRF as client
#if it errors out here then the bluetooth modem is unavailable

print("Connected!")

while True: #shell
    command = input('bluesmirf> ')
    words = command.split() #separate by space to parse commands
    if words[0] == quit:
        break
    elif words[0] not in valid:
        print(words[0] + ": command not found")
    elif not words[1].isnumeric():
        print(words[0] + " must be followed by an integer value")
    else: #valid command
        txdata = parse(words)
        sock.send(txdata) #transmit command

sock.close() #done
