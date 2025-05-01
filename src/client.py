from enum import Enum

import argparse, ctypes, os
lib_path = os.path.abspath(os.path.join(os.path.dirname(__file__), "libclaves.so"))
claves = ctypes.CDLL(lib_path)

# Tipos de las funciones de la libreria
claves.register_user.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
claves.register_user.restype = ctypes.c_int

claves.unregister.argtypes = [ctypes.c_char_p]
claves.unregister.restype = ctypes.c_int

claves.connect_user.argtypes = [ctypes.c_char_p]
claves.connect_user.restype = ctypes.c_int

claves.disconnect.argtypes = [ctypes.c_char_p]
claves.disconnect.restype = ctypes.c_int

claves.publish.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
claves.publish.restype = ctypes.c_int

claves.delete.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
claves.delete.restype = ctypes.c_int

claves.listusers.argtypes = [ctypes.c_char_p]
claves.listusers.restype = ctypes.c_int

claves.listcontent.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
claves.listcontent.restype = ctypes.c_int

claves.get_file.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_int]
claves.get_file.restype = ctypes.c_int

class client:



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

    username_conectado = None



    # ******************** METHODS *******************





    @staticmethod

    def  register(user) :

        ip = client._server.encode("utf-8")
        port = str(client._port).encode("utf-8")
        username = user.encode("utf-8")

        result = claves.register_user(username, ip, port)

        if (result == 0) :

            return client.RC.OK

        if (result == 1) :

            return client.RC.USER_ERROR



   

    @staticmethod

    def  unregister(user) :

        username = user.encode("utf-8")

        result = claves.unregister(username)

        if (result == 0) :

            return client.RC.OK

        if (result == 1) :

            return client.RC.USER_ERROR





    

    @staticmethod

    def  connect(user) :

        username = user.encode("utf-8")

        result = claves.connect_user(username)

        client.username_conectado = username

        if (result == 0) :

            return client.RC.OK

        if (result == 1) :

            return client.RC.USER_ERROR





    

    @staticmethod

    def  disconnect(user) :

        username = user.encode("utf-8")

        result = claves.disconnect(username)

        client.username_conectado = None

        if (result == 0) :

            return client.RC.OK

        if (result == 1) :

            return client.RC.USER_ERROR



    @staticmethod

    def  publish(fileName,  description) :

        filename = fileName.encode("utf-8")
        descripcion = description.encode("utf-8")

        result = claves.publish(client.username_conectado, filename, descripcion)

        if (result == 0) :

            return client.RC.OK

        if (result == 1) :

            return client.RC.USER_ERROR



    @staticmethod

    def  delete(fileName) :

        filename = fileName.encode("utf-8")

        result = claves.delete(filename)

        if (result == 0) :

            return client.RC.OK

        if (result == 1) :

            return client.RC.USER_ERROR



    @staticmethod

    def  listusers() :

        if (client.username_conectado is None) :

            return client.RC.USER_ERROR
        
        result = claves.listusers(client.username_conectado)

        if (result == 0) :

            return client.RC.OK

        if (result == 1) :

            return client.RC.USER_ERROR



    @staticmethod

    def  listcontent(user) :

        username = user.encode("utf-8")

        result = claves.listcontent(username, client.username_conectado)

        if (result == 0) :

            return client.RC.OK

        if (result == 1) :

            return client.RC.USER_ERROR



    @staticmethod

    def  getfile(user,  remote_FileName,  local_FileName) :

        username = user.encode("utf-8")
        remote_filename = remote_FileName.encode("utf-8")
        local_filename = local_FileName.encode("utf-8")

        result = claves.get_file(username, remote_filename, client.username_conectado, local_filename, len(local_filename))

        if (result == 0) :

            return client.RC.OK

        if (result == 1) :

            return client.RC.USER_ERROR



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