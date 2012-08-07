<?php
/**
 * @file
 * Plugin include file for export style plugin.
 */

/**
 * Generalized style plugin for export plugins.
 *
 * @ingroup views_style_plugins
 */
class tripal_views_plugin_style_export_fasta extends views_data_export_plugin_style_export {

  /**
   * Set options fields and default values.
   *
   * @return
   * An array of options information.
   */
  function option_definition() {
    $options = parent::option_definition();


    return $options;
  }

  /**
   * Options form mini callback.
   *
   * @param $form
   * Form array to add additional fields to.
   * @param $form_state
   * State of the form.
   * @return
   * None.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);


    $form['display'] = array(
      '#type' => 'fieldset',
      '#title' => 'Format Output',
    );
    
    $form['display']['residues_colname'] = array(
      '#type' => 'textfield',
      '#title' => t('The name of the residues column'),
      '#description' => t("The name of the column that contains the sequence ".
         "residues. To discover this field edit the feature residues field ".
         "and click the checkbox \"Rewrite ".
         "the output of this field\".  In the replacement patterns section are ".
         "field names.  The field name for the residues column should be the ".
         "last one"),
      '#required' => TRUE,
      '#default_value' => $this->options['display']['residues_colname'],
    );
    
    $form['display']['use_residues'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use the value as is'),
      '#description' => t('Check this box if you do not want the FASTA exporter ' .
         'to provide a definition line or add line breaks to the sequence. This is '.
         'useful when the field options provided for the residues column has '.
         'already been formatted'),
      '#default_value' => $this->options['display']['use_residues'],
    );


    $form['display']['num_bases_per_line'] = array(
      '#type' => 'textfield',
      '#title' => t('Number of bases per line'),
      '#description' => t('Specify the number of bases per line. If no value is ' .
        'provided. The sequence will be one long string (default)'),
      '#default_value' => $this->options['display']['num_bases_per_line'],
    );
    
    $form['display']['defline_fields'] = array(
      '#type' => 'textfield',
      '#title' => t('Defenition line fields'),
      '#default_value' => $this->options['display']['defline_fields'],
      '#description' => t("This field controls the information present in the definition ".
         "line for each sequence. Any of the fields in the view can be used in the ".
         "definition line.  To discover these fields, add the feature residues as the ".
         "last field in the view.  Then edit the field and click the checkbox \"Rewrite ".
         "the output of this field\".  In the replacement patterns section are the ".
         "field replacement tokens that can be used in this field. You may include any ".
         "additional text to separte or qualify these fields.  Only fields that have not ".
         "been excluded from the display will appear."),
    );
  }
}
