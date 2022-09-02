# Continuous Scan with GUI

## install gtk packages for the GUI
Install required packages:
  ```sh
  sudo apt install libgtk-3-dev at-spi2-core autoconf libtool
  ```
## About
This example demonstrates how to perform a continuous acquisition on one or more 
channels.

Analog input data is acquired in blocks for a specified group of channels.

The last sample of data for each channel is displayed for each block of data 
received from the device. The acquisition is stopped when the Enter key is 
pressed.

This example is compiled and ready-to-run, and can be customized to suit 
your needs.

## Running the example
To run the example, open a terminal window and enter the following commands:
```sh
   cd ~/daqhats/examples/c/mcc118/continuous_scan
   ./continuous_scan
```

## Data recording
When the data recording button is switched, only when the motor is activated, a loggind file for the PDO data will be created. However, the MCC 118 board is logging all the time.  

## when the logging time ist reached, the control loop will take a time period more than a normal cycle. The control loop may become unstable because of the varing control loop period. 


## Support/Feedback
Contact technical support through our
[support page](https://www.mccdaq.com/support/support_form.aspx).
