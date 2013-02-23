#!/usr/bin/python 
import socket

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

client.connect(("192.168.1.3",9099))

client.send("LST\n")

print client.recv(100)


client.send("RTF 28-000004472e3e\n")
print client.recv(100)

client.send("RTC 28-000004472e3e\n")
print client.recv(100)

client.close()

    
