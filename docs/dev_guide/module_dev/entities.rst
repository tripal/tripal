
Pages and Page Types
=======================

In Drupal terminology, each Tripal content page is an content entity (e.g. MyFavGene-1) and each Tripal Content Type (e.g. Genes) is a configuration entity. Tripal core has already done the work of defining these to Drupal and creates a number of common content types on install.

  A content entity (or more commonly, entity) is an item of content data, which can consist of text, HTML markup, images, attached files, and other data that is intended to be displayed to site visitors. Content entities can be defined by the core software or by modules.

  Content entities are grouped into entity types, which have different purposes and are displayed in very different ways on the site. Most entity types are also divided into entity sub-types, which are divisions within an entity type to allow for smaller variations in how the entities are used and displayed.

  *Excerpt from* `Official Drupal Docs: What are Content Entities and Fields <https://www.drupal.org/docs/user_guide/en/planning-data-types.html>`_

This architecture allows you to categorize your data by type (e.g. gene versus germplasm variety) and provide specialized displays specific to each type.

Additional Resources:
 - `Official Drupal Docs: What are Content Entities and Fields <https://www.drupal.org/docs/user_guide/en/planning-data-types.html>`_
 - `Official Drupal Docs: Introduction to Entity API in Drupal 8 <https://www.drupal.org/docs/8/api/entity-api/introduction-to-entity-api-in-drupal-8>`_
 - `Official Drupal Docs: Entity Types <https://www.drupal.org/docs/8/api/entity-api/entity-types>`_
 - `Unleashed Technologies: Drupal Entities - Part 1: What are they? <https://www.unleashed-technologies.com/blog/2017/04/10/drupal-entities-part-1-what-are-they>`_

Create Custom Tripal Content Type
----------------------------------

.. code::

  // First create the controlled vocabulary.
  $vocab = \Drupal\tripal\Entity\TripalVocab::create();
  $vocab->setLabel('SO');
  $vocab->setName('sequence');
  $vocab->setDescription('The Sequence Ontology is a set of terms and relationships used to describe the features and attributes of biological sequence. SO includes different kinds of features which can be located on the sequence.');
  $vocab->save();

  // Or load an existing 

  // Next Create the controlled vocabulary term.
  $term = \Drupal\tripal\Entity\TripalTerm::create();
  $term->setVocabID($vocab->id())
  $term->save();

.. warning::

  The functionality behind this page is under active development and as such, the documentation is not yet complete.

Create Tripal Content
-----------------------
