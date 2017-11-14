# graduation_project
Graduation project about **RFID based electronic voting system using Arduino**.
This project consists of two source files: a Python script *(.py)* and a Arduino script *(.ino)*.

- Checking the received UID from lists in py script to see whether they can vote or not is added and tested. *(10/28)*
- Py reads uid,vote,name data from shelve database and assign variables. At the end of the voting, it updates the database. *(10/29)*
- A tkinter based **GUI has been added** to the py script which shows voter's photo and prints whether he can vote or he has already voted. Also sample images (*.png) for the script has been uploaded. *(10/30)*
- Code for second booth is improved and added.

**Current Status:**
- Two people can vote simultaneously in parallel booths. Python script will detect which booth is available and will tell user. After voting booth is again set to "available" and new voter can vote.

**What's Next:**
- Tkinter GUI will be adapted to two booth.

