import bluetooth
import sys
import socket

bd_addr = "00:0D:19:03:AF:9F" #Arduino bluetooth MAC
port = 1

def wait_for_connection():
	rcv_count = 0
        
        sock = bluetooth.BluetoothSocket (bluetooth.RFCOMM)
	try:
            print("DEBUG: Waiting for connection...")
            sock.connect((bd_addr, port))

	    while True:
                print("DEBUG: Waiting for packet...")
	    	data = sock.recv(1024)
                print("DEBUG: Packet received.")
                rcv_count = rcv_count + 1

	except bluetooth.btcommon.BluetoothError as e:
            print("DEBUG:" + str(e))
            print("DEBUG: Session finished.")
            sock.shutdown(socket.SHUT_RDWR)
            sock.close()

	return rcv_count

while True:
	code = wait_for_connection()
        
        if code == 1:
            print('INFO: Postman arrived!')
        elif code == 2:
            print('INFO: Post read!')
        else:
            if code != 0: # Default behaviour
                print('WARN: Unknown code: ' + str(code))
