#!/bin/bash -e

SCRIPT_DIR=`pwd`/`dirname $0`

INSTALL_LIB=/usr/lib
INSTALL_BIN=/usr/bin
INSTALL_ETC=/usr/etc/primesense
INSTALL_RULES=/etc/udev/rules.d

if [ "`uname -s`" == "Darwin" ]; then
    MODULES="libXnDeviceSensorV2.dylib libXnDeviceFile.dylib"
else
    MODULES="libXnDeviceSensorV2.so libXnDeviceFile.so"
fi

RULES_FILE="55-primesense-usb.rules"

# read arguments
INSTALL="1"

while (( "$#" )); do
	case "$1" in
	"-i")
		INSTALL="1"
		;;
	"-u")
		INSTALL="0"
		;;
	*)
		echo "Usage: $0 [options]"
		echo "Available options:"
		printf "\t-i\tInstall (default)\n"
		printf "\t-u\tUninstall\n"
		exit 1
		;;		
	esac
	shift
done

# create file list
LIB_FILES=`ls $SCRIPT_DIR/Lib/*`
BIN_FILES=`ls $SCRIPT_DIR/Bin/*`

if [ "$INSTALL" == "1" ]; then

    # create config dir
    printf "creating config dir $INSTALL_ETC..."
    mkdir -p $INSTALL_ETC
    printf "OK\n"

    # Copy shared libraries
    printf "copying shared libraries..."
    cp $LIB_FILES $INSTALL_LIB
    printf "OK\n"

    # Copy executables
    printf "copying executables..."
    cp $BIN_FILES $INSTALL_BIN
    printf "OK\n"

    # register modules
    for module in $MODULES; do
        printf "registering module '$module' with OpenNI..."
	niReg -r $INSTALL_LIB/$module $INSTALL_ETC
        printf "OK\n"
    done

    # copy config file
    printf "copying server config file..."
    cp Config/GlobalDefaults.ini $INSTALL_ETC
    printf "OK\n"

    # make server run as root
    printf "setting uid of server..."
    chown root $INSTALL_BIN/XnSensorServer
    chmod +s $INSTALL_BIN/XnSensorServer
    printf "OK\n"

    # create server log dir
    printf "creating server logs dir..."
    mkdir -p /var/log/primesense/XnSensorServer
    # make this dir readable and writable by all (we allow anyone to delete logs)
    chmod a+w /var/log/primesense/XnSensorServer
    printf "OK\n"

    if [ "`uname -s`" != "Darwin" ]; then
        # install USB rules (so that PrimeSense sensors will be mounted with write permissions)
        printf "installing usb rules..."
        cp Install/$RULES_FILE $INSTALL_RULES
        printf "OK\n"
    fi

else #uninstall

    # unregister modules
    for module in $MODULES; do
    	printf "unregistering module '$module' from OpenNI..."
        if niReg -u $INSTALL_LIB/$module; then
            printf "OK\n"
        fi
    done

    # delete shared libraries
    printf "removing shared libraries..."
    for filename in $LIB_FILES; do
        rm -f $INSTALL_LIB/`basename $filename`
    done
    printf "OK\n"
	
    # delete executables
    printf "removing executables..."
    for filename in $BIN_FILES; do
        rm -f $INSTALL_BIN/`basename $filename`
    done
    printf "OK\n"
	
    # delete config dir
    printf "removing config dir..."
	rm -rf $INSTALL_ETC
    printf "OK\n"

    if [ "`uname -s`" != "Darwin" ]; then
        # remove USB rules
        printf "removing usb rules..."
	    rm -f $INSTALL_RULES/$RULES_FILE
        printf "OK\n"
    fi

fi

printf "\n*** DONE ***\n\n"
