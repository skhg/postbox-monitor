import bluetooth
import sys

bd_addr = "00:0D:19:03:AF:9F" #Arduino bluetooth MAC
port = 1

sock = bluetooth.BluetoothSocket (bluetooth.RFCOMM)

def wait_for_connection(timeout):
	int rcv_count = 0

	try:
		print("Waiting for connection")
		sock.settimeout(None) #Infinite timeout while we wait for something to happen
		sock.connect((bd_addr, port))

		while True:
			print("Awake - reading...")
			sock.settimeout(timeout) #Arduino now awake so short timeout
			sock.recv(1024)
			rcv_count = rcv_count + 1

	except bluetooth.btcommon.BluetoothError as e:
		print("Session finished (timeout)")

	return rcv_count




while True:
	code = wait_for_connection(10)

	print(code)
