echoserver
==========

  ___________      .__            _________                                
  \_   _____/ ____ |  |__   ____ /   _____/ ______________  __ ___________ 
   |    __)__/ ___\|  |  \ /  _ \\_____  \_/ __ \_  __ \  \/ // __ \_  __ \
   |        \  \___|   Y  (  <_> )        \  ___/|  | \/\   /\  ___/|  | \/
  /_______  /\___  >___|  /\____/_______  /\___  >__|    \_/  \___  >__|   
          \/     \/     \/              \/     \/                 \/      
          
  ©2013 - Narwaro (admin@narwaro.com), http://narwaro.com

  This is the Server-Software of the Echo-Collection. The Echo-Collection is a type of Open-source Backdoor-Trojan.
  Everybody is allowed to compile for himself, I will not give support in preventing the software will be manipulated
  for illegal uses. I think, that everybody who is able to compile it, is reasonable for what he is doing.
  The Client, the Virus "itself", is not developed so far that it can be publicated. On request I will pass on the 
  Client. 
  
  Development-Version: 0.18 (11/21/2013)
  
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
  2013. Only a few days later here is an realease, that isn't stable yet, (oh long time it will take ;) ) but is a view
  to a cool Weekend- and Coffee-Break-Project. I hope to your delingent support and ideas, a friendly comment or 
  suggestions for improovement would be nice.
  
  Changelog:
  
  v0.18 - 11/21/2013:
  
  - MySQL-Connnections work now
  - Threading added
  - Fixed TCP and Pipe errors
  - Error Codes added
  - Unique ID added
  - Information exchange protocol added
  
  
  v0.1 - 11/19/2013:
  
  - first, ugly Release
  
