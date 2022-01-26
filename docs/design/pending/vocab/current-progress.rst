
Current Progress
==================

The design for this API is fairly stable with the interface + plugin structure finalized.

Complete:
------------

- Create Vocabulary + ID Space Plugin definitions
- Create plugin managers and ensure plugins are discoverable
- Design TripalTerm class
- Design interfaces for Vocabulary and ID Space plugins
- Stub out base classes

To Be Done:
-------------

1. Fill in base method functionality based on stub. Specifically, see ``tripal/src/TripalVocabTerms/TripalVocabularyBase.php`` and ``tripal/src/TripalVocabTerms/TripalIdSpaceBase.php``
2. Fill in method functionality for TripalTerm class (``tripal/src/TripalVocabTerms/TripalTerm.php``). There should be no database access in these methods. For example, the TripalTerm->getIdSpaceObject method should use the idspace name associated with the term and the IdSpace plugin manager to retrieve the object.
3. Create a core implementation (or two). We will definitely want a chado-focused implementation which takes into account there may be multiple chado schema. Additionally, we may want a Tripal implementation with it's own storage tables to overcome some limitations in Chado. More discussion within the community is needed.
4. Remove old entity-based implementation of vocabulary, ID Spaces, Terms
5. Switch Chado prepare to use new vocabularies?
6. Switch current entities to use new vocab design?
