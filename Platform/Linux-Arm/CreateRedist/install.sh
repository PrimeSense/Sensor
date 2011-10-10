#!/bin/sh -e

SCRIPT_DIR=`pwd`/`dirname $0`

STAGING_DIR=$2

INSTALL_LIB=$STAGING_DIR/usr/lib
INSTALL_BIN=$STAGING_DIR/usr/bin
INSTALL_ETC=$STAGING_DIR/usr/etc/primesense

# Create staging dirs if needed
if [ "$STAGING_DIR" != "" ]; then

	mkdir -p $STAGING_DIR
	mkdir -p $INSTALL_LIB
	mkdir -p $INSTALL_BIN
fi 

MODULES="libXnDeviceSensorV2.so libXnDeviceFile.so"
RULES_FILE="55-primesense-usb.rules"

# create file list
LIB_FILES=`ls $SCRIPT_DIR/Lib/*`
BIN_FILES=`ls $SCRIPT_DIR/Bin/*`

INSTALL=1
if [ "$1" == "-u" ]; then
	INSTALL=0
else
    if [ "$1" != "-i" ]; then
	echo "Usage: $0 [options]"
	echo "Available options:"
	printf "\t-i\tInstall (default)\n"
	printf "\t-u\tUninstall\n"
	exit 1
    fi
fi

if [ $INSTALL == 1 ]; then 
	printf "Installing PrimeSense Sensor\n"
	printf "****************************\n\n"

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

	# copy config file
	printf "copying server config file..."
	cp Config/GlobalDefaults.ini $INSTALL_ETC
	printf "OK\n"

	if [ "$STAGING_DIR" == "" ]; then
		
		# register modules
		for module in $MODULES; do
			printf "registering module '$module' with OpenNI..."
			niReg -r $INSTALL_LIB/$module $INSTALL_ETC
			printf "OK\n"
		done

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
	fi
else
	printf "Uninstalling PrimeSense Sensor\n"
	printf "******************************\n\n"

	if [ "$STAGING_DIR" == "" ]; then
		# unregister modules
		for module in $MODULES; do
			printf "unregistering module '$module' from OpenNI..."
			if niReg -u $INSTALL_LIB/$module; then
	    			printf "OK\n"
			fi
		done 
	fi

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

	printf "\n*** DONE ***\n\n"
fi
