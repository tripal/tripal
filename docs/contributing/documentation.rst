.. _tripal_rtd:

Contributing to the Documentation
==================================

The Tripal documentation is written in `**Restructured Text** <http://docutils.sourceforge.net/rst.html>`_, compiled with `Sphinx <http://www.sphinx-doc.org/en/master/usage/quickstart.html>`_, and built/hosted with `ReadTheDocs  <https://readthedocs.org/>`_.  The ``docs`` directory, when compiled, is hosted at https://tripal.readthedocs.io/en/latest/.

For minor changes, you can simply `Edit the file using the Github editor <https://help.github.com/articles/editing-files-in-your-repository/>`_, which will allow you to make a Pull Request.  Once approved, your changes will be reflected in the documentation automatically!

Guide
------


Install Sphinx
~~~~~~~~~~~~~~~~~

For minor changes, you don't need to build the documentation!  If you want to see how your changes will look on the built site, however, you will need Sphinx installed.

For more information, please see the Sphinx setup guide:
http://www.sphinx-doc.org/en/master/usage/quickstart.html


Building your changes
~~~~~~~~~~~~~~~~~~~~~~~


For more extensive edits, or when contributing new guides, you should build the documentation locally. From the ``docs`` root (eg ``/var/www/html/sites/all/modules/tripal/docs/``, execute ``make html``.  The built site will be in ``docs/_build/html/index.html``.

Tripal conventions
~~~~~~~~~~~~~~~~~~~~~~~

Please follow these guidelines when updating our docs. Let us know if you have any questions or something isn't clear.

Please place images in the same folder as the guide text file, following the convention [file_name].[n].[optional description].[extension].  For example, ``configuring_page_display.3.rearrange.png`` or ``configuring_page_display.1.png`` are both located in ``docs/user_guide/`` and are part of the ``configuring_page_display.rst`` guide.

We currently use the following syntax:

.. code-block:: rst

  Title of File (using title case)
  =================================

  Introduction text.

  Section Title
  -------------

  We use double backticks to indicate ``inline-code`` including file names, function and method names, paths, etc.

  Longer code-blocks should begin with the ``.. code-block:: [type]`` directive and should be indented at least one
  level. There should also be a blank line before and after it as shown below.

  .. code-block:: sql
    if ($needs_documentation) {
        use $these_guidelines;
        $contribute_docs = $appreciated;
    }

  Section 1.1 Title
  ^^^^^^^^^^^^^^^^^

  The use of appropriate sections makes reading documentation and later specific details easier. Sub sections such
  as this one will be hidden unless the main section is already selected.
  ```
