

# Option Menu BW
A renovated implementation of option menu feature.

# It is Compumaxx's Code and translated from Spainish to English by me.

<image src="Option Menu_1721864152058.png"> </image>
## Building
### Same build as Main menu BW injection.
Requirements (Windows):
- DevkitPro/ARM (https://devkitpro.org/wiki/Getting_Started/devkitARM windows installer here)
- Armips(Latest: https://buildbot.orphis.net/armips/)
- Given path variable to gbagfx and preproc of pokeemrrald/pokefirered/pokeruby
Make sure you have an *environment variable* for "DEVKITARM", "armips" and "gbagfx" and "preproc. If not, create them.

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

