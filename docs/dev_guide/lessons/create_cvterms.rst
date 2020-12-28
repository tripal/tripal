
Controlled Vocabulary Terms Create/Load
=========================================

This lesson will teach you how to programmatically create new controlled vocabularies (CVs) and their terms (CVterms). It will also show you how to load both CVs and CVterms which already exist in your Tripal site.

.. warning::

  This lesson was written before Chado integration. As such it shows how database agnostic Tripal 4 is. In the future we will add a section about how to connect these terms to your Chado cvterms and how to create terms from those existing in Chado.

.. note::

  You can try out this code anywhere in your Tripal extension module except in Twig templates. If you do not have a Tripal extension module, you can use the `Devel PHP <https://www.drupal.org/project/devel_php>`_ Drupal module on your **development Tripal site** and then execute the code at ``https://yourdrupalsite/devel/php``.

Controlled Vocabularies (CVs)
------------------------------

.. note::

  **Background:** :doc:`/dev_guide/cvterms`

  Questions:
    - How do I programmatically create a controlled vocabulary?
    - How do I load existing controlled vocabularies?
    - How do I access values once I have the vocabulary?


Create CV
^^^^^^^^^^

The following code demonstrates how to create a Tripal Controlled Vocabulary (CV) for the sequence ontology.

.. code:: php

  $details = [
    'short_name' => 'SO',
    'name' => 'sequence',
    'description' => 'The Sequence Ontology is a set of terms and relationships used to describe the features and attributes of biological sequence. SO includes different kinds of features which can be located on the sequence.'
  ];
  \Drupal::service('tripal.tripalVocab.manager')->addVocabulary($details);

This code sample simply describes the controlled vocabulary you would like to create as an associative array and then uses the ``tripal.tripalVocab.manager`` service to create it.

You can check that the CV saves properly by navigating to Home > Administration > Structure > Tripal Controlled Vocabularies (``admin/structure/tripal_vocab``) and ensuring your new CV is in the list of existing CVs.

.. image:: images/create_cvterms.1.png

Load CV
^^^^^^^^^

Now that you have at least one CV, you can load an existing CV. This is demonstrated in the following code.

.. code:: php

  $vocab = \Drupal::service('tripal.tripalVocab.manager')->getVocabularies([
    'name' => 'sequence',
    'short_name' => 'SO'
  ]);

The getVocabularies() method allows you to retrieve Tripal Vocabulary objects using their name and/or short_name.

Once you have a Tripal Vocabulary object, you can retrieve the value of various properties by using the following methods:

.. code:: php

  $id = $vocab->id();
  $short_name = $vocab->getLabel();
  $humanreadable_name = $vocab->getName();
  $description = $vocab->getDescription();

Controlled Vocabulary Terms (CVterms)
---------------------------------------

.. note::

  **Background:** :doc:`/dev_guide/cvterms`

  Questions:
    - How do I programmatically add a term to an existing vocabulary.
    - How do I load an existing CVterm?
    - How do I access values once I have the term?

Create CVterm
^^^^^^^^^^^^^^^

The following code demonstrates how to create a Tripal Controlled Vocabulary Term (CVterm) in the "sequence ontology" controlled vocabulary (CV). Specifically, we are going to create a Tripal CVterm for the `gene <http://www.sequenceontology.org/miso/release_2.5/term/SO:0000704>`_ term from the sequence ontology.

.. code:: php

  $details = [
    'accession' => '0000704',
    'name' => 'gene',
    'vocabulary' => [
      'name' => 'sequence',
      'short_name' => 'SO',
    ],
    'definition' => 'A region (or regions) that includes all of the sequence elements necessary to encode a functional transcript. A gene may include regulatory regions, transcribed regions and/or other functional sequence regions.',
  ];
  \Drupal::service('tripal.tripalTerm.manager')->addTerm($details);

This follows the same format as for creating the sequence ontology CV. First we describe the term we want to create including the Tripal Vocabulary and then we use the ``tripal.tripalTerm.manager`` service to create it. This service will create the controlled vocabulary if it doesn't already exist!

To check if your CVterm was created properly you can look on the listing at Home > Administration > Structure > Tripal Controlled Vocabulary Terms (``admin/structure/tripal_term``) and ensuring your new CVterm is in the list of existing CVterms.

.. image:: images/create_cvterms.2.png

Load CVterm
^^^^^^^^^^^^^

Now that you have at least one CVterm, you can load an existing CVterm. This is demonstrated in the following code.

.. code::

  $details = [
    'accession' => '0000704',
    'vocabulary' => [
      'short_name' => 'SO',
    ],
  ];
  $term = \Drupal::service('tripal.tripalTerm.manager')->getTerms($details);

Once you have a TripalTerm object, you can retrieve the value of various properties by using the following methods:

.. code::

  $vocab = $term->getVocab();
  $vocab_short_name = $vocab->getLabel();
  $accession = $term->getAccession();
  $full_accession = $vocab_short_name . ':' . $accession;
  $name = $term->getName();
  $definition = $term->getDefinition();
