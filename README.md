# Option Menu BW
A renovated implementation of option menu feature
Its Compumaxx's Code and translated from Spainish to English by me.
<image src=
## Building

Requirements (Windows):
- DevkitPro/ARM (https://devkitpro.org/wiki/Getting_Started/devkitARM windows installer here)
- Python (Atleast 3.4 https://www.python.org/downloads/)
- Armips(Latest: https://buildbot.orphis.net/armips/)
- Visual C++ runtime DLL https://www.microsoft.com/en-ca/download/details.aspx?id=48145

Make sure you have an *environment variable* for "DEVKITARM", "python" and "armips". If not, create them.

After cloning the repository, extract the "deps.zip" file. 

Next, place a clean FireRed US version 1.0 ROM and rename it to "BPRE0.gba".
Modify the line ".org 0x08800000" in "main.s", to your free rom address.
Now we can build the project by running "make".

If the last line is "Build Complete" then everything succeeded.

A new rom named "Option Menu.gba" should have appeared in the directory "build"


Credits:
- https://github.com/Touched project structure
- https://github.com/pret string related tools and pokeemerald resources
- Wobb
- All contributors!

