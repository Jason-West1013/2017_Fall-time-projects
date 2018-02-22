"""
Client side of the jumble game. Connects to the server
via an already open socket. The program then receives 
words from the server and returns guess for confirmation. 
The result is then sent back, the game runs until the 
server decides to quit. 
"""
import sys
from socket import *     
serverHost = 'localhost'                    # hostname
serverPort = 50007                          # port number
turn = 0                                    # turn counter

sockobj = socket(AF_INET, SOCK_STREAM)      # creates socket 
sockobj.connect((serverHost, serverPort))   # connects to server socket

# Game Loop
while True:
    turn = turn + 1
    word = sockobj.recv(1024)               # receives word from server
    if not word:                            # checks that data is actually received 
        break

    print("\n", word.decode())              # prints scrambled word
    if turn == 1:                           
        print('\nType your answer or type "quit" to end the game.')
    else:
        print('\nType your answer.')
    
    match_word = input()                    # prompts user for input, tests is user wants to quit 
    if match_word == 'quit': 
        print('Goodbye!')
        break 
    sockobj.send(match_word.encode())       # sends guess word back to server

    result = sockobj.recv(1024)             # receives result and prints it. 
    if not result: 
        print('There was an error receiving data, ending connection...')
        break
    print(result.decode())


sockobj.close()                             # closes socket       
