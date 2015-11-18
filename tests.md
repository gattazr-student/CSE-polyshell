# Tests of the Polyshell

## Testing simple commands
```
pwd
mkdir tests
cd tests
cd ..
ls
rmdir tests
```

## File redirections
```
ls > file
cat < file
rm file
```

### errors
#### Only 1 file supported (readline) (would create 2 files and use the second in bash)
```
ls > file1 > file2
```

#### Only one file supported (readline error) (would work in bash using the second file)
```
ls > file1 > file2
```

#### readline is buggy :-). The returned struct is bad and make the app crash
```
ls <>file
ls ><file
```


## pipe
```
ls | grep read | grep \.c
```

### errors
##### misplaced pipe (readline error)
```
|
ls || grep read
|ls | grep read
ls | grep read|
```

## pipe and redirection
```
ls > file1
cat < file1 | grep read | grep \.c > file2
cat file2
rm file1 file2
```

### errors
#### misplaced pipe (readline error)
```
ls || grep read > test
```

#### instead of writing in file the result of ls and not grepping(feof), result of ls and grep in file. Because of readcmd() the order of operations is wrong
```
ls > test | grep read
```

#### Because of readcmd() the order of operations is wrong
```
cat | grep read | grep \.c < test
```

#### Only one file supported (readline error) (would work in bash)
```
cat < test < test2 | grep read | grep \.c
ls > test > test2 | grep read
```
