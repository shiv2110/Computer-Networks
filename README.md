# Reassembly

### Assumptions
- Maximum data length = 1480
- The code will work for any maximum data length, only the 
  payload_len macro has to be set to the same
- If a datagram has more than 1 error, only 1 error
  message will be displayed in the output

### To compile the program

```
make all
```

### To run the program

```
./test <data_file_path>
```

### Output will be of the following form

```
========== Message List ==========

IP      ID      Success Message     Total Datalen

                OR

IP      ID      Error Message

========== Successfully Reassembled Fragments ==========

IP: __, ID: __, Total Data Length: __
        MF: __, Offset: __, Fragment Data Length: __
        MF: __, Offset: __, Fragment Data Length: __
        MF: __, Offset: __, Fragment Data Length: __
        .
        .
        .
```

### To clean object files and executable

```
make clean
```
