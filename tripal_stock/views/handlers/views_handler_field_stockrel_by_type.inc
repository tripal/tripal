<?php

/**
 * Field handler allowing all relationships of a specified type for a given stock to be displayed in 
 * a single cell
 *
 * @ingroup tripal_stock
 * @ingroup views_field_handlers
 */
class views_handler_field_stockrel_by_type extends views_handler_field_prerender_list {
  function init(&$view, $options) {
    parent::init($view, $options);

    // Boolean to determine whether
    //    TRUE =>  value (property type)      -more than one property type displayed
    //    FALSE => or just value is rendered  -only 1 porperty type is displayed    
    $this->display_type = TRUE;

  }

  function option_definition() {
    $options = parent::option_definition();
    $options['stockrel_type_ids'] = array('default' => array());
    $options['stockrel_display_options'] = array('default' => array('subject','type','object'));
    return $options;
  }

  /**
   * Provide "link to term" option.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['stockrel_display_parts'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Display Relationship Parts'),
      '#description' => t('Check each part of the relationship you want displayed where the part '
        .'of a relationship are: \<Subject\> \<Relationship Type\> \<Object\>. '
        .'For example, with the relationship Sarah is the maternal parent of Fred '
        .'if you checked only Object then "Fred" would be displayed.'),
      '#options' => array(
        'subject' => 'Subject',
        'type' => 'Relationship Type',
        'object' => 'Object',
      ),
      '#default_value' => array($this->options['stockrel_display_parts']['subject'], $this->options['stockrel_display_parts']['type'], $this->options['stockrel_display_parts']['object']),
    );
    
    $form['stockrel_display_rels'] = array(
      '#type' => 'radios',
      '#title' => t('Display Relationships Where'),
      '#description' => t('Only relationships where the selected criteria is met will be shown. '
        .'The parts of a relationship are: \<Subject\> \<Relationship Type\> \<Object\>. '
        .'For example, with the relationships Sarah is the maternal parent of Fred and '
        .'Fred is the paternal_parent of Max where Fred is the current stock, '
        .'if you selected "Current Stock is the Object" only Sarah is the maternal parent of Fred'
        .' would be displayed.'),
      '#options' => array(
        'subject' => 'Current Stock is the Subject',
        'object' => 'Current Stock is the Object',
        'all' => 'Current Stock is the Subject and/or Object',
      ),
      '#default_value' => $this->options['stockrel_display_rels'],
    );
    
    $options = tripal_cv_get_cvterm_options( variable_get('chado_stock_relationship_cv', 'null') );
    $form['stockrel_type_ids'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Relationship Types'),
      '#options' => $options,
      '#default_value' => $this->options['stockrel_type_ids'],
    );
  }

  /**
   * Add this term to the query
   */
  function query() {
    $this->add_additional_fields();
  }

  function pre_render($values) {
    $this->aliases['relationships'] = 'relationships';
    $this->aliases['stock_id'] = 'stock_id';
    $this->field_alias = $this->aliases['stock_id'];
    
    //for each stock in this view page
    foreach ($values as $result) {
      if (!empty($result->{$this->aliases['relationships']})) {
          
        // all relationships including the current stock
        $relationships = $result->{$this->aliases['relationships']};
        foreach ($relationships as $relationship) {      
          // perform filtering------
          //type
          if (!empty($this->options['stockrel_type_ids'])) {
            $relationships2keep = array_filter($this->options['stockrel_type_ids']);
            if (!in_array($relationship->type_id, $relationships2keep)) {
              continue;
            }
          }
          
          //"Display Relationships Where" criteria
          if (preg_match('/subject/', $this->options['stockrel_display_rels'])) {
            if ($relationship->stock_id != $relationship->subject_id) {
              continue;
            }      
          } elseif (preg_match('/object/', $this->options['stockrel_display_rels'])) {
            if ($relationship->stock_id != $relationship->object_id) {
              continue;
            }
          }
          
          // Add relationship to the list of items to be rendered
          $this->items[$relationship->stock_id][$relationship->stock_relationship_id]['stock_id'] = $relationship->stock_id;
          $this->items[$relationship->stock_id][$relationship->stock_relationship_id]['stock_relationship_id'] = $relationship->stock_relationship_id;
          $this->items[$relationship->stock_id][$relationship->stock_relationship_id]['subject_id'] = $relationship->subject_id;
          $this->items[$relationship->stock_id][$relationship->stock_relationship_id]['subject_name'] = $relationship->subject_name;
          $this->items[$relationship->stock_id][$relationship->stock_relationship_id]['object_id'] = $relationship->object_id;
          $this->items[$relationship->stock_id][$relationship->stock_relationship_id]['object_name'] = $relationship->object_name;
          $this->items[$relationship->stock_id][$relationship->stock_relationship_id]['type_id'] = $relationship->type_id;
          $this->items[$relationship->stock_id][$relationship->stock_relationship_id]['type_name'] = $relationship->type_name;
        }
      }
    }
  }

  function render_item($count, $item) {
    $text = array();
    
    // Render Parts
    if ($this->options['stockrel_display_parts']['subject']) {
      $text[] = $item['subject_name'];
    }
    if ($this->options['stockrel_display_parts']['type']) {
      $text[] = $item['type_name'];
    }
    if ($this->options['stockrel_display_parts']['object']) {
      $text[] = $item['object_name'];
    }
    
    return implode(' ', $text);
  }

  function document_self_tokens(&$tokens) {
    $tokens['[' . $this->options['id'] . '-stock_id' . ']'] = t('The Stock ID.');
    $tokens['[' . $this->options['id'] . '-stock_relationship_id' . ']'] = t('Relationship ID');
    $tokens['[' . $this->options['id'] . '-subject_id' . ']'] = t('Subject ID');
    $tokens['[' . $this->options['id'] . '-subject_name' . ']'] = t('Subject Name');
    $tokens['[' . $this->options['id'] . '-object_id' . ']'] = t('Object ID');
    $tokens['[' . $this->options['id'] . '-object_name' . ']'] = t('Object Name');
    $tokens['[' . $this->options['id'] . '-type_id' . ']'] = t('Type ID');
    $tokens['[' . $this->options['id'] . '-type_name' . ']'] = t('Type Name');
  }

  function add_self_tokens(&$tokens, $item) {
    $tokens['[' . $this->options['id'] . '-stock_id' . ']'] = $item['stock_id'];
    $tokens['[' . $this->options['id'] . '-stock_relationship_id' . ']'] = $item['stock_relationship_id'];
    $tokens['[' . $this->options['id'] . '-subject_id' . ']'] = $item['subject_id'];
    $tokens['[' . $this->options['id'] . '-subject_name' . ']'] = $item['subject_name'];
    $tokens['[' . $this->options['id'] . '-object_id' . ']'] = $item['object_id'];
    $tokens['[' . $this->options['id'] . '-object_name' . ']'] = $item['object_name'];
    $tokens['[' . $this->options['id'] . '-type_id' . ']'] = $item['type_id'];
    $tokens['[' . $this->options['id'] . '-type_name' . ']'] = $item['type_name'];

  }
}

