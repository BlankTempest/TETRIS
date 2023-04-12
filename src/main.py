import socket
import threading

def get_ip_address():
    #get ip address of hotspot. does so by connecting 
    # to a site(google in this case), then gets the ip.
    #udp is fine in this case
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect(('8.8.8.8', 80))
    hostname = socket.gethostname()
    ip_address = socket.gethostbyname(hostname)
    s.close()
    return ip_address

def client_handler(client_socket, server_socket):
    while True:
        try:
            data = client_socket.recv(1024)
        except socket.error:
            print(f"Client {client_socket.getpeername()} disconnected")
            break
        if not data:
            print(f"Client {client_socket.getpeername()} disconnected")
            break
        if data and data.decode()=="9":
            print(f"Client {client_socket.getpeername()} loses")
            continue
        try:
            server_socket.sendall(data)
        except socket.error as e:
            print(f"Error sending data to client {server_socket.getpeername()}: {e}")
            break

    client_socket.close()


def main():

    instances = []

    try:
        ip_address = get_ip_address()
        #ip_address = "10.20.200.142"
        print(f"Server running at {ip_address}:8080")

        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_socket.bind((ip_address, 8080))
        server_socket.listen()

        while True:
            client_socket, client_address = server_socket.accept()
            print(f"Client connected from {client_address}")
            #creates instance if no instances or last instance already has 2 clients
            if len(instances) == 0 or len(instances[-1]) == 2:
                instances.append([client_socket])
            else:
                #adds client to instance
                instances[-1].append(client_socket)
                #thread the clients
                client1_thread = threading.Thread(target=client_handler, args=(instances[-1][0], instances[-1][1]))
                client2_thread = threading.Thread(target=client_handler, args=(instances[-1][1], instances[-1][0]))
                client1_thread.start()
                client2_thread.start()

    except KeyboardInterrupt:
        print("KeyboardInterrupt: Exiting program")
    except Exception as e:
        print(f"Error: {e}")
    finally:
        #cleanup
        for instance in instances:
            for client_socket in instance:
                client_socket.close()
        server_socket.close()

if __name__ == '__main__':
    main()
