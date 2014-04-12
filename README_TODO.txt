README
libvpoc++
Ken Lamarche - April 2014

A "new" approach to my miscellaneous library.
This will bring me up to about 1990 technology ;-(

Here are some of the key points:

1.
Having a separate Git branch for the C code and the C++ code was pretty unreasonable.
In reality, the interfaces to the library are so different between a C implementation
(which uses functions), and a C++ implementation (which uses classes), these are really
two different products.

2.
Each the C and C++ versions of the library should have a name that indicates what they
are.  The C version should have "c", C++ version "c++"

3.
Each product will have it's own Git repos, but I was to preserve the changes from the
work previously done in lib.org.sinkme.misc.

4.
"lib.org.sinkme.misc" is an outrageous name.  I know I wanted to make a name that would
never be used by some other entity - and so I keyed off the Domain name as Java tells
us to do with packages.  But the name needs to be in the vein of normal C/C++ libraries
that are linked in.

5.
This README_TODO is for the C++ library.  And it's name is libvpoc++.  The "vpo" is for
"valuedprojects.org", and will be a common signature on other libraries I build.

6.
The old lib.org.sinkme.misc code made an archive file (lib.org.sinkme.misc.a).
I wanted to install this library so that a program using it would not have to include a
library path.  The using program should be built with a simple "-lvpoc++" argument.
This is possible if the library is installed in /usr/local/lib.  But this is not 
possible if the library is an archive.
For all modern purposes, the library should be a Dynamically Linked Shared Library.
For Mac, this is a ".dylib" file.  And from the command line, they are created with
"g++ -shared", not "ar".  This is a significant change from lib.org.sinkme.misc.


10 April, 2014
There is not an OS variable implicit to make.  I found someone's solutions to determining
the OS on the web.  But it seems a little rough.
Still, I'm using it until I have time/interest/energy to figure something else out.

TODO

2.
Do all this with Xcode.  Control the Xcode project under Git.

3.
Do I need a different git branch for different architectures?

4.
What about cross-compiling for small board platforms (Raspberry / BeagleBone)?

5.
Decide on version tagging.