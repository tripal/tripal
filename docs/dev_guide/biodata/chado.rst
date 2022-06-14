
GMOD Chado Schema Integration
=================================

.. _GMOD: http://gmod.org/wiki/Main_Page

.. _Chado: http://gmod.org/wiki/Introduction_to_Chado

The Tripal Chado module provides integration between Tripal and the GMOD Chado schema. This provides flexible, default storage for many biological data types including genes, genetic markers, germplasm, as well as associated meta data such as project and analysis details. More specifically:

	Chado is a relational database schema that underlies many GMOD installations. It is capable of representing many of the general classes of data frequently encountered in modern biology such as sequence, sequence comparisons, phenotypes, genotypes, ontologies, publications, and phylogeny. It has been designed to handle complex representations of biological knowledge and should be considered one of the most sophisticated relational schemas currently available in molecular biology. The price of this capability is that the new user must spend some time becoming familiar with its fundamentals.

	--  `GMOD Chado Documentation <https://chado.readthedocs.io/en/rtd/>`_

Chado was selected for Tripal because it is open-source, it is maintained by the community in which anyone can provide input, and use of Chado_ encourages common data storage between online biological sites which decreases duplication of effort.

Chado_ is meant to be installed into a PostgreSQL database and is designed to house a variety of biological data.   For example, Tripal comes with a variety of content types. However, if you want to create new content types you must know how that data will be stored in Chado_.  Additionally, use of the Bulk Loader (a tab-delimited data loader for custom data formats) requires a good understanding of Chado_.  Finally, creating extensions to Tripal requires an understanding of Chado_ to write SQL and or new Tripal fields.  The following links provide training for Chado_.


.. csv-table::
	:header: "Resource", "Link"

	"Chado Home Page", "http://gmod.org/wiki/Chado"
	"Chado Tutorial", "http://gmod.org/wiki/GMOD_Online_Training_2014/Chado_Tutorial"
	"Chado ReadtheDocs", "https://chado.readthedocs.io/en/rtd/"
	"Chado Table List", "https://chado.readthedocs.io/en/rtd/_static/schemaspy_integration/index.html"
	"Chado Best Practices", "http://gmod.org/wiki/Chado_Best_Practices"
	"Chado GitHub", "https://github.com/GMOD/Chado"

Chado Installation
--------------------

When you install the Tripal Chado module you will be automatically prompted to install Chado. This creates a schema within your Drupal database to house the Chado tables listed in the resources above. To install Chado manually navigate to Structure > Tripal > Data Storage > Chado > Install Chado. Then just choose your version and run the associated Tripal job.

If you need to install Chado programmatically, use the following service from within a fully bootstrapped Tripal site.

.. code-block:: php
	:caption: Installs Chado version 1.3 in a schema named 'chado'.

	$installer = \Drupal::service('tripal_chado.chadoInstaller');
	$installer->setSchema('chado');
	$success = $installer->install(1.3);

Alternatively, you can install chado via the command line using the following Drush command.

.. code-block:: php
	:caption: Installs Chado version 1.3 in a schema named 'chado' using Drush.

	drush trp-install-chado --schema-name='chado' --version=1.3

Tripal Vocabularies & Terms
-----------------------------

Tripal Vocabularies and Terms are database agnostic and store their details in the Drupal database as controlled by the Drupal Entity API. Tripal has implemented a TripalTermStorage Plugin to allow Tripal extension modules to provide additional storage for Tripal Vocabularies, IDSpaces and Terms. The core Tripal Chado module has implemented this plugin to ensure these Tripal entities are linked to their Chado equivalents.

The following describes the mapping between Tripal Entities and their Chado counterparts:

 - Tripal Vocabularies (see TripalVocab class) = cv

 		- TripalVocab::namespace = cv.name
		- TripalVocab::name = cv.definition
		- TripalVocab::url = db.url

 - Tripal Vocabulary IDSpaces (see TripalVocabSpace class) = db

 		- TripalVocabSpace::name = db.name
		- TripalVocabSpace::description = db.description
		- TripalVocabSpace::urlprefix = db.urlprefix

 - Tripal Terms (see TripalTerm class) = cvterm and dbxref

 		- TripalTerm::name = cvterm.name
		- TripalTerm::definition = cvterm.definition
		- TripalTerm::accession = dbxref.accession

For information on this mapping on a per entity basis, the chado details have been added to the Tripal entities. The following examples show how to access them.

.. code-block:: php

	// First, retrieve the Tripal Vocabulary object.
	$tripalvocab = \Drupal::service('tripal.tripalVocab.manager')->getVocabularies([
		'namespace' => 'sequence',
	]);

	// Now access the cv.cv_id from that object.
	$cv_id = $tripalvocab->chado_record_id;

	// You can also access the chado record directly...
	$cv = $tripalvocab->chado_record;

	// The same pattern holds true for IDspaces and Terms.
	$db_id = $tripalIDSpace->chado_record_id;
	$db = $tripalIDSpace->chado_record;
	$cvterm_id = $tripalTerm->chado_record_id;
	$cvterm = $tripalTerm->chado_record;
