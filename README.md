# graduation_project
Graduation project about **RFID based electronic voting system using Arduino**.
This project consists of two source files: a Python script *(.py)* and a Arduino script *(.ino)*.

- Checking the received UID from lists in py script to see whether they can vote or not is added and tested. *(10/28)*
- Py reads uid,vote,name data from shelve database and assign variables. At the end of the voting, it updates the database. *(10/29)*
- A tkinter based **GUI has been added** to the py script which shows voter's photo and prints whether he can vote or he has already voted. Also sample images (*.png) for the script has been uploaded. *(10/30)*
- Tkinter problems solved and GUI is improved *(11/5)*
- **LCD screen** and 3 new buttons (2 option , 1 selection button) are added. *(11/7)*

**What's Next:**
- Figure out:
--how to continuosly run the program
--how to make parallel voting possible
- **BUG:** Arduino keeps reading card and getting vote even after python script terminated !
*(.ino can check if .py started by using a serial command (while ser.read()!='s': wait) )
