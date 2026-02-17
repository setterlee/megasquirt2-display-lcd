Quick guide instructions to installing MS2/Extra 3.4.x

Full instructions can be found through the documentation on
http://www.msextra.com

1. If you are presently using a different code version on your MS2 you
should save your settings.
e.g. File->SaveAs in TunerStudio.

2. Ensure you have TunerStudio v3.x or later installed.

3. Users of FTDI USB cables - ensure you have set the port latency to 1 or
   the firmware will take forever to load. (Typical is a few minutes.)
   Windows users can change this through Device Manager. Linux users can run
   the supplied script 'linux-usb-latency'

4. Open the Tools menu in TunerStudio and select Update / Install Firmware.
(Alternatively, the old text based firmware loaders supplied in the zipfile
may be used.)

5. You may need to untick File -> Work Offline

6. If this is your first time, you need to create a project, try to use detect
and if required, browse to the ini file supplied in this distribution.

7. When upgrading, just connect using your existing project. Usually you will
need to "update Project ini" and browse to the ini file provided in this zip
file.
  megasquirt2.ini  for Megasquirt-2
  microsquirt.ini  for cased Microsquirt units
  microsquirt-module.ini for Megasquirt ECUs based on the Microsquirt-module
  mspnp2.ini for MS PNP2 plug-n-play from DIYautotune.
Note: If you don't see the "ini" file extension then fix you "Folder Options"
    in Windows by unticking "Hide known file extensions"

8. Note!! When first creating a tune, you MUST be connected to the ECU
'online' or open an existing MSQ file. If you start tuning offline with blank
settings you will create huge problems for yourself. The default 'base map'
files are provided in the tune_files directory.

Come to  www.msextra.com  for support and documentation.
