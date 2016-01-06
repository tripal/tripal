<?php
/**
 *
 */
class TripalEntityViewsController extends EntityDefaultViewsController {

  /**
   * Edit or add extra fields to views_data().
   */
  public function views_data() {
    $data = parent::views_data();

    // Change handlers for Date Timestamps.
    // Date Created.
    $data['tripal_entity']['created']['field']['handler'] = 'views_handler_field_date';
    $data['tripal_entity']['created']['sort']['handler'] = 'views_handler_sort_date';
    $data['tripal_entity']['created']['filter']['handler'] = 'views_handler_filter_date';
    // Date Changed/Updated.
    $data['tripal_entity']['changed']['field']['handler'] = 'views_handler_field_date';
    $data['tripal_entity']['changed']['sort']['handler'] = 'views_handler_sort_date';
    $data['tripal_entity']['changed']['filter']['handler'] = 'views_handler_filter_date';

    return $data;
  }

}
