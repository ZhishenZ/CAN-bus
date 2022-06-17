# CAN bus

## set up the CAN-USB converter on raspbian
 
type `ifconfig -a` to check if `can0` is available in system. console window of terminal: 

``` 
can0: flags=128<NOARP>  mtu 16
        unspec 00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00  txqueuelen 10  (UNSPEC)
        RX packets 0  bytes 0 (0.0 B)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 0  bytes 0 (0.0 B)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
``` 

more information about the USB-CAN module by typing `dmesg` in terminal. 

### use `can-utils` tool to test CAN communication. 

``` 
sudo apt-get install can-utils
```
iitialize CAN port

```
sudo ip link set can0 down
sudo ip link set can0 type can bitrate 500000
sudo ip link set can0 up
```

## MCP2515
```
sudo ip link set can0 up type can bitrate 500000
```

```
pi@raspberrypi:~/CAN-bus $ python can_rec.py 
Timestamp: 1655497028.113329        ID: 0100    S Rx                DL:  8    00 01 02 03 04 05 06 07     Channel: can0
Timestamp: 1655497029.123062        ID: 0100    S Rx                DL:  8    01 02 03 04 05 06 07 08     Channel: can0
Timestamp: 1655497030.130317        ID: 0100    S Rx                DL:  0                                Channel: can0
Timestamp: 1655497031.137020        ID: 0100    S Rx                DL:  8    03 04 05 06 07 08 09 0a     Channel: can0
Timestamp: 1655497032.143140        ID: 0100    S Rx                DL:  8    04 05 06 07 08 09 0a 0b     Channel: can0
Timestamp: 1655497033.148966        ID: 0100    S Rx                DL:  8    05 06 07 08 09 0a 0b 0c     Channel: can0
Timestamp: 1655497034.154008        ID: 0100    S Rx                DL:  4    03 03 84 04                 Channel: can0
Timestamp: 1655497035.158772        ID: 0100    S Rx                DL:  8    07 08 09 0a 0b 0c 0d 0e     Channel: can0
Timestamp: 1655497036.161426        ID: 0100    S Rx                DL:  8    08 09 0a 0b 0c 0d 0e 0f     Channel: can0
Timestamp: 1655497037.164074        ID: 0100    S Rx                DL:  8    09 0a 0b 0c 0d 0e 0f 10     Channel: can0
Timestamp: 1655497038.166736        ID: 0100    S Rx                DL:  8    0a 0b 0c 0d 0e 0f 10 11     Channel: can0
Timestamp: 1655497039.169337        ID: 0100    S Rx                DL:  8    0b 0c 0d 0e 0f 88 08 89     Channel: can0
Timestamp: 1655497040.172027        ID: 0100    S Rx                DL:  8    0c 0d 0e 0f 10 11 12 13     Channel: can0
Timestamp: 1655497041.174517        ID: 0100    S Rx                DL:  8    0d 0e 0f 10 11 12 13 14     Channel: can0
Timestamp: 1655497042.177111        ID: 0100    S Rx                DL:  8    0e 0f 10 11 12 13 14 15     Channel: can0
Timestamp: 1655497043.179633        ID: 0100    S Rx                DL:  8    0f 10 11 12 13 14 15 16     Channel: can0
Timestamp: 1655497044.182244        ID: 0100    S Rx                DL:  8    10 11 12 13 14 15 16 17     Channel: can0
Timestamp: 1655497045.184741        ID: 0080    S Rx                DL:  1    81                          Channel: can0
Timestamp: 1655497046.187271        ID: 0100    S Rx                DL:  8    12 13 14 15 16 17 18 19     Channel: can0
Timestamp: 1655497046.187377        ID: 009f    S Rx   R            DL:  7                                Channel: can0
Timestamp: 1655497046.187443        ID: 0004    S Rx E              DL:  8    00 20 00 00 00 00 00 00     Channel: can0
Timestamp: 1655497046.187462        ID: 07ee    S Tx                DL:  6    00 01 03 01 04 01           Channel: can0
Timestamp: 1655497046.188326        ID: 07ee    S Rx                DL:  6    00 01 03 01 04 01           Channel: can0
Timestamp: 1655497046.197466        ID: 0100    S Rx                DL:  8    12 13 14 15 16 17 18 19     Channel: can0
Timestamp: 1655497046.197580        ID: 07ee    S Rx                DL:  6    00 01 03 01 04 01           Channel: can0
Timestamp: 1655497046.199466        ID: 07ee    S Rx                DL:  6    00 01 03 01 04 01           Channel: can0
Timestamp: 1655497046.202694        ID: 07ee    S Rx                DL:  6    00 01 03 01 04 01           Channel: can0
Timestamp: 1655497046.204402        ID: 0004    S Rx E              DL:  8    00 20 00 00 00 00 00 00     Channel: can0
Timestamp: 1655497046.204437        ID: 07ee    S Rx                DL:  6    00 01 03 01 04 01           Channel: can0
Timestamp: 1655497046.204695        ID: 009f    S Rx   R            DL:  7                                Channel: can0
Timestamp: 1655497046.206508        ID: 07ee    S Rx                DL:  6    00 01 03 01 04 01           Channel: can0
Timestamp: 1655497046.207408        ID: 07ee    S Rx                DL:  6    00 01 03 01 04 01           Channel: can0
``` 
