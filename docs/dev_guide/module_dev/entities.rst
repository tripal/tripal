
Pages and Page Types
=======================

In Drupal terminology, each Tripal content page is an content entity (e.g. MyFavGene-1) and each Tripal Content Type (e.g. Genes) is a configuration entity. Tripal core has already done the work of defining these to Drupal and creates a number of common content types on install.

Both Tripal content and Tripal content types can be created through the administrative user interface or programatically. Tripal content entities and entity types have been extended to provide functionality specific to biological data. As such we recommend you create custom Tripal Content types rather then using the Drupal API directly.

Additional Resources:
 - `Official Drupal Docs: Content Entity and Field Concepts <https://www.drupal.org/docs/user_guide/en/planning-data-types.html>`_
 - `Official Drupal Docs: Working with the entity API <https://www.drupal.org/docs/8/api/entity-api/working-with-the-entity-api>`_
 - `Drupalize Me: Entity API Overview <https://drupalize.me/tutorial/entity-api-overview?p=2792>`_
