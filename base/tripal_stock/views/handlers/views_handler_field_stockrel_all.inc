<?php

/**
 * Field handler allowing all relationships for a given stock to be displayed in a single cell
 *
 * @ingroup tripal_stock
 * @ingroup views_field_handlers
 */
class views_handler_field_stockrel_all extends views_handler_field_prerender_list {
  function init(&$view, $options) {
    parent::init($view, $options);
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
		return $item['subject_name'].' '.$item['type_name'].' '.$item['object_name'];
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

