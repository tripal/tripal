Using ReadTheDocs
=================

.. note::

  For Tripal's own ReadTheDocs guidelines: :ref:`tripal_rtd`.


What is ReadTheDocs?
--------------------

Documentation is important. It tells users how to use our product, and developers how to read our code.  We recommend hosting documentation for your custom module somewhere easily accessible, like ReadTheDocs.

`ReadTheDocs <https://readthedocs.org/>`_ (RTD) uses the `Sphinx <http://www.sphinx-doc.org/en/master/>`_ framework to build a website in a Continuous Integration setup. Your RTD-compatible documentation is added directly to your module and when code changes are pushed to GitHub, the documentation website as defined by sphinx is built, and the "live" documentation website is updated.

Benefits to housing documentation inside of your module code are:

- Code changes can include documentation updates **in the same pull request**.
- Changes to the documentation is **subject to review, just like code changes**.
- Documentation changes are under **version control**.

How do I add ReadTheDocs to my project?
---------------------------------------
Below is a quick overview of steps for integrating your module's documentation with RTD:

- Set up your ReadTheDocs account and import your project.
- Install Sphinx.
- Create a ``docs`` folder at the root of your project and navigate into it.
- Run the quickstart command: ``sphinx-quickstart``.
  - This creates necessary site configuration files (``conf.py``) as well as the make script to build your site.
- Write your documentation (we're using reStructuredText (RST) format):
  - Create an ``index.rst`` as the home page.
  - Link other RST documents in your ``index.rst``.
- Once the guide is on your master branch on GitHub, ReadTheDocs will handle the rest!

For a detailed walkthrough, please see the `official ReadTheDocs getting started guide <https://docs.readthedocs.io/en/latest/getting_started.html>`_.

For RTD integration we recommend using reStructuredText (RST) to write your documentation. It might seem a little more complicated than markdown (and it is), but it's also more powerful.  The choice is yours for which format to use.

ReadTheDocs also provides **versioning** tools, allowing you to post releases of the documentation so users can go back and find older documentation with almost no effort on your part.

What should my documentation include?
-------------------------------------

We suggest that your module include the following sections:

- Overview
- Installation and Setup Guide
- User's Manual

The Overview section should describe what features your module offers.

The Installation and setup section should guide a site administrator through installing and setting up your module.  Any site-wide settings that need to be configured, environmental variables set, or anything else not handled by the automated Drupal install should be covered.

The User's guide should walk through the day-to-day usage of your module.  This may include using custom importers, dashboards, or simply summaries of the new content this module provides.
