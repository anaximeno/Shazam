# Shazam
This is a command-line tool for calculating different types of hash sums like MD5, SHA256, etc. Its use is intended primarily on unix-based Operating Systems.

## Build
The requirements are: **g++>=8**  and **make**

To build the code use the following command:

```bash
make
```

Then execute it using:

```bash
./shazam
```

## Usage

To calculate the sha256sum of one file use the option '-sha256', and substitute '\<files\>' with the name of the files wanted.

```bash
./shazam -sha256 <files>
```

For more options use:

```bash
./shazam --help
```

## Tests

Basically, to run unit and integrations tests of the program use the following command:

```bash
make test
```

Then the results should be printed on the terminal.

## License

This projects is under the [BSD 3-Clause License](LICENSE).
