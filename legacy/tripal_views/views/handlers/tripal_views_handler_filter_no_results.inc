<?php
/**
 * @file
 * Contains tripal_views_handler_filter_no_results
 */

/**
 * This filter ensures that when a page is first loaded, there are no results (ie: the
 * query is not run). This imitates an advanced search form allowing users to fist
 * enter other filter values and then retrieve results.
 *
 * @ingroup tripal_views
 */
class tripal_views_handler_filter_no_results extends views_handler_filter {

  /**
   * Ensure this filter is not exposed
   */
  function can_expose() {
    return FALSE;
  }

  /**
   * {@inheritdoc}
   */
  function options_form(&$form, &$form_state) {

    $form['msg'] = array(
      '#type' => 'item',
      '#markup' => t('This filter ensures that when the page is first loaded, there are no results '
        .'(query is not run). This imitates an advanced search form allowing users to first enter '
        .'filter values and then retrieve results. This is especially suited for views with a very '
        .'large number of results if left unfiltered since it removes the long load time before '
        .'even allowing users the ability to filter down the results.')
    );

    $form['apply_button'] = array(
      '#type' => 'item',
      '#title' => t('Apply Button Text'),
      '#markup' => t('This can now to be changed through the Views UI in the "Advanced" '
        . 'area by clicking "Settings" for the "Exposed Form Style" when the style is "Basic".')
    );

    $form['no_results_text'] = array(
      '#type' => 'item',
      '#title' => t('Header Text before Search'),
      '#markup' => t('When the view is first loaded (before any filters are applied), '
        . 'the "No Results Behaviour" textare added in the "Advanced" are of the Views '
        . 'UI will be shown to the user.'),
    );

  }

  /**
   * {@inheritdoc}
   */
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
    }
  }

}
