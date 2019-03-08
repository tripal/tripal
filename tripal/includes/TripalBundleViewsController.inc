<?php

/**
 *
 */
class TripalBundleViewsController extends EntityDefaultViewsController {

  /**
   * Edit or add extra fields to views_data().
   */
  public function views_data() {
    $data = parent::views_data();

    // Join the bundle to it's entity.
    $data['tripal_bundle']['table']['join']['tripal_entity'] = [
      'handler' => 'views_join',
      'left_field' => 'bundle',
      'field' => 'name',
    ];

    // This should not be a base table. It's not useful to create a
    // view of the bundles.
    unset($data['tripal_bundle']['table']['base']);


    return $data;
  }

}
