<?php

/**
 * Field handler allowing all relationships of a specified type for a given chado entity to be displayed in 
 * a single cell
 *
 * @ingroup views_field_handlers
 */
class views_handler_field_chado_rel_by_type extends views_handler_field_prerender_list {
  function init(&$view, $options) {
    parent::init($view, $options);

    // Boolean to determine whether
    //    TRUE =>  value (property type)      -more than one property type displayed
    //    FALSE => or just value is rendered  -only 1 porperty type is displayed    
    $this->display_type = TRUE;

    $this->chado_type = $this->table;
    
    // Check that this chado type has a <chado type>_relationship table
    $sql = "SELECT true as bool FROM pg_tables WHERE tablename='%s_relationship'";
    $previous_db = tripal_db_set_active('chado');
    $exists = db_fetch_object(db_query($sql,$this->table));
    tripal_db_set_active($previous_db);
    if (!$exists->bool) {
      drupal_set_message('The Relationship by Type handler cannot be used with this table since the '.$this->table.'_relationship table doesn\'t exist', 'error');
      $this->broken = TRUE;
    }
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['rel_type_ids'] = array('default' => array());
    $options['rel_display_options'] = array('default' => array('subject','type','object'));
    return $options;
  }

  /**
   * Provide "link to term" option.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    if ($this->broken) {
      $form = array(
        'broken' => array(
          '#type' => 'item',
          '#value' => 'The Relationship by Type handler cannot be used with this table since the '.$this->table.'_relationship table doesn\'t exist',
        ),
      );
    } else {    
      $form['rel_display_parts'] = array(
        '#type' => 'checkboxes',
        '#title' => t('Display Relationship Parts'),
        '#description' => t('Check each part of the relationship you want displayed where the part '
          .'of a relationship are: [Subject] [Relationship Type] [Object]. '
          .'For example, with the relationship Sarah is the maternal parent of Fred '
          .'if you checked only Object then "Fred" would be displayed.'),
        '#options' => array(
          'subject' => 'Subject',
          'type' => 'Relationship Type',
          'object' => 'Object',
        ),
        '#default_value' => array($this->options['rel_display_parts']['subject'], $this->options['rel_display_parts']['type'], $this->options['rel_display_parts']['object']),
      );
      
      $form['rel_display_rels'] = array(
        '#type' => 'radios',
        '#title' => t('Display Relationships Where'),
        '#description' => t('Only relationships where the selected criteria is met will be shown. '
          .'The parts of a relationship are: [Subject] [Relationship Type] [Object]. '
          .'For example, with the relationships Sarah is the maternal parent of Fred and '
          .'Fred is the paternal_parent of Max where Fred is the current '.$this->chado_type.', '
          .'if you selected "Current '.$this->chado_type.' is the Object" only Sarah is the maternal parent of Fred'
          .' would be displayed.'),
        '#options' => array(
          'subject' => 'Current '.$this->chado_type.' is the Subject',
          'object' => 'Current '.$this->chado_type.' is the Object',
          'all' => 'Current '.$this->chado_type.' is the Subject and/or Object',
        ),
        '#default_value' => $this->options['rel_display_rels'],
      );
      
      $options = array();
      $sql = 'SELECT rel.type_id, cvt.name FROM %s_relationship rel LEFT JOIN cvterm cvt ON cvt.cvterm_id=rel.type_id GROUP BY rel.type_id,cvt.name';
      $previous_db = tripal_db_set_active('chado');
      $resource = db_query($sql,$this->table);
      tripal_db_set_active($previous_db);      
      while ($r = db_fetch_object($resource)) {
        $options[ $r->type_id ] = $r->name;
      }
      $form['rel_type_ids'] = array(
        '#type' => 'checkboxes',
        '#title' => t('Relationship Types'),
        '#options' => $options,
        '#default_value' => $this->options['rel_type_ids'],
      );
    }
  }

  /**
   * Add this term to the query
   */
  function query() {
    $this->add_additional_fields();
  }
  
  function pre_render($values) {
    $this->aliases['relationships'] = $this->chado_type . '_relationships';
    $chado_id = $this->table . '_id';
    $chado_relationship_id = $this->table . '_relationship_id';
    $this->aliases[$chado_id] = $chado_id;
    $this->field_alias = $this->aliases[$chado_id];
    
    //check if relationships added to results, if not then add
    if (empty( $this->view->result[0]->{$this->aliases['relationships']} )) {
      
      // retrieve the chado_id for each record in the views current page
      $chado_ids = array();
      foreach ($this->view->result as $row_num => $row) {
        $chado_ids[$row_num] = $row->{$chado_id};
      }
      
      // Add relationships to the view results
      $sql = "SELECT rel.*, cvterm.name as type_name, "
        ."subject.name as subject_name, object.name as object_name "
        ."FROM ".$this->table."_relationship rel "
        ."LEFT JOIN ".$this->table." subject ON rel.subject_id=subject.".$chado_id." "
        ."LEFT JOIN ".$this->table." object ON rel.object_id=object.".$chado_id." "
        ."LEFT JOIN cvterm cvterm ON rel.type_id = cvterm.cvterm_id "
        ."WHERE rel.subject_id IN (".implode(',',$chado_ids).") "
        ."OR rel.object_id IN (".implode(',',$chado_ids).") ";
      $previous_db = tripal_db_set_active('chado');
      $resource = db_query($sql);
      tripal_db_set_active($previous_db);
    
      while ($r = db_fetch_object($resource)) {
        if (in_array($r->subject_id, $chado_ids)) {
          $key = array_search($r->subject_id, $chado_ids);
          $r->{$chado_id} = $r->subject_id;
          $this->view->result[$key]->{$this->aliases['relationships']}[] = clone $r;
        }
        if (in_array($r->object_id, $chado_ids)) {
          $key = array_search($r->object_id, $chado_ids);
          $r->{$chado_id} = $r->object_id;
          $this->view->result[$key]->{$this->aliases['relationships']}[] = clone $r;
        }
      }

    }
    
    // add relationships to this field
    foreach ($this->view->result as $result) {
      if (!empty($result->{$this->aliases['relationships']})) {
        $relationships2keep = array_filter($this->options['rel_type_ids']);
        
        // all relationships including the current chado guy
        foreach ($result->{$this->aliases['relationships']} as $relationship) {      
          // perform filtering------
          //type
          if (!empty($this->options['rel_type_ids'])) {
            if (!in_array($relationship->type_id, $relationships2keep)) {
              continue;
            }
          }
          
          //"Display Relationships Where" criteria
          if (preg_match('/subject/', $this->options['rel_display_rels'])) {
            if ($relationship->{$chado_id} != $relationship->subject_id) {
              continue;
            }      
          } elseif (preg_match('/object/', $this->options['rel_display_rels'])) {
            if ($relationship->{$chado_id} != $relationship->object_id) {
              continue;
            }
          }
          
          // Add relationship to the list of items to be rendered
          $this->items[$relationship->{$chado_id}][$relationship->{$chado_relationship_id}][$chado_id] = $relationship->{$chado_id};
          $this->items[$relationship->{$chado_id}][$relationship->{$chado_relationship_id}][$chado_relationship_id] = $relationship->{$chado_relationship_id};
          $this->items[$relationship->{$chado_id}][$relationship->{$chado_relationship_id}]['subject_id'] = $relationship->subject_id;
          $this->items[$relationship->{$chado_id}][$relationship->{$chado_relationship_id}]['subject_name'] = $relationship->subject_name;
          $this->items[$relationship->{$chado_id}][$relationship->{$chado_relationship_id}]['object_id'] = $relationship->object_id;
          $this->items[$relationship->{$chado_id}][$relationship->{$chado_relationship_id}]['object_name'] = $relationship->object_name;
          $this->items[$relationship->{$chado_id}][$relationship->{$chado_relationship_id}]['type_id'] = $relationship->type_id;
          $this->items[$relationship->{$chado_id}][$relationship->{$chado_relationship_id}]['type_name'] = $relationship->type_name;
        }
      }
    } 
    
  }

  function render_item($count, $item) {
    $text = array();
    
    // Render Parts
    if ($this->options['rel_display_parts']['subject']) {
      $text[] = $item['subject_name'];
    }
    if ($this->options['rel_display_parts']['type']) {
      $text[] = $item['type_name'];
    }
    if ($this->options['rel_display_parts']['object']) {
      $text[] = $item['object_name'];
    }
    
    return implode(' ', $text);
  }
}
