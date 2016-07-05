http://forum.sa-mp.com/showthread.php?t=611399

# gini.inc
Only for the sake of those who still use DINI, switch your gamemodes to this.

Yo! So i have been using this INI processor which works similar to EasyDB opening and closing. So i thought of sharing this. This is faster, a lot than DINI and supports the similar syntax. Many people were use to DINI's syntax so here it is! 

If you don't use INI anymore, thats a good thing. Just leave a good comment here and peace out :)

##BRIEF: How it works?
When you use any of the INI function, lets say:
```pawn
INI_Set("MyFile.ini", "Owner", "Gammix");
```
The include will check if the file is already an instance if not one will be created. Instance doesn't mean the file remain open for that time, it just read all the data from the file and then close it. A timer is registered after which the file is going to close.

So by default the timer is of 1000 ms (1 seconds), after that time the file auto closes after saving your data or changes. 
You can also set your own timer time to something smaller if you don't want to access the file after certain intervals.
