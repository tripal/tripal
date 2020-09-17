Attach Fields to Content Types
==============================
In summary, creation of a new field requires creation of three classes that inherit from the ``TripalField``, ``TripalFieldWidget`` and ``TripalFieldFormatter`` base classes.  If the fields are created correctly and placed in the ``includes/TripalFields`` directory of your module then Tripal will automatically find them.  However, the field is not yet attached to any content type. They must be attached.  Fields can be attached programmatically or via the online Drupal interface by a site admin. 

The hook_bundle_fields_info() function
-------------------------------------
 The three TripalField classes simply define how the field will function, but Drupal does not yet know about the field.  The ``hook_bundle_fields_info`` function tells Drupal about your field. It must be implemented in a custom Drupal module, and provides an array that tells Drupal about the fields and the classes to use for the field.  Suppose we were creating a field named ``obi__genus`` which displays the Genus for a species and we have a custom module named ``tripal_org2``.  The hook function would be named ``tripal_org2_bundle_fields_info()``:

.. code-block:: php
  :linenos:

  function tripal_org2_bundle_fields_info($entity_type, $bundle) {
    $info = [];
    
    // Make sure this bundle is an organism (OBI:0100026) then we'll attach our 
    // field to display the genus.
    $term = tripal_load_term_entity(array('term_id' => $bundle->term_id));
    $term_accession = $term->vocab->vocabulary . '__' . $term->accession;
    if ($term_accession == 'OBI:0100026') {
      $field_name = 'obi__genus';
      $field_type = 'obi__genus';
      $info[$field_name] = [
        'field_name' => $field_name,
        'type' => $field_type,
        'cardinality' => 1,
        'locked' => FALSE,
        'storage' => [
          'type' => 'field_chado_storage',
        ],
        'settings' => [],
      ];
   }
    
    return $info
  }
  
This function receives as its second argument the ``$bundle`` object. This is the bundle that Drupal is requesting new fields for.  For this example we only want to attach the field if the content type is the organism content type.  The format of the returned ``$info`` array should have the field name as the key and an array that follows the instructions provided by Drupal's `field_create_field() <https://api.drupal.org/api/drupal/modules%21field%21field.crud.inc/function/field_create_field/7.x>`_ function. 

The settings indicate the field name, the field type, the cardinality (how many values are allowed), any default settings and the storage type.  Because we expect our data to come from Chado we set the ``field_chado_storage`` as the type.  The ``locked`` setting is set to FALSE indicating that Drupal will allow the field to be deleted if the site developer desires.

When the site administrator navigates to **Administer > Structure > Tripal Content Types**, clicks on a content type, and then the **manage fields** tab, a link appears at the top titled **Check for new fields**.  When that link is clicked, this hook function is called.

Programmatically Attaching Fields
---------------------------------
You probably want to programmatically attach fields to content types if your have existing data that you know should be made available. For example, an organism always has a genus and only one genus.  If we have a field that displays the genus for an organism then we will want it automatically attached on installation of our module.  We can do this programmatically using two hook functions: ``hook_bundle_fields_info()`` and ``hook_bundle_instances_info()``.  Both functions are required to attach a field to a content type. 

The hook_bundle_instances_info() function.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The previous hook tells Drupal that our field exists and is allowed to be connected to the organism bundle.  Next we need to create an actual instance of this field for the bundle.  We do this with the ``hook_bundle_instances_info()`` function.  The format is the same as the previous hook but the info array is different.  For example:

.. code-block:: php
  :linenos:

  function tripal_org2_bundle_instances_info($entity_type, $bundle) {
    $info = []
    
    // Make sure this bundle is an organism (OBI:0100026) then we'll attach our 
    // field to display the genus.
    $term = tripal_load_term_entity(array('term_id' => $bundle->term_id));
    $term_accession = $term->vocab->vocabulary . '__' . $term->accession;
    if ($term_accession == 'OBI:0100026') {
    
      $field_name = 'obi__genus';
      $is_required = FALSE;
      $info[$field_name] =  [
        'field_name' => $field_name,
        'entity_type' => $entity_type,
        'bundle' => $bundle->name,
        'label' => 'Genus',
        'description' => 'The genus for the organism',
        'required' => TRUE,
        'settings' => [
          'auto_attach' => TRUE,
          'chado_table' => 'organism',
          'chado_column' => 'genus',
          'base_table' => 'organism',
          'term_accession' => '0000005',
          'term_vocabulary' => 'TAXRANK',
          'term_name' => 'Genus',
        ],
        'widget' => [
          'type' => 'obi__genus_widget',
          'settings' => [
            'display_label' => 1,
          ),
        ],
        'display' => [
          'default' => [
            'label' => 'inline',
            'type' => 'obi__genus_formatter',
            'settings' => [],
          ],
        ],
      ];
    }
    return $info;
  }
  
The format of the returned ``$info`` array should have the field name as the key and an array that follows the instructions provided by Drupal's `field_create_instance() <https://api.drupal.org/api/drupal/modules%21field%21field.crud.inc/function/field_create_instance/7.x>`_ function. 

Unique to this info array are the settings related to Chado.  Because we expect our data to be loaded from Chado we must specify these settings:

 - ``base_table``: the name of the base table to which the record will be associated. In our case the ``organism`` table of Chado is the base table.
 - ``chado_table``: the name of the actual table form which the value of the field will be loaded or saved to.  In our case the ``organism`` table is also the ``chado_table``.  
 - ``chado_column``: the name of the column in the ``chado_table`` where the data is loaded from. if the ``base_table`` and ``chado_table`` are the same then this is the name of the column. In our case the ``genus`` columns.  If the base and chado tables are different then it is the name o the primary key column in the ``chado_table``
 - ``auto_attach``:  set this to TRUE if you want the field to automatically be added to an entity when it is generated for viewing.  Set it to FALSE to allow the field to be added via AJAX. For fields that require time to load setting to FALSE is preferred. 
 
.. note::
  A base table is one that contains the primary records to which ancillary data (e.g. properties, cross references, CV terms, publications, contacts, etc) are associated via linker tables. For example some base tables include: ``feature``, ``organism``, ``stock``, ``library``, etc.).  The ``base_table`` and ``chado_table`` will always be the same when you are mapping a field to data in a column in a base table. If your field maps data to a "linker" table where ancillary data is stored then the ``chado_table`` will be the linker table.

Notice as well that the ``display`` and ``widget`` sections list the name of our TripalEntityWidget and TripalEntityFormatter classes respectively.  This tells drupal to use our widget and formatter classes by default.

When the site administrator navigates to **Administer > Structure > Tripal Content Types**, clicks on a content type, and then the **manage fields** tab, a link appears at the top titled **Check for new fields**.  When that link is clicked, this hook function is called.  

.. note::

  Both hook functions must be properly constructed for the field to be automatically attached to the content type.
  
Allowing Manual Attachment of Fields
------------------------------------
Not all fields are created equal.  Some fields can be added by the site developer to a bundle and some cannot.  When the ``TripalField`` class is implemented for a class the ``$no_ui`` parameter is set to indicate if a field can be added via the web interface or not.  See the :doc:`manual_field_creation` page for more details. But in short the following setting does not allow a field to be added using the web interface

.. code-block::  php

 public static $no_ui = TRUE;
 
The following setting will allow the field to be added:

.. code-block::  php

 public static $no_ui = FALSE;

Next, we must let Drupal know that our field exists.  We do this by adding an entry to the ``$info`` array in the ``hook_bundle_fields_info()`` function described above.  This lets Drupal know about our field. However, because we are not programmatically creating an instance of the field on a content type, but allowing the user to create them we do not need to implement the ``hook_bundle_instances_info()`` function. Instead, we must implement ``hook_bundle_create_user_field()``.  This function is called when the user attempts to add our new field to a bundle.  One field that comes with Tripal is the ``chado_linker__prop`` field.  Most Chado base tables have an associated property table (e.g. ``organismprop``, ``featureprop``, ``stockprop``, etc). By default, the ``tripal_chado`` module automatically adds this field to all bundles that have existing properties. It adds a new instance for every property type.  However, new properties can be added to bundle, and the site admin may want to add those properties via the user interface rather. Therefore, this field has the ``$no_ui`` set to TRUE and uses the  ``hook_bundle_create_user_field()`` to create the new field instance for the user.

The following code is a snippet from the ``tripal_chado_bundle_create_user_field`` function of the ``tripal_chado`` module. Note that it uses the ``field_create_field`` function and the ``field_create_instance`` functions directly.  The arrays passed to these functions are identical to the ``$info`` arrays of both the ``hook_bundle_fields_info`` and ``hook_bundle_instances_info`` functions described above.

.. code-block:: php
  :linenos:
  
  function tripal_chado_bundle_create_user_field($new_field, $bundle) {

    // Get the table this bundle is mapped to.
    $term = tripal_load_term_entity(array('term_id' => $bundle->term_id));
    $vocab = $term->vocab;
    $params = array(
      'vocabulary' => $vocab->vocabulary,
      'accession' => $term->accession,
    );
    $chado_table = $bundle->data_table;
    $chado_type_table = $bundle->type_linker_table;
    $chado_type_column = $bundle->type_column;
    $chado_type_id = $bundle->type_id;
    $chado_type_value = $bundle->type_value;
  
    // We allow site admins to add new chado_linker__prop fields to an entity.
    // This function will allow us to properly add them.  But at this point we
    // don't know the controlled vocabulary term.  We'll have to use the
    // defaults and let the user set it using the interface.
    if ($new_field['type'] == 'chado_linker__prop') {
      $table_name = $chado_table . 'prop';
  
      if (chado_table_exists($table_name)) {
        $schema = chado_get_schema($table_name);
        $pkey = $schema['primary key'][0];
        $field_name = $new_field['field_name'];
        $field_type = 'chado_linker__prop';
  
        // First add the field.
        field_create_field(array(
          'field_name' => $field_name,
          'type' => $field_type,
          'cardinality' => FIELD_CARDINALITY_UNLIMITED,
          'locked' => FALSE,
          'storage' => array(
            'type' => 'field_chado_storage',
          ),
        ));
  
        // Now add the instance
        field_create_instance(array(
          'field_name' => $field_name,
          'entity_type' => 'TripalEntity',
          'bundle' => $bundle->name,
          'label' => $new_field['label'],
          'description' => '',
          'required' => FALSE,
          'settings' => array(
            'auto_attach' => TRUE,
            'base_table' => $chado_table,
            'chado_table' => $table_name,
            'chado_column' => $pkey,
            'term_vocabulary' => '',
            'term_accession' => '',
            'term_name' => ''
          ),
          'widget' => array(
            'type' => 'chado_linker__prop_widget',
            'settings' => array(
              'display_label' => 1,
            ),
          ),
          'display' => array(
            'default' => array(
              'label' => 'inline',
              'type' => 'chado_linker__prop_formatter',
              'settings' => array(),
            ),
          ),
        ));
      }
      else {
        drupal_set_message('Cannot add a property field to this entity. Chado does not support properties for this data type.', 'error');
      }
    }
  }



.. note::
  
  It is possible to have a field that is both programmatically attached to some content types but is also allowed to be attached to another content type by the site admin using the web interface. To do this, programmatically add the field to the content types using the ``hook_bundle_instances_info`` function and also implement the ``hook_bundle_create_user_field`` function to support manual adding.
  
 
