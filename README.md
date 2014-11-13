ssfh
====

Snow Stuff For Hire (SSFH) - a POS-like system designed for very quick
rental operations at small ski resorts

## What it ssfh

SSFH is a Point of Sale (POS)-like GUI program written in C++/Qt to
handle rental operations at
[Hedeland Skicenter](http://www.hedelandskicenter.dk/), which is the
largest ski resort in Denmark, run on a voluntarily basis by
[Roskilde Skiklub](http://www.roskildeskiklub.dk).

Although the largest ski resort in Denmark, it is still very small by
international standards, and are only visited on a single-day
basis. As such, customers in the ski rental needs quick handling of
rental operations, so they can go skiing for as much of the day as
possibly. The system is designed to offer this, by using the
universially held "sundhedskort" as identification for the rental
operations, and reading the information using a standard magnetic card
reader.

The actual item handling is based on barcode marking of all items, and
using a standard barcode reader to scan the barcodes. The system
supports writing receipts to a standard thermal printer.

Aside from the rental operations, the system supports some basic item
handling, such as creating and deleting items, and it supports making
a status of the items in your store.

The system is designed to be cheap to implement, using cheap hardware,
and obviously, free software.

## What platforms are supported?

SSFH is written in C++/Qt, and as such, it should be portable to all
platforms that is supported by Qt. 

The hardware interactions have only been tested under Linux,
though. The magnetic card reader and barcode scanner is assumed to
generate keyboard events, which in principle should work unchanged on
any platform, but the interaction with the thermal printer requires
opening the relevant USB port, and writing commands directly to
it. Opening the port will require some changes to work on another
platform.

### Requirements

Qt version 5.

(For audio feedback, currently mplayer - this will be changed to Qt's
multimedia platform later).

You also need a magnetic card reader, a barcode scanner, and a thermal
printer. You can test the system without, but for actual use you will
want at least a barcode scanner..

## What is the status of the system

The system has been in use for the last three seasons and does not
contain any known serious bugs.

Some values are hardcoded to support the processes at Hedeland
Skicenter, and is currently not configurable.

## What is not supported

The system does not support the calculating of prices.

## License

SSFH is licensed under GPL v3 or later. This is not currently marked in any parts of the code.

## Compiling

SSFH uses the Qt tool qmake to build and compile. Run qmake, then
make, to build it.

## Extremely quick test

If you just want to check the program out, here are the steps to get you started:

```shell
git clone https://github.com/madsdyd/ssfh.git
cd ssfh
qmake
make -j4
cd schema
make reset-db
cd ../src
./SnowStuffForHire.bin ../schema/test.db /tmp/ssfh.log
```
