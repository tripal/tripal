<?php
/**
 *
 */
class TripalTermViewsController extends EntityDefaultViewsController {

  /**
   * Edit or add extra fields to views_data().
   */
  public function views_data() {
    $data = parent::views_data();

    // Change handlers for Date Timestamps.
    // Date Created.
    $data['tripal_term']['created']['field']['handler'] = 'views_handler_field_date';
    $data['tripal_term']['created']['sort']['handler'] = 'views_handler_sort_date';
    $data['tripal_term']['created']['filter']['handler'] = 'views_handler_filter_date';
    // Date Changed/Updated.
    $data['tripal_term']['changed']['field']['handler'] = 'views_handler_field_date';
    $data['tripal_term']['changed']['sort']['handler'] = 'views_handler_sort_date';
    $data['tripal_term']['changed']['filter']['handler'] = 'views_handler_filter_date';

    $data['tripal_term']['table']['join'] = array();

    // Join the term to it's entity type.
    $data['tripal_term']['table']['join']['tripal_entity'] = array(
      'handler' => 'views_join',
      'left_field' => 'term_id',
      'field' => 'id',
    );

    return $data;
  }

}
