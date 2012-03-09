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
class tripal_core_plugin_style_export_fasta extends views_data_export_plugin_style_export {

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

    $form['defline_fields'] = array(
      '#prefix' => '<div><div id="edit-options-newline-replacement">',
      '#suffix' => '</div></div>',
      '#type' => 'textfield',
      '#title' => t('Defenition line fields'),
      '#default_value' => $this->options['defline_fields'],
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
