import serial # serial module is used to communicate with the serial port
import time
import shelve # shelve module is used to read data from the database
import sys
from Tkinter import * # Tkinter is used for GUI
import tkMessageBox
try:
    cmdArg = str(sys.argv[1])
    if(cmdArg == '-reset'):
        # this script can be called with the arg. '-reset'
        # to reset database (THIS REMOVES ALL SAVED DATA !)
        dbs = shelve.open('uid_data')
        dbs['votedUids'] = []
        dbs['voteCounts'] = {'red':0,'green':0,'blue':0,'blank':0}
        dbs.close()
        print "="*10
        print " ** DATABASE IS RESET **"
        print "="*10
        print "arg 1 : " , str(sys.argv[1])
    elif(cmdArg == '-showDb'):
        # also can be called with the arg. '-showDb'
        # to see the results
        dbs = shelve.open('uid_data')
        print "="*15
        print " > VOTED LIST : " , dbs['votedUids']
        print " > COUNTS : " , dbs['voteCounts']
        if(len(dbs['votedUids']) != len(dbs['uidsNames'].keys())):
            numOfVoted = len(dbs['votedUids'])
            numOfVoters = len(dbs['uidsNames'])
            print numOfVoted , "voters have voted out of " , numOfVoters
            percentOfAttendance = (numOfVoted * 100 / numOfVoters)
            print percentOfAttendance , "percent of the voters has voted."
            print (100-percentOfAttendance) , "percent of the voters didn't attend."
        if(len(dbs['votedUids']) == len(dbs['uidsNames'].keys())):
            print(" VOTING FINISHED!")
            redCount = dbs['voteCounts']['red']
            greenCount = dbs['voteCounts']['green']
            blueCount = dbs['voteCounts']['blue']
            blankCount = dbs['voteCounts']['blank']
            sys.exit(0)
    print "="*15
    dbs.close()
except IndexError:
    x = 0; # do nothing if no argument is written in commandline

# NOTE the user must ensure that the serial port and baudrate are correct
serialPort = "/dev/ttyUSB0"
serialPort2 = "/dev/ttyUSB1"
baudRate = 9600
baudRate2 = 19200

ser1 = serial.Serial(serialPort, baudRate)
# ser1(ttyUSB0) should be connect to the main circuit with the RFID reader
# or ser1 and ser2 must be swapped
ser2 = serial.Serial(serialPort2 , baudRate2)
print "Serial port " + serialPort + " opened Baudrate " + str(baudRate)
print "Serial port " + serialPort2 + " opened Baudrate " + str(baudRate2)

votingUids = [] #this list stores the UIDs that are currently voting
booth1available = True
booth2available = True
booth1Uid = 0
booth2Uid = 0
def showGuiMsg(msg , booth , cardUID , voterName):
    #this fn. shows a GUI message according to
    #the status of the voter such as 'YOU CAN VOTE' , 'YOU ALREADY VOTED' etc.
    master = Tk()
    imgPath = str(cardUID) + ".png"
    name = voterName
    img = PhotoImage(file=imgPath)
    imgPanel = Label(master , image = img)
    def canVoteButton():
        #if the person is eligible to vote
        #print the starting command to the serial of the available circuit
        if(booth == "booth1"):
            ser1.write('S')
        elif(booth == "booth2"):
            ser2.write('<start>')
            print "ser2 write start"
        master.destroy()
        master.quit()
    if(msg=="voted"):
        #if the person has already voted, shows an error message
        nameMsgText = "YOU HAVE ALREADY VOTED\n" + name
        textBg = 'red'
        statusImg = PhotoImage(file = 'error.png')
        okButton = Button(master , text="OK" , command=master.destroy)
    elif(msg=="canVote"):
        nameMsgText = "YOU CAN VOTE\n" + name
        textBg = 'green'
        statusImg = PhotoImage(file = 'ok.png')
        okButton = Button(master , text="OK" , command=canVoteButton)
    elif(msg=="occupied"):
        #if all of the booths are occupied, show a warning
        nameMsgText = "ALL BOOTHS ARE OCCUPIED NOW!\nPLEASE WAIT!"
        textBg = 'blue'
        statusImg = PhotoImage(file = 'error.png')
        okButton = Button(master , text="OK" , command=master.destroy)
    boothMsgText = " "
    #print which booth is available (if any)
    if(booth == "booth1"):
        boothMsgText = "GO TO BOOTH 1"
    elif(booth == "booth2"):
        boothMsgText = "GO TO BOOTH 2"

    boothMsg = Message(master , text=boothMsgText , width =550 , anchor = CENTER)
    nameMsg = Message(master , text=nameMsgText , width=550, anchor=CENTER)
    nameMsg.config(bg=textBg ,fg='white', font=( 48 ) , anchor=CENTER )
    statusImgPanel = Label(master, image = statusImg)
    statusImgPanel.pack()
    imgPanel.pack()
    boothMsg.pack()
    nameMsg.pack()
    okButton.pack()
    master.mainloop()

def startBooth(cardUID , voterName):
    #direct voter to the available booth(if any)
    #and start the voting circuit
    global booth1available , booth2available , booth1Uid , booth2Uid
    if(booth1available):
        booth1Uid = cardUID
        showGuiMsg("canVote" , "booth1" , booth1Uid , voterName)
        print " >> GO BOOTH 1"
        print "="*15
        booth1available = False #mark booth1 as occupied
        votingUids.append( cardUID ) #add card UID to currently voting UIDs list
    
    elif(booth2available):
        booth2Uid = cardUID
        showGuiMsg("canVote" , "booth2" , booth2Uid , voterName)
        print " >> GO BOOTH 2"
        print "="*15
        booth2available = False
        votingUids.append( cardUID )
    
    else: #if no booths are available
        print("Both booths are occupied")
        showGuiMsg("occupied" , "x" , cardUID , "test")
        print "="*15

def checkForNewCard():
    global booth1available , booth2available , booth1Uid , booth2Uid
    if(ser1.inWaiting()): #if there is new data in the serial
        serLine = ser1.readline() #read the line
        if(serLine[0] == 'R'): #if the fetched line is uid
                #'R' is a marker to show that the line contains Rfid card UID number
                cardUidLine = serLine.split("\n")[0]
                cardUID = int(cardUidLine[1::])
                print " >> CARD UID:" , cardUID
            if(cardUID in allUids):
                voterName = uidNameDict[cardUID]
                print " >> NAME : " , voterName
                if(cardUID in votingUids): #if the voter is currently voting
                    print "YOU ARE VOTING NOW."
                    print "="*15
                elif(cardUID in votedUids): #if the voter has already voted
                    print "YOU HAVE ALREADY VOTED"
                    showGuiMsg("voted" ,"x" , cardUID , voterName)
                    print "="*15
                elif(cardUID not in votingUids and cardUID not in votedUids):
                #if the voter is eligible to vote
                print "START VOTING " , voterName
                startBooth(cardUID , voterName)
                print "="*15
            else:
                c = 0 #do nothing
def logVote(booth, vote):
    #this fn. increments the vote counts according to
    #the selected option
    global redCount , greenCount , blueCount , blankCount
    if(vote == 'RED'):
        print(" >> RED selectedx")
        redCount += 1
    elif(vote == 'GREEN'):
        print(" >> GREEN selected")
        greenCount += 1
    elif(vote == 'BLUE'):
        print(" >> BLUE selected")
        blueCount += 1
    elif(vote == 'BLANK'):
        print(" >> BLANK selected")
        blankCount += 1
    else:
        print(" >> SELECTION IS INVALID")

def checkForVote():
    #this fn. checks the serial for new
    #vote data
    global booth1available , booth2available
    if(ser1.inWaiting()): #if new data is waiting in ser1
        serLine1 = ser1.readline()
        if(serLine1[0] == 'V'): # if the fetched line is vote
            #'V' is a marker to show that the line contains Vote data
            voteLine1 = serLine1.split("\n")[0]
            vote1 = voteLine1[1::]
            print " >> VOTE 1: " , vote1
            logVote(booth1available , vote1)
            print "="*15
            votedUids.append(booth1Uid)
            votingUids.remove(booth1Uid)
            booth1available = True
            ser1.write('X') # tell arduino to restart
    elif(ser2.inWaiting()): #if new data is waiting in ser2
        serLine2 = ser2.readline()
        if(serLine2[0] == 'V'): # if the fetched line is vote
            voteLine2 = serLine2.split("\n")[0]
            vote2 = voteLine2[1::]
            print " >> VOTE 2: " , vote2
            logVote(booth2Uid , vote2)
            print "="*15
            votedUids.append(booth2Uid)
            votingUids.remove(booth2Uid)
            booth2available = True
            #ser2.write('X') # tell arduino to restart
    else:
        x = 0 # do nothing

while(1):
    #start an infinite loop to continuously check for new data
    #the program will continue until terminated by the user
    dbShelfFile = shelve.open('uid_data') # open the database(shelve)
    
    #get the data and store them in variables
    uidNameDict = dbShelfFile['uidsNames']
    allUids = uidNameDict.keys()
    
    name = dbShelfFile['uidsNames'].values()
    votedUids = dbShelfFile['votedUids']
    voteCounts = dbShelfFile['voteCounts']
    redCount = voteCounts['red']
    greenCount = voteCounts['green']
    blueCount = voteCounts['blue']
    blankCount = voteCounts['blank']
    
    dbShelfFile.close()
    
    #main loop
    checkForNewCard() #check if a new card is present
    checkForVote() #get the vote data
    
    #write new data to the database
    dbShelfFile = shelve.open('uid_data')
    dbShelfFile['votedUids'] = votedUids
    voteCounts['red']=redCount
    voteCounts['green']=greenCount
    voteCounts['blue']=blueCount
    voteCounts['blank']=blankCount
    dbShelfFile['voteCounts'] = voteCounts
    dbShelfFile.close() #close the database