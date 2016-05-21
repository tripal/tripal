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

    $data['tripal_vocab']['table']['group'] = 'Tripal Content Vocabulary';

    $data['tripal_vocab']['vocabulary']['title'] = 'Vocabulary';
    $data['tripal_vocab']['vocabulary']['help'] = t('The short name of the vocabulary.');


    // It is not intended that the following fields will every be used by the
    // end-user.
    unset($data['tripal_vocab']['id']);
    unset($data['tripal_vocab']['changed']);
    unset($data['tripal_vocab']['created']);

    // This should not be a base table. It's not useful to create a
    // view of the tripal_term table.
    unset($data['tripal_vocab']['table']['base']);

    return $data;
  }

}
