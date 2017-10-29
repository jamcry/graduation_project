# graduation_project
Graduation project about **RFID based electronic voting system using Arduino**.
This project consists of two source files: a Python script *(.py)* and a Arduino script *(.ino)*.

- Checking the received UID from lists in py script to see whether they can vote or not is added and tested. *(10/28)*
- Py reads uid,vote,name data from shelve database and assign variables. At the end of the voting, it updates the database. *(10/30)*

**What's Next:**
* a Tkinter GUI will be added to show voter's image and name on the computer's screen when a new card is read.
* duplicate variables such as 'yesCount' and 'noCount' will be combined into a dict.
* *#NOTE: img path can also be added to 'nameDict'*
