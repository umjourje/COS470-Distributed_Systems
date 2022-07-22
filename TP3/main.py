import sys 
import os
parent_dir_name = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))

import coordenador
import processos
from log_validator import validate


if __name__ == "__main__":
    import time
    from multiprocessing import Pool
    repetitions = 3     # r
    n = 16
    k = 0
    pedidos = [0 for _ in range(n)]

    start_time = time.time()
    listener = coordenador.Listener(pedidos)
    listener.start() 

    interface = coordenador.Interface(pedidos)
    interface.start()

    str_time = time.strftime('%H:%M:%S', time.gmtime(time.time() - start_time))
    print(f"Initiated 'Coordenador'. Elapsed time: {str_time}")


    start_time = time.time()
    client = processos.Client(repetitions, n, k)

    p_id = [i for i in range(client.n)]
    
    with Pool(client.n) as p:
        p.map(client.run, p_id)

    str_time = time.strftime('%H:%M:%S', time.gmtime(time.time() - start_time))
    print(f"Terminated 'Processos'. Elapsed time: {str_time}")

    start_time = time.time()
    validate()
    str_time = time.strftime('%H:%M:%S', time.gmtime(time.time() - start_time))