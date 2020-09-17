Creating a Custom Widget
========================

In Drupal/Tripal terminology, **widget** refers to the form elements for a specific Tripal Field on the "Edit" form of a piece of Tripal Content. For example, the ``obi__organism`` field widget creates the "Organism" drop-down on the edit form of a gene. All fields come with a default widget; however, you can create a custom widget if the default one doesn't meet your needs.

.. note::
  This guide assumes you already have your widget class file created. For more information, see :doc:`manual_field_creation` or, :doc:`tripal_field_generator`.

.. note::
	If you are only creating a widget and not the whole field, you still need to follow the expected directory structure. For example, if your widget is going to be named ``obi__organism_fancy`` then your file would be ``[your_module]/includes/TripalField/obi__organism_fancy/obi__organism_fancy_widget.inc``.

The Form
--------

The form elements of your widget are defined in the ``form()`` method of your widget according to the `Drupal Form API <https://api.drupal.org/api/drupal/developer%21topics%21forms_api_reference.html/7.x>`_. As such the ``$widget`` variable is actually a nested associative array describing what the widget portion of the form should look like. For example, the following is how the ``obi__organism`` widget creates the drop-down.

.. code-block:: php

  /**
   * @see TripalFieldWidget::form()
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {

    $field_name = $this->field['field_name'];
    $field_table = $this->instance['settings']['chado_table'];
    $linker_field = 'chado-' . $field_table . '__organism_id';

    // The value presented to the user via load.
    // If $items['delta']['value'] is set then we are updating and already have this
    // information. As such, simply save it again.
    $widget['value'] = array(
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    );

    // Pull out the value previously saved to be used as the default.
    $organism_id = 0;
    if (count($items) > 0 and array_key_exists($linker_field, $items[0])) {
      $organism_id = $items[0][$linker_field];
    }

    // Define a drop-down form element where the options are organisms retrieved using
    // the Tripal API, the default is what we looked up above, and the title and
    // description are those set when defining the field.
    $widget[$linker_field] = array(
      '#type' => 'select',
      '#title' => $element['#title'],
      '#description' => $element['#description'],
      '#options' => chado_get_organism_select_options(FALSE),
      '#default_value' => $organism_id,
      '#required' => $element['#required'],
      '#delta' => $delta,
    );

  }

At a minimum, the form must have a ``value`` element.  For Tripal, the ``value`` element of a field always corresponds to the value that is presented to the end-user either directly on the page (with formatting) or via web services, or some other mechanism. Convention is to store the value of the field as a hidden ``value`` form element as is shown in the above example.

.. note::
	For more information on how to use the Drupal Form API, check out the `official Drupal Documentation <https://www.drupal.org/docs/7/api/form-api>`_.

.. note::
	The current item is saved in ``$items[$delta]`` as an array where the keys will match those set by the field ``load()`` function.


Validation
----------

The ``validate()`` function of your widget allows you to confirm that the values entered by the user are valid. It is recommended to consider each form element you created above and consider what is required for that element to be entered "correctly". For example, for an organism drop-down, the organism chosen must exist in our chado database (since this is a ``ChadoFieldWidget``). Luckily this doesn't need to be validated since Drupal ensures only elements in our select list are chosen.

.. warning::
	The ``value`` key of this field must be set in the ``$form_state['values']`` array to a **TRUE** value (e.g. a string or non-zero integer) anytime data is entered by the user.

.. note::
	For more information on how to validate your data, see the official `Drupal Form Validation Documentation <https://www.drupal.org/docs/7/creating-custom-modules/validating-the-data>`_

Saving the Data
---------------

The Drupal Storage Backend handles saving of your widget data. As such, **you do not and should not insert, update or delete the data yourself**. It should happen automatically, assuming you've followed the conventions of the specific storage backend.

Chado Fields utilize the chado storage backend to save your data. Thus to ensure your data is saved, you set the columns of your chado table to the values you want them set via the ``$form_state['values']`` array using the ``chado-[table]__[column]`` convention. This should be done at the end of the validation function above, if the data submitted is valid.

For our ``obi__organism`` example, the drop-down returns the chado organism_id of the record chosen by the user. We would like to save that as the organism_id of the chado table the field references, which the following code specifies.

.. code-block:: php

  /**
   * @see TripalFieldWidget::validate()
   */
  public function validate($element, $form, &$form_state, $langcode, $delta) {

    $field_name = $this->field['field_name'];
    $field_table = $this->instance['settings']['chado_table'];
    $linker_field = 'chado-' . $field_table . '__organism_id';

    //...
    // Validate your data here
    //...

    // In this case, if you have an organism_id, then your user selected this field.
    $organism_id = $form_state['values'][$field_name]['und'][0][$linker_field];
    if ($organism_id > 0) {
      $form_state['values'][$field_name]['und'][0]['value'] = $organism_id;
      // This is where we tell the storage backend what we want to save.
      // Specifically, that we want to save $organism_id to $field_table.organism_id
      $form_state['values'][$field_name]['und'][$delta][$linker_field] = $organism_id;
    }
  }

But what do you do if the record you want to link to via foreign key constraint doesn't yet exist? Luckily the Chado Storage API has a solution for this as well. Consider the example of the ``sbo__relationship_widget``. When this widget is on the create form for a given content type, we will first need to create the base record before we can create a relationship to it. This is done by setting the values you do know (e.g. ``chado-feature__type_id`` and ``chado-feature__object_id``) but not setting the column mapping to the base record. The Chado Storage API will then fill it in automatically once the base record is created.

.. code-block:: php

  /**
   * @see TripalFieldWidget::validate()
   */
  public function validate($element, $form, &$form_state, $langcode, $delta) {

    $field_name = $this->field['field_name'];
    $field_table = $this->instance['settings']['chado_table'];
    $linker_field = 'chado-' . $field_table . '__organism_id';

    //...
    // Validate your data here
    //...

    //...
    // Determine the subject_id, object_id and type_id based on user input.
    // User input is found in $form_state['values'].
    //...

    // If we have all the keys then set the columns as in the obi__organism ex.
    if ($subject_id && $object_id && $type_id) {
      // Set all chado fields to their values.
    }
    // Otherwise, maybe we are creating the entity...
    // The storage API should handle this case and automagically add the key in // once the chado record is created... so all we need to do is set the
    // other columns.
    elseif ($subject_name && $object_id && $type_id) {
      $form_state['values'][$field_name][$langcode][$delta]['value'] = 'value must be set but is not used';
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__' . $object_id_key] = $object_id;
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__type_id'] = $type_id;
      // Notice that the subject_id is not set here.
    }
    // Otherwise, we don't have a value to insert so leave them blank.
    else {
      // Set all chado fields to empty string.
    }

Drupal typically does not provide a submit hook for fields because, as mentioned above, saving should be done by the storage backend. However, the TripalField provides a ``TripalFieldWidget::submit()`` to allow for behind-the-scenes actions to occur. This function should never be used for updates, deletes or inserts for the Chado table associated with the field as these actions should be handled by the storage backend.

However, it is permissible to perform inserts, updates or deletions within Chado using this function.  Those operations can be performed if needed but on other tables not directly associated with the field. An example is the ``chado.feature_synonym`` table.  The ``chado_linker__synonym`` field allows the user to provide a brand new synonym and it must add it to the chado.synonym table prior to the record in the chado.feature_synonym table.
