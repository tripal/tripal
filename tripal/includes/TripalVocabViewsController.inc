<?php
/**
 *
 */
class TripalVocabViewsController extends EntityDefaultViewsController {

  /**
   * Edit or add extra fields to views_data().
   */
  public function views_data() {
    $data = parent::views_data();

    // Change handlers for Date Timestamps.
    // Date Created.
    $data['tripal_vocab']['created']['field']['handler'] = 'views_handler_field_date';
    $data['tripal_vocab']['created']['sort']['handler'] = 'views_handler_sort_date';
    $data['tripal_vocab']['created']['filter']['handler'] = 'views_handler_filter_date';
    // Date Changed/Updated.
    $data['tripal_vocab']['changed']['field']['handler'] = 'views_handler_field_date';
    $data['tripal_vocab']['changed']['sort']['handler'] = 'views_handler_sort_date';
    $data['tripal_vocab']['changed']['filter']['handler'] = 'views_handler_filter_date';

    // Join the term to it's vocabulary so we know it's namespace.
    $data['tripal_vocab']['table']['join'] = array();
    $data['tripal_vocab']['table']['join']['tripal_term'] = array(
      'handler' => 'views_join',
      'left_field' => 'vocab_id',
      'field' => 'id',
    );

    return $data;
  }

}
