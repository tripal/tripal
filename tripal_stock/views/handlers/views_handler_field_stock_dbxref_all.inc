<?php

/**
 * Field handler allowing all database references for a given stock to be displayed in a single cell
 *
 * This handler only deals with database references joined to the stock through stock_dbxref. For 
 * the database reference joined to the stock by stock.dbxref_id simply join to the dbxref table.
 *
 * @ingroup tripal_stock
 * @ingroup views_field_handlers
 */
class views_handler_field_stock_dbxref_all extends views_handler_field_prerender_list {
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
		$this->aliases['dbxref'] = 'dbxref';
		$this->aliases['stock_id'] = 'stock_id';
		$this->field_alias = $this->aliases['stock_id'];
		
		//for each stock in this view page
		foreach ($values as $result) {
			if (!empty($result->{$this->aliases['dbxref']})) {
				
				// all dbxrefs including the current stock
				$dbxrefs = $result->{$this->aliases['dbxref']};
				foreach ($dbxrefs as $dbxref) {			
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
					$this->items[$dbxref->stock_id][$dbxref->stock_dbxref_id]['description'] = $dbxref->description;
				}
			}
		}
	}

  function render_item($count, $item) {
		if (!empty($item['urlprefix'])) {
			return l($item['accession'],$item['urlprefix'].$item['accession']).' ('.$item['db_name'].')';
		} else {
			return $item['accession'].' ('.$item['db_name'].')';
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

