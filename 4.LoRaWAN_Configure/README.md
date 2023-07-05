# CONFIGURE LORA

### I. HARDWARE 
+ E32-TTL-100 SX1278 LoRa module have 2 pin MO and M1. Used to set the modes of the module. There are 4 modes: `normal`, `wake-up`, `power-saving`, `sleep`

    |   Mode            |   M1  |   M0  |                                                                        Explanations                                                                                                           |
    |:-----------------:|:-----:|:-----:|:---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------:|
    |   Normal          |   0   |   0   |   UART and wireless channel is good to go                                                                                                                                                     |
    |   Wake-up         |   0   |   1   |   Same as normal but a preamble code is <br /> added to transmitted data for waking-up <br /> the receiver                                                                                    | 
    |   Power-saving    |   1   |   0   |   UART is disable and wireless is on <br /> WOR (Wake On Radio) mode which means <br /> the device will turn on when there <br /> is data to be received. Transmission <br /> is not allowed  |
    |   Sleep           |   1   |   1   |   Used in setting parameters. Transmitting <br />  and receiving disabled                                                                                                                     | 

+ When you want to configure LoRa, you set 2 pins M0 and M1 to `High Level`. Otherwise, you set 2 pins M0 and M1 to `Low Level` when normal mode activity 
+ Use TTL-USB connect to LoRa to configure 

    |   LoRa    |   |   TTL-USB |
    |:---------:|:-:|:---------:|
    |   GND     |<=>|   GND     |
    |   Vcc     |<=>|   3V3     |
    |   AUX     |<=>|   reserve |
    |   TXD     |<=>|   RXD     |
    |   RXD     |<=>|   TXD     |
    |   M0      |<=>|   3V3     |
    |   M1      |<=>|   3V3     | 

### II. SOFTWARE 
+ There is 2 method for parameter settings to LoRa
    + Send `Hexadecimal Command` for LoRa via CoolTerm software, Hercules software, etc 
    + Using `RF_Setting3.47 Software` wrote by EBYTE firm

    ***A. Use Hexadecimal Command***
    
    + When the module is in sleep mode (M0 = 1, M1 = 1), you can change some options like `Baud rate`, `UART format`, etc. 
    + *Syntax*
        >   0xC0/0xC2 ADDH ADDL SPED CHAN OPTION 
        > - Notes: when you want the parameter to be `saved to EPPROM`, use **0xC0**. `Otherwise`, use **0xC2**

        |   Item    |                                       Description                                                     |               Note            |
        |:---------:|:-----------------------------------------------------------------------------------------------------:|:-----------------------------:|
        |   ADDH    |   Module high address byte (00H by default)                                                           |                               |
        |   ADDL    |   Module low address byte (00H by default)                                                            |                               |
        |   SPED    |   UART Baud Rate and Air Data Rate                                                                    |   See bit descriptions below  |
        |   CHAN    |   Operating Frequency (00H to FFH corresponding <br /> to 410 to 411 MHz. Default is 17H - 433MHz)    |                               |
        |   OPTION  |   Other options                                                                                       |   See bit descriptions below  | 
            
    + SPED Bit Descriptions 

        |   Bit 7,6 - <br /> UART Parity    |   Bit 5,4,3 - <br /> UART Baud Rate   |  Bit  2,1,0 - <br /> Air Data Rate    |
        |:---------------------------------:|:-------------------------------------:|:-------------------------------------:|
        |   00: 8N1 <br /> (***default***)  |   000: 1200bps                        |   000: 0.3kbps                        |
        |   01: 8O1                         |   001: 2400bps                        |   001: 1.2kbps                        |
        |   10: 8E1                         |   010: 4800bps                        |   010: 2.4kbps <br /> (***default***) |
        |   11: 8N1 <br /> (equal to 00)    |   011: 9600bps <br /> (***default***) |   011: 4.8kbps                        |
        |                                   |   100: 19200bps                       |   100: 9.6kbps                        |
        |                                   |   101: 38400bps                       |   101: 19.2kbps                       |
        |                                   |   110: 57600bps                       |   110: 19.2kbps <br /> (equal to 101) |
        |                                   |   111: 115200bps                      |   111: 19.2kbps <br /> (equal to 101) |
    
    + OPTION Bit Description 

        |               Bit 7 - <br /> Fixed Transmission                                                       |               Bit 6 - <br /> I/O Drive Mode           |   Bit 5,4,3 - <br /> Wireless Wake-up Time    |   Bit 2 - <br /> Forward Error Correction (FEC) Swicth    |   Bit 1,0  - <br /> Transmission power    |
        |:-----------------------------------------------------------------------------------------------------:|:-----------------------------------------------------:|:---------------------------------------------:|:---------------------------------------------------------:|:-----------------------------------------:|
        |   0: Transparent transmission <br /> (***default***)                                                  |   0: TXD, RXD, AUX are open-collectors (open drain)   |   000: 250ms <br /> (***default***)           |   0: Turn off FEC                                         |   00: 20dBm <br /> (***default***)        |
        |   1: Fixed transmission (first <br /> three bytes can be used as <br /> high/low address and channel) |   1: TXD, RXD, AUX are push-pulls/pull-ups            |   001: 500ms                                  |   1: Turn on FEC <br /> (***default***)                   |   01: 17dBm                               |
        |                                                                                                       |                                                       |   010: 750ms                                  |                                                           |   10: 14dBm                               |
        |                                                                                                       |                                                       |   011: 1000ms                                 |                                                           |   11: 10dBm                               |
        |                                                                                                       |                                                       |   100: 1250ms                                 |                                                           |                                           |
        |                                                                                                       |                                                       |   101: 1500ms                                 |                                                           |                                           |
        |                                                                                                       |                                                       |   110: 1750ms                                 |                                                           |                                           |
        |                                                                                                       |                                                       |   111: 2000ms                                 |                                                           |                                           |
    
    + Example for Hexadecimal Commands
        + You want to change the UART baud rate to `115200bps`, parity to `8N1`, air data rate to `19.2Kbps`, use the default `433MHz` operating frequency, enable `Fixed transmission` modec, choose `Push-pull` pins, wake-up time to `250ms`, FEC `enable`, transmission power to `20dBm`. Send the commands
            ```
            0xC0 0x00 0x00 0x3D 0x17 0xC4
            ```
            
        + For reading the current options, send the commands
            ```
            0xC1 0xC1 0xC1
            ```
            
        + For reading the version number 
            ```
            0xC3 0xC3 0xC3
            ```
            
        + Resetting the module
            ```
            0xC4 0xC4 0xC4
            ```
        
    ***B. Use RF_Setting3.47 Software***
    + **STEP1: Open tool RF_Setting3.47 software**
        + Go to link PC/Laptop `...\m.e.s-iiot\4.LoRaWAN Configure` then click `RF_Setting3.47.exe`application to run  
            <img src="https://i.imgur.com/G0UvZNR.png"> 
        
    + **STEP2: Setting**
        + Choose language: `English` language or  `Chinese` language
        
            <img src="https://i.imgur.com/uhpzckv.png">
            
        + Choose `COM Port` then click to `OpenPort` for connect to LoRa 
        
            <img src="https://i.imgur.com/bzpnC5K.png">
        
        + After successfully connect to LoRa, click to `GetParam` for getting previous parameter setting then click to `OK` in notification messenger
        
            <img src="https://i.imgur.com/YqS0IeA.png"> 
        
        + Parameter setting
            + FRAME 1: 
                + Settings `Address`, the each device will set up the each different address 
                + Settings `Channel`, you can keep parameter if you develop the LoRaWAN network 
            + FRAME 2: 
                + UartRate: `9600bps` or `115200bps` standard. You can set up the different options
                + Parity: `8N1` by default. You can set up the different options
                + AirRate: `19.2Kbps` (option user). Default as `2.4Kbps`
                + Power: `20dBm` by default. You can set up the different options
                + FEC: `Enable` (option user). Default as `Disable`
                + Fixed mode: `Enable` (option user). Default as `Disable`
                + WOR timing: `250ms` by default. You can set up the different options
                + IO mode: `PushPull` by default. You can set up the different options 
            
            <img src="https://i.imgur.com/AEg7BcZ.png"> 
        
    + **STEP3: Upload parameter** 
        + After prepared parameters, click to `SetParam` for setting to LoRa then click `OK` in notification messenger
        
            <img src="https://i.imgur.com/b3L3dwu.png"> 
        
        + Dissconnect to LoRa with software, click to `ClosePort` 
        
            <img src="https://i.imgur.com/HcOfruS.png">
            
### III. OPERATION 
+ You should change 2 pins `M0 and M1` to `Low Level` when normal mode activity  
+ When you transmit data from A-LoRa to B-LoRa with the same channel in the LoRaWAN network, send the commands (Hexadecimal format)
    > ADDH ADDL CHAN DATA
    > - Notes: 
    >   - ADDH: Address high byte of LoRa receiver 
    >   - ADDL: Address low byte of LoRa receiver
    >   - CHAN: Channel of LoRa receiver 
    >   - DATA: Data from LoRa transmit to LoRa receiver 

+ Example: A LoRa is address `0x0101` send data to B LoRa is address `0x0202` with the same channel `0x01` and otherwise

    + A-LoRa (0x0101) transmit:
        ```
        0x02 0x02 0x01 0xAA 0xBB 0xCC
        ```
    + B-LoRa (0x0201) receiver:
        ```
        0xAA 0xBB 0xCC 
        ```


-------------- ALL COPYRIGHT NICHIETSU SYSTEM DEVELOPMENT CO.,LTD -----------------