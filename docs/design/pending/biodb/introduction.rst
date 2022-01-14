
Introduction
============

At its very beginning, Tripal was created to enable the use of Chado schema
under Drupal CMS. Since its version 3, Tripal design changed in order to be
ontology driven (like Chado) but database agnostic. However, Tripal version 3
was only able to support the Chado database schema. With version 4, a new
biological database layer has been added: the *Biological Database* layer. This
layer provides a database API extending Drupal database API that enables the use
of other database schemas in Drupal. While it is currently limited to PostgreSQL
database type, it has no number or type of schema limit. It means that it
supports querying more than one schema at a time, and it is not limited to Chado
or Drupal.

Since the Biological Database layer is an API, it does not work on it own but is
rather a basis to build other extensions that will work on proprietary schema
definitions like Chado. Therefore, the Tripal Chado extension is provided within
Tripal package as an implementation of the Biological Database Layer for Chado.

To sum up:

 - The Biological Database API provides an abstraction layer to support any
   biological database schema.
 - It supports multiple schemas and cross-schema queries.
 - Tripal Chado extension is an implementation of the Biological Database API
   to support Chado schema.
