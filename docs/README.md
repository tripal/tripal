The Tripal documentation is written in [**Restructured Text**](http://docutils.sourceforge.net/rst.html), compiled with [Sphinx](http://www.sphinx-doc.org/en/master/usage/quickstart.html), and built/hosted with [ReadTheDocs](https://readthedocs.org/).  This directory, when compiled, is hosted at https://tripal.readthedocs.io/en/latest/

For minor changes, you can simply [Edit the file using the Github editor](https://help.github.com/articles/editing-files-in-your-repository/), which will allow you to make a Pull Request.  Once approved, your changes will be reflected in the documentation automatically! 

# Guide

### Install Sphinx
For minor changes, you don't need to build the documentation!  If you want to see how your changes will look on the built site, however, you will need Sphinx installed.

For more information, please see the Sphinx setup guide:
http://www.sphinx-doc.org/en/master/usage/quickstart.html


### Building your changes

For more extensive edits, or when contributing new guides, you should build the documentation locally. From the `docs` root (eg `/var/www/html/sites/all/modules/tripal/docs/`, execute `make html`.  The built site will be in `docs/_build/html/index.html`.

### Tripal conventions

Please place images in the same folder as the guide text file, following the convention [file_name].[n].[optional description].[extension].  For example, `configuring_page_display.3.rearrange.png` or `configuring_page_display.1.png` are both located in `docs/user_guide/` and are part of the `configuring_page_display.rst` guide.

More guidelines coming soon...