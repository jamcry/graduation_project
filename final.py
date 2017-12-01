
def recvFromArduino():
  global startMarker, endMarker

  ck = ""
  x = "z" # any value that is not an end- or startMarker
  byteCount = -1 # to allow for the fact that the last increment will be one too many

  # wait for the start character
  while  ord(x) != startMarker:
    x = ser.read()

  # save data until the end marker is found
  while ord(x) != endMarker:
    if ord(x) != startMarker:
      ck = ck + x
      byteCount += 1
    x = ser.read()

  return(ck)


#============================

import serial
import time
import shelve # shelve module is used to read variables from saved database in hdd
import os # for future uses to check if path is correct etc.
import sys
from Tkinter import *
import tkMessageBox
import thread

# this script can be called as 'python uid-comm.py -reset'

# to reset database (THIS REMOVES ALL SAVED DATA !)
try:
    cmdArg = str(sys.argv[1])
    if(cmdArg == '-reset'):
        dbs = shelve.open('uid_data')
        dbs['votedUids'] = []
        dbs['voteCounts'] = {'red':0,'green':0,'blue':0,'white':0}
        dbs.close()
        print "="*10
        print " ** DATABASE IS RESET **"
        print "="*10
        print "arg 1 : " , str(sys.argv[1])
    elif(cmdArg == '-showDb'):
        dbs = shelve.open('uid_data')
        print "="*15
        print " > VOTED LIST : " , dbs['votedUids']
        print " > COUNTS     : " , dbs['voteCounts']
        if(len(dbs['votedUids']) == len(dbs['uidsNames'].keys())):
            print(" VOTING FINISHED!")
            redCount = dbs['voteCounts']['red']
            greenCount = dbs['voteCounts']['green']
            blueCount = dbs['voteCounts']['blue']
            whiteCount = dbs['voteCounts']['white']
            sys.exit(0)
        print "="*15
        dbs.close()
except IndexError:
    x=0
        # do nothing if no argument is written in commandline


# if all registered uids has voted, exit program
# if(len(dbShelfFile['votedUids']) == len(dbShelfFile['uidsNames'].keys())):
#    print("VOTING HAS FINISHED")
#    sys.exit(0)




# NOTE the user must ensure that the serial port and baudrate are correct
serialPort = "/dev/ttyUSB0"
serialPort2 = "/dev/ttyUSB1"
baudRate = 9600
baudRate2 = 19200
ser1 = serial.Serial(serialPort, baudRate) # with rfid
ser2 = serial.Serial(serialPort2 , baudRate2)
print "Serial port " + serialPort + " opened  Baudrate " + str(baudRate)
print "Serial port " + serialPort2 + " opened  Baudrate " + str(baudRate2)

votingUids = []
booth1available = True
booth2available = True
booth1Uid = 0
booth2Uid = 0
def showGuiMsg(msg , booth , cardUID , voterName):
    master = Tk()
    imgPath = str(cardUID) + ".png"
    name = voterName
    img = PhotoImage(file=imgPath)
    imgPanel = Label(master , image = img)

    def canVoteButton():
        if(booth == "booth1"):
            ser1.write('S')
        elif(booth == "booth2"):
            ser2.write('<start>')
            print "ser2 write start"
        master.destroy()
        master.quit()

    if(msg=="voted"):
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
        nameMsgText = "ALL BOOTHS ARE OCCUPIED NOW!\nPLEASE WAIT!"
        textBg = 'blue'
        statusImg = PhotoImage(file = 'error.png')
        okButton = Button(master , text="OK" , command=master.destroy)

    boothMsgText = " "
    if(booth == "booth1"):
        boothMsgText = "GO TO BOOTH 1"
    elif(booth == "booth2"):
        boothMsgText = "GO TO BOOTH 2"
    boothMsg = Message(master , text=boothMsgText , width =550 ,  anchor = CENTER)
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
    global booth1available , booth2available , booth1Uid , booth2Uid

    if(booth1available):
        booth1Uid = cardUID
        showGuiMsg("canVote" , "booth1" , booth1Uid , voterName)
        print " >> GO BOOTH 1"
        #ser1.write('S')
        print "="*15
        booth1available = False
        votingUids.append( cardUID )

    elif(booth2available):
        booth2Uid = cardUID
        showGuiMsg("canVote" , "booth2" , booth2Uid , voterName)
        print " >> GO BOOTH 2"
        #ser2.write('<start>')
        #print "serial write <start>"
        #ser2.write('<start>')
        print "="*15
        booth2available = False
        votingUids.append( cardUID )

    else:
        print("both booths are occupied")
        showGuiMsg("occupied" , "x" , cardUID , "test")
        print "="*15


def checkForNewCard():
    global booth1available , booth2available , booth1Uid , booth2Uid
    if(ser1.inWaiting()):
        serLine = ser1.readline()
        if(serLine[0] == 'R'): #if the fetched line is uid
            cardUidLine = serLine.split("\n")[0]
            cardUID = int(cardUidLine[1::])
            print " >> CARD UID:" , cardUID

            if(cardUID in allUids):
                voterName = uidNameDict[cardUID]
                print " >> NAME : " , voterName

                if(cardUID in votingUids):
                    print "YOU ARE VOTING NOW."
                    print "="*15
                elif(cardUID in votedUids):
                    print "YOU HAVE ALREADY VOTED"
                    showGuiMsg("voted" ,"x" , cardUID , voterName)
                    print "="*15
                elif(cardUID not in votingUids and cardUID not in votedUids):
                    print "START VOTING " , voterName
                    startBooth(cardUID , voterName)
                    print "="*15
            else:
                print "CARD ID NOT FOUND"
                print "="*15
        else:
            c=0 # do nothing

def logVote(booth, vote):
    global redCount , greenCount , blueCount , whiteCount
    if(vote == 'RED'):
        print(" >> RED selectedx")
        redCount += 1

    elif(vote == 'GREEN'):
        print(" >> GREEN selected")
        greenCount += 1

    elif(vote == 'BLUE'):
        print(" >> BLUE selected")
        blueCount += 1

    elif(vote == 'WHITE'):
        print(" >> WHITE selected")
        whiteCount += 1

    else:
        print(" >> SELECTION IS INVALID")

def checkForVote():
    global booth1available , booth2available
    if(ser1.inWaiting()):
        serLine1 = ser1.readline()
        if(serLine1[0] == 'V'): # if the fetched line is vote
            voteLine1 = serLine1.split("\n")[0]
            vote1 = voteLine1[1::]
            print " >> VOTE 1: " , vote1
            logVote(booth1available , vote1)
            print "="*15
            votedUids.append(booth1Uid)
            votingUids.remove(booth1Uid)
            booth1available = True
            ser1.write('X') # tell arduino to restart
    #    else:
    #        print " [serial 1 incoming but not starting with V : ]" + serLine1

    elif(ser2.inWaiting()):
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
    #    else:
    #        print " [serial 2 incoming but not starting with V : ]" + serLine2
    else:
        x=2



while(1):
    ##ser2.write("*")
    dbShelfFile = shelve.open('uid_data')
    uidNameDict = dbShelfFile['uidsNames']
    allUids = uidNameDict.keys()
    ##print( " == ALL UIDS : " , allUids)

    name = dbShelfFile['uidsNames'].values()

    votedUids = dbShelfFile['votedUids']

    voteCounts = dbShelfFile['voteCounts']
    redCount = voteCounts['red']
    greenCount = voteCounts['green']
    blueCount = voteCounts['blue']
    whiteCount = voteCounts['white']

    dbShelfFile.close()

    checkForNewCard()
    checkForVote()

    dbShelfFile = shelve.open('uid_data')
    dbShelfFile['votedUids'] = votedUids
    voteCounts['red']=redCount
    voteCounts['green']=greenCount
    voteCounts['blue']=blueCount
    voteCounts['white']=whiteCount
    dbShelfFile['voteCounts'] = voteCounts
    dbShelfFile.close()
