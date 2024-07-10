#!/bin/bash

# Control file for xdebug
xdebug_conf_e=/usr/local/etc/php/conf.d/docker-php-ext-xdebug.ini
xdebug_conf_d=/usr/local/etc/php/conf.d/docker-php-ext-xdebug.dis

# If the file exists, disable it by renaming s/ini/dis/
# otherwise, rename it back s/dis/ini/
if test -f "$xdebug_conf_e"; then
    echo "xdebug is currently enabled, disabling... "
    mv $xdebug_conf_e $xdebug_conf_d
else
    echo "xdebug is currently disabled, enabling... "
    mv $xdebug_conf_d $xdebug_conf_e
fi

if [ $? -eq 0 ]; then
    echo "Done."
else
    echo "Unable to toggle xdebug."
    exit 1
fi

# Restart apache
echo "Restarting Apache..."
apachectl restart
if [ $? -eq 0 ]; then
    echo "Done."
else
    echo "There was an error restarting Apache"
fi