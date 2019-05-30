Data Loading/Collection
=======================

The following modules provide interfaces for collection and/or loading of biological data.

Genotype Loader
----------------

A Drush-based loader for VCF files that follows the genotype storage rules outline by ND genotypes. It has been optimized to handle very large files and supports customization of ontology terms used.

`Documentation <https://genotypes-loader.readthedocs.io/en/latest/>`__
`Repository <https://github.com/UofS-Pulse-Binfo/genotypes_loader>`__

Mainlab Chado Loader
---------------------

MCL (Mainlab Chado Loader) is a module that enables users to upload their biological data to Chado database schema. Users are required to transfer their biological data into various types of data template files. MCL, then, uploads these data template files into a chado schema.

`Documentation <https://gitlab.com/mainlabwsu/mcl/blob/master/README.md>`__
`Repository <https://gitlab.com/mainlabwsu/mcl>`__

Raw Phenotypes
---------------

This module was designed to aid in collection and further analysis of raw phenotypic data. It supports Excel drag-n-drop uploads with immediate validation and researcher feedback. Additionally, it provides summary charts and download functionality.

`Documentation <https://github.com/UofS-Pulse-Binfo/rawphenotypes/blob/master/README.md>`__
`Repository <https://github.com/UofS-Pulse-Binfo/rawphenotypes>`__

Tripal BibTeX
--------------

A BibTEX importer for Tripal Publications. Currently this module only provides a Drush function (``tripal-import-bibtex-pubs``; ``trpimport-bibtex``) for import of BibTEX files.

`Documentation <https://github.com/UofS-Pulse-Binfo/tripal_bibtex/blob/7.x-3.x/README.md>`__
`Repository <https://github.com/UofS-Pulse-Binfo/tripal_bibtex>`__

Tripal Plant PopGen Submission
-------------------------------

The Tripal Plant PopGen Submit (TPPS) Module supports a flexible submission interface for genotype, phenotype, environmental, and metadata for population, association, or landscape genetics studies. The portal walks the user through specific questions and collects georeferenced coordinates on plant accessions and also supports ontology standards, including the Minimal Information About a Plant Phenotyping Experiment (MIAPPE) (http://www.miappe.org/) and standard genotyping file formats, such as VCF.

`Documentation <https://tpps.readthedocs.io/en/latest/>`__
`Repository <https://gitlab.com/TreeGenes/TGDR>`__

Migrate Chado
-------------

This module is a collection of destination plugins to import biological data to a Chado database using `Drupal Migrate <https://www.drupal.org/project/migrate>`_. The Migrate module provides a flexible framework for migrating content into Drupal from other sources (e.g., when converting a web site to Drupal). Content is imported and rolled back using a bundled web interface (Migrate UI module) or included Drush commands (strongly recommended).

`Documentation <https://www.drupal.org/docs/7/modules/migrate-chado>`__
`Repository <https://www.drupal.org/project/migrate_chado>`__

Tripal Contact Profile
-----------------------
[![Tripal Rating Silver Status](https://tripal.readthedocs.io/en/7.x-3.x/_images/Tripal-Silver.png)](https://tripal.readthedocs.io/en/7.x-3.x/extensions/module_rating.html#Silver)

The Tripal Contact Profile (TCP) Module allows Tripal users to create their own extended profiles which can optionally be displayed using our auxillary Colleagues Directory module. It does this by creating a Tripal Contact Profile content type which can also be administered directly from the standard Tripal Content Type administration interface. The module also provides a well organized administration user interface with many value added options including additional form validation, conversion of standard text input fields into more user friendly fields for better contact recording. There is even an option to override the standard registration form into a multistep registration form allowing users to sign up and created both their standard user account as well as a TCP profile in a single process. Check out the documentation for more!

`Documentation <https://tripal-contact-profile.readthedocs.io/en/latest/>`__
`Repository <https://gitlab.com/TreeGenes/tripal_contact_profile>`__
