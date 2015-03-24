EchoServer
==========

<pre>
  ___________       __            _________                                
  \_   _____/ ____ |  |__   ____ /   _____/ ______________  __ ___________ 
   |    __)__/ ___\|  |  \ /  _ \\_____  \_/ __ \_  __ \  \/ // __ \_  __ \
   |        \  \___|   Y  (  <_> )_____   \  ___/|  | \/\   /\  ___/|  | \/
  /_______  /\___  >___|  /\____/_______  /\___  >__|    \_/  \___  >__|   
          \/     \/     \/              \/     \/                 \/      
</pre>
          
  ©2015 - Narwaro (admin@narwaro.com), http://narwaro.com; ThePorngamer971 (david@yansonline.de), http://yansonline.de

  This is the Server-Software of the Echo-Collection. The Echo-Collection is a type of Open-source Backdoor-Trojan.
  Everybody is allowed to compile for himself, I will not give support in preventing the software will be manipulated
  for illegal uses. I think, that everybody who is able to compile it, is reasonable for what he is doing.
  The Client, the Virus "itself", is not developed so far that it can be publicated. On request I will pass on the 
  Client. 
  
  Development-Version: 0.8.4 PRE (2015/01/15)
  
  Stable Release: - 
  
  
  General Information: How to install:
  Use git on your server or download the repository somehow else. It contains 2 or 3 main files, the "main.cpp",
  a Makefile, and maybe something in /obj. You need some C++ Libaries, major ones are listed here: libmysqlclient and 
  pthread. The server is written for Linux-/POSIX-Systems, if you want to run on Windows, you need somthing like 
  "winpthread". You may see an executable in the /bin folder. But I have to disappoint you, this is an ARMHF-executable.
  
  Concept: How to use:
  The server features a hybrid Server/Client-Model. There is a Server, the EchoServer, which is the main One.
  There is also the EchoClient, what is a kind of console for the EchoVirus. You start it to send commands to the
  virus, so that they can be executed. The EchoVirus is at the moment only a client for the EchoServer, its backdoor-
  fuctionalities haven't been programmed yet. The same with the EchoClient. Every client will have a unique ID, so that
  it can be authentificated. The Client is delivered with an empty ID (some zeros) and fetches one from the EchoServer
  as it is starting for the first time. The Server will generate an ID and send it back, storing the Virus' IP-Adress, 
  the last-connected-date and the ID. So every Virus-Client will be indentifiable by the server and every ID is unique.
  The Virus will have some cool Functions, a buddy brought me on. For example it will be able to use WIN32-API to 
  get into Upstart, have a hidden console access and of course to download and upload files where and when you want.
  Right now there are 2 versions of the virus: The dev-client to connect to the server and for debugging and the one
  for testing Backdoor-Ideas. 
  
  Background:
  A buddy wrote a Program that could download and upload files from another computer. He just wanted to steal some 
  Pictures, but I thought that this can do more and can be more fun. I just started Development on November the 19th, 
  '13. Only a few days later here is an realease, that isn't stable yet, (oh long time it will take ;) ) but is a view
  to a cool Weekend- and Coffee-Break-Project. I hope to your delingent support and ideas, a friendly comment or 
  suggestions for improovement would be nice.
  
  Changelog:
  
  v0.8.15 PRE - 2015/03/24:
  
    - Some updates after dev pause for focusing on embedded devices and assemby lang
    - Lots of bugfixes
    - Fixed a bug in server and EchoClient that prevented viruses from being found in the db
    - Upload and download fully working, encrypted
    - Increased transfer speed
    - Currently working on further imrovements 
    - 2 options for transfers in new version: Speed or reliability
    - Echo is becoming stable, yeah
  
  v0.8.14 PRE - 01/17/2015:
  
    - Added Webinterface
    - Added more detailed logging
    - Added 3 tables to record actions
    - Encryption is working now
    - Recieving files from the viurs is currently producing errors, investigating
    - Currently stuck in v0.8.x, more features soon! I am first trying to get everything running smoothly
  
  
  v0.8PRE - 10/03/2014:
  
    - Added communication encryption, not used right now
    - Added RSA keychain generation
    - Added AES256 implementation, not used right now
    - Added YansLibary
    - More ideas in development, coming in v0.8


  v0.7 - 06/10/2014:
  
    - Intensive development, more changes than just recoreded here
    - Added mouse block
    - Mouse block now working properly under Windows 8
    - Protocol extended


  v0.6 - 04/06/2014:
  
    - Added download command to project, just working directly
    - Added upload command, not working so far (Virus issue)
    - Added exit command to client
    - Virus: New screenshot libary with WindowsAPI and LodePNG, not implemented yet
    - Virus: New multi-language keylogger libary, not implemented yet


  v0.5 - 03/20/2014:
  
    - Added pipe and working commands to EchoClient and Virus 
    - Added help command
    - Third server thread added
    - Various performance and runtime fixes


  v0.4 - 03/10/2014:
  
    - EchoClient added, used to control Viruses
    - Second sever thread for EchoClient added
    - Fixed socket errors
    - Fixed tunneling issues


  v0.18 - 11/21/2013:
  
    - MySQL-Connnections work now
    - Threading added
    - Fixed TCP and Pipe errors
    - Error Codes added
    - Unique ID added
    - Information exchange protocol added


  v0.1 - 11/19/2013:
  
    - initial, ugly Release
    - Right now just a real "echo" server, sends requests immediately back to client (the Virus)
  
