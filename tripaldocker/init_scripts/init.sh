#!/usr/bin/env bash

## Order is very important ;-p The apache command should always be last!

## Start PostgreSQL
service postgresql start

## Start Apache
apache2ctl -D FOREGROUND
