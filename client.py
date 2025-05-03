from enum import Enum
import threading
import argparse
import socket
import sys
import time




class client :

    # ******************** TYPES *********************
    # *
    # * @brief Return codes for the protocol methods
    class RC(Enum) :
        OK = 0
        ERROR = 1
        USER_ERROR = 2

    # ****************** ATTRIBUTES ******************
    _server = None
    _port = -1

    # ******************** METHODS *******************

    @staticmethod
    def tratar_respuesta(sock, status_dict):
        ''' 
        La intención de esta función es unificar las respuestas pero hay un problema y es que algunas respuestas tienen no solo es status.
        Por ello para esto pasaremos la flag m que indicará al programa que se ejecutó correctamente y que hay más información para que haga otro listen.
        '''
        response = sock.recv(1)[0]
        if response[0] == 'm':
            print(status_dict['0'])
            print(response[1:])
        else:
            print(status_dict[response[0]])
        sock.close()

    @staticmethod
    def tratar_mensaje(message, status_dict):
        '''
        La intención de esta función es unificar el mandado de mensajes como primer argumento siempre se mandará el usuario aún siendo erroneo ya que el 
        mirar si el usuario existe o no, no es competencia del cliente pero el tener el usuario que eres si es la competencia.
        '''
        
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_address = (client._server, client._port)
        sock.connect(server_address)
        sock.sendall(message)
        listener_thread = threading.Thread(target=client.tratar_respuesta, args=(sock, status_dict,))
        listener_thread.start()
        listener_thread.join()


    @staticmethod
    def  register(user) :

        status_dict = {
            '0' : 'REGISTER OK',
            '1' : 'USERNAME IN USE',
            '2' : 'REGISTER FAIL'
        }

        client.tratar_mensaje(b''.join([b'0',user.encode()]),status_dict)
        return client.RC.ERROR
   
    @staticmethod
    def  unregister(user) :

        status_dict = {
            '0' : 'UNREGISTER OK',
            '1' : 'USER DOES NOT EXIST',
            '2' : 'UNREGISTER FAIL'
        }

        client.tratar_mensaje(b''.join([b'1',user.encode()]),status_dict)
        return client.RC.ERROR
    
    @staticmethod
    def  connect(user) :

        status_dict = {
            '0' : 'CONNECT OK',
            '1' : 'CONNECT FAIL , USER DOES NOT EXIST',
            '2' : 'USER ALREADY CONNECTED',
            '3' : 'CONNECT FAIL'
        }
        client.user = user
        client.tratar_mensaje(b''.join([b'2',user.encode()]),status_dict)
        return client.RC.ERROR

    @staticmethod
    def  disconnect(user) :

        status_dict = {
            '0' : 'DISCONNECT OK',
            '1' : 'DISCONNECT FAIL, USER DOES NOT EXIST',
            '2' : 'DISCONNECT FAIL, USER NOT CONNECTED',
            '3' : 'DISCONNECT FAIL'
        }
        client.user = 0
        client.tratar_mensaje(b''.join([b'3',user.encode()]),status_dict)
        return client.RC.ERROR

    @staticmethod
    def  publish(fileName,  description) :

        status_dict = {
            '0' : 'PUBLISH OK',
            '1' : 'PUBLISH FAIL, USER DOES NOT EXIST',
            '2' : 'PUBLISH FAIL, USER NOT CONNECTED',
            '3' : 'PUBLISH FAIL, CONTENT ALREADY PUBLISHED',
            '4' : 'PUBLISH FAIL'
        }

        client.tratar_mensaje(b''.join([b'4', client.user, fileName.encode(), description.encode()]),status_dict)
        return client.RC.ERROR

    @staticmethod
    def  delete(fileName) :

        status_dict = {
            '0' : 'DELETE OK',
            '1' : 'DELETE FAIL, USER DOES NOT EXIST',
            '2' : 'DELETE FAIL, USER NOT CONNECTED',
            '3' : 'DELETE FAIL, CONTENT NOT PUBLISHED',
            '4' : 'DELETE FAIL'
        }

        client.tratar_mensaje(b''.join([b'5', client.user, fileName.encode()]),status_dict)
        return client.RC.ERROR

    @staticmethod
    def  listusers() :

        status_dict = {
            '0' : 'LIST_USERS OK',
            '1' : 'LIST_USERS FAIL, USER DOES NOT EXIST',
            '2' : 'LIST_USERS FAIL, USER NOT CONNECTED',
            '3' : 'LIST_USERS FAIL'
        }
    
        client.tratar_mensaje(b''.join([b'6', client.user]),status_dict)
        return client.RC.ERROR

    @staticmethod
    def  listcontent(user) :

        status_dict = {
            '0' : 'LIST_CONTENT OK',
            '1' : 'LIST_CONTENT FAIL, USER DOES NOT EXIST',
            '2' : 'LIST_CONTENT FAIL, USER NOT CONNECTED',
            '3' : 'LIST_CONTENT FAIL, REMOTE USER DOES NOT EXIST',
            '4' : 'LIST_CONTENT FAIL'
        }

        client.tratar_mensaje(b''.join([b'7',user.encode()]),status_dict)
        return client.RC.ERROR

    @staticmethod
    def  getfile(user,  remote_FileName,  local_FileName) :

        status_dict = {
            '0' : 'GET_FILE OK',
            '1' : 'GET_FILE FAIL, FILE NOT EXIST',
            '2' : 'GET_FILE FAIL'
        }

        client.tratar_mensaje(b''.join([b'8',user.encode(),remote_FileName.encode(), local_FileName.encode()]),status_dict)
        return client.RC.ERROR

    # *
    # **
    # * @brief Command interpreter for the client. It calls the protocol functions.
    @staticmethod
    def shell():

        while (True) :
            try :
                command = input("c> ")
                line = command.split(" ")
                if (len(line) > 0):

                    line[0] = line[0].upper()

                    if (line[0]=="REGISTER") :
                        if (len(line) == 2) :
                            client.register(line[1])
                        else :
                            print("Syntax error. Usage: REGISTER <userName>")

                    elif(line[0]=="UNREGISTER") :
                        if (len(line) == 2) :
                            client.unregister(line[1])
                        else :
                            print("Syntax error. Usage: UNREGISTER <userName>")

                    elif(line[0]=="CONNECT") :
                        if (len(line) == 2) :
                            client.connect(line[1])
                        else :
                            print("Syntax error. Usage: CONNECT <userName>")
                    
                    elif(line[0]=="PUBLISH") :
                        if (len(line) >= 3) :
                            #  Remove first two words
                            description = ' '.join(line[2:])
                            client.publish(line[1], description)
                        else :
                            print("Syntax error. Usage: PUBLISH <fileName> <description>")

                    elif(line[0]=="DELETE") :
                        if (len(line) == 2) :
                            client.delete(line[1])
                        else :
                            print("Syntax error. Usage: DELETE <fileName>")

                    elif(line[0]=="LIST_USERS") :
                        if (len(line) == 1) :
                            client.listusers()
                        else :
                            print("Syntax error. Use: LIST_USERS")

                    elif(line[0]=="LIST_CONTENT") :
                        if (len(line) == 2) :
                            client.listcontent(line[1])
                        else :
                            print("Syntax error. Usage: LIST_CONTENT <userName>")

                    elif(line[0]=="DISCONNECT") :
                        if (len(line) == 2) :
                            client.disconnect(line[1])
                        else :
                            print("Syntax error. Usage: DISCONNECT <userName>")

                    elif(line[0]=="GET_FILE") :
                        if (len(line) == 4) :
                            client.getfile(line[1], line[2], line[3])
                        else :
                            print("Syntax error. Usage: GET_FILE <userName> <remote_fileName> <local_fileName>")

                    elif(line[0]=="QUIT") :
                        if (len(line) == 1) :
                            break
                        else :
                            print("Syntax error. Use: QUIT")
                    else :
                        print("Error: command " + line[0] + " not valid.")
            except Exception as e:
                print("Exception: " + str(e))

    # *
    # * @brief Prints program usage
    @staticmethod
    def usage() :
        print("Usage: python3 client.py -s <server> -p <port>")


    # *
    # * @brief Parses program execution arguments
    @staticmethod
    def  parseArguments(argv) :
        parser = argparse.ArgumentParser()
        parser.add_argument('-s', type=str, required=True, help='Server IP')
        parser.add_argument('-p', type=int, required=True, help='Server Port')
        args = parser.parse_args()

        if (args.s is None):
            parser.error("Usage: python3 client.py -s <server> -p <port>")
            return False

        if ((args.p < 1024) or (args.p > 65535)):
            parser.error("Error: Port must be in the range 1024 <= port <= 65535");
            return False;
        
        client._server = args.s
        client._port = args.p

        return True


    # ******************** MAIN *********************
    @staticmethod
    def main(argv) :
        if (not client.parseArguments(argv)) :
            client.usage()
            return

        #  Write code here
        client.shell()
        print("+++ FINISHED +++")
    

if __name__=="__main__":
    client.main([])