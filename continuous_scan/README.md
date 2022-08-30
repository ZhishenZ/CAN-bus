# Continuous Scan Example

## About
This program demonstrates how to perform a continuous acquisition on one or more 
channels.

The sensor data in eight channels will be stored in `YYYY_MM_DD_HH_MM_SS_TIMEZONE.csv` files. Analog input data is acquired in blocks for a specified group of channels. After the logging time is reached, the current `csv` file is closed and a new `csv` file will be created and the logging time is set to zero. The acquisition is stopped when the Enter key is 
pressed.

The last sample of data for each channel is displayed for each block of data 
received from the MCC 118 board. 


## Running the example

TODO: Add the todo to install the libraries. 

type `make` in the terminal to compile.

type `./main` to run the program 



To run the example, open a terminal window and enter the following commands:
```sh
   cd ~/daqhats/examples/c/mcc118/continuous_scan
   ./continuous_scan
```


