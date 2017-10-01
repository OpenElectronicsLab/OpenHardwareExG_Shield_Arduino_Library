#!/usr/bin/perl

use strict;
use warnings;
use Time::HiRes;

use Device::SerialPort;

# turn off output buffering
$| = 1;

my $port;
if ( -e "/dev/ttyACM0" ) {
    $port = Device::SerialPort->new("/dev/ttyACM0");
}
else {
    $port = Device::SerialPort->new("/dev/ttyUSB0");
}
$port->databits(8);
$port->baudrate(115200);
$port->parity("none");
$port->stopbits(1);

while (1) {

    # Poll to see if any data is coming in
    my $received = $port->lookfor();

    # If we get data, then print it
    if ($received) {
        print $received, "\n";
    }
    else {
        Time::HiRes::usleep(2000);
    }
}
