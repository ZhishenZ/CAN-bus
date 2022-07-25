# two-HAT Synchronous Scan

## Configuring the MCC 118 HATs for a Synchronous Acquisition
Perform the following procedure to configure a stack of MCC 118 HATs to
synchronously acquire data.

> **Note**: The board stack may contain up to eight MCC 118 HATs.
Designate one MCC 118 as the master device.

1. Stack the MCC 118 HATs onto the Rasberry Pi per the DAQ HAT documentation.
2. Wire the **CLK** terminals together on each MCC 118 HAT board.
3. Connect an external trigger source to the **TRIG** terminal on the MCC 118
master device.
4. Set the **OPTS_EXTCLOCK** scan option on each MCC 118 HAT except the master device.
5. Set the **OPTS_EXTTRIGGER** scan option on the MCC 118 master.

When triggered, the master device outputs the sample clock to the other MCC 118
HATs. The acquisition begins when the MCC 118 HATs receive the clock signal.

> Refer to the documentation for **mcc118_a_in_scan_start()** for information
about the OPTS_EXTCLOCK and OPTS_EXTTRIGGER scan options.