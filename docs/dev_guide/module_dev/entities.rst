
Pages and Page Types
=======================

In Drupal terminology, each Tripal content page is an content entity (e.g. MyFavGene-1) and each Tripal Content Type (e.g. Genes) is a configuration entity. Tripal core has already done the work of defining these to Drupal and creates a number of common content types on install.

  A content entity (or more commonly, entity) is an item of content data, which can consist of text, HTML markup, images, attached files, and other data that is intended to be displayed to site visitors. Content entities can be defined by the core software or by modules.

  Content entities are grouped into entity types, which have different purposes and are displayed in very different ways on the site. Most entity types are also divided into entity sub-types, which are divisions within an entity type to allow for smaller variations in how the entities are used and displayed.

  *Excerpt from* `Official Drupal Docs: What are Content Entities and Fields <https://www.drupal.org/docs/user_guide/en/planning-data-types.html>`_

This architecture allows you to categorize your data by type (e.g. gene versus germplasm variety) and provide specialized displays specific to each type.

Both Tripal content and Tripal content types can be created through the administrative user interface or programmatically. Tripal content entities and entity types have extended Drupal's default content entities to provide functionality specific to biological data. As such we recommend you create custom Tripal Content types rather then using the Drupal API directly.

Additional Resources:
 - `Official Drupal Docs: What are Content Entities and Fields <https://www.drupal.org/docs/user_guide/en/planning-data-types.html>`_
 - `Official Drupal Docs: Introduction to Entity API in Drupal 8 <https://www.drupal.org/docs/8/api/entity-api/introduction-to-entity-api-in-drupal-8>`_
 - `Official Drupal Docs: Entity Types <https://www.drupal.org/docs/8/api/entity-api/entity-types>`_
 - `Unleashed Technologies: Drupal Entities - Part 1: What are they? <https://www.unleashed-technologies.com/blog/2017/04/10/drupal-entities-part-1-what-are-they>`_
 - `Drupalize Me: Entity API Overview <https://drupalize.me/tutorial/entity-api-overview?p=2792>`_
