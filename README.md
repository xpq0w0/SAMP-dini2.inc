http://forum.sa-mp.com/showthread.php?t=611399

# dini2.inc
Here is an improved version of DINI, file/INI processor which supports multiple files at one time plus is a lot faster, comparable with today's standard INI processors(y_ini/eINI etc).

INI is mostly used for initializing data to your server. more of configurations for your mode.
But i have seen many gamemodes/filterscripts still use this old library which is very inappropriate in today's time, so i though of making an improved version.

##BRIEF: How it works?
When you use any of the INI function, lets say:

```pawn
dini_Set("MyFile.ini", "Owner", "Gammix");
```
The include will check if the file is already an instance if not one will be created. Instance doesn't mean the file remain open for that time, it just reads all the data from the file and then close it. A global timer is always on and this instance gets a tickcount when its initially opened. Lets say the we opened the file 1 second before, after INI_FILE_TIMEOUT (milliseconds), the instance is closed and all the data(new one) is written to the file.

So by default the tickrate is of 1000 ms (1 seconds), after that time the file auto closes after saving your data or changes. 
You can also set your own timer time to something smaller if you don't want to access the file after certain intervals.

