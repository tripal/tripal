<?php

/**
 * Field handler allowing all properties for a given stock to be displayed in a single cell
 *
 * @ingroup tripal_stock
 * @ingroup views_field_handlers
 */
class views_handler_field_stockprop_all extends views_handler_field_prerender_list {
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
		$this->aliases['properties'] = 'properties';
		$this->aliases['stock_id'] = 'stock_id';
		$this->field_alias = $this->aliases['stock_id'];
		
		//for each stock in this view page
		foreach ($values as $result) {
			if (!empty($result->{$this->aliases['properties']})) {
				
				// all properties for the current stock
				$properties = $result->{$this->aliases['properties']};
				foreach ($properties as $property) {					
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
		return $item['value'].' ('.$item['type_name'].')';
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

