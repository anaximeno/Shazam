# Shazam
This is a command-line tool for calculating different types of hash sums like MD5, SHA256, etc. Its use is intended primarily on \*nix-based Operating Systems.

### Build
The requirements are: **g++>=8**  
  
To compile the program, execute to following command on the root dir of the program:

```bash
g++ main.cpp -std=c++17 -o main
```

Or using the Makefile:
```bash
make
```

To create a release build use:

```bash
make release
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

### Name
The program will have the same name of the binary file being executed so if another name is wanted, just rename the binary file 'main' to whatever is wanted and the problem is solved. Example:

```bash
mv main shazam
```

Then run it with the new name:

```bash
./shazam --help
```

### Tests

The tests are inside the tests directory. To run the tests, you'll have hust to run the bash script named 'run_test.sh', located inside the tests dir. The file named tests.cpp contains all tests inside. Basically, to run the unit and integrations tests of the program the following commands should be executed:

```bash
cd ./tests
sh run_test.sh
```

Then the results should be printed on the terminal.

