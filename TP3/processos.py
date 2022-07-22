import socket
from datetime import datetime
import time
import random

TCP_IP = '127.0.0.1'
TCP_PORT = 8888

def current_milli_time():
    return datetime.utcnow().isoformat(sep=' ', timespec='milliseconds')


class Client():
    def __init__ (self, repetitions, clients, timeout):
        self.r = repetitions
        self.n = clients
        self.k = timeout

    def run(self, id):
    
        number = time.time()
        
        for i in range(self.r):

            s = socket.socket()
            s.connect((TCP_IP, TCP_PORT))
            #print(i)
            message = f"{i}|{id}|{number}".encode('utf-8')
            s.sendall(message)
            waitAnswer = True

            while waitAnswer:
                try:
                    reply = s.recv(2).decode('utf-8')
                    if(reply == "GR"):
                        #print(reply)
                        with open("resultado.txt", "a") as f:
                            now = current_milli_time()
                            f.write(f"|{id:03d}|{now}\n")

                        #time.sleep(random.randrange(10))
                        time.sleep(self.k)
                        time.sleep(0.01)
                        s.send(f"RL".encode('utf-8'))
                        waitAnswer = False

                except Exception:
                    break
                time.sleep(0.01)
            s.close()

