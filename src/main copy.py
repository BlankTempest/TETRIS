#single instance

import socket
import threading

def get_ip_address():
    # Get the IP address of the WiFi network that the computer is connected to
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
            print(f"Client {client_socket.getsockname()} disconnected")
            break
        if not data:
            print(f"Client {client_socket.getsockname()} disconnected")
            break
        try:
            server_socket.sendall(data)
        except socket.error:
            print(f"Client {server_socket.getsockname()} disconnected")
            break

    client_socket.close()

def main():
    ip_address = get_ip_address()
    print(f"Server running at {ip_address}:8080")

    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((ip_address, 8080))
    server_socket.listen(2)

    clients = []
    while len(clients) < 2:
        client_socket, client_address = server_socket.accept()
        print(f"Client {len(clients)+1} connected from {client_address}")
        clients.append(client_socket)

    # Start two threads to handle communication between clients
    client1_thread = threading.Thread(target=client_handler, args=(clients[0], clients[1]))
    client2_thread = threading.Thread(target=client_handler, args=(clients[1], clients[0]))
    client1_thread.start()
    client2_thread.start()

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print("KeyboardInterrupt: Exiting program")
