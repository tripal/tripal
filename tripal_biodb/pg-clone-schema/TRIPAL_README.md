# About pg-clone-schema included in Tripal

This directory contains PostgreSQL schema cloning utility. The very original
script come from a comment of on PostgreSQL official wiki by Emanuel '3manuek'
https://wiki.postgresql.org/wiki/Clone_schema
It has been updated to support newer version of PostgreSQL with a couple of bug
fixes on the following github project called "pg-clone-schema":
https://github.com/denishpatel/pg-clone-schema
The pg-clone-schema project comes with an MIT License.

The clone schema script has been modified to fulfill Drupal and Tripal needs.
An additional script has been added to remove added functions when the module is
uninstalled.
