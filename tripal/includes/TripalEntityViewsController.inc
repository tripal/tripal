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
    $data['tripal_entity']['created']['help'] = t('The date that the content was created.');
    // Date Changed/Updated.
    $data['tripal_entity']['changed']['field']['handler'] = 'views_handler_field_date';
    $data['tripal_entity']['changed']['sort']['handler'] = 'views_handler_sort_date';
    $data['tripal_entity']['changed']['filter']['handler'] = 'views_handler_filter_date';
    $data['tripal_entity']['changed']['help'] = t('The date that the content was last updated.');

    $data['tripal_entity']['id']['title'] = 'tid';
    $data['tripal_entity']['id']['help'] = 'The unique numeric ID for the content';

    $data['tripal_entity']['title']['help'] = 'The content\'s title.';
    $data['tripal_entity']['title']['field']['handler'] = 'tripal_views_handler_field_entity';

    $data['tripal_entity']['uid']['help'] = 'The User\'s unique ID.';

    $data['tripal_entity']['status']['help'] = 'The publish status.';
    $data['tripal_entity']['status']['field']['handler'] = 'tripal_views_handler_field_boolean';

    // We want to use our own query plugin.
    $data['tripal_entity']['table']['base']['query class'] = 'tripal_views_query';

    // Join with the User's table.
    $data['users']['table']['join']['tripal_entity'] = [
      'handler' => 'views_join',
      'left_field' => 'uid',
      'field' => 'uid',
    ];

    // It is not intended that the following fields will ever be used by the
    // end-user within Views.
    unset($data['tripal_entity']['bundle']);
    unset($data['tripal_entity']['term_id']);
    unset($data['tripal_entity']['type']);


    // This should not be a base table. We're not going to allow
    // for views based on TripalEntity content types or the list of
    // fields would become outrageously overwhelming. Instead we will
    // provide views by bundle.
    unset($data['tripal_entity']['table']['base']);

    return $data;
  }

}
