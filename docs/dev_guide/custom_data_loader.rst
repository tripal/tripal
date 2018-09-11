Creating Custom Data Loaders
==============================


.. note::

  This documentation is currently being developed.


Youtube link for this tutorial:



The `TripalImporter` class can be extended to create your own data loader.

Static Variables
-----------------

Your importer should overwrite any of the ``public static`` variables that should be different from the default.

.. note::

  For the sake of simplicity, we do not override many of the default settings, and we do not include the full inline code documentation.  Please see the class documentation for a full list of options.


.. code-block:: php


  /**
   * @see TripalImporter
   */
  public static $name = 'Example TST File Importer';

  public static $machine_name = 'tripal_tst_loader';

  public static $description = 'Loads TST files';

  public static $file_types = ['txt', 'tst', 'csv'];

  public static $upload_description = 'TST is a fictional format.  Its a 2-column, CSV file.  The columns should be of the form featurename, and text';

  public static $methods = [
    // Allow the user to upload a file to the server.
    'file_upload' => TRUE,
    // Allow the user to provide the path on the Tripal server for the file.
    'file_local' => TRUE,
    // Allow the user to provide a remote URL for the file.
    'file_remote' => TRUE,
  ];

The variables that are ``private static`` **should not** be changed.


Form Components
-----------------

There are three standard Drupal form hooks: ``form``, ``form_validate``, ``form_submit``. The TripalImporter deals with these for us as ``form`` and ``formValidate``: typically the base class's ``formSubmit`` does not need to be modified.

.. note::

  Please see the Drupal documentation for the Form API reference, available `here for Drupal 7 <https://api.drupal.org/api/drupal/developer%21topics%21forms_api_reference.html/7.x>`_.  This tutorial will only scratch the surface of the Form API.


form
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


This function will provide all of the input widgets required for the user to run the form.  The global settings above ::ref:`<Static Variables>`_ provide some elements "out of the box".  A totally empty TripalImporter class can provide the tow below components: the **files** section, and an **analysis** selector.

The **File Upload** area lets users choose to upload a file manually using the interface, or, to provide a **Server path** or **Remote path** for the file.

.. image:: ./custom_data_loader.1.oob_file_interface.png

.. image:: ./custom_data_loader.2.oob_analysis_select.png

Our overly simplistic TST reader example only needs to do one thing: let the user pick a CVterm.  The importer will then read the file, split it into feature and values, and insert into featureprop using the ``type_id`` the user specified in the form.

Our form might therefore be something as simple as this:

.. code-block:: php
  :name: ExampleImporter::form


  public function form($form, &$form_state) {
  $options = [];

      #an array of random sequence ontology terms the user can select from.
      $terms = [array('id' => 'SO:0000235'), ['id' => 'SO:0000238'], ['id' => 'SO:0000248'] ];

      $options[0] = '--please select an option--';

      foreach ($terms as $term){
        $term_object = chado_get_cvterm($term);
        $id = $term_object->cvterm_id;
        $options[$id] = $term_object->name;
      }

          $form['pick_cvterm'] =  [
            '#title' => 'CVterm',
            '#description' => 'Please pick a CVterm.  The loaded TST file will associate the values with this term as a feature property.',
            '#type' => 'select',
            '#default_value' => '0',
            '#options' => $options
            ];

    return $form;
  }

Our form now has a select box!

.. image:: ./custom_data_loader.3.cvterm_select.png



What about responsive form elements?
"""""""""""""""""""""""""""""""""""""

.. note::

  This section coming soon. For now, check out the Drupal AJAX guide https://api.drupal.org/api/drupal/includes%21ajax.inc/group/ajax/7.x



formValidate
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^



This function is responsible for verifying that required fields are filled out, and that supplied values are valid.  If something is invalid, use ``form_set_error()`` provide an error message and Drupal will mark that piece of the form in red.
In our example code, we should check that the user picked a CVterm in the ``pick_cvterm`` element.


.. code-block:: php

  public function formValidate($form, &$form_state) {
    parent::formValidate($form, $form_state);

    $chosen_cvterm = $form_state['values']['pick_cvterm'];
    if ($chosen_cvterm == 0) {
      form_set_error('pick_cvterm', 'Please choose a CVterm.');
    }
  }

This very simple validation function looks for the ``pick_cvterm`` element of the ``$form_state`` and ensures the user selected something.  Your own validation may be more complex (for example, ensuring a regular expression is valid, or that a term exists in the database) but the principle will be the same.



Importer Logic
---------------

run
^^^^^^^^^^^^

If ``formValidate`` did not encounter any ``form_set_error``, the importers ``run`` function will execute.  Between the ``formValidate`` and the ``run``, other things have happened: for example, the file was downloaded if a remote URL was given.

The run function should collect the arguments from the importer, and perform the logic of loading your file.


.. code-block:: php

    /**
     * @see TripalImporter::run()
     */
    public function run() {

      $arguments = $this->arguments['run_args'];
      $file_path = $this->arguments['files'][0]['file_path'];

      $analysis_id = $arguments['analysis_id'];
      $cvterm = $arguments['pick_cvterm'];

      $this->loadMyFile($analysis_id, $file_path, $cvterm);
    }

Loading the File
^^^^^^^^^^^^^^^^^^




Testing Importers
------------------
