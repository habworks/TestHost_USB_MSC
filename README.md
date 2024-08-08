# TestHost_USB_MSC
This application is a test applicaiton of Spokane Hardware as a USB Host with MSC.
The software is configured to use the USB OTG and UART2 as a debug port.
When a USB Flash drive is connected to the Spokane a message will be displayed on the debug port that USB Flash drive inserted
If that USB Flash drive contains a file by the name of #define TEST_FILE_NAME then it will report that file as found or not
When the USB Flash drive is removed a message will be displayed on the debug port that the USB Flash drive was removed
---The above behavior can be found (only) on git hash 5124106 

You can turn on and off the USB MSC.  
On the debug terminal type the command Help or ? to see a list of commands
As it is written you can only turn off when the flash drive is removed.
As it is written you can only turn on if it was previously turned off.
To turn off: 
  On the debug terminal type the command MSC Off to turn off
To turn on:
  On the debug terminal type the command MSC On to turn on


