<?php
// $Id: views_handler_field_term_node_tid.inc,v 1.4 2009/07/01 23:07:14 merlinofchaos Exp $

/**
 * Field handler for terms.
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
    $options['stockprop_type_ids'] = array('default' => array());
		$options['display_options'] = array('default' => array('subject','type','object'));
    return $options;
  }

  /**
   * Provide "link to term" option.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

		$form['display_options'] = array(
			'#type' => 'checkboxes',
			'#title' => t('Display Relationship Parts'),
			'#description' => t('Check each part of the relationship you want displayed where the part '
				.'of a relationship are: <Subject> <Relationship Type> <Object>. '
				.'For example, with the relationship Sarah is the maternal parent of Fred '
				.'if you checked only Object then "Fred" would be displayed.'),
			'#options' => array(
				'subject' => 'Subject',
				'type' => 'Relationship Type',
				'object' => 'Object',
			),
			'#default_value' => $this->options['display_options'],
		);
		
    $options = _get_cvterm_options( variable_get('chado_stock_relationship_cv', 'null') );
    $form['stockrel_type_ids'] = array(
      '#prefix' => '<div><div id="edit-options-stockrel_type_ids">',
      '#suffix' => '</div></div>',
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
					// perform filtering
					if (!empty($this->options['stockrel_type_ids'])) {
						$relationships2keep = array_filter($this->options['stockrel_type_ids']);
						if (!in_array($relationship->type_id, $relationships2keep)) {
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
		if ($this->options['display_options']['subject']) {
			$text[] = $item['subject_name'];
		}
		if ($this->options['display_options']['type']) {
			$text[] = $item['type_name'];
		}
		if ($this->options['display_options']['object']) {
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

