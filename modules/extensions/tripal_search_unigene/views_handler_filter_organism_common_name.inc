<?php
class views_handler_filter_organism_common_name extends views_handler_filter_string {

	function value_form(&$form, &$form_state) {
		$sql = "SELECT distinct common_name, organism_id FROM {organism}";
		$results = chado_query($sql);
		$organisms = array();
		
		while ($org = db_fetch_object($results)) {
			// check if the organisms is synced
			if (db_result(db_query("SELECT * FROM {chado_organism} WHERE organism_id = $org->organism_id"))) {
				$organisms[$org->common_name] = t($org->common_name);
			}
		}
		$form['value'] = array(
		'#type' => 'select',
		'#title' => t('Organism'),
		'#options' => $organisms,
		'#default_value' => 0,
		);
 
	return $form;
	}
	
	function operator_options () {
		return array(
			'=' => t('Is equal to')
		);
	}
}