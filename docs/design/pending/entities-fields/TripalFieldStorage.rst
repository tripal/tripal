
TripalFieldStorage
=====================

.. image:: overview.png

Tripal will have a `TripalFieldStorage` abstract class which is a Drupal plugin interface. It will provide methods to support the following functionality

- `load`: for loading data from the underlying data store
- `save`: for inserting or updating data in the data store
- `delete`: for removing data in the data store

We are ensuring this plugin is completely agnostic to the details mentioned regarding the TripalEntity class. This ensures that this class will be unaffected if the design of entities changes. It also allows this plugin to be more intuitive and easier to implement then alternate data storage in Tripal3.

.. note::

    More functionality may be added to this class but for now, the design is focused on these methods.

Example Usage: Chado
-----------------------

The `TripalFieldStorage` class is data store agnostic.  However, implementation of this class will be data store specific.  The `ChadoFieldStorage` class is one such possible implementation where it is responsible for interacting with Chado in a PostgreSQL database.  We currently expect that all interactions with Chado in such a class would occur using the new `BioDB` API that is currently being proposed by Valentin.

.. note::

    As a note, we currently have the Chado API (flat functions) and the ChadoRecord class for interacting with Chado.  While these will remain for backwards compatibility we anticipate they will be deprecated in favour of the `BioDB` API as it is matured.
