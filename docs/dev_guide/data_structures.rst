Tripal Data Structures
=======================

This page explains the relationships between Entity types, Bundles (content type), Entities and Fields. These are data structures provided by Drupal that Tripal v3 uses to expose biological content through your Drupal site, and to provide flexibility in site customization.  It is important to understand these terms and their relationships before developing new modules or custom functionality.   A quick summary of these Drupal data structures are as follows:

* Entity:  a discrete data record.  Entities are most commonly are seen as "pages" on a Drupal web site.
* Field:  an "atomic" piece of ancillary data that describes, defines or expands the entity.  Common fields include the name of an entity, a "body" of descriptive text, etc.
* Bundle:  a content type.  An entity must always have a content type.  Drupal provides several content types by default:  Basic Page and Article.  Tripal provides biological bundles (e.g. genes, organisms, etc).
* Entity Type:  despite the confusing name, an entity type is simply a group of bundles that are somehow related.  Drupal provides a "Node" Entity type that includes the Page and Article bundles.  All of the Tripal bundles (content types) belong to the TripalEntity Entity type.


The following figure describes the hierarchical relationship between Drupal Entity types (e.g. Node) in comparison with TripalEntity entity types (e.g. Chromosome, Germplasm, etc.).


.. figure:: /_images/dev_guide/data_structures/Terminology-Diagram.png
   :scale: 100 %
   :alt: Entity terminology guide


Furthermore, fields are "attached" to a Bundle and hold unique values for each Entity. The following figure describes this relationship for a Gene Bundle that has several fields attached: name, description and organism.  Note that in this figure the Entity and each of the Fields are defined using a controlled vocabulary term.  As a result, bundles and fields can be described using the `Semantic Web <https://en.wikipedia.org/wiki/Semantic_Web>`_ concepts of a "subject", "predicate" and "object".  We will discuss more on controlled vocabularies a bit later in the Handbook.



.. figure:: /_images/dev_guide/data_structures/Tripal-Semantic-web.png
   :scale: 50 %
   :alt: Semantic web



Bundles (Content Types)
-----------------------

Bundles are types of content in a Drupal site.  By default, Drupal provides the Basic Page and Article content types, and Drupal allows a site developer to create new content types on-the-fly using the administrative interface--no programming required.  Tripal also provides several Content Type by default. During installation of Tripal the Organism, Gene, Project, Analysis and other content types are created automatically.  The site developer can then create new content types for different biological data--again, without any programming required.

In order to to assist with data exchange and use of common data formats, Tripal Bundles are defined using a controlled vocabulary term (cvterm). For example, a "Gene" Bundle is defined using the Sequence Ontology term for gene whose term accession is: SO:0000704. This mapping allows Tripal to compare content across Tripal sites, and expose data to computational tools that understand these vocabularies. You can create as many Tripal Content Types as you would like through Administration > Structure > Tripal Content Types provided you can define it using a controlled vocabulary term.

By default, Tripal uses Chado as its primary data storage back-end.  When a bundle is created, the Tripal Chado module allows you to map a Bundle to a table in Chado.  Thus, any content type desired can be define as well as how it is stored in Chado--all using the administrative interface.

Entity
------

An entity is a discrete data record.  Entities are most commonly seen as "pages" on a Drupal web site and are instances of a Bundle (i.e content type). When data is published on a Tripal site such as organisms, genes, germplasm, maps, etc., each record is represented by a single entity with an entity ID as its only attribute.   All other information that the entity provides is made available via Fields.

Fields
------

A field is a reusable "data container" that is attached to a Bundle. Programmatically, each field provides one or more primitive data types, with validators and widgets for editing and formatters for display. Each field independently manages the data to which it assigned.  Just like with Bundles, Fields are also described using controlled vocabulary terms.  For example, a gene Bundle has a field attached that provides the name of the gene.   This field only provides the name and nothing more.  Tripal uses the `schema:name <http://schema.org/name>`_ vocabulary term to describe the field.

Field Instances
---------------

Fields describe "atomic" units of data that are associated with an entity.  For example, a "name" is an atomic unit of data about a Gene or Organism entity. Fields can be reused for multiple Bundles. For example, gene, mRNA, genetic markers and variants all have name data.  Despite that all of these Bundles provides a "name", we only need one field to describe that this data is a "name".  However, we may want to customize a field specific to each bundle.  Therefore, an Instance of a field is attached to a bundle, and field instances can then be customized differently.  The most important customization is the one that defines the Chado table from which the data for a field is retrieved.   Despite that field instances are attached to bundles, they become visible with Entities.  When an entity is loaded for display, Drupal examines all of the fields that are attached to the entity's bundle, and then populates the fields instances with data specific to the entity being loaded.

Entity Types
------------

An entity type is simply a group of Bundles that have some similarity.  For examples Drupal provides a Node entity type. The Node entity type contains the Basic Page and Article Bundles.  Tripal v2 expanded the Node entity type when creating new content.  Tripal v3, however, uses an a new entity type named TripalEntity that provides the Organism, Gene, Analysis, etc. content types.  Using these new entity types provides a a more responsive solution then the Node entity type, is more flexible, and supports the new ontology-driven approach of Tripal v3.
