
File Structure
=================

.. warning::

  All work is currently being completed in branch `129-TripalFields-vocabRabbitHole <https://github.com/tripal/t4d8/tree/129-TripalFields-vocabRabbitHole>`_.

.. note::

  For reference, these are the core classes we are talking about:

  .. image:: vocab-class-diagram.png

The base structure of this API is found in ``tripal/src/TripalVocabTerms/``. Specifically, you can find

 - The **TripalTerm class**.
 - The **base classes** to extend when making your own vocabulary plugin implementation.
 - The **interfaces** you should implement are in the Interface directory and describe the methods you must implement in your vocabulary plugin implementation.
 - The **annotation classes** describe the metadata needed in the comment header of your implemented plugin class.
 - The **plugin managers** are in the PluginManager directory and simply link these plugins to the Drupal API.

We still need to make a core implementation for the vocabulary and id space plugins. This needs further discussion as we may want a Tripal implementation which stores vocabularies in the Drupal database and thus transcend some known issues with storing vocabularies in Chado.

**All implementations** should be in the ``src/TripalVocabTerms`` directory of their respective modules. For example, a Chado implementation would be in ``tripal_chado/src/TripalVocabTerms/ChadoVocabulary.php`` and ``tripal_chado/src/TripalVocabTerms/ChadoIdSpace.php``. You do not need to implement the TripalTerm class as all storage is handled by the vocabulary or ID space containing a given term.
