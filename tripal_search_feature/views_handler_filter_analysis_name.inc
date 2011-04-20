<?php
class views_handler_filter_analysis_name extends views_handler_filter_string {
	function value_form(&$form, &$form_state) {
		$type_id = tripal_get_cvterm_id("analysis_type");
		$sql = "SELECT name, A.analysis_id FROM {analysis} A INNER JOIN {analysisprop} AP ON A.analysis_id = AP.analysis_id WHERE type_id = $type_id AND value = 'tripal_analysis_unigene'";

		$results = chado_query($sql);
		$analyses = array();

		while ($ana = db_fetch_object($results)) {
			// check if the organisms is synced
			if (db_result(db_query("SELECT * FROM {chado_analysis} WHERE analysis_id = $ana->analysis_id"))) {
				$analyses[$ana->name] = t($ana->name);
			}
		}
		$form['value'] = array(
		'#type' => 'select',
		'#title' => t('Organism'),
		'#options' => $analyses,
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