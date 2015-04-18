# CatForce
GOL catalyst search utility using brute force. Catalyst search utility based on LifeAPI library. 

Usage
--

Pass the .in to the CatForce.exe. 

Technical Details
--

**.in file format** 

Please see 1.in in the repository for correct usage. 

Notice the delimeter is " " - i.e. space. 
Don't place "" for strings. Just use them with space delimeter. 
FileOutput can use spaces in the name - but this is not recommended. 

---

`max-gen`

CatForce will never iterate after max-gen. 

`start-gen`

The first gen encounter is allowed. 

`num-catalyst`

The number of catalyst to place (note that if you chose 3 catalyst all three of the SLs should be catalysts). 

`stable-interval`

For how long all the catalysts should remain untouched (after activation) to be considered stable. 

`search-area x y w h`

Search area is always rectangular. 

(x, y) is the starting point (w, h) - width height. 

`pat rle dx dy`

The main pattern rle. 
(dx, dy) - optional. Transformation for the pat. 

`cat <rle> <max-gen-petrube> dx dy <symmetries-char>`

catalysts - can be few in a single .in file. 

rle - the rle of the catalyst 
max-gen-petrube - the maximal generations that a pattern can be idle. Transparent catalysts might be idle for longer periods. 
(dx, dy) - Mandatory parameters to define the center of the catalyst. 
symmetries-char - character that defines all symmetries the catalyst can be used. Currently supports: 
| + / x *

-  |   mirror by y.
-  +   mirror by x, y and both. 
-  /  diagonal mirror
-  x  diagonal mirror by both diagonals and rotations clock and anti clock wise. 
-  *  All 8 variations of transformations. 

`output <file.rle>`

output file name. 

Compilation
--

Compiled using g++. 
Compile the CatForce as any c++ application with single file. 
Just make sure you have the latest LifeAPI.h (or the one in the repository) in the same folder. 
