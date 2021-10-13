from server import *

if __name__ == '__main__':
    try:
        server = Server()
        server.run()
    except:
        print("error. terminating program.")
        exit(1)
