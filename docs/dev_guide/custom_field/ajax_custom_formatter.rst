AJAX Responsive Formatters
===========================


Some fields need to be responsive.  For example, a user might select an analysis or organism to display data from. Drupal developers often use AJAX to rebuild the page based on user input.

Drupal and AJAX
---------------

Drupal has its special way of doing AJAX!  You should `read the documentation carefully! <https://api.drupal.org/api/drupal/includes%21ajax.inc/group/ajax/7.x>`_ To Drupal, AJAX only makes sense as on forms, and field formatters **are not forms**.  Instead, they are `renderable arrays <https://www.drupal.org/docs/7/api/render-arrays/render-arrays-overview>`_
This may seem obvious in hindsight: rather than accepting ``$form`` and ``&$form_state``, they accept ``&$element``, ``$entity_type``, ``$entity``, ``$langcode``, ``$items``, and ```$display``, where ``$element`` is the renderable array.

This means if you want to add an AJAX callback to a field formatter, you  need a **seperate form file** that gets added in using ``drupal_get_form()``.  If you do this, you can build the AJAX as Drupal expects it.


Example form and field
----------------------

Here's an example form file below: as you can see it's a standard form following Drupal AJAX conventions.  We provide a ``rendered_maps`` fieldset with the prefix defining the wrapper (``examplemap-featuremap-organism-selector-wrapper``).  This is what we want to re-draw depending on what the user selects.

The selector has specified that wrapper, and the AJAX callback function ``examplemap_organism_featuremap_callback``.  We then define that function to simply return  the piece of the form that should be rebuilt: the ``rendered_maps`` fieldset!



.. code-block:: php

  #./includes/form_for_the_field.inc

  function tripal_example_map_organism_featuremap_selector($form, &$form_state, $select) {

    $selected = 0;

    if (isset($form_state['values']['featuremap_select'])) {
      $selected = isset($form_state['values']['featuremap_select']);
    }


    $form['rendered_maps'] = [
      '#type' => 'fieldset',
      '#collapsible' => FALSE,
      '#prefix' => '<div id="examplemap-featuremap-organism-selector-wrapper">',
      '#suffix' => '</div>',
    ];


    $form['rendered_maps']['featuremap_select'] = [
      '#type' => 'select',
      '#options' => $select,
      '#title' => 'Please select a map to view',
      '#default_value' => $selected,
      '#ajax' => [
        'callback' => 'examplemap_organism_featuremap_callback',
        'wrapper' => 'examplemap-featuremap-organism-selector-wrapper',
        'effect' => 'fade',
      ],
    ];


    $chosen = 0;

    if (isset($form_state['values']['featuremap_select'])) {
      $chosen = $form_state['input']['featuremap_select'];
    }

    if ($chosen != 0) {


      $mini_form = tripal_example_map_genetic_map_overview_form([], $form_state, $chosen);

      $form['rendered_maps']['map'] = $mini_form;

      return $form;
    }

    return $form;
  }

  /**
  * The callback will return the part o the form you want to re-draw.
  *
   */
  function examplemap_organism_featuremap_callback($form, &$form_state) {

    return $form['rendered_maps'];
  }



In the field formatter, we simply add this form and put the markup in the element:

.. code-block:: php

      //Our__field_formatter.inc

      //multiple maps for this organism, let user select.  Create a special form for that so we can have an AJAX select box
      $select= $select + $select_add;

      $form = drupal_get_form('tripal_example_map_organism_featuremap_selector', $select);
      $content = drupal_render($form);
        $element[] = [
          '#type' => 'markup',
          '#markup' => $content,
        ];
        return $element;
    }
