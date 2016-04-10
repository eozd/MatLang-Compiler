## Matlang2C MatLang to C translator
MatLang2C is a translator from Matlang to C. Matlang is a made up language, and
currently has only some basic constructs. Later additions to the language may
be possible.  See project report in doc folder for implementation and
design details.

## DEPENDENCIES
glibc

## INSTALL
```bash
./install.sh
```

## RUN
1. To see the help directives
```bash
./matlang2c
```

2. Compile MatLang source file to a file with the same name but with a c
extension.
```bash
./matlang2c SOURCE_FILE
```

3. compile Matlang source file to a new file
```bash
./matlang2c SOURCE_FILE -o OUTPUT_FILE
```

## RUNNING TESTS
```bash
./run_tests.py
```
For a given .mat file, the correct output should be given in the corresponding
.test file.
