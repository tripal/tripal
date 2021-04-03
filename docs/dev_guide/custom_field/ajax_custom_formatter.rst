AJAX Responsive Formatters
===========================


Some fields need to be responsive.  For example, a user might select an analysis or organism to display data from. Drupal developers often use AJAX to rebuild the page based on user input.

Drupal and AJAX
---------------

Drupal has its own special way of doing AJAX! This is important to ensure that changes are executed in the correct order. You should `read the documentation carefully! <https://api.drupal.org/api/drupal/includes%21ajax.inc/group/ajax/7.x>`_  The Drupal AJAX API works best on forms, and field formatters **are not forms**.  Instead, they are `renderable arrays. <https://www.drupal.org/docs/7/api/render-arrays/render-arrays-overview>`_
As such, rather than accepting ``$form`` and ``&$form_state``, they accept ``&$element``, ``$entity_type``, ``$entity``, ``$langcode``, ``$items``, and ```$display``, where ``$element`` is the renderable array.

This means if you want to add an AJAX callback to a field formatter, you  need a **separate form function** that gets added in using ``drupal_get_form()``.  If you do this, you can build the AJAX as Drupal expects it.


Example form and field
----------------------

Here's an example form file below: as you can see it's a standard form following Drupal AJAX conventions.  We provide a ``rendered_maps`` fieldset with the prefix defining the wrapper (``examplemap-featuremap-organism-selector-wrapper``).  This is what we want to re-draw depending on what the user selects.

The selector has specified that wrapper, and the AJAX callback function ``examplemap_organism_featuremap_callback``.  We then define that function to simply return  the piece of the form that should be rebuilt: the ``rendered_maps`` fieldset!



.. code-block:: php

  /**
   * AJAX-enabled form for [field formatter name].
   */
  function tripal_example_map_organism_featuremap_selector_form($form, &$form_state, $select) {

    $selected = 0;

    // $form_state['values'] will be set if the form has been submitted via AJAX
    if (isset($form_state['values']['featuremap_select'])) {
      $selected = isset($form_state['values']['featuremap_select']);
    }

    // We need to provide a container for Drupal AJAX to replace.
    // Here we use a fieldset with a set ID which we can refer to below.
    $form['rendered_maps'] = [
      '#type' => 'fieldset',
      '#collapsible' => FALSE,
      '#prefix' => '<div id="examplemap-featuremap-organism-selector-wrapper">',
      '#suffix' => '</div>',
    ];

    // This is the element which will trigger AJAX.
    $form['rendered_maps']['featuremap_select'] = [
      '#type' => 'select',
      '#options' => $select,
      '#title' => 'Please select a map to view',
      '#default_value' => $selected,
      '#ajax' => [
        // Your Drupal AJAX callback
        // which simply returns the form element to be re-rendered.
        'callback' => 'examplemap_organism_featuremap_callback',
        // This should be the ID you set above on your container to be replaced.
        'wrapper' => 'examplemap-featuremap-organism-selector-wrapper',
        'effect' => 'fade',
      ],
    ];

    // Check the AJAX submitted values...
    $chosen = 0;
    if (isset($form_state['values']['featuremap_select'])) {
      $chosen = $form_state['input']['featuremap_select'];
    }

    // If the user chose an option (triggered AJAX).
    if ($chosen != 0) {
      // Then change the form accordingly...
      // Notice that you react to the AJAX change in the form
      // not in the AJAX callback.
      $mini_form = tripal_example_map_genetic_map_overview_form([], $form_state, $chosen);

      $form['rendered_maps']['map'] = $mini_form;

      return $form;
    }

    return $form;
  }

  /**
   * The callback will return the part of the form you want to re-draw.
   */
  function examplemap_organism_featuremap_callback($form, &$form_state) {

    return $form['rendered_maps'];
  }



In the field formatter, we simply add this form and put the markup in the element:

.. code-block:: php

    /**
     * In our Our__field_formatter.inc
     */
    public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {

      // Select choices would be loaded in the base field's load method.
      $select = $items[0]['select_choices'];

      $form = drupal_get_form('tripal_example_map_organism_featuremap_selector_form', $select);
      $content = drupal_render($form);
      $element[] = [
          '#type' => 'markup',
          '#markup' => $content,
      ];
      return $element;
    }
