
TripalEntity
==============

.. image:: overview.png

The TripalEntity class is an instance of a ContentBaseEntity interface.  It inherits the functionality of a Drupal Entity but allows us to provide Tripal specific customizations. In particular the following functions will be overridden:

- `preSave()`
   - Cache the field data provided by the user.
   - Simplify the biological data to identifying information only for Drupal to store using it's own `EntityStorageInterface`.
   - Note: Caching and then removing the biological data prevents Drupal from duplicating it.
- `postSave()`
    - Pulls the biological field data data from the cache
    - Determines the proper TripalFieldStorage implementation that is needed for each field
    - Calls the proper TripalFieldStorage instance for each field to save the data.
- `postLoad()`
    - Determines the proper TripalFieldStorage implementation that is needed for each field
    - Uses the identifying information Drupal saved for each field
    - Calls the proper TripalFieldStorage instance for each field to load the data.
