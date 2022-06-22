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


## if the INT wiring is wrong, 

the terminal does not respond after typing a `sudo ip link ...` command, however when checking the can bus connection with`ip a`, the right message will still show up.

in `/dev/`path, there is no `spidev0.0` 
```
ls /dev/spi*
>> /dev/spidev0.1 /dev/spidev1.0
```
