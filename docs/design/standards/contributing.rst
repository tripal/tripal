Contributing to Design
======================
We welcome anyone who would like to contribute to the Tripal v4 Design. The following policies should be followed by everyone contributing:

Verbal Communication
--------------------
When verbal communication is necessary to work out a details, designers can meet in the following ways:

- Mondays between 16:00-18:00 UTC is the Tripal Developer Meeting in GatherTown.  Join the `Tripal Slack <https://tripal-project.slack.com/join/shared_invite/zt-590q4q2f-YlO6xn7ri5UiCUZVx9M_lg#/shared-invite/email>`_ to get information to join these meetings.
- Zoom or Slack:  currently anyone working with the design team can create impromptu scheduled meetings. These only last as long or as often as the designers need. Reach out to those working on a particular item if you are interested in participating.


Design Documentation
--------------------
Organization
^^^^^^^^^^^^
All documentation for Tripal v4 design can be found here in one of two sections:

- :doc:`../current`
- :doc:`../pending`

Designs that are in active preparation should be found in the :doc:`../pending` section. Docs that have been approved by the PMC are found in the :doc:`../current`

.. note::

    Deigns in the :doc:`../current` section may change as needed but are not expected to undergo major changes.

How to Contribute Design Documents
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Adding New Designs
""""""""""""""""""
All design documentation must first be added to the :doc:`../pending` section. Anyone working on the design can do so.  To propose a pending design:

1. Clone or fork the `t4d8` repository.
2. Create a new branch for you design.
3. Design documentation is housed in the `docs/design` folder.
4. Only add new documentation into the `docs/design/pending` folder. If you are unsure where to add your contributions please ask.
5. Follow `RST <https://sublime-and-sphinx-guide.readthedocs.io/en/latest/index.html>`_ markup.
6. Use the consistent RST headers as found in other design documents.
7. Once you have added your documentation, you can submit a pull request for the `9.x-4.x` branch.
8. Pending design do not need PMC review and can be immediately merged. However, the act of creating a pull request alerts the PMC that documentation is being prepared.

.. warning::
    While you may start implementation of your design prior to formal approval by the PMC please remember that the PMC must approve all designs and the implementation of a design must match the documentation for full inclusion of new code into Tripal 4.  For this reason, it is recommended to wait on implementation until the PMC has fully reviewed any design you submit.

Submitting Designs
""""""""""""""""""
The PMC must approve all pending design documentation for it to be offically part of the Tripal v4 design.  Designs that get approved are moved into the :doc:`../current` section.  These designs can still be altered but are now considered "official".  To submit a pending design do the following:

1.  Move your documentation from the `docs/design/pending` section to the `docs/design/current` section. Ask if you are unsure of where to place it.
2.  Submit a pull request requesting review by the PMC.
3.  The pull request must stay in the queue for at least 2 days to allow for comment by the community. This is to allow others to have a say if they feel the design is lacking.
4.  The PMC merge the pull request if the design passes review or suggest changes if needed.

.. note::

    Members of the PMC who are involved in design development will also adhere to the rules for submitting designs for approval in order to allow the community to comment and to support transparency.


Formatting Design Documentation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Structure of the Document
"""""""""""""""""""""""""

Headers
"""""""
Use the following for headers

- ``#`` Page Titles
- ``=``, for sections
- ``-``, for subsections
- ``^``, for sub-sub sections
- ``"``, for sub-sub-subsections
