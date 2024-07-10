# pg-clone-schema

This directory contains PostgreSQL schema cloning utility. The very original
script come from a comment of on PostgreSQL official wiki by Emanuel '3manuek'
https://wiki.postgresql.org/wiki/Clone_schema
It has been updated to support newer version of PostgreSQL with a couple of bug
fixes on the following github project called "pg-clone-schema":
https://github.com/denishpatel/pg-clone-schema
The pg-clone-schema project comes with an MIT License.

## Modifications for Drupal/Tripal

The clone schema script has been modified to fulfill Drupal and Tripal needs.
Specifically, it was altered to ensure it could be run multiple times without
causing problems. For instance, use "CREATE OR REPLACE" or "IF NOT EXISTS"
expressions when possible. Furthermore, an additional script has been added
to remove added functions when the module is uninstalled.

All functions added are created within the Drupal schema in order to be available
to all Tripal DBX managed schema through a single instance.

## Original Documentation

Handles following objects:

* Tables - structure (indexes and keys) and optionally, data
* Views
* Materialized Views - Structure and data
* Sequences
* Functions/Procedures
* Types (composite and enum)
* Collations and Domains
* Triggers
* Permissions/GRANTs

Arguments:

* source schema
* target schema
* clone with data
* only generate DDL

You can call function like this to copy schema with data:

```
select clone_schema('sample', 'sample_clone', true, false);
```

Alternatively, if you want to copy only schema without data:

```
select clone_schema('sample', 'sample_clone', false, false);
```

If you just want to generate the DDL, call it like this:

```
select clone_schema('sample', 'sample_clone', false, true);
```

In this case, standard output with "INFO" lines are the generated DDL.

### Limitations

* Foreign Tables are not handled at the present time.  They must be done manually.
* DDL only option is not complete since it depends on objects that aren't created yet. See issue#29

Sponsor:
 http://elephas.io/

Compare cloning with EnterpriseDB's version that only works with their Advanced Server:
https://www.enterprisedb.com/edb-docs/d/edb-postgres-advanced-server/user-guides/user-guide/11/EDB_Postgres_Advanced_Server_Guide.1.078.html
