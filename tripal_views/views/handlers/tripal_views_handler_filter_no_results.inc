<?php

/**
 * @file
 * This filter ensures that when a page is first loaded, there are no results (ie: the
 * query is not run). This imitates an advanced search form allowing users to fist
 * enter other filter values and then retrieve results.
 */
class tripal_views_handler_filter_no_results extends views_handler_filter {

  /**
   * Ensure this filter is not exposed
   */
  function can_expose() {
    return FALSE;
  }

  /**
   * Defines the options form (form available to admin when they add a field to a view)
   */
  function options_form(&$form, &$form_state) {

    $form['msg'] = array(
      '#type' => 'item',
      '#value' => t('This filter ensures that when the page is first loaded, there are no results '
        .'(query is not run). This imitates an advanced search form allowing users to first enter '
        .'filter values and then retrieve results. This is especially suited for views with a very '
        .'large number of results if left unfiltered since it removes the long load time before '
        .'even allowing users the ability to filter down the results.')
    );

    $form['apply_button'] = array(
      '#type' => 'textfield',
      '#title' => 'Apply Button Text',
      '#default_value' => isset($this->options['apply_button']) ? $this->options['apply_button'] : 'Search'
    );

    $form['no_results_text'] = array(
      '#type' => 'textarea',
      '#title' => 'Header Text before Search',
      '#description' => 'This text will replace the no results text when the view is first loaded (before the user clicks Apply/Search)',
      '#default_value' => isset($this->options['no_results_text']) ? $this->options['no_results_text'] : ''
    );

  }

  function query() {
    global $base_url;
    $jquery_loaded = FALSE;

    // Check that all filter types don't return arrays
    // If they do then just show results by default b/c there is no way to tell the
    // different between before and after submitted
    $all_arrays = TRUE;
    if (is_array($this->view->exposed_raw_input)) {
      foreach ($this->view->exposed_raw_input as $v) {
        if (!is_array($v)) {
          $all_arrays = FALSE;
        }
      }
    }
    else {
      $all_arrays = FALSE;
    }

    if (empty($this->view->exposed_input) && !$all_arrays) {
      $this->view->executed = TRUE;

      $jquery_loaded = FALSE;

      // If set, change the no_results text
      if (!empty($this->options['apply_button'])) {
        if (!$jquery_loaded) {
          drupal_add_html_head('<script type="text/javascript" src="' . $base_url . '/misc/jquery.js"></script>');
          $jquery_loaded = TRUE;
        }
        $javascript = '<script type="text/javascript">'."\n"
          ."$(document).ready(function(){\n"
          ."  $('div.view-empty').html('" . addslashes($this->options['no_results_text']) . "');\n"
          ."});\n"
          .'</script>'."\n";
        drupal_add_html_head($javascript);
      }
    }

    // Set the Apply button text
    if (!empty($this->options['apply_button'])) {
      if (!$jquery_loaded) {
        drupal_add_html_head('<script type="text/javascript" src="' . $base_url . '/misc/jquery.js"></script>');
        $jquery_loaded = TRUE;
      }
      $javascript = '<script type="text/javascript">'."\n"
        ."$(document).ready(function(){\n"
        ."  $('div.views-submit-button').html(" . '"' . "<input type='submit' id='edit-submit-all-analysis' value='" . $this->options['apply_button'] . "' class='form-submit'>" . '"' . ");\n"
        ."});\n"
        .'</script>' . "\n";
      drupal_add_html_head($javascript);
    }
  }

}
