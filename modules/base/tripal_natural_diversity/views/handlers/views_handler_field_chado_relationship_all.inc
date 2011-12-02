<?php
// $Id: views_handler_field_term_node_tid.inc,v 1.4 2009/07/01 23:07:14 merlinofchaos Exp $

/**
 * Field handler for terms.
 */
class views_handler_field_chado_relationship_all extends views_handler_field_prerender_list {
  function init(&$view, $options) {
    parent::init($view, $options);
  }

  function query() {
    // not sure why this is needed since nothing is added to the query...
    $this->add_additional_fields();
  }

	function pre_render($values) {
		$this->aliases['relationships'] = 'relationships';
		$this->aliases['primary_id'] = $this->view->base_table.'_id';
		$this->field_alias = $this->aliases['primary_id'];
		$relationship_tablename = $this->view->base_table.'_relationship';
		$this->aliases['relationship_primary_id'] = $relationship_tablename . '_id';
		$table_desc = module_invoke_all('chado_'.$relationship_tablename.'_schema');
		foreach($table_desc['foreign keys'][$this->view->base_table]['columns'] as $k => $v) {
		  if (preg_match('/subject/',$k)) { $subject_id = $k; }
		  if (preg_match('/object/', $k)) { $object_id = $k; }
		}
		$this->aliases['subject_id'] = $subject_id;
		$this->aliases['object_id'] = $object_id;
		  
    //Add values for all if possible
    if (!$this->view->result[0]->relationships) {
      //get base table primary keys
      $primary_ids = array();
		  foreach ($this->view->result as $row_num => $row) {
			  $primary_ids[$row_num] = $row->{$this->aliases['primary_id']};
		  }
		
		  //generate results from db
      $sql = "SELECT ".$relationship_tablename.".*, cvterm.name as type_name, "
					."subject_parent.name as subject_name, object_parent.name as object_name "
					."FROM ".$relationship_tablename." "
					."LEFT JOIN ".$this->view->base_table." subject_parent ON "
					  .$relationship_tablename.".".$subject_id."=subject_parent.".$this->aliases['primary_id']." "
					."LEFT JOIN ".$this->view->base_table." object_parent ON "
					  .$relationship_tablename.".".$object_id."=object_parent.".$this->aliases['primary_id']." "
					."LEFT JOIN cvterm cvterm ON "
					  .$relationship_tablename.".type_id = cvterm.cvterm_id "
					."WHERE ".$relationship_tablename.".".$subject_id." IN (".implode(',',$primary_ids).") "
					."OR ".$relationship_tablename.".".$object_id." IN (".implode(',',$primary_ids).") ";
			$previous_db = tripal_db_set_active('chado');
			$resource = db_query($sql);
			tripal_db_set_active($previous_db);
			
			//add results to views results
			while ($r = db_fetch_object($resource)) {
				if (in_array($r->{$subject_id}, $primary_ids)) {
					$key = array_search($r->{$subject_id}, $primary_ids);
					$r->{$this->aliases['primary_id']} = $r->{$subject_id};
					$this->view->result[$key]->relationships[] = clone $r;
				}
				if (in_array($r->{$object_id}, $primary_ids)) {
					$key = array_search($r->{$object_id}, $primary_ids);
					$r->{$this->aliases['primary_id']} = $r->{$object_id};
					$this->view->result[$key]->relationships[] = clone $r;
				}
			} //end of while
    }// end of if add values

		//for each stock in this view page, add the values
		foreach ($values as $result) {
			if (!empty($result->{$this->aliases['relationships']})) {
				
				// all relationships including the current stock
				$relationships = $result->{$this->aliases['relationships']};
				foreach ($relationships as $relationship) {			
					// Add relationship to the list of items to be rendered
					// Note: $this->aliases['primary_id'] = base table primary key
					
					$elements = array(
					  $this->aliases['primary_id'] => $relationship->{$this->aliases['primary_id']},
					  $this->aliases['relationship_primary_id'] => $relationship->{$this->aliases['relationship_primary_id']},
					  'subject_id' => $relationship->{$this->aliases['subject_id']},
					  'subject_name' => $relationship->subject_name,
					  'object_id' => $relationship->{$this->aliases['object_id']},
            'object_name' => $relationship->object_name,
            'type_id' => $relationship->type_id,
            'type_name' => $relationship->type_name,
					);
					$this->items[$relationship->{$this->aliases['primary_id']}][$relationship->{$this->aliases['relationship_primary_id']}] = $elements;
				}
			}
		}
	}

  function render_item($count, $item) {
		return $item['subject_name'].' '.$item['type_name'].' '.$item['object_name'];
  }

}

