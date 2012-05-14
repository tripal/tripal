<?php

/**
 * Field handler allowing all database references of a specified db for a given stock to be 
 * displayed in a single cell.
 *
 * This handler only deals with database references joined to the stock through stock_dbxref. For 
 * the database reference joined to the stock by stock.dbxref_id simply join to the dbxref table.
 *
 * @ingroup tripal_stock
 * @ingroup views_field_handlers
 */
class views_handler_field_stock_dbxref_by_type extends views_handler_field_prerender_list {
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
		$options['display_options'] = array('default' => array('accession','db_name','link'));
    return $options;
  }

  /**
   * Provide "link to term" option.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

		$form['display_options'] = array(
			'#type' => 'checkboxes',
			'#title' => t('Display DB Reference Parts'),
			'#description' => t('Check each part of the database reference you want displayed where the parts '
				.'of a database relationship are: <Accession> (<Datbase Name>). You can also specify whether the accession '
				.'should be linked to the record in the database by checking "Link to External Record".'),
			'#options' => array(
				'accession' => 'Accession',
				'db_name' => 'Database Name',
				'link' => 'Link to External Record',
			),
			'#default_value' => array($this->options['display_options']['accession'], $this->options['display_options']['db_name'], $this->options['display_options']['link']),
		);
		
    $db_options = tripal_db_get_db_options();
	  $db_options[0] = 'Select a Database';
	  ksort($db_options);
    $form['db_ids'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Database'),
      '#options' => $db_options,
      '#default_value' => $this->options['db_ids'],
    );
  }

  /**
   * Add this term to the query
   */
  function query() {
    $this->add_additional_fields();
  }

	function pre_render($values) {
		$this->aliases['dbxref'] = 'dbxref';
		$this->aliases['stock_id'] = 'stock_id';
		$this->field_alias = $this->aliases['stock_id'];
		
		//for each stock in this view page
		foreach ($values as $result) {
			if (!empty($result->{$this->aliases['dbxref']})) {
				
				// all dbxrefs including the current stock
				$dbxrefs = $result->{$this->aliases['dbxref']};
				foreach ($dbxrefs as $dbxref) {
							
					// perform filtering
					if (!empty($this->options['db_ids'])) {
						$dbs2keep = array_filter($this->options['db_ids']);
						if (!in_array($dbxref->db_id, $dbs2keep)) {
							continue;
						}
					}
					
					// Add dbxref to the list of items to be rendered
					$this->items[$dbxref->stock_id][$dbxref->stock_dbxref_id]['stock_id'] = $dbxref->stock_id;
					$this->items[$dbxref->stock_id][$dbxref->stock_dbxref_id]['stock_dbxref_id'] = $dbxref->stock_dbxref_id;
					$this->items[$dbxref->stock_id][$dbxref->stock_dbxref_id]['dbxref_id'] = $dbxref->dbxref_id;
					$this->items[$dbxref->stock_id][$dbxref->stock_dbxref_id]['is_current'] = $dbxref->is_current;
					$this->items[$dbxref->stock_id][$dbxref->stock_dbxref_id]['db_id'] = $dbxref->db_id;
					$this->items[$dbxref->stock_id][$dbxref->stock_dbxref_id]['db_name'] = $dbxref->db_name;
					$this->items[$dbxref->stock_id][$dbxref->stock_dbxref_id]['urlprefix'] = $dbxref->urlprefix;
					$this->items[$dbxref->stock_id][$dbxref->stock_dbxref_id]['accession'] = $dbxref->accession;
					$this->items[$dbxref->stock_id][$dbxref->stock_dbxref_id]['version'] = $dbxref->version;
					$this->items[$dbxref->stock_id][$dbxref->stock_dbxref_id]['description'] = $dbxref->description;				}
			}
		}
	}

  function render_item($count, $item) {
		$text = array();
		if ($this->options['display_options']['link']) {
			if ($this->options['display_options']['accession']) {
				$text[] = l($item['accession'],$item['urlprefix'].$item['accession']);
			} else {
				$text[] = l('External Record',$item['urlprefix'].$item['accession']);
			}	
		} else {
			if ($this->options['display_options']['accession']) {
				$text[] = $item['accession'];
			}
		}
		
		
		if ($this->options['display_options']['db_name']) {
			$text[] = $item['db_name'];
		}
		
		if (sizeof($text) > 1) {
			return $text[0].' ('.$text[1].')';
		} else {
			return $text[0];
		}
  }

  function document_self_tokens(&$tokens) {
    $tokens['[' . $this->options['id'] . '-stock_id' . ']'] = t('The Stock ID.');
		$tokens['[' . $this->options['id'] . '-stock_dbxref_id' . ']'] = t('Stock Database Reference ID');
		$tokens['[' . $this->options['id'] . '-dbxref_id' . ']'] = t('Database Reference ID');
		$tokens['[' . $this->options['id'] . '-is_current' . ']'] = t('Is Current');
		$tokens['[' . $this->options['id'] . '-db_id' . ']'] = t('Database ID');
		$tokens['[' . $this->options['id'] . '-db_name' . ']'] = t('Database Name');
		$tokens['[' . $this->options['id'] . '-urlprefix' . ']'] = t('URL Prefix');
		$tokens['[' . $this->options['id'] . '-accession' . ']'] = t('Accession');
		$tokens['[' . $this->options['id'] . '-version' . ']'] = t('Version');
		$tokens['[' . $this->options['id'] . '-description' . ']'] = t('Description');
  }

  function add_self_tokens(&$tokens, $item) {
    $tokens['[' . $this->options['id'] . '-stock_id' . ']'] = $item['stock_id'];
		$tokens['[' . $this->options['id'] . '-stock_dbxref_id' . ']'] = $item['stock_dbxref_id'];
		$tokens['[' . $this->options['id'] . '-dbxref_id' . ']'] =  $item['dbxref_id'];
		$tokens['[' . $this->options['id'] . '-is_current' . ']'] = $item['is_current'];
		$tokens['[' . $this->options['id'] . '-db_id' . ']'] = $item['db_id'];
		$tokens['[' . $this->options['id'] . '-db_name' . ']'] = $item['db_name'];
		$tokens['[' . $this->options['id'] . '-urlprefix' . ']'] = $item['urlprefix'];
		$tokens['[' . $this->options['id'] . '-accession' . ']'] = $item['accession'];
		$tokens['[' . $this->options['id'] . '-version' . ']'] = $item['version'];
		$tokens['[' . $this->options['id'] . '-description' . ']'] = $item['description'];
  }
}

