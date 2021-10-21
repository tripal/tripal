
TripalFieldStorage
=====================

.. image:: overview.png

Tripal will have a `TripalFieldStorage` abstract class which is a Drupal interface plugin. It will provide methods to support the following functionality

- `load`: for loading data from the underlying data store
- `save`: for inserting or updating data in the data store
- `delete`: for removing data in the data store

.. note::

    More functionality may be added to this class but for now, the design is focused on these methods.

Example Usage: Chado
-----------------------

The `TripalFieldStorage` class is database agnostic.  However, implementation of this class will be database sepcific.  The `ChadoFieldStorage` class is one such possible implementation where it is responsible for interacting with Chado in a PostgreSQL database.  We currently expect that all interactions with Chado in such a class would occur using the new `BioDB` API that is currently being proposed by Valentin.

.. note::

    As a note, we currently have the Chado API (flat functions) and the ChadoRecord class for interacting with Chado.  While these will remain for backwards compatibilty we anticipate they will be deprecated in favor of the `BioDB` API as it is matured.
