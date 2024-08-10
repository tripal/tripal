#!/usr/bin/env bash

supervisord -c /etc/supervisord.conf

## Now handled by Supervisord:
## Start PostgreSQL
# service postgresql start
## Start Apache
# apache2ctl -D FOREGROUND
