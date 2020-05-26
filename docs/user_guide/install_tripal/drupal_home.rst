DRUPAL_HOME Variable
====================
An important convention in this document is the use of the ``$DRUPAL_HOME`` environment variable.  If you are new to UNIX/Linux you can learn about environment variables `here <https://www.tutorialspoint.com/unix/unix-environment.htm>`_.  Drupal is a necessary dependency of Tripal.  The setup and installation sections describe how to install Drupal.  If you follow the instructions exactly as described in this User's Guide you will install Drupal into ``/var/www/html``. However, some may desire to install Drupal elsewhere.  To ensure that all command-line examples in this guide can be cut-and-pasted you **must** set the ``$DRUPAL_HOME`` variable.  You can set the variable in the following way:

  .. code-block:: bash

    DRUPAL_HOME=/var/www/html
    
Be sure to change the path ``/var/www/html`` to the location where you have installed Drupal.  If you have never installed Drupal and you intend on following this guide step-by-step then use the command-line above to get started.

.. note::

  You will have to set the ``$DRUPAL_HOME`` environment variable anytime you open a new terminal window.