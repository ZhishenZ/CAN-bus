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



##  Add MCP2515 to RSPi 4B to SPI0.0

### Wiring


|Rspi pin|label|MCP2515 Pin|
|---|---|------|
|02 |5V | VCC  |
|06 |GND | GND  |
|19 |GPIO 10 | SI(MOSI)  |
|21 |GPIO 9 | SO(MISO) |
|22 |GPIO 25 | INT  |
|23 |GPIO 11 | SCK  |
|24 |GPIO 8 | CS  |


check the device tree in `/boot/overlays/README`


Add the following in the `/boot/config.txt` file. 
```
dtparam=spi=on
dtoverlay=mcp2515-can0,oscillator=8000000,interrupt=25
dtoverlay=spi1-1cs
``` 

`dmesg |grep spi` 
https://forums.raspberrypi.com/viewtopic.php?t=196733


**Check if the hardware is successfully mounted**
```
pi@raspberrypi:~ $ ls /sys/bus/spi/devices/spi1.0/net/
can0
```

```
sudo ip link set can0 up type can bitrate 500000
```
loopback to check 
```
sudo ip link set can0 up type can bitrate 500000 loopback on
```
check the can bus connection locally 
``` 
cangen can0
````
open another terminal to receive
```
candump can0
```

### Connect to the second SPI

In the linux device tree there are only `mcp2515-can0.dtbo` and `mcp2515-can1.dtbo` which configure `spi0.0` and `spi0.1`. But there is no `dtbo` file for `spi1.2`. To solve this problem, we have two options:

1. write and compile a new `mcp251-can2.dts` file and then sudo cp the  `mcp251-can2.dtbo` into the device tree overlays folder `/boot/overlays`. 

2. The new released rasbian has a devce tree file `mcp2515.dtbo` and this can configure devices on spi1 or spi2. And **we choose this approach** to add the second SPI. 


The RSPi 4B has another three SPIs on 

|Rspi pin|SPI|BCM|
|---|---|------|
|11 |SPI1 CE0 | BCM17  |
|12 |SPI1 CE1 | BCM18 |
|36 |SPI1 CE2| BCM16  |

To make the wring neat, the `spi1.2` is enabled(becasue it is near to the SPI1 SI,SO,SCLK).

#### wiring

|Rspi pin|label|MCP2515 Pin|
|---|---|------|
|02 |5V | VCC  |
|35 |GPIO 19 | SO(MISO) |
|36 |GPIO 16 | CS  |
|37 |GPIO 26 | INT  |
|38 |GPIO 20 | SI(MOSI)  |
|39 |GND | GND  |
|40 |GPIO 21 | SCK  |

Add the following in the `/boot/config.txt` file. 
```
dtparam=spi=on
dtoverlay=spi1-3cs,cs0_spidev=off,cs1_spidev=off,cs2_spidev=off
dtoverlay=mcp2515,spi1-2,oscillator=8000000,interrupt=26
``` 
`dtoverlay=spi1-3cs` enable `spi1.0`, `spi1.1`, and `spi1.2`

desable `spidev1.0 `, `spidev1.1` and `spidev1.2` which are associated with `spi1.0`, `spi1.1`, and `spi1.2` 
set the interrupt pin to BCM(GPIO) 26.
reboot and  check if the can0 is mounted.
``` sh
pi@raspberrypi:~/CAN-bus $ dmesg | grep "spi"
[    9.926040] mcp251x spi1.2 can0: MCP2515 successfully initialized.
```
`ip a` to check
``` sh
pi@raspberrypi:~/CAN-bus $ ip a
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    inet 127.0.0.1/8 scope host lo
       valid_lft forever preferred_lft forever
    inet6 ::1/128 scope host 
       valid_lft forever preferred_lft forever
2: eth0: <NO-CARRIER,BROADCAST,MULTICAST,UP> mtu 1500 qdisc mq state DOWN group default qlen 1000
    link/ether e4:5f:01:8d:fb:c8 brd ff:ff:ff:ff:ff:ff
3: wlan0: <BROADCAST,MULTICAST> mtu 1500 qdisc pfifo_fast state DOWN group default qlen 1000
    link/ether e4:5f:01:8d:fb:c9 brd ff:ff:ff:ff:ff:ff
4: can0: <NOARP,UP,LOWER_UP,ECHO> mtu 16 qdisc pfifo_fast state UP group default qlen 10
    link/can 
```

### SDO write
`mc.Can.SdoWr(NodeId, 0x4732(addr), 1(sub_addr), 123(data)) `
```
"67D";"8";"23 32 47 01 7B 00 00 00"
^^^^^ 0x600+NodeID  

"67D";"8";"23 32 47 01 7B 00 00 00"
      ^^^ Can message length always 8 

"67D";"8";"23 32 47 01 7B 00 00 00"
           ^^ for sdo write here 23 (1 byte)

"67D";"8";"23 32 47 01 7B 00 00 00"
              ^^^^^ address 0x4732 (2 bytes) 

"67D";"8";"23 32 47 01 7B 00 00 00"
                    ^^ sub address 0x01 (1 byte)
"67D";"8";"23 32 47 01 7B 00 00 00"
                       ^^^^^^^^^^^ can message (4 bytes)
```
Example
```

# Parameter: NodeId
NodeId = 127                                            # Node Id
mc.Can.SdoWr(NodeId, 0x4732, 1, 123)                    # Position following error - window
mc.Can.SdoWr(NodeId, 0x473B, 2, 456)
mc.Can.SdoWr(NodeId, 0x4310, 1, 10000)
mc.Can.SdoWr(NodeId, 0x4310, 1, 100000)


"Bus";"No";"Time (abs)";"State";"ID (hex)";"DLC";"Data (hex)";"ASCII"
"USB-to-CAN V2 compact  CAN-1";"13";"00:03:03.521";"      ";"67F";"8";"23 32 47 01 7B 00 00 00";"#2G.{..."
"USB-to-CAN V2 compact  CAN-1";"14";"00:03:03.522";"      ";"5FF";"8";"60 32 47 01 00 00 00 00";"`2G....."
"USB-to-CAN V2 compact  CAN-1";"15";"00:03:03.523";"      ";"67F";"8";"23 3B 47 02 C8 01 00 00";"#;G....."
"USB-to-CAN V2 compact  CAN-1";"16";"00:03:03.523";"      ";"5FF";"8";"60 3B 47 02 00 00 00 00";"`;G....."
"USB-to-CAN V2 compact  CAN-1";"17";"00:03:03.525";"      ";"67F";"8";"23 10 43 01 10 27 00 00";"#.C..'.."
"USB-to-CAN V2 compact  CAN-1";"18";"00:03:03.527";"      ";"5FF";"8";"60 10 43 01 00 00 00 00";"`.C....."
"USB-to-CAN V2 compact  CAN-1";"19";"00:03:03.529";"      ";"67F";"8";"23 10 43 01 A0 86 01 00";"#.C....."
"USB-to-CAN V2 compact  CAN-1";"20";"00:03:03.531";"      ";"5FF";"8";"60 10 43 01 00 00 00 00";"`.C....."

```


### SDO read
msg = mc.Can.SdoRd(NodeId, 0x4310, 1)  
`mc.Can.SdoRd(NodeId, 0x4310(addr), 1(sub_addr)) `
```
"67F";"8";"40 10 43 01 00 00 00 00"
^^^^^ 0x600+NodeID  

"67F";"8";"40 10 43 01 00 00 00 00"
      ^^^ Can message length always 8 

"67F";"8";"40 10 43 01 00 00 00 00"
           ^^ for sdo write here 23 (1 byte)

"67F";"8";"40 10 43 01 00 00 00 00"
              ^^^^^ address 0x4310 (2 bytes) 

"67F";"8";"40 10 43 01 00 00 00 00"
                    ^^ sub address 0x01 (1 byte)

"67F";"8";"40 10 43 01 00 00 00 00"
                       ^^^^^^^^^^^ can message here for read is always 0
```

Example
```
msg = mc.Can.SdoRd(NodeId, 0x4310, 1)  




"Bus";"No";"Time (abs)";"State";"ID (hex)";"DLC";"Data (hex)";"ASCII"
"USB-to-CAN V2 compact  CAN-1";"29";"00:15:46.285";"      ";"67F";"8";"40 10 43 01 00 00 00 00";"@.C....."
"USB-to-CAN V2 compact  CAN-1";"30";"00:15:46.285";"      ";"5FF";"8";"4B 10 43 01 A0 86 00 00";"K.C....."
```



### TODO

1. Check if the response of the motor prompts the right answer. (1. time out, 2: motor error. )
   1. can message`80 11 41 01 02 00 01 06` only 80 matters it is a motor error. If so, prompts an error and then send the error message. 
   2. If there is a time out, error message. 
2. Record the PDO data (which have the index numbers 1FF. 2FF. 3FF, 4FF ) in a csv file with time. The detailed information is in the email. 



``` c
// Motor configuration 

write_sdo();
check_sdo();
write_sdo();
check_sdo();

// ...

// Now comes the PDO to get the motor data
write_PDO();

// ---------AFTER WE POWER UP PDO---------
record_with_another_thread_PDO();
// some condition is triggered, and then break the PDO recording thread

// ... wait for some trigger to set the new motor state
// the tirger commes, jump out of the current function.

write_sdo();
record_PDO_and_chck_sdo();//this function can listen both sdo and PDO response from the motor,
                          //it stops record when a sdo message shows up. 
record_with_another_thread_PDO();// function breaks when some condition is triggered

//...

write_sdo()
record_PDO_and_chck_sdo();//this function can listen both sdo and PDO response from the motor, 
                          // it stops record when a sdo message shows up. 
record_with_another_thread_PDO();// function breaks when some condition is triggered


write_sdo();// the last sdo message to disable power
check_sdo();// check the mesage

close_can();
```

