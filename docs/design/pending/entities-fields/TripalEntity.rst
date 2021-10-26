
TripalEntity
==============

.. image:: overview.png

The TripalEntity class is an instance of a ContentBaseEntity interface.  It inherits the functionality of a Drupal Entity but allows us to provide Tripal specific customizations. In particular the following functions will be overridden:

- `preSave()`
   - Cache the biological field data provided by the user.
   - Remove the biological data so Drupal doesn't store it using it's own `EntityStorageInterface`. If Drupal stores anything it will be context information for the TripalFieldStorage plugin implementations.
   - Note: Caching and then removing the biological data prevents Drupal from duplicating it.
- `postSave()`
    - Pulls the biological field data from the cache.
    - Determines the proper TripalFieldStorage implementation that is needed for each field
    - Calls the proper TripalFieldStorage instance for each field to save the data.
    - We are exploring performance improvements by passing multiple fields with the same storage at once.
- `postLoad()`
    - Determines the proper TripalFieldStorage implementation that is needed for each field.
    - Passes the identifying context information Drupal saved for each field to the correct TripalFieldStorage plugin implementation based on the field definitions.
    - TripalFieldStorage instances will load the data for each field and add it to the entity.
    - We are exploring performance improvements by passing multiple fields with the same storage at once.
