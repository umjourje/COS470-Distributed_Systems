from concurrent.futures import process
import time
import socket, os
from threading import Thread, Lock
from _thread import *
import queue

TCP_IP = '127.0.0.1'
TCP_PORT = 8888
BUFFER_SIZE = 10


param = []
fila = queue.Queue()
nextClient = 0


mutexRun = Lock()
mutexQueue = Lock()
mutexLog = Lock()

def multi_threaded_client(connection, processo):

    #Loop para aguardar ser o próximo da fila
    aguardaFila = True
    #while (int(fila.queue[0]) != int(processo)):
    while(aguardaFila):
        mutexQueue.acquire()
        if(int(fila.queue[0]) == int(processo)):
            #Se for o próximo, adquire lock da fila e da run
            mutexRun.acquire()
            aguardaFila = False
            #Define o próximo da fila    
            nextClient = fila.get()
        #pass
        #Libera a fila
        mutexQueue.release()
        time.sleep(0.01)


    #Envia a mensagem de GRANT para o processo
    connection.send(f"GR".encode('utf-8'))
    mutexLog.acquire()
    with open("log.txt", "a") as f:
        #f.write("Granted access to process " + str(processo) + "\n")
        f.write("[S] Grant-" + str(processo) + "\n")
    #print("Granted access to process " + str(processo))
    mutexLog.release()

    #Aguarda o processo responder
    waitAnswer = True
    while waitAnswer:
        try:
            reply = connection.recv(2).decode('utf-8')
            #Verifica se recebeu o release do processo
            if (reply == "RL"):
                mutexLog.acquire()
                with open("log.txt", "a") as f:
                    #f.write("Release recieved from process " + str(processo) + "\n")                
                    f.write("[R] Release-" + str(processo) + "\n")
                #print("Release recieved from process " + str(processo))
                mutexLog.release()
                waitAnswer = False
        except Exception:
            break

    #Libera a região crítica e encerra a conexão
    mutexRun.release()
    connection.close()
    #print("Connection closed with process " + str(processo) + "\n")



class Listener(Thread):
    def __init__ (self, p):
        Thread.__init__(self)
        self.pedidos = p

    def run(self):
        
        while (True):
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.bind((TCP_IP,TCP_PORT))
                s.listen(1)
            except socket.error:
                print ('Failed to create socket')
                quit()

            while (True): 
                #Aguarda conexão de um cliente
                conn, addr = s.accept()
                data = conn.recv(BUFFER_SIZE).decode("utf-8")
                if data:
                    #Pega os dados da comunicação
                    timer = data[0]
                    processo = data[2:4]
                    mutexLog.acquire()
                    with open("log.txt", "a") as f:
                        #f.write("Received connection request from process: " + str(processo) + " with time " + str(timer) + "\n")
                        f.write("[R] Request-" + str(processo) + "\n")
                    #print("Received connection request from process: " + str(processo) + " with time " + str(timer))
                    mutexLog.release()

                    #Incrementa os pedidos daquele processo
                    self.pedidos[int(processo)] += 1

                    #Adiciona o processo à fila
                    mutexQueue.acquire()
                    fila.put(processo)
                    mutexQueue.release()

                    #Inicia uma thread para o processo
                    start_new_thread(multi_threaded_client, (conn, processo, ))
            s.close()



class Interface(Thread):
    def __init__ (self, p):
        Thread.__init__(self)
        self.pedidos = p

    def opcoes(self, item):
        match item:
            case "1":
                mutexQueue.acquire()
                for q_item in fila.queue:
                    print(q_item)
                mutexQueue.release()

            case "2":
                print(self.pedidos)

            case "3":
                os._exit(1)
            
    def run(self):
        while True:
            item = input("Select one item: ")
            self.opcoes(item)

