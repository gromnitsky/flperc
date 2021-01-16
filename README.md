# flperc

flperc (File List Percolator) -- filter-out non-existing file names
from stdin.

~~~
$ ./flperc -h
./flperc: invalid option -- 'h'
Usage: printf '%s\n%s' /usr /usr/file | flperc [-0L] [-d char] [bcdpfls]
-L         is -L from find(1)
-d CHAR    a custom delimiter instead of a newline
-0         a shortcut for -d $'\0'
-V         print the program version

bcdpfls    see find(1) -type option
~~~

## Why?

Sometimes you have a list of files coming from a pipe that you need to
filter-out. Grep has no idea whether `/some/dir` string represents a
file that

a) exists;
b) is a directory, a 'regular' file or a symlink.

find(1) does a file traversal while applying its filters--it doesn't
accept a list of files from stdin.

## Compilation

Type `make`.

## TODO

Additional find(1)-like filters could be useful like -mtime, -size,
-empty.

## License

MIT.
