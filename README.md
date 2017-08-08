# nhtml

!html, a very simple template language

Syntax
------------
As said above, the Template Language is simple.
That also implies a few limitations:
1. No Parametric instantiation (for now) since this would bloat the compiler too much
   (I was in no need for that feature when writing this compiler).
2. No language integration of any kind.
   This makes this template compiler useful for static webcontent only!

Sooo, how does it work? 
* text enclosed with `""` will be placed in the html output, but will be html escaped and `\n` will be replaced with `<br/>`.

* text enclosed with () will be placed *without any changes*.

* One can include other files by writing either 
`@<filename>`, e.g. `@hi.nhtml` or
`@"<filename>"`, e.g. `@"filename with spaces.nhtml"`
These files will pe pasted in place unless they end with `.nhtml`, in which case
they get compiled before getting pasted.

* There are comment blocks (`/*text*/`) and linecomments (`//text`) like there are in C.

* A left curly brace (`{`) opens a tag, a coresponding right curly brace (`}`) closes it.

* Finally, tag names are simply written as text and can be accompied by an attribute set
enclosed in `[]`. So in order to e.g. produce
`<div style="color:white">Hi</div>`, the corresponding nhtml would look something like
`div[style=color:white]{"Hi"}`.

* If the Attributes Key-Name is `class`, its pair can be written as `.<value>` instead of `class=<value>`.
Note that multiple mentions of the same attribute key will result in the attribute values 
beeing concatenated with a space. So e.g.
`p[.A class=hi]{}` will become `<p class="A hi"></p>`.

That about covers everything this little thing can do :)
For a simple demo, look in example.

How to compile
--------------

The Project used cmake to build. It has no hard dependencies but it requires unix-style pathnames. (`/path/to/file`) for includes to work.

## on linux
Simply install cmake for your distro
### Ubuntu
`apt-get install cmake`
### Arch Linux 
`pacman -S cmake`

The create a subdirectory for the build to reside in
`mkdir build`
`cd build`

And run cmake
Release: `cmake -G"Unix Makefiles" ..`
Debug: `cmake -G"Unix Makefiles" .. -DCMAKE_BUILD_TYPE=DEBUG`

after that run `make` and you should be left with a compiled version of the !html compiler `nhtmlc`

How to use
----------
```
./nhtmlc [-o <filename>] file [file...]
-I	Add Path to include paths
--output
-o	The output file to write the html to
	When missing this option, stdout is used instead
-v	Enable verbose output
--help Print this usage
```

When given multple input files, the resulting html is concatenated into output.
