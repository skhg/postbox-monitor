import bluetooth
import sys
import socket
import logging

bd_addr = "00:0D:19:03:AF:9F" #Arduino bluetooth MAC
port = 1

logging.basicConfig(level=logging.DEBUG)

logging.info("Starting up!")

def wait_for_connection():
	rcv_count = -1
        
        sock = bluetooth.BluetoothSocket (bluetooth.RFCOMM)
	try:
            logging.debug("Waiting for connection...")
            sock.connect((bd_addr, port))
            rcv_count = 0

	    while True:
                logging.debug("Waiting for packet...")
	    	data = sock.recv(1024)
                logging.debug("Packet received.")
                rcv_count = rcv_count + 1

	except bluetooth.btcommon.BluetoothError as e:
            logging.debug(str(e))
            logging.debug("Session finished.")
            sock.shutdown(socket.SHUT_RDWR)
            sock.close()

	return rcv_count

while True:
	code = wait_for_connection()
        
        if code == 1:
            logging.info('Postman arrived!')
        elif code == 2:
            logging.info('Post read!')
        else:
            if code != -1:
                logging.warn('Unknown code: ' + str(code))
