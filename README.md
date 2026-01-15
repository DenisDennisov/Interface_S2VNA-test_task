# Test task - Interface to S2VNA

A graphical interface for the S2VNA program created as part of a test project. The interface connects via a TCP socket to the S2VNA application, which acts as a server.

#### The application is created as a portfolio and is not commercial! It was created solely as a test project.

![Screenshot](https://github.com/DenisDennisov/Interface_S2VNA-test_task/blob/main/images/main_menu.png)

## My instruments

- С++
- STL
- [Qt6](https://www.qt.io/development/qt-framework/qt6/)
- [S2VNA + Docs](https://planarchel.ru/catalog/analizatory_tsepey_vektornye/po_vna/s2vna/)
- [Enigma Virtual Box (for compile)](https://enigmaprotector.com/ru/aboutvb.html/)


## What function?

The graphical user interface has components for entering the start and end frequencies, IF filter bandwidth, number of frequency points, output power of the device, and a graph plotting area.

* The interface acts as a client. The connection is established locally: 127.0.0.1, port 5025.
An attempt to connect to the server and request data for the fields is made by clicking the "Measure" button.

* If no data was entered during the initial connection, the client requests it from the server. Data from the client takes precedence over data from the server.
## How to use?
### Start file - "interface_test_task.exe" in Release folder. 

In the Release folder there is an exe compiled using MinGW64 and assembled into 1 file using Enigma Virtual Box.


## License

This project is licensed under the Apache-2.0 License - see the [LICENSE](https://github.com/DenisDennisov/Video-downloader?tab=Apache-2.0-1-ov-file#readme) file for details.
