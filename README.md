# Shazam
This is a tool for calculating different type of hash sums like MD5, SHA256, etc. Its use is intended primarly on *nix based Operating Systems.

***It is on a very initial phase, you should not use it on your main machine already.***

### Build
To compile the program, execute to following command on the root dir of the program:

```bash
g++ main.cpp -std=c++17 -o main
```

Or using the compile.sh script:
```bash
sh compile.sh
```

You can also give arguments to the compile.sh script, like, for example:
```bash
sh compile.sh -Wall -static
```

Finally execute it using:

```bash
./main
```

### Usage

To calculate the sha256sum of one file use the option '-sha256', and substitute '\<files\>' with the name of the files wanted.

```bash
./main -sha256 <files>
```

For more options use:

```bash
./main --help
```
