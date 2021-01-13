
Alternate Database Backends
==============================

Drupal is database agnostic; however, Tripal is still PostgreSQL leaning. Tripal 4+ are completely Chado agnostic though with all core functionality including Biological Vocabularies and Content Types standing independent of Chado. This allows for the development of alternate data backends with Chado already being implemented by the Tripal Chado core module. **This guide will describe how to integrate additional storage backends using Chado as an example.**

Tripal Vocabularies, IDSpaces and Terms
----------------------------------------

Tripal provides a Drupal Plugin for hooking into TripalVocab, TripalVocabSpace and TripalTerm classes. The TripalTermStorage plugin provides a number of methods mapping to preSave, postSave, load and delete functions for each entity type. This allows developers to implement this plugin through creation of a single class which can then handle full integration of all three classes with an additional data backend.

To create your own data backend for Vocabularies and Terms, you can follow the standard Drupal procedure for implementing plugins which will be detailed below.

Step 1: Create your plugin implementation class.
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Your entire data backend will exist in a single class. This class implements the TripalTermStorageInterface and extends the TripalTermStorageBase. It also uses a number of other classes in order to pull them into the current scope. The following shows the Chado Integration class as an example. To create your own change the namespace to match your modules, the annotation to describe your data backend and the class name. This file should be created in your ``src/Plugin/TripalTermStorage`` directory in order to be discovered by Drupal/Tripal.

.. code-block:: php

	namespace Drupal\tripal_chado\Plugin\TripalTermStorage;

	use Drupal\tripal\Entity\TripalVocab;
	use Drupal\tripal\Entity\TripalVocabSpace;
	use Drupal\tripal\Entity\TripalTerm;

	use Drupal\tripal\Plugin\TripalTermStorage\TripalTermStorageBase;
	use Drupal\tripal\Plugin\TripalTermStorage\TripalTermStorageInterface;
	use Drupal\Core\Entity\EntityStorageInterface;

	/**
	 * TripalTerm Storage plugin: Chado Integration.
	 *
	 * @ingroup tripal_chado
	 *
	 * @TripalTermStorage(
	 *   id = "chado",
	 *   label = @Translation("GMOD Chado Integration"),
	 *   description = @Translation("Ensures Tripal Vocabularies are linked with chado cvterms."),
	 * )
	 */
	class TripalTermStorageChado extends TripalTermStorageBase implements TripalTermStorageInterface {

	}

This plugin will work without any methods implemented although it obviously will not connect your data backend just yet. To test that your new implementation is registered properly with Drupal/Tripal you can use `Drupal Console <https://drupalconsole.com>`_. Specifically you would use the `debug:plugin Command <https://drupalconsole.com/docs/en/commands/debug-plugin>`_ command as shown here:

.. code::

	drupal debug:plugin tripal.termStorage

This will output a list of implementations for the Tripal Term Storage plugin and should include both your plugin implementation, as well as, the chado one.

Step 2: Implement the methods you need for integration.
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

For a list of available methods including documentation, check out the ``tripal/src/Plugin/TripalTermStorage/TripalTermStorageInterface.php`` file. There is an example for chado available in the ``tripal_chado/src/Plugin/TripalTermStorage/TripalTermStorageChado.php``
