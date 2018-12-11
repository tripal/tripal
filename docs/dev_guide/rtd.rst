Using ReadTheDocs
=================

.. note::

  For Tripal's own ReadTheDocs guidelines: :ref:`tripal_rtd`


What is ReadTheDocs?
--------------------

Documentation is important. It tells users how to use our product, and developers how to read our code.  We recommend hosting documentation for your custom module somewhere easily accessible, like ReadTheDocs.

`ReadTheDocs <https://readthedocs.org/>`_ uses the `Sphinx <http://www.sphinx-doc.org/en/master/>`_ framework to build a website in a Continuous Integration setup. When new code is pushed to GitHub, the documentation website as defined by sphinx is built, and the "live" documentation website is updated.

- Code changes can include documentation updates **in the same pull request**.
- Changes to the documentation is **subject to review, just like code changes**.
- Documentation changes are under **version control**.

How do I add ReadTheDocs to My Project?
---------------------------------------

- Set up your ReadeTheDocs account and add your project integration
- Install Sphinx
- Run the quickstart command: ``sphinx-quickstart``
- Write your documentation (we're using RST format)
- run ``make html`` in the docs folder to build your site for testing purposes
- Push your changes to GitHub

For a detailed walkthrough, please see the `official ReadTheDocs getting started guide <https://docs.readthedocs.io/en/latest/getting_started.html>`_.

We use RST format to write our documentation. It might seem a little more complicated than markdown (and it is), but it's also more powerful.  The choice is yours for which format to use.

Link documents to your ``index.rst`` and Sphinx will build you a searchable site with nicely formatted navigation.


ReadTheDocs also provides some really awesome **versioning** tools, allowing you to post releases of the documentation so users can go back and find older documentation with almost no effort on your part.
