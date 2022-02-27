# Shazam
This is a tool for calculating different type of hash sums like MD5, SHA256, etc. Its use is intended primarly on *nix based Operating Systems.

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

To create a release build use:

```bash
sh compile.sh -O3 -finline-functions -static -Wall
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

### Tests

The tests are inside the tests folder which inside is the script run_test.sh responsible for running the tests
and the main test file named tests.cpp which contains all tests inside. So, basically, to run the unit and integration test on that file,
it would be necessary to execute the following commands:

```bash
cd ./tests
sh run_test.sh
```

Then the results should be printed on the terminal.

