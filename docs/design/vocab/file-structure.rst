
File Structure
=================

.. image:: vocab-class-diagram.png

The base structure of this API is found in ``tripal/src/TripalVocabTerms/``. Specifically, you can find

 - The **TripalTerm class**.
 - The **base classes** to extend when making your own vocabulary plugin implementation.
 - The **interfaces** you should implement are in the Interface directory and describe the methods you must implement in your vocabulary plugin implementation.
 - The **annotation classes** describe the metadata needed in the comment header of your implemented plugin class.
 - The **plugin managers** are in the PluginManager directory and simply link these plugins to the Drupal API.

.. warning::

  Core Implementations for the vocabulary and id space plugins are still underway. The design base is in the main branch but work is not complete.

  **All implementations** should be in the ``src/Plugin/TripalIdSpace`` and ``src/Plugin/TripalVocabulary`` directory of their respective modules. For example, a Chado implementation would be in ``tripal_chado/src/Plugin/TripalVocabulary/ChadoVocabulary.php`` and ``tripal_chado/src/Plugin/TripalIdSpace/ChadoIdSpace.php``. You do not need to implement the TripalTerm class as all storage is handled by the vocabulary or ID space containing a given term.
