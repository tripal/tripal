
Tripal Module Rating System
=============================

This module rating system is meant to aid Tripal Site Administrators in choosing extension modules for their site. It is also meant to guide developers in module best practices and celebrate modules which achieve these goals.

Bronze
-------

.. image:: Tripal-Bronze.png

- Has a public release.
- Should install on a Tripal site appropriate for the versions it supports.
- Defines any custom tables or materialized views in the install file (if applicable).
- Adds any needed controlled vocabulary terms in the install file (Tripal3).
- Provides Installation and admin instructions README.md (or `RTD <https://tripal.readthedocs.io/en/latest/dev_guide/rtd.html>`_).
- Has a license (distributed with module).

Silver
-------

.. image:: Tripal-Silver.png

- Follows basic Drupal Coding standards; specifically, `code format <https://www.drupal.org/docs/develop/standards/coding-standards>`_ and `API documentation <https://www.drupal.org/docs/develop/standards/api-documentation-and-comment-standards#drupal>`_.
- Uses Tripal API functions. Specifically, it should use the
    - Chado Query API for querying chado (if using chado as the storage system). (`API <http://api.tripal.info/api/tripal/tripal_chado%21api%21tripal_chado.query.api.inc/group/tripal_chado_query_api/3.x>`_, :doc:`Tutorial <../dev_guide/chado>`)
    - Tripal Jobs API for long running processes. (`API  <http://api.tripal.info/api/tripal/tripal%21api%21tripal.jobs.api.inc/group/tripal_jobs_api/3.x>`_)
    - TripalField class to add data to pages (Tripal3). (:doc:`Tutorial <../dev_guide/custom_field>`)
- Provides ways to customize the module (e.g. drush options, field/formatter settings, admin UI).
- Latest releases should follow Drupal naming best practices.
    - e.g. first release for Drupal 7 should be: ``7.x-1.0``.

Gold
-----

.. image:: Tripal-Gold.png

- Extensive documentation for the module (similar to Tripal User's Guide). ( `Tutorial <https://tripal.readthedocs.io/en/latest/dev_guide/rtd.html>`_)
- Unit testing is implemented using PHPUnit with the TripalTestSuite or something similar.
- Continuous integration is setup (e.g. such as with TravisCI).
- Imports data via Tripal's importer class (Tripal3) (:doc:`Tutorial <../dev_guide/custom_data_loader>`).
- Tripal 3 fields are (:doc:`Tutorial <../dev_guide/custom_field/manual_field_creation>`)
    - Fully compatible with web services.
    - The elementInfo function is fully implemented.
    - The query and queryOrder functions fully implemented.
- Web Services uses Tripal's Web Service Classes (Tripal3). (:doc:`Tutorial <../dev_guide/custom_web_services>`)
- Code sniffing and testing coverage reports (optional but encouraged).
- Drupal.org vetted release (optional but encouraged).

Rate your Extension Module!
-----------------------------

We encourage Tripal module developers to rate their modules. This can be done by :doc:`./instructions`

The following badges are for inclusion on your module README and documentation; however, they are only valid if your module has been included in :doc:`../extensions` with the given rating.

reStructuredText

.. code-block:: RST

    .. image:: https://tripal.readthedocs.io/en/7.x-3.x/_images/Tripal-Bronze.png
      :target: https://tripal.readthedocs.io/en/7.x-3.x/extensions/module_rating.html#Bronze
      :alt: Tripal Rating: Bronze


Markdown

.. code-block:: MD

    [![Tripal Rating Bronze Status](https://tripal.readthedocs.io/en/7.x-3.x/_images/Tripal-Bronze.png)](https://tripal.readthedocs.io/en/7.x-3.x/extensions/module_rating.html#Bronze)


HTML

.. code-block:: html

    <a href='https://tripal.readthedocs.io/en/7.x-3.x/extensions/module_rating.html#Bronze'>
        <img src='https://tripal.readthedocs.io/en/7.x-3.x/_images/Tripal-Bronze.png' alt='Tripal Rating: Bronze' />
    </a>

.. note::

   Replace all instances of ``Bronze`` with either ``Silver`` or ``Gold`` for those badges.
