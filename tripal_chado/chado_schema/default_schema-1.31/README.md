
# Chado version 1.31

This directory contains a number of SQL files that combined originate from the
official GMOD Chado 1.31. More specifically, these files originate from
https://github.com/GMOD/Chado/blob/1.4/schemas/1.31/default_schema.sql.

This original version was separated into it's component parts to ensure that
tables, functions and views are being created in the right schema. This was
needed because Tripal allows multiple instances of the Chado schema, each
with their own unique name, but only keeps around one version of the accessory
so, frange and genetic_code schema. Furthermore, in order to ensure that the
various functions can find each other, these all go into the main public schema.

The files should be applied in the following order:

1. Chado Schema tables: default_schema-1.31-chado.sql
2. Genetic Code schema: default_schema-1.31-genetic_code.sql
3. SO schema: default_schema-1.31-so.sql
4. Frange schema: default_schema-1.31-frange.sql
5. PostgreSQL Views for chado schema: default_schema-1.31-views.sql
6. PostgreSQL Functions for chado schema: default_schema-1.31-functions.sql
7. PostgreSQL Functions for so schema: default_schema-1.31-so-functions.sql
8. PostgreSQL Functions for frange schema: default_schema-1.31-frange-functions.sql
