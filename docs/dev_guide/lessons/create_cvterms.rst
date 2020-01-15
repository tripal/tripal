
Controlled Vocabulary Terms Create/Load
=========================================

This lesson will teach you how to programatically create new controlled vocabularies (CVs) and their terms (CVterms). It will also show you how to load both CVs and CVterms which already exist in your Tripal site.

.. warning::

  This lesson was written before Chado integration. As such it shows how database agnostic Tripal 4 is. In the future we will add a section about how to connect these terms to your Chado cvterms and how to create terms from those existing in chado.

.. note::

  You can try out this code anywhere in your Tripal extension module except in Twig templates. If you do not have a Tripal extension module, you can use the `Devel PHP <https://www.drupal.org/project/devel_php>`_ Drupal module on your **development Tripal site** and then execute the code at ``https://yourdrupalsite/devel/php``.

Controlled Vocabularies (CVs)
------------------------------

.. note::

  **Background:** :doc:`/dev_guide/cvterms`

  Questions:
    - How do I programatically create a controlled vocabulary?
    - How do I load existing controlled vocabularies?
    - How do I access values once I have the vocabulary?


Create CV
^^^^^^^^^^

The following code demonstrates how to create a Tripal Controlled Vocabulary (CV) for the sequence ontology.

.. code:: php

  $vocab = \Drupal\tripal\Entity\TripalVocab::create();
  $vocab->setLabel('SO');
  $vocab->setName('sequence');
  $vocab->setDescription('The Sequence Ontology is a set of terms and relationships used to describe the features and attributes of biological sequence. SO includes different kinds of features which can be located on the sequence.');
  $vocab->save();

First we create an empty object instance to act as our CV using the line ``$vocab = \Drupal\tripal\Entity\TripalVocab::create();``. Notice that we use the full namespace of the TripalVocab class to instantiate it -this ensures that Drupal magic will be able to autoload the class on demand.

Next we set the values for the CV. This is done using a number of set methods provided by TripalVocab. The label is the short name of the vocabulary (in this case SO) and the name is the full human-readable name of the vocabulary (in this case sequence). We can also provide a description to provide additional context to the CV.

.. code:: php

  $vocab->setLabel('SO');
  $vocab->setName('sequence');
  $vocab->setDescription('The Sequence Ontology is a set of terms and relationships used to describe the features and attributes of biological sequence. SO includes different kinds of features which can be located on the sequence.');

The last but critical step is to call ``$vocab->save();`` which saves your new CV to the database.

You can check that the CV saves properly by navigating to Home > Administration > Structure > Tripal Controlled Vocabularies (``admin/structure/tripal_vocab``) and ensuring your new CV is in the list of existing CVs.

.. image:: create_cvterms.1.png

Load CV
^^^^^^^^^

Now that you have at least one CV, you can load an existing CV. This is demonstrated in the following code.

.. code:: php

  $vocab_id = 1;
  $vocab = \Drupal\tripal\Entity\TripalVocab::load($vocab_id);

.. warning::

  We are still working on APIs to help you load CVs by name or label.

Once you have a TripalVocab object, you can retrieve the value of various properties by using the following methods:

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
    - How do I programatically add a term to an existing vocabulary.
    - How do I load an existing CVterm?
    - How do I access values once I have the term?

Create CVterm
^^^^^^^^^^^^^^^

The following code demonstrates how to create a Tripal Controlled Vocabulary Term (CVterm) in the "sequence ontology" controlled vocabulary (CV). Specifically, we are going to create a Tripal CVterm for the `gene <http://www.sequenceontology.org/miso/release_2.5/term/SO:0000704>`_ term from the sequence ontology.

.. code:: php

  $vocab_id = 1;
  $term = \Drupal\tripal\Entity\TripalTerm::create();
  $term->setVocabID($vocab_id);
  $term->setAccession('0000704');
  $term->setName('gene');
  $term->setDefinition('A region (or regions) that includes all of the sequence elements necessary to encode a functional transcript. A gene may include regulatory regions, transcribed regions and/or other functional sequence regions.');
  $term->save();

This follows the same format as for creating the sequence ontology CV. First we create the empty TripalTerm object, then we set the values for the various properties and finally, we save it to the database.

To check if your CVterm was created properly you can look on the listing at Home > Administration > Structure > Tripal Controlled Vocabulary Terms (``admin/structure/tripal_term``) and ensuring your new CVterm is in the list of existing CVterms.

.. image:: create_cvterms.2.png

Load CVterm
^^^^^^^^^^^^^

Now that you have at least one CVterm, you can load an existing CVterm. This is demonstrated in the following code.

.. code::

  $term_id = 1;
  $term = \Drupal\tripal\Entity\TripalTerm::load($term_id);


.. warning::

  We are still working on APIs to help you load CVterms by name, accession or vocabulary.

Once you have a TripalTerm object, you can retrieve the value of various properties by using the following methods:

.. code::

  $vocab = $term->getVocab();
  $vocab_short_name = $vocab->getLabel();
  $accession = $term->getAccession();
  $full_accession = $vocab_short_name . ':' . $accession;
  $name = $term->getName();
  $definition = $term->getDefinition();
