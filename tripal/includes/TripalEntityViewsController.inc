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

    $data['tripal_entity']['id']['title'] = 'Tripal Content ID';
    $data['tripal_entity']['id']['help'] = 'The unique numeric ID for the content';


    $data['tripal_entity']['title']['help'] = 'The content\'s title.';
    $data['tripal_entity']['title']['field']['handler'] = 'tripal_views_handler_field_entity';

    $data['tripal_entity']['uid']['help'] = 'The User\'s unique ID.';

    $data['tripal_entity']['status']['help'] = 'The publish status.';
    $data['tripal_entity']['status']['field']['handler'] = 'tripal_views_handler_field_entity_status';


    $data['tripal_entity']['uid']['relationship'] = array(
      'base' => 'users',
      'base field' => 'uid',
      'handler' => 'views_handler_relationship',
      'label' => t('Users'),
      'title' => t('Users'),
      'help' => t('Associates the user information with this record.')
    );

    // It is not intended that the following fields will every be used by the
    // end-user within Views.
    unset($data['tripal_entity']['bundle']);
    unset($data['tripal_entity']['term_id']);
    unset($data['tripal_entity']['type']);

    return $data;
  }

}
