
Design Requirements
=======================

The following are the requirements we took into account for our design. Please let us know if you have requirements not listed in the document.

1. Support multiple data backends
-----------------------------------

A theme with Tripal 4 is flexible storage for data. We want to ensure our design for vocabularies supports Chado but also has flexibility to be extended (e.g. use Drupal database, graph database, multiple schema in Chado). We also want to make it easier to create custom storage backends than it was in Tripal 3.

2. Performance
----------------

Vocabularies and their terms are central to the organization of biological data in Tripal 4. We are focusing on performance to reduce barriers to using vocabulary terms extensively throughout your content.

3. Support borrowing terms from existing vocabularies
-------------------------------------------------------

As the available terms increases, we are seeing new ontologies choosing to borrow terms from existing ontologies. As described in `Chado#68 <https://github.com/GMOD/Chado/issues/68>`_, Chado has difficulties storing these relationships. We want to ensure that our design takes borrowed terms into account in a Chado agnostic way.

4. Model vocabularies intuitively
-----------------------------------

Chado's storage of vocabularies can be a little confusing and not ideal (e.g. `Chado#68 <https://github.com/GMOD/Chado/issues/68>`_). As such we want to design Tripal vocabularies independent of Chado and their storage in general. Specifically, we want to bring in the concept of ID Spaces as described in the `OBO Format v1.4 <https://owlcollab.github.io/oboformat/doc/GO.format.obo-1_4.html>`_.
