# TunnelMan Videogame

In TunnelMan, the player has to dig underground tunnels through an oil field looking for oil. 

After the player has gathered all available oil within a particular field, the level is completed and the player advances to a new, more difficult oil field.

## Installation
To see what the game looks like in action:

## Mac

o Install XQuartz:

http://xquartz.org/

* Click the download link, open the downloaded .dmg file, double-click on XQuartz.pkg, and follow the installation instructions.

* Log out and log back in again.

* To verify the installation, open a Terminal window and run the command echo $DISPLAY. That should produce one line of output that ends with org.macosforge.xquartz:0.


o Install freeGLUT. You have two options:

https://guide.macports.org/#installing.macports

* Option 1: Install the MacPorts package manager. Select the appropriate installation links and install the package

  To verify the installation, open a new Terminal window and run the command port version. That should produce the output Version: 2.3.4. In that new Terminal window, run the command sudo port install freeglut, which will prompt you for your password to authorize installation. A lot of output will be produced. To verify the installation, run the command port installed freeglut.

  When some people run sudo port install freeglut, the response is Error: Port freeglut not found and the installation fails. That's probably because you are behind the a firewall. Bypassing the firewall (perhaps by creating a hotspot with your phone) may do the trick. You might also need to run sudo port selfupdate, but that step is probably not necessary.

* Option 2: Install the homebrew package manager.
* Option 3: Install MacPorts.

o Build on Xcode by opening project file (.xcodeproj) 

*make sure to change Derived Data to Project-relative location in project settings underneath file tab*

## Windows

o Copy and replace all code files into: /TunnelMan-skeleton-windows/TunnelMan/TunnelMan

o Build in visual studio


# Screenshot of the game
![image](https://user-images.githubusercontent.com/96732758/188429840-faadedf7-b1f5-4d58-9d2c-ea757ddb4a03.png)

