<?php
// $Id: views_handler_field_term_node_tid.inc,v 1.4 2009/07/01 23:07:14 merlinofchaos Exp $

/**
 * Field handler for terms.
 */
class views_handler_field_stockprop_by_type extends views_handler_field_prerender_list {
  function init(&$view, $options) {
    parent::init($view, $options);

    // Boolean to determine whether
    //    TRUE =>  value (property type)      -more than one property type displayed
    //    FALSE => or just value is rendered  -only 1 porperty type is displayed    
    $this->display_type = TRUE;

  }

  function option_definition() {
    $options = parent::option_definition();
    $options['stockprop_type_ids'] = array('default' => array());
    return $options;
  }

  /**
   * Provide "link to term" option.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $options = _get_cvterm_options( variable_get('chado_stock_prop_types_cv', 'null') );
    $form['stockprop_type_ids'] = array(
      '#prefix' => '<div><div id="edit-options-stockprop_type_ids">',
      '#suffix' => '</div></div>',
      '#type' => 'checkboxes',
      '#title' => t('Property Types'),
      '#options' => $options,
      '#default_value' => $this->options['stockprop_type_ids'],
    );
  }

  /**
   * Add this term to the query
   */
  function query() {
    $this->add_additional_fields();
  }

	function pre_render($values) {
		$this->aliases['properties'] = 'properties';
		$this->aliases['stock_id'] = 'stock_id';
		$this->field_alias = $this->aliases['stock_id'];

		// $this->display_type is used to hide (type name) when only one type is displayed in the column
		if (sizeof(array_filter($this->options['stockprop_type_ids'])) == 1) {
			$this->display_type = FALSE;        
		}
		
		//for each stock in this view page
		foreach ($values as $result) {
			if (!empty($result->{$this->aliases['properties']})) {
				
				// all properties for the current stock
				$properties = $result->{$this->aliases['properties']};
				foreach ($properties as $property) {
					// perform filtering
					if (!empty($this->options['stockprop_type_ids'])) {
						$types2keep = array_filter($this->options['stockprop_type_ids']);
						if (!in_array($property->type_id, $types2keep)) {
							continue;
						}
					}
					
					// Add property to the list of items to be rendered
					$this->items[$property->stock_id][$property->stockprop_id]['stock_id'] = $property->stock_id;
					$this->items[$property->stock_id][$property->stockprop_id]['stockprop_id'] = $property->stockprop_id;
					$this->items[$property->stock_id][$property->stockprop_id]['type_id'] = $property->type_id;
					$this->items[$property->stock_id][$property->stockprop_id]['type_name'] = check_plain($property->type_name);
					$this->items[$property->stock_id][$property->stockprop_id]['value'] = check_plain($property->value);
					$this->items[$property->stock_id][$property->stockprop_id]['rank'] = $property->rank;
				}
			}
		}
	}

  function render_item($count, $item) {
    if ($this->display_type) {
      return $item['value'].' ('.$item['type_name'].')';
    } else {
      if ($item['value']) {
        return $item['value'];
      } else {
        //display checkmark image
				$image_path = drupal_get_path('module','chado_views').'/images/checkmark.gif';
        return '<img border="0" src="'.url($image_path).'" alt="checkmark" />';
      }
    }
  }

  function document_self_tokens(&$tokens) {
    $tokens['[' . $this->options['id'] . '-stock_id' . ']'] = t('The Stock ID.');
    $tokens['[' . $this->options['id'] . '-stockprop_id' . ']'] = t('The Property ID.');
    $tokens['[' . $this->options['id'] . '-type_id' . ']'] = t('The Property Type ID.');
    $tokens['[' . $this->options['id'] . '-type_name' . ']'] = t('The Property Type.');
    $tokens['[' . $this->options['id'] . '-value' . ']'] = t('The Value of the Property.');
    $tokens['[' . $this->options['id'] . '-rank' . ']'] = t('The Rank of the Property.');
  }

  function add_self_tokens(&$tokens, $item) {
    $tokens['[' . $this->options['id'] . '-stock_id' . ']'] = $item['stock_id'];
    $tokens['[' . $this->options['id'] . '-stockprop_id' . ']'] = $item['stockprop_id'];
    $tokens['[' . $this->options['id'] . '-type_id' . ']'] = $item['type_id'];
    $tokens['[' . $this->options['id'] . '-type_name' . ']'] = $item['type_name'];
    $tokens['[' . $this->options['id'] . '-value' . ']'] = $item['value'];
    $tokens['[' . $this->options['id'] . '-rank' . ']'] = $item['rank'];
  }
}

