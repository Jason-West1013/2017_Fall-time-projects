"""
Server side of the jumble game. Contains main game 
loop. Opens a socket that stays open to receive 
connections from clients. Max clients allowed is 5. 
Send scrambled words to client, validates guess words, 
and returns result. 
"""
import random
import time, _thread as thread          
from socket import *                   
myHost = ''                                 # hostname           
myPort = 50007                              # port number                         

F = open('wordlist.txt')                    # opens word bank file, stores in variable
words = F.readlines()
F.close()

sockobj = socket(AF_INET, SOCK_STREAM)      # creates socket
sockobj.bind((myHost, myPort))              
sockobj.listen(5)                           # allows maximum of 5 clients

"""
Function returns the current time. 
"""
def now():
    return time.ctime(time.time())               

"""
Contains main game loop. Once a client is connected 
a thread is created and this process is run. This 
function scrambles a random word from the word bank
and sends it to the client. Then the function receives 
the guess, validates it, and returns the result. When
the data received is not present the connection is loop 
is broken and the connection is closed. 
"""
def runGame(connection):               
    # Game loop
    while True:
        result = ''
        guessWord = ''
        word = words[random.randrange(len(words))]
        while len(word) > 5 or len(word) == 0:
            word = words[random.randrange(0, len(words))]
        word = word.rstrip()
        old_word = word
        word = list(word)
        while word:
            guessWord = guessWord + word.pop(random.randrange(len(word))) + " "

        connection.send(guessWord.encode())       
        match_word = connection.recv(1024)     
        if not match_word: break

        new_word = match_word.decode() + '\n'
        if new_word in words and set(match_word.decode()) == set(old_word):
            result = 'You win.'
        else:
            result = 'The answer is ' + old_word  

        connection.send(result.encode())     
    connection.close()

"""
Continually checks for client connections. Once a 
connection is established a thread is started and 
the runGame() function is run. 
"""
def dispatcher():                   
    while True:                                
        connection, address = sockobj.accept()  
        print('Server connected by', address, end=' ')
        print('at', now())
        thread.start_new_thread(runGame, (connection,))

dispatcher()                                # run the dispatcher function
