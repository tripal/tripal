<?php
class views_handler_filter_feature_type_id extends views_handler_filter_string {

	function value_form(&$form, &$form_state) {

		$sql = "SELECT distinct feature_type FROM {unigene_for_search}";
		$results = chado_query($sql);
		$options = array ();
		while($type = db_fetch_object($results)) {
			$options[$type->feature_type] = $type->feature_type;
		}

		$form['value'] = array(
		'#type' => 'select',
		'#title' => t('Type of sequence'),
		'#options' => $options,
		);
		return $form;
	}

	function operator_options () {
		return array(
			'=' => t('Is equal to')
		);
	}
}