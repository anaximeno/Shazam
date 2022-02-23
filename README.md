# Shazam
This is a tool for calculating different type of hash sums like MD5, SHA256, etc. Its use is intended primarly on *nix based Operating Systems.

***It is on a very initial phase, you should not use it on your main machine already.***

### Build
Before building you must install the required library hashlibpp, and to do so, you'll have to go inside the directory 'hashlib2plus/truck/src/' and install it using 'make install', like shown below:

```bash
cd hashlib2plus/truck/src/
make install
```

Then to compile the program, return to the root dir of this program and compile it using:

```bash
g++ main.cpp -I/usr/include/hashlib++/ -lhl++ -o main
```

Or using the compile.sh script:
```bash
sh compile.sh
```


Finally execute it using:

```bash
./main
```